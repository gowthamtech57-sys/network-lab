#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX_ROWS 10
#define COLS 7
#define CRC_BITS 3
#define TOTAL_BITS (COLS + CRC_BITS)

// CRC-4 Generator Polynomial: x^3 + x + 1 -> "1011"
const char DIVISOR[] = "1011";

// Modified: Helper function to calculate CRC remainder via modulo-2 division
void calculateCRC(const char *inputRow, char *crcOut) {
    char temp[TOTAL_BITS + 1];
    int i, j;

    strcpy(temp, inputRow);
    for (i = COLS; i < TOTAL_BITS; i++) {
        temp[i] = '0';
    }
    temp[TOTAL_BITS] = '\0';

    for (i = 0; i <= TOTAL_BITS - 4; i++) {
        if (temp[i] == '1') {
            for (j = 0; j < 4; j++) {
                temp[i + j] = (temp[i + j] == DIVISOR[j]) ? '0' : '1';
            }
        }
    }
    strncpy(crcOut, temp + COLS, CRC_BITS);
    crcOut[CRC_BITS] = '\0';
}

int main(int argc, char *argv[])
{
    int sockfd;
    struct sockaddr_in server_addr;
    char data[MAX_ROWS][COLS + 1];
    char crcMatrix[MAX_ROWS][CRC_BITS + 1]; // Replaced: Stores CRC bits instead of rowParity
    int rows;
    int i, j;

    if (argc != 3)
    {
        printf("Usage: %s <server_ip> <port>\n", argv[0]);
        return 1;
    }

    // -------------------------------
    // 1. Create TCP socket
    // -------------------------------
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("socket");
        return 1;
    }

    // -------------------------------
    // 2. Server address
    // -------------------------------
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[2]));
    inet_pton(AF_INET, argv[1], &server_addr.sin_addr);

    // -------------------------------
    // 3. Connect to server
    // -------------------------------
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("connect");
        close(sockfd);
        return 1;
    }
    printf("Connected to server.\n\n");

    // -------------------------------
    // 4. Get number of rows
    // -------------------------------
    printf("Enter number of 7-bit binary rows: ");
    scanf("%d", &rows);
    if (rows <= 0 || rows > MAX_ROWS)
    {
        printf("Invalid number of rows.\n");
        close(sockfd);
        return 1;
    }

    // -------------------------------
    // 5. Get binary input
    // -------------------------------
    for (i = 0; i < rows; i++)
    {
        while (1)
        {
            printf("Enter Row %d (7 bits): ", i + 1);
            scanf("%7s", data[i]);

            if (strlen(data[i]) != 7)
            {
                printf("Please enter exactly 7 bits.\n");
                continue;
            }

            int valid = 1;
            for (j = 0; j < COLS; j++)
            {
                if (data[i][j] != '0' && data[i][j] != '1')
                {
                    valid = 0;
                    break;
                }
            }

            if (!valid)
            {
                printf("Only 0 and 1 are allowed.\n");
                continue;
            }
            break;
        }
    }

    // -------------------------------
    // 6. Modified: Calculate CRC for each row
    // -------------------------------
    for (i = 0; i < rows; i++)
    {
        calculateCRC(data[i], crcMatrix[i]);
    }

    // -------------------------------
    // 8. Display CRC Matrix Data
    // -------------------------------
    printf("\n========== CRC MATRIX ==========\n");
    for (i = 0; i < rows; i++)
    {
        for (j = 0; j < COLS; j++)
        {
            printf("%c ", data[i][j]);
        }
        printf("| CRC: %s\n", crcMatrix[i]);
    }

    // -------------------------------
    // 9. Send number of rows
    // -------------------------------
    send(sockfd, &rows, sizeof(rows), 0);

    // -------------------------------
    // 10. Send binary data
    // -------------------------------
    send(sockfd, data, sizeof(data), 0);

    // -------------------------------
    // 11. Send CRC values (Replaced row/col parity chunks)
    // -------------------------------
    send(sockfd, crcMatrix, sizeof(crcMatrix), 0);
    printf("\nCRC matrix data sent to server.\n");

    // -------------------------------
    // 13. Close socket
    // -------------------------------
    close(sockfd);
    return 0;
}