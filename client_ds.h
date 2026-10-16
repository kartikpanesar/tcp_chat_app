

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

typedef struct linked_list{
        client_n *head;
        client_n *tail;
} client_list;


int username_exist(client_list*, client_t*);

client_list *client_list_init();

int client_add(client_list *c_list, client_t* him);

int get_sockfd(client_list *c_list, char *username);



