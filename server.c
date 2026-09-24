#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#define PORT 5050
#define BUFF_MAX 1024

int main(){

        char buffer[BUFF_MAX];

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

                printf("Client connected .....\n");

                while(1){
                        while((bytes_count=read(STDIN_FILENO, &buffer, BUFF_MAX))!=0){

                                // sending data to client_socket.
                                send(client_sockfd, &buffer, bytes_count, 0);
                        }
                }

        }

        return 0;
}
