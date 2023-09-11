#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#define PORT 8080

char *arr_to_str(int *arr)
{
    char *str = malloc(sizeof(char) * 1024);
    for (int i = 0; i < 10; i++)
    {
        char *temp = malloc(sizeof(char) * 1024);
        snprintf(temp, sizeof(temp), "%d", arr[i]);
        strcat(str, temp);
        strcat(str, " ");
    }
    return str;
}

int main(int argc, char const *argv[])
{
    int status, valread, client_fd;
    struct sockaddr_in serv_addr;
    int arr[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    char buffer[1024] = {0};
    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary
    // form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        printf(
            "\nInvalid address/ Address not supported \n");
        return -1;
    }

    if ((status = connect(client_fd, (struct sockaddr *)&serv_addr,
                          sizeof(serv_addr))) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }
    printf("connected to server...\n");
    char *str = arr_to_str(arr);
    send(client_fd, str, strlen(str), 0);
    valread = read(client_fd, buffer, 1024);
    printf("The sum is: %s\n", buffer);

    // closing the connected socket
    close(client_fd);
    return 0;
}
