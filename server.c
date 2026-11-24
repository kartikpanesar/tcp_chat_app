#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include "client_ds.h"

#define PORT 5050
#define BUFF_MAX 1024

static client_list *clients = 0;

static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;


void broadcast(char *buffer, int bytes, int sockfd){
        client_n *current_node = 0;


        pthread_mutex_lock(&lock);
        for(current_node = clients->head; current_node != NULL; current_node = current_node->next){
                if(current_node->data->sockfd == sockfd)
                        continue;

                int bytes_sent = 0;
                int n = 0;
                while(bytes_sent < bytes){
                        n = send(current_node->data->sockfd, buffer + bytes_sent, bytes-bytes_sent, 0);
                        bytes_sent += n;
                }
        }
        pthread_mutex_unlock(&lock);

        return;
}

void *handle_client(void *arg){
        int client_sockfd = *(int *)arg;
        free(arg);


        char buffer[BUFF_MAX] = {0};
        int bytes_count = 0;

        while((bytes_count=recv(client_sockfd, buffer, BUFF_MAX, 0))>0){
                if(bytes_count == 0){
                        close(client_sockfd);
                }
                broadcast(buffer, bytes_count, client_sockfd);
        }

        close(client_sockfd);

        return NULL;
}


void *listen_loop(int server_sockfd){
        int rc = 0;

        client_t current = {0};
        clients = client_list_init();

        pthread_t per_client;


        while(1){
                current.sockfd = accept(server_sockfd, NULL, NULL);
                current.status = 1;
                printf("Client Connected ...\n");

                int *c_sockfdp = malloc(sizeof(int));
                if(c_sockfdp == NULL){
                        perror("Unable to allocate memory for sockfd.\n");
                }

                *c_sockfdp = current.sockfd;

                // adding client to the client list .
                pthread_mutex_lock(&lock);
                client_add(clients, &current);
                pthread_mutex_unlock(&lock);

                // Seperate thread for handling the client.
                pthread_create(&per_client, NULL, handle_client, c_sockfdp);
                pthread_detach(per_client);


        }

        return NULL;
}


int main(){

        int server_socketfd;
        server_socketfd = socket(AF_INET, SOCK_STREAM, 0);
        if(server_socketfd==-1){
                fprintf(stderr, "Error in socket syscall.\n");
                perror("Error: ");
                exit(1);
        }

        // This set the socket such that we can make connection on the same port again 
        // without waiting.
        int opt = 1;
        setsockopt(server_socketfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        printf("Server socket created.\n");

        // now we have a stream socket.


        struct sockaddr_in server_addr = {0};
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(PORT);
        server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

        socklen_t server_sl = sizeof(server_addr);

        if(bind(server_socketfd, (struct sockaddr *)&server_addr, sizeof(server_addr))!=0){
                fprintf(stderr, "Error in bind.\n");
                perror("Error:");
                exit(1);
        }

        printf("Binding the server is completed.\n");

        int client_sockfd;
        int rc; // result code

        struct sockaddr_in client_addr = {0};
        socklen_t client_sl = sizeof(client_addr);


        int backlog = 10;
        int file_fd = 0;

        if(listen(server_socketfd, backlog)==-1){
                fprintf(stderr, "Error in listen syscall.\n");
                perror("Error: ");
                close(server_socketfd);
                exit(1);
        }

        printf("Listening on Port %d\n", PORT);


        listen_loop(server_socketfd);

        return 0;
}





