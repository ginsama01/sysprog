#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<pthread.h>
#include<unistd.h>

void* receiveData(void* data) {
    int connect_socket = *(int*)data;
    int iResult;
    do {
        char recvbuf[1024];
        iResult = read(connect_socket, recvbuf, 1024);
        if (iResult > 0) {
            recvbuf[iResult] = 0;
            printf("%s", recvbuf);

        } else if (iResult == 0) {
            printf("Closed connection!\n");
        } else {
            printf("Closed connection!\n");
            close(connect_socket);
        }
    } while (iResult > 0);
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Invalid argument\n");
        return 0;
    }
    int connect_socket;
    struct sockaddr_in server;
    
    //Create socket
    connect_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (connect_socket == -1) {
        printf("Could not create socket\n");
        return 1;
    }
        
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    inet_pton(AF_INET, argv[1], &server.sin_addr);
    server.sin_port = htons(strtol(argv[2], NULL, 10));
        
    //Connect to remote server
	if (connect(connect_socket , (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		printf("Connect error\n");
		return 1;
	}

    printf("Connected to server.\nType message begin with q to close connection.\nType message begin with m to send message (example mTestmessage)\n");
    pthread_t id;
    pthread_create(&id, NULL, receiveData, &connect_socket);

    while (1) {
        char type_mess;
        scanf("%c", &type_mess);
        if (type_mess == 'q') {
            printf("Client closed\n");
            close(connect_socket);
            return 1;
        } else if (type_mess == 'm') {
            char input_mess[1024];
            fgets(input_mess, 1024, stdin);
            send(connect_socket, input_mess, (int)strlen(input_mess), 0);
        } else {
            char input_mess[1024];
            fgets(input_mess, 1024, stdin);
            printf("Invalid message\n");
        }
    }
}