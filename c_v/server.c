#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 8080
#define BUFFER_SIZE 1024
#define XOR_KEY 0xAA

void xorCipher(char *data)
{
    for (int i = 0; data[i] != '\0'; i++)
        data[i] ^= XOR_KEY;
}

int main()
{
    WSADATA wsa;
    SOCKET server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE];

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        printf("WSAStartup failed\n");
        return 1;
    }

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
    {
        printf("Socket creation failed\n");
        WSACleanup();
        return 1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == SOCKET_ERROR)
    {
        printf("Bind failed\n");
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    listen(server_fd, 3);
    printf("Server listening on port %d...\n", PORT);

    while (1)
    {
        new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen);
        if (new_socket == INVALID_SOCKET)
        {
            printf("Accept failed\n");
            continue;
        }

        printf("Client connected.\n");

        while (1)
        {
            memset(buffer, 0, BUFFER_SIZE);
            int valread = recv(new_socket, buffer, BUFFER_SIZE, 0);
            if (valread <= 0)
            {
                printf("Client disconnected.\n");
                closesocket(new_socket);
                break;
            }

            xorCipher(buffer);

            if (strcmp(buffer, "q") == 0 || strcmp(buffer, "Q") == 0)
            {
                printf("Client requested termination.\n");
                closesocket(new_socket);
                break;
            }

            printf("Client: %s\n", buffer);

            char msg[BUFFER_SIZE];
            printf("Server: ");
            fgets(msg, BUFFER_SIZE, stdin);
            msg[strcspn(msg, "\n")] = '\0';

            if (strcmp(msg, "q") == 0 || strcmp(msg, "Q") == 0)
            {
                xorCipher(msg);
                send(new_socket, msg, strlen(msg), 0);
                printf("Server exiting chat.\n");
                closesocket(new_socket);
                break;
            }

            xorCipher(msg);
            send(new_socket, msg, strlen(msg), 0);
        }
    }

    closesocket(server_fd);
    WSACleanup();
    return 0;
}
