#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <libgen.h>
#define BUF_SIZE 1024
int main(int argc, char *argv[])
{
int client_fd; // Client socket descriptor
char buffer[BUF_SIZE]; // Buffer for reading/sending data chunks
char file_path[256]; // Storage for the file path input
struct sockaddr_in server_addr; // Server address configurations
if (argc != 3)
{
printf("Usage: %s <server_ip> <port>\n", argv[0]);
return 1;
}
// Ask user for file location
printf("Enter path of file to send: ");
fgets(file_path, sizeof(file_path), stdin);
file_path[strcspn(file_path, "\n")] = '\0'; // Strip newline character
// Open target file
FILE *file_ptr = fopen(file_path, "rb");
if (file_ptr == NULL)
{
perror("fopen");
return 1;
}
// Create communication endpoint
client_fd = socket(AF_INET, SOCK_STREAM, 0);
if (client_fd < 0)
{
perror("socket");
return 1;
}
// Address configuration structure setups
server_addr.sin_family = AF_INET;
server_addr.sin_port = htons(atoi(argv[2]));
inet_pton(AF_INET, argv[1], &server_addr.sin_addr);
// Initialize server connection
if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
{
perror("connect");
return 1;
}
// ---- Step 1: Send the filename terminated by '\n' ----
char *base_name = basename(file_path);
char header_packet[300];
snprintf(header_packet, sizeof(header_packet), "%s\n", base_name);
send(client_fd, header_packet, strlen(header_packet), 0);
// ---- Step 2: Stream raw file content chunks ----
int bytes_read;
long total_bytes = 0;
while ((bytes_read = fread(buffer, 1, sizeof(buffer), file_ptr)) > 0)
{
send(client_fd, buffer, bytes_read, 0);
total_bytes += bytes_read;
}
fclose(file_ptr);
printf("Sent file '%s' (%ld bytes) to server\n", base_name, total_bytes);
close(client_fd);
return 0;
}