
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#define PORT 8080
#define BUFFER_SIZE 1024

DWORD WINAPI HandleReceive(LPVOID arg) {
    SOCKET sockfd = *(SOCKET *)arg;
    char buffer[BUFFER_SIZE];

    while (true) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = recv(sockfd, buffer, BUFFER_SIZE, 0);

        if (bytes_received <= 0) {
            printf("Server disconnected.\n");
            break;
        }

        printf("Received: %s\n", buffer);
    }

    return 0;
}

DWORD WINAPI HandleSend(LPVOID arg) {
    SOCKET sockfd = *(SOCKET *)arg;
    char buffer[BUFFER_SIZE];

    while (true) {
        memset(buffer, 0, BUFFER_SIZE);
        printf("Enter message: ");
        fgets(buffer, BUFFER_SIZE, stdin);

        send(sockfd, buffer, strlen(buffer), 0);
    }

    return 0;
}

int main() {
    WSADATA wsaData;
    SOCKET sockfd;
    struct sockaddr_in serverAddr;
    char name[20];

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed\n");
        return 1;
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == INVALID_SOCKET) {
        perror("socket");
        WSACleanup();
        return 1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        perror("connect");
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }

    printf("Enter your name: ");
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = '\0';
    send(sockfd, name, strlen(name), 0);

    printf("Connected to server as %s.\n", name);

    CreateThread(NULL, 0, HandleReceive, (LPVOID)&sockfd, 0, NULL);
    CreateThread(NULL, 0, HandleSend, (LPVOID)&sockfd, 0, NULL);

    while (true) {
        Sleep(1000);
    }

    closesocket(sockfd);
    WSACleanup();
    return 0;
}



