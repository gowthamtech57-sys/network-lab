#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#define BUF_SIZE 1024
int main(int argc, char *argv[])
{
int listen_fd, client_fd; // Server listener and active client descriptor
char buffer[BUF_SIZE]; // Main stream receive buffer
struct sockaddr_in server_addr, client_addr; // Server/client addresses
socklen_t addr_len; // Holds size of address storage length
if (argc != 2)
{
printf("Usage: %s <port>\n", argv[0]);
return 1;
}
// Initialize TCP stream hub
listen_fd = socket(AF_INET, SOCK_STREAM, 0);
if (listen_fd < 0)
{
perror("socket");
return 1;
}
server_addr.sin_family = AF_INET;
server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
server_addr.sin_port = htons(atoi(argv[1]));
if (bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
{
perror("bind");
return 1;
}
listen(listen_fd, 5);
printf("File transfer server running on port %s...\n", argv[1]);
while (1)
{
addr_len = sizeof(client_addr);
client_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &addr_len);
if (client_fd < 0)
{
perror("accept");
continue;
}
// Fork sub-process to accommodate specific connection
if (fork() == 0)
{
close(listen_fd); // Child drops master loop handler
// ---- Step 1: Read filename up to the newline character ----
char file_name[256];
int index = 0;
char read_char;
while (recv(client_fd, &read_char, 1, 0) > 0 && read_char != '\n' && index < 255)
file_name[index++] = read_char;
file_name[index] = '\0';
char save_path[300];
snprintf(save_path, sizeof(save_path), "received_%s", file_name);
FILE *file_ptr = fopen(save_path, "wb");
if (file_ptr == NULL)
{
perror("fopen");
close(client_fd);
exit(1);
}
// ---- Step 2: Accumulate stream data blocks into target file ----
int bytes_received;
long total_bytes = 0;
while ((bytes_received = recv(client_fd, buffer, sizeof(buffer), 0)) > 0)
{
fwrite(buffer, 1, bytes_received, file_ptr);
total_bytes += bytes_received;
}
fclose(file_ptr);
printf("Received file '%s' (%ld bytes) -> saved as '%s'\n", file_name, total_bytes, save_path);
close(client_fd);
exit(0);
}
close(client_fd); // Parent closes standard link tracking context
}
close(listen_fd);
return 0;
}
