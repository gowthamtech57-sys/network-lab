#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main(int argc, char *argv[])
{
    int client_sock;
    char message[1024];
    struct sockaddr_in server;

    if (argc != 3) {
        printf("Usage: %s <server_ip> <port>\n", argv[0]);
        return 1;
    }

    client_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (client_sock < 0) { perror("socket"); return 1; }

    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(argv[2]));
    inet_pton(AF_INET, argv[1], &server.sin_addr);

    if (connect(client_sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("connect");
        return 1;
    }

    printf("Connected to server.\n");
    printf("Enter a string: ");
    fgets(message, sizeof(message), stdin);
    message[strcspn(message, "\n")] = '\0';

    int size = strlen(message);
    int pos;
    for (pos = 0; pos < size / 2; pos++) {
        char swap = message[pos];
        message[pos] = message[size - pos - 1];
        message[size - pos - 1] = swap;
    }

    printf("Reversed string: %s\n", message);
    send(client_sock, message, strlen(message), 0);

    close(client_sock);
    return 0;
}
