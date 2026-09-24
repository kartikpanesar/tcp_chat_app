#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>

#define PORT "5050"
#define HOSTNAME "machine"

#define BUFFMAX 1024

int main(){

        char buffer[BUFFMAX];

        int client_sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if(client_sockfd==-1){
                fprintf(stderr, "Couldn't create socket.\n");
                perror("Error: ");
                exit(1);
        }

        struct addrinfo hint = {0};
        hint.ai_family = AF_INET;
        hint.ai_socktype = SOCK_STREAM;

        struct addrinfo *result;
        struct addrinfo *p;

        int connected = 0;
        int bytes_count = 0;


        if(getaddrinfo(HOSTNAME, PORT, &hint, &result)!=0){
                fprintf(stderr, "Error in getaddrinfo.\n");
                exit(1);
        }

        for(p = result; p!=NULL; p=p->ai_next){
                // if the socket connects to a server out of all the elements in the list.
                if(connect(client_sockfd, p->ai_addr, p->ai_addrlen)==0){
                        connected = 1;
                        break;
                }
        }

        if(!connected){
                fprintf(stderr, "Couldn't connect.\n");
                perror("Errno: \n");
                exit(1);
        }

        // get the data from the server and the print it at standard Output.
        while(1){
                //recieving message.
                while((bytes_count=recv(client_sockfd, buffer, BUFFMAX, 0))>0){

                        int bytes_written = 0;
                        while(bytes_written < bytes_count){
                                ssize_t bytes = write(STDOUT_FILENO, buffer + bytes_written , bytes_count-bytes_written);
                                if(bytes==-1){
                                        fprintf(stderr, "Error in write syscall.\n");
                                        perror("Error: ");
                                        exit(1);
                                }
                                bytes_written += bytes;
                        }

                }
                if(bytes_count==-1){
                        fprintf(stderr, "Error while receiving \n");
                        perror("Error: ");
                        exit(1);
                }
        }


        close(client_sockfd);
        freeaddrinfo(result);

        return 0;

}
