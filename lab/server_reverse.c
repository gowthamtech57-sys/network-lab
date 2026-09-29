#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]) {
    int listen_sock, connection;
    char received[1024];
    struct sockaddr_in server, client;
    socklen_t address_len;

    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        return 1;
    }

    listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock < 0) {
        perror("socket");
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(atoi(argv[1]));

    if (bind(listen_sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("bind");
        return 1;
    }

    listen(listen_sock, 1);
    printf("Server waiting for client...\n");

    address_len = sizeof(client);
    connection = accept(listen_sock, (struct sockaddr *)&client, &address_len);
    if (connection < 0) {
        perror("accept");
        return 1;
    }
    printf("Client connected.\n");

    memset(received, 0, sizeof(received));
    int received_len = recv(connection, received, sizeof(received) - 1, 0);

    if (received_len > 0) {
        received[received_len] = '\0';
        // Displays the reversed string directly as received from the client
        printf("Reversed string received: %s\n", received);
    }

    close(connection);
    close(listen_sock);
    return 0;
}