#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main(int argc, char *argv[])
{
    int sockfd;
    int n;
    char buffer[1024];

    struct sockaddr_in server_addr;

    if (argc != 3)
    {
        printf("Usage: %s <server_ip> <port>\n", argv[0]);
        return 1;
    }

    // 1. Create TCP socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0)
    {
        perror("socket");
        return 1;
    }

    // 2. Specify server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[2]));

    inet_pton(AF_INET,
              argv[1],
              &server_addr.sin_addr);

    // 3. Connect to server
    if (connect(sockfd,
                (struct sockaddr *)&server_addr,
                sizeof(server_addr)) < 0)
    {
        perror("connect");
        return 1;
    }

    printf("Connected to server.\n");

    // 4. Get message from user
    printf("Enter a message: ");

    fgets(buffer, sizeof(buffer), stdin);

    // Remove newline
    buffer[strcspn(buffer, "\n")] = '\0';

    // 5. Send message to server
    send(sockfd,
         buffer,
         strlen(buffer),
         0);

    // 6. Receive echo from server
    n = recv(sockfd,
             buffer,
             sizeof(buffer) - 1,
             0);

    if (n > 0)
    {
        buffer[n] = '\0';

        printf("Echo from server: %s\n", buffer);
    }

    // 7. Close socket
    close(sockfd);

    return 0;
}
[24bcs169@mepcolinux exp5]$