#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct client_s {
        int client_sockfd;
        int status;
        char *username;
} client_t; 

typedef struct client_node {
        struct client_node *prev;
        client_t *client_data;
        struct client_node *next;
} client_n;

typedef struct dynamic_array {
        client_n *head;
        client_n *tail;
} client_array;

int same_username(client_array*, client_t*);


client_array *client_array_init()
{
        client_array *c_array = malloc(sizeof(client_array));

        if(c_array == 0)
        {
                fprintf(stderr, "Couldn't allocate memory for creating client_array\n");
                exit(1);
        }

        c_array -> head = 0;
        c_array -> tail = 0;

        return c_array;
}


int client_add(client_array *c_array, client_t *him){

        if(same_username(c_array, him)==1)
        {
                fprintf(stderr, "Change username, This username is already taken.\n");
                return 1;
        }

        // create our own client_t 
        client_t *own = malloc(sizeof(client_t));
        if(own == 0)
        {
                fprintf(stderr, "Couldn't allocate memory for adding a client.\n");
                return 1;
        }

        memcpy((void *) own , (void *) him, sizeof(client_t));

        // create node
        client_n *node = malloc(sizeof(client_n));

        if(node == 0)
        {
                fprintf(stderr, "Couldn't allocat memory for client_node.\n");
                return 1;
        }

        node -> prev = 0;
        node -> client_data = own;
        node -> next = 0;

        if(c_array -> tail == 0)
        {
                c_array -> head = node;
                c_array -> tail = node;
        }
        else {
                c_array -> tail -> next = node;
                node -> prev = c_array -> tail;
                c_array -> tail = node;
        }

        return 0;
}

int get_sockfd(client_array *c_array, char *username)
{
         client_n *current = c_array -> head;

         while(current != 0)
         {
                 if(strcmp(current -> client_data -> username, username) == 0)
                        return current -> client_data -> client_sockfd ;

                 current = current -> next;
         }

         return -1;
}

void client_array_destroy(client_array *c_array)
{
        client_n *current = c_array -> head;
        client_n *next = 0;

        while(current!=0){
                next = current -> next;

                free(current -> client_data);
                free(current);

                current = next;
        }

        free(c_array);
}


int same_username(client_array *c_array, client_t *him)
{
        client_n *current = c_array -> head;

        while(current != 0)
        {
                if(strcmp(current -> client_data -> username, him -> username) == 0)
                        return 1;
                current = current -> next;
        }

        return 0;

}



