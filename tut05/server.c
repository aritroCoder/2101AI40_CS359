#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#define PORT 8080

int countVowels(char* c){
    int count = 0;
    // printf("%s\n", c);
    int i = 0;
    while (c[i] != '\0')
    {
        if (c[i] == 'a' || c[i] == 'A' || c[i] == 'e' || c[i] == 'E' || c[i] == 'i' || c[i] == 'I' || c[i] == 'o' || c[i] == 'O' || c[i] == 'u' || c[i] == 'U')
            count++;
        c++;
    }
    return count;
}

int isPallindrome(char* c){
    int i=0, j=strlen(c)-1;
    while(i<j){
        if(c[i]!=c[j]){
            return 0;
        }
        i++;
        j--;
    }
    return 1;
}

char* add(char* c){
    // split the char by spaces
    // convert each char to int
    // add them
    // convert back to char
    // return
    char* sum = malloc(sizeof(char)*1024);
    char* token = strtok(c, " ");
    int total = 0;
    while(token!=NULL){
        total += atoi(token);
        token = strtok(NULL, " ");
    }
    snprintf(sum, sizeof(sum), "%d", total);
    return sum;
}

int main(int argc, char const *argv[])
{
    int server_fd, new_socket, sock02, sock03, sock04, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *hello = "Have a good day";

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET,
                   SO_REUSEADDR | SO_REUSEPORT, &opt,
                   sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address,
             sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // QUESTION 1
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                             (socklen_t *)&addrlen)) < 0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    valread = read(new_socket, buffer, 1024);
    printf("%s\n", buffer);
    send(new_socket, hello, strlen(hello), 0);
    close(new_socket);
    memset(buffer, '\0', sizeof(buffer));

    printf("-------------------------------------------------\n");

    // QUESTION 2
    if ((sock02 = accept(server_fd, (struct sockaddr *)&address,
                             (socklen_t *)&addrlen)) < 0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    valread = read(sock02, buffer, 1024);
    printf("Recieved: %s\n", buffer);
    char *vowelCount = malloc(sizeof(char) * 1024);
    printf("number of chars: %ld\n", strlen(buffer));
    int vc = countVowels(buffer);
    snprintf(vowelCount, sizeof(vowelCount), "%d", vc);
    printf("number of vowels is: %s\n", vowelCount);
    send(sock02, vowelCount, sizeof(vowelCount), 0);
    close(sock02);
    memset(buffer, '\0', sizeof(buffer));

    printf("-------------------------------------------------\n");

    // QUESTION 3
    if ((sock03 = accept(server_fd, (struct sockaddr *)&address,
                             (socklen_t *)&addrlen)) < 0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    valread = read(sock03, buffer, 1024);
    printf("Recieved: %s\n", buffer);
    if(isPallindrome(buffer)){
        send(sock03, "String is pallindrome", strlen("String is pallindrome"), 0);
    }else{
        send(sock03, "String is not pallindrome", strlen("String is not pallindrome"), 0);
    }
    close(sock03);
    memset(buffer, '\0', sizeof(buffer));

    printf("-------------------------------------------------\n");

    // QUESTION 4
    if ((sock04 = accept(server_fd, (struct sockaddr *)&address,
                             (socklen_t *)&addrlen)) < 0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    valread = read(sock04, buffer, 1024);
    printf("Recieved Array: %s\n", buffer);
    char* sum = add(buffer);
    send(sock04, sum, strlen(sum), 0);
    close(sock04);
    memset(buffer, '\0', sizeof(buffer));

    // closing the listening socket
    shutdown(server_fd, SHUT_RDWR);
    return 0;
}
