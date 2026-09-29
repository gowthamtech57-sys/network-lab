#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#define port 8081
#define buffer_size 1024
int main(int argc, char *argv[])
{
    int server_fd, client_fd;
    char buffer[1024];

    struct sockaddr_in server_addr, client_addr;
    socklen_t len;

    if (argc != 2)
    {
        printf("Usage: %s <port>\n", argv[0]);
        return 1;
    }

    // 1. Create TCP socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd < 0)
    {
        perror("socket");
        return 1;
    }

    // 2. Specify server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(atoi(argv[1]));

    // 3. Bind socket
    if (bind(server_fd,
             (struct sockaddr *)&server_addr,
             sizeof(server_addr)) < 0)
    {
        perror("bind");
        return 1;
    }

    // 4. Listen for one client
    listen(server_fd, 1);

    printf("TCP Echo Server waiting for client...\n");

    // 5. Accept one client
    len = sizeof(client_addr);

    client_fd = accept(server_fd,
                       (struct sockaddr *)&client_addr,
                       &len);

    if (client_fd < 0)
    {
        perror("accept");
        return 1;
    }

    printf("Client connected.\n");

    // 6. Receive message
    int n = recv(client_fd,
                 buffer,
                 sizeof(buffer) - 1,
                 0);

    if (n > 0)
    {
        buffer[n] = '\0';

        printf("Message from client: %s\n", buffer);

        // 7. Echo message back to client
        send(client_fd, buffer, n, 0);
    }

    // 8. Close client socket
    close(client_fd);

    // 9. Close server socket
    close(server_fd);

    return 0;
}
