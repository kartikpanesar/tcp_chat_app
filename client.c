#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

#define PORT "5050"
#define HOSTNAME "machine"
#define BUFF_MAX 1024


void *sending_routine(void *arg){
        int client_sockfd = *(int *)arg;
        int bytes_read = 0;
        int bytes_sent = 0;
        char buffer[BUFF_MAX] = {0};

        while((bytes_read = read(STDIN_FILENO, buffer, BUFF_MAX))>0){
                bytes_sent = 0;
                int n = 0;
                while(bytes_sent < bytes_read){
                        n = send(client_sockfd, buffer+bytes_sent, bytes_read-bytes_sent, 0);
                        if(n==-1){
                                fprintf(stderr, "Error in send.\n");
                                exit(1);
                        }
                        bytes_sent += n;
                }
        }


        if(bytes_read==-1){
                fprintf(stderr, "Error in read\n");
                perror("Error: ");
                exit(1);
        }

        return NULL;
}


void *receiving_routine(void *arg){
        int client_sockfd = *(int *)arg;
        int bytes_recv= 0;
        char buffer[BUFF_MAX] = {0};

        while((bytes_recv=recv(client_sockfd, buffer, BUFF_MAX, 0))>0){
                int bytes_written = 0;
                int n = 0;
                while(bytes_written < bytes_recv){
                        n = write(STDOUT_FILENO, buffer+bytes_written, bytes_recv-bytes_written);
                        if(n==-1){
                                fprintf(stderr, "Error in write.\n");
                                perror("Error: ");
                                exit(1);
                        }
                        bytes_written += n;
                }
        }

        return NULL;
}

int main(){

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


        int rc = 0;
        pthread_t sending;
        rc = pthread_create(&sending, NULL, sending_routine, &client_sockfd);
        if(rc!=0){
                fprintf(stderr, "Error while creating sending thread.\n");
                exit(1);
        }

        pthread_t receiving;
        rc = pthread_create(&receiving, NULL, receiving_routine, &client_sockfd);
        if(rc!=0){
                fprintf(stderr, "Error while creating receiving thread.\n");
                exit(1);
        }

        pthread_join(sending, NULL);
        pthread_join(receiving, NULL);


        close(client_sockfd);
        freeaddrinfo(result);

        return 0;

}
