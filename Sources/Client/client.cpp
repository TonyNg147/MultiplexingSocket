#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <arpa/inet.h>
#include <iostream>
#define MAX_TRANSFER 4096
void throwError(const char* errorMessage){
    perror(errorMessage);
    exit(EXIT_FAILURE);
}
int main(){
    ssize_t client_fd;
    ssize_t nByteTransferred;
    char buffer[MAX_TRANSFER];
    struct sockaddr_in address;
    size_t structLength = sizeof(address);
    if ((client_fd = socket(AF_INET,SOCK_STREAM, 0)) < 0){
        throwError("Init socket failed\n");
    }
    address.sin_family      =  AF_INET;
    address.sin_addr.s_addr = inet_addr("127.0.0.1");
    address.sin_port        =  htons(5000);

    if (connect(client_fd, (struct sockaddr*)&address, sizeof(address))< 0){
        throwError("COnnect failed\n");
    }
    const char* message = "Hello from the client";
    nByteTransferred = send(client_fd, message, strlen(message),  0);
    printf("Send to server %s\n", message);
    std::string msgServer;
    while(1){
        printf("[Client] Send to server: ");
        std::getline(std::cin, msgServer);
        send(client_fd, msgServer.c_str(), msgServer.size(),0);

    }
    // read(client_fd, buffer, MAX_TRANSFER);
    // printf("Received from server %s\n", buffer);
    close(client_fd);
    return EXIT_SUCCESS;
}
