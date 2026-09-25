#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>

#define PORT 5050
#define BUFF_MAX 1024

static char buffer[BUFF_MAX];

void *handle_client(void *arg){
        int client_sockfd = *(int *)arg;
        int bytes_count = 0;

        printf("Client is connected ...\n");

        while((bytes_count=read(STDIN_FILENO, buffer, BUFF_MAX))>0){

                send(client_sockfd, buffer, bytes_count, 0);
        }

        if(bytes_count==-1){
                perror("read error: ");
        }

        close(client_sockfd);

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
        int bytes_count = 0;

        if(listen(server_socketfd, backlog)==-1){
                fprintf(stderr, "Error in listen syscall.\n");
                perror("Error: ");
                close(server_socketfd);
                exit(1);
        }

        printf("Listening on Port %d\n", PORT);

        while(1){
                client_sockfd = accept(server_socketfd,(struct sockaddr *) &client_addr, &client_sl);
                if(client_sockfd==-1){
                        fprintf(stderr, "Error while accept syscall.\n");
                        perror("Error: ");
                        continue;
                }

                int *p_client_sockfd = malloc(sizeof(int));
                if(p_client_sockfd==NULL){
                        perror("Malloc Error: ");
                        close(client_sockfd);
                        continue;
                }

                *p_client_sockfd = client_sockfd;

                pthread_t thr;
                rc = pthread_create(&thr, NULL, handle_client, p_client_sockfd);
                if(rc!=0){
                        fprintf(stderr, "Error while creating thread.\n");
                        continue;
                }

                rc = pthread_detach(thr);
        }

        return 0;
}
