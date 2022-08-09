#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<pthread.h>
#include<unistd.h>

int client_socket_list[SOMAXCONN];
int number_of_client = 0;

void* recvClient(void* data) {
    int client_socket = *(int*)data;

    struct sockaddr_in sin;
    socklen_t len = sizeof(sin);
	getsockname(client_socket, (struct sockaddr*)&sin, &len);
    char *client_ip = inet_ntoa(sin.sin_addr);
    int client_port = ntohs(sin.sin_port);

    char *client_add = (char*)malloc(11 + strlen(client_ip));
    sprintf(client_add, "(%s,%d):", client_ip, client_port);
    printf("Client %s connected\n", client_add);
    int iResult;
    do {
        char recvbuf[1024];
        iResult = recv(client_socket, recvbuf, 1024, 0);
        if (iResult > 0) {
            recvbuf[iResult] = 0;
            char message[1024];
            sprintf(message, "%s%s", client_add, recvbuf);
            for (int i = 0; i < number_of_client; ++i) {
                write(client_socket_list[i], message, (int)strlen(message));
            }

        } else if (iResult == 0) {
            printf("Client %s disconected\n", client_add);
            for (int i = 0; i < number_of_client; ++i) {
                struct sockaddr_in tmp_sin;
                socklen_t tmp_len = sizeof(tmp_sin);
                getsockname(client_socket_list[i], (struct sockaddr*)&tmp_sin, &tmp_len);
                char *tmp_client_ip = inet_ntoa(tmp_sin.sin_addr);
                int tmp_client_port = ntohs(tmp_sin.sin_port);
                if (strcmp(tmp_client_ip, client_ip) == 0 && client_port == tmp_client_port) {
                    client_socket_list[i] = client_socket_list[number_of_client - 1];
                    --number_of_client;
                    break;
                }
            }
        } else {
            printf("Client %s disconected\n", client_add);
            close(client_socket);
            for (int i = 0; i < number_of_client; ++i) {
                struct sockaddr_in tmp_sin;
                socklen_t tmp_len = sizeof(tmp_sin);
                getsockname(client_socket_list[i], (struct sockaddr*)&tmp_sin, &tmp_len);
                char *tmp_client_ip = inet_ntoa(tmp_sin.sin_addr);
                int tmp_client_port = ntohs(tmp_sin.sin_port);
                if (strcmp(tmp_client_ip, client_ip) == 0 && client_port == tmp_client_port) {
                    client_socket_list[i] = client_socket_list[number_of_client - 1];
                    --number_of_client;
                    break;
                }
            }
        }
    } while (iResult > 0);
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
            break;
        }
        client_socket_list[number_of_client++] = client_socket;

        pthread_t id;
        pthread_create(&id, NULL, recvClient, &client_socket);
    }
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Invalid argument\n");
        return 0;
    }

    int listen_socket;
    struct sockaddr_in server;
    
    //Create socket
    listen_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listen_socket == -1) {
        printf("Could not create socket\n");
        return 1;
    }
        
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    inet_pton(AF_INET, argv[1], &server.sin_addr);
    server.sin_port = htons(strtol(argv[2], NULL, 10));
        
    //Bind
    if(bind(listen_socket,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        printf("Bind failed\n");
        return 1;
    }

    //Listen
    listen(listen_socket, SOMAXCONN);

    printf("Server started\nType q or Q to stop server\n");

    pthread_t id;
    pthread_create(&id, NULL, acceptClient, &listen_socket);

    while (1) {
        char input_mess;
        scanf("%c", &input_mess);
        if (input_mess == 'q' || input_mess == 'Q') {
            close(listen_socket);
            printf("Server stopped\n");
            return 0;
        }
    }
}