#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<pthread.h>

int client_socket_list[SOMAXCONN];
int number_of_client = 0;

void* recvClient(void* data) {
    int client_socket = *(int*)data;

    struct sockaddr_in sin;
    socklen_t len = sizeof(sin);
	getsockname(client_socket, (struct sockaddr*)&sin, &len);
    char *client_ip = inet_ntoa(sin.sin_addr);
    int client_port = ntohs(sin.sin_port);

    int iResult;
    char *client_add = (char*)malloc(11 + strlen(client_ip));
    sprintf(client_add, "(%s,%d):", client_ip, client_port);
    
}

void* acceptClient(void* data) {
    int listen_socket = *(int*) data;

    // Accept client sockets
    int client_socket;
    struct sockaddr_in client;
    int c = sizeof(client);

    while (1) {
        client_socket = accept(listen_socket, (struct sockaddr *)&client, (socklen_t*)&c);
        if (client_socket < 0) {
            printf("Accept failed\n");
            continue;
        }
        client_socket_list[++number_of_client] = client_socket;

        pthread_t id;
        pthread_create(&id, NULL, recvClient, &client_socket);
    }
}

int main(int argc, char** argv) {
    int listen_socket;
    struct sockaddr_in server;
    
    //Create socket
    listen_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listen_socket == -1) {
        printf("Could not create socket");
        return;
    }
        
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    inet_pton(AF_INET, argv[1], &server.sin_addr);
    server.sin_port = htons(argv[2]);
        
    //Bind
    if(bind(listen_socket,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        printf("Bind failed");
        return;
    }

    //Listen
    listen(listen_socket, SOMAXCONN);

    printf("Server started\n");

    pthread_t id;
    pthread_create(&id, NULL, acceptClient, &listen_socket);


}