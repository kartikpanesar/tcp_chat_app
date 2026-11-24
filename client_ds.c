#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// the suffix _s means nothing just a style convention.
typedef struct client_s {
        int sockfd;
        int status;
        char *username;
} client_t; 

typedef struct client_node {
        struct client_node *prev;
        client_t *data;
        struct client_node *next;
} client_n;

typedef struct linked_list{
        client_n *head;
        client_n *tail;
} client_list;

int username_exist(client_list*, client_t*);


client_list *client_list_init()
{
        client_list *c_array = malloc(sizeof(client_list));

        if(c_array == 0)
        {
                fprintf(stderr, "Couldn't allocate memory for creating client_array\n");
                exit(1);
        }

        c_array -> head = NULL;
        c_array -> tail = NULL;

        return c_array;
}


int client_add(client_list *c_list, client_t *him){

        if(username_exist(c_list, him)==1)
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

        node -> prev = NULL;
        node -> data = own;
        node -> next = NULL;

        if(c_list -> tail == NULL)
        {
                c_list -> head = node;
                c_list -> tail = node;
        }
        else {
                c_list -> tail -> next = node;
                node -> prev = c_list -> tail;
                c_list -> tail = node;
        }

        return 0;
}

int get_sockfd(client_list *c_list, char *username)
{
         client_n *current = c_list -> head;

         while(current != 0)
         {
                 if(strcmp(current -> data -> username, username) == 0)
                        return current -> data -> sockfd ;

                 current = current -> next;
         }

         return -1;
}

void client_list_destroy(client_list *c_list)
{
        client_n *current = c_list -> head;
        client_n *next = 0;

        while(current!=0){
                next = current -> next;

                free(current -> data);
                free(current);

                current = next;
        }

        free(c_list);
}


int username_exist(client_list *c_list, client_t *him)
{
        if( him->username == NULL){
                return 0;
        }
        client_n *current = c_list -> head;

        while(current != 0)
        {
                if(strcmp(current -> data -> username, him -> username) == 0)
                        return 1;
                current = current -> next;
        }

        return 0;
}



