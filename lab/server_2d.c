#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX_ROWS 11
#define COLS 7
#define CRC_BITS 3
#define TOTAL_BITS (COLS + CRC_BITS)

const char DIVISOR[] = "1011";

// Modified: Verify if incoming frame contains a checksum error
int verifyCRC(const char *inputRow, const char *incomingCrc) {
    char temp[TOTAL_BITS + 1];
    int i, j;

    strcpy(temp, inputRow);
    strcat(temp, incomingCrc);

    for (i = 0; i <= TOTAL_BITS - 4; i++) {
        if (temp[i] == '1') {
            for (j = 0; j < 4; j++) {
                temp[i + j] = (temp[i + j] == DIVISOR[j]) ? '0' : '1';
            }
        }
    }

    for (i = COLS; i < TOTAL_BITS; i++) {
        if (temp[i] == '1') return 1; // Error found
    }
    return 0; // Pristine
}

int main(int argc, char *argv[])
{
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t len;
    char data[MAX_ROWS][COLS + 1];
    char crcMatrix[MAX_ROWS][CRC_BITS + 1]; // Replaced parity container arrays
    int rows;
    int i, j;
    int errorDetected = 0;

    if (argc != 2)
    {
        printf("Usage: %s <port>\n", argv[0]);
        return 1;
    }

    // -------------------------------
    // 1. Create TCP socket
    // -------------------------------
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        perror("socket");
        return 1;
    }

    // -------------------------------
    // 2. Server address
    // -------------------------------
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(atoi(argv[1]));

    // -------------------------------
    // 3. Bind socket
    // -------------------------------
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("bind");
        close(server_fd);
        return 1;
    }

    // -------------------------------
    // 4. Listen
    // -------------------------------
    listen(server_fd, 1);
    printf("TCP CRC Verification Server waiting for client...\n");

    // -------------------------------
    // 5. Accept ONE client
    // -------------------------------
    len = sizeof(client_addr);
    client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &len);
    if (client_fd < 0)
    {
        perror("accept");
        close(server_fd);
        return 1;
    }
    printf("Client connected.\n");

    // -------------------------------
    // 6. Receive number of rows
    // -------------------------------
    recv(client_fd, &rows, sizeof(rows), 0);

    // -------------------------------
    // 7. Receive binary data
    // -------------------------------
    recv(client_fd, data, sizeof(data), 0);

    // -------------------------------
    // 8. Receive CRC Matrix
    // -------------------------------
    recv(client_fd, crcMatrix, sizeof(crcMatrix), 0);

    printf("\n========== RECEIVED DATA ANALYSIS ==========\n");
    for (i = 0; i < rows; i++)
    {
        printf("Row %d: ", i + 1);
        for (j = 0; j < COLS; j++)
        {
            printf("%c ", data[i][j]);
        }

        // Replaced Parity Check with CRC validation
        int errorInRow = verifyCRC(data[i], crcMatrix[i]);
        if (errorInRow) {
            printf("| CRC Check: [ %s ] -> ❌ CORRUPTED!\n", crcMatrix[i]);
            errorDetected = 1;
        } else {
            printf("| CRC Check: [ %s ] ->  Valid\n", crcMatrix[i]);
        }
    }
    printf("--------------------------------------------\n");

    // -------------------------------
    // 12. Modified: Final Output Summary
    // -------------------------------
    if (errorDetected)
    {
        printf("\nRESULT: Data transmission corrupted. Packet rejected.\n");
    }
    else
    {
        printf("\nRESULT: No error detected. Data matched cleanly.\n");
    }

    close(client_fd);
    close(server_fd);
    return 0;
}