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
#define CLIENT_MAX 256


// the suffix _s means nothing just a style convention.
typedef struct client_s {
        int client_sockfd;
        struct sockaddr_in client_addr;
        int status;
} client_t ;

static client_t clients[CLIENT_MAX] = {0};
static int client_index = 0;

static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;


void broadcast(char *buffer, int bytes, int current_sfd){
        int current_sockfd = current_sfd;
        int this_sockfd = 0;

        pthread_mutex_lock(&lock);
        for(int j=0; j<client_index; j++){
                this_sockfd = clients[j].client_sockfd;
                if(current_sockfd == this_sockfd)
                        continue;

                int bytes_sent = 0;
                int n = 0;
                while(bytes_sent < bytes){
                        n = send(this_sockfd, buffer, bytes-bytes_sent, 0);
                        bytes_sent += n;
                }
        }
        pthread_mutex_unlock(&lock);

        return;
}

void *handle_client(void *arg){
        int client_sockfd = *(int *)arg;

        char buffer[BUFF_MAX] = {0};
        int bytes_count = 0;

        while((bytes_count=recv(client_sockfd, buffer, BUFF_MAX, 0))>0){
                broadcast(buffer, bytes_count, client_sockfd);
        }

        close(client_sockfd);

        return NULL;
}


void *listen_loop(void *arg){
        int server_sockfd = *(int *)arg;
        int rc = 0;

        client_t current = {0};
        socklen_t addrlen = sizeof(current.client_addr);

        pthread_t per_client;


        while(1){
                current.client_sockfd = accept(server_sockfd, (struct sockaddr *)&current.client_addr, &addrlen);
                current.status = 1;
                printf("Client Connected ...\n");

                pthread_create(&per_client, NULL, handle_client, &current.client_sockfd);
                pthread_detach(per_client);

                pthread_mutex_lock(&lock);
                clients[client_index++] = current;
                pthread_mutex_unlock(&lock);
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


        pthread_t listen_thread;

        rc = pthread_create(&listen_thread, NULL, listen_loop, &server_socketfd);
        if(rc!=0){
                fprintf(stderr, "Error while creating thread.\n");
                exit(1);
        }

        pthread_join(listen_thread, NULL);

        return 0;
}





