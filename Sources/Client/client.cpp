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
#include <string.h>
#include <sys/epoll.h>
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

    std::string message = "HELLO server";
    if (connect(client_fd, (struct sockaddr*)&address, sizeof(address))< 0){
        throwError("COnnect failed\n");
    }
    int epFd = epoll_create1(0);
    size_t ready;
    struct epoll_event pollEvent[2],
                       rEvents[2];
    pollEvent[0].events  = EPOLLIN | EPOLLOUT;
    pollEvent[0].data.fd =  client_fd;
    epoll_ctl(epFd, EPOLL_CTL_ADD, client_fd, &pollEvent[0]);

    pollEvent[1].events  = EPOLLIN ;
    pollEvent[1].data.fd =  STDIN_FILENO;
    epoll_ctl(epFd, EPOLL_CTL_ADD, STDIN_FILENO, &pollEvent[1]);
    bool shouldStop = false;
    while(1){
        ready = epoll_wait(epFd,rEvents,1,-1);
        if (ready > 0){
            for (int i = 0 ; i< ready ; ++i){
                if (rEvents[i].events & EPOLLIN){
                    if (rEvents[i].data.fd == client_fd){
                        nByteTransferred = read(client_fd, buffer, MAX_TRANSFER);
                        if (nByteTransferred > 0){
                            buffer[nByteTransferred] = '\0';
                            printf("%s", buffer);
                        }else if (nByteTransferred == 0){
                            printf("Server Terminated\n");
                            shouldStop = true;
                        }

                    }else  if (rEvents[i].data.fd == STDIN_FILENO){
                        nByteTransferred = read(STDIN_FILENO, buffer, MAX_TRANSFER);
                        buffer[nByteTransferred] = '\0';
                        message.reserve(nByteTransferred);
                        message = buffer;
                    }
                }else{
                    if (message.size()){
                        send(client_fd,message.c_str(),message.size(), 0);
                        message = "";
                    }
                }
            }
        }
        if (shouldStop) break;
    }
    close(client_fd);
    return EXIT_SUCCESS;
}
