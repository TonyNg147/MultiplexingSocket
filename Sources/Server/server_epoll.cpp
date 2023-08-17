#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/epoll.h>
#define MAX_TRANSFER 4096
#define MAX_CLIENT   15
void throwError(const char* errorMessage){
    perror(errorMessage);
    exit(EXIT_FAILURE);
}
int main(){
    ssize_t server_fd = 3, listen_fd;
    ssize_t nByteTransferred;
    char buffer[MAX_TRANSFER];
    struct sockaddr_in address;
    socklen_t structLength = sizeof(address);
    if ((server_fd = socket(AF_INET,SOCK_STREAM, 0)) < 0){
        throwError("Init socket failed\n");
    }
    printf("Server fd is %ld\n", server_fd);
    address.sin_family      =  AF_INET;
    address.sin_addr.s_addr =  INADDR_ANY;
    address.sin_port        =  htons(5000);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0){
        throwError("Bind failed\n");
    }
    if (listen(server_fd, 3) < 0){
        throwError("Listen error\n");
    }
    const char* msgWelcome = "Hello from server";
    int client_fds[MAX_CLIENT];

    for (int i = 0 ; i < MAX_CLIENT ; ++i){
        client_fds[MAX_CLIENT] = 0;
    }

    struct epoll_event client_events[MAX_CLIENT + 1], readyEvents[MAX_CLIENT + 1];

    int epfd = epoll_create1(0);

    client_events[0].events  = EPOLLIN;
    client_events[0].data.fd = server_fd;
    size_t byteRead;
    epoll_ctl(epfd, EPOLL_CTL_ADD, server_fd, &client_events[0]);
    size_t readys;
    while(1){
        readys = epoll_wait(epfd, readyEvents, MAX_CLIENT, -1);
        for (int i =0 ;i < readys; ++i){
            if (readyEvents[i].data.fd == server_fd){
                // If server Socket has ready, that means it has another connections
                for (int i = 0 ; i < MAX_CLIENT ; ++i){
                    if (client_fds[i] == 0){
                        client_fds[i] = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&structLength);
                        printf("Connect new client at %s with port %d\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));
                        client_events[1].events = EPOLLIN;
                        client_events[1].data.fd = client_fds[i];
                        epoll_ctl(epfd, EPOLL_CTL_ADD, client_fds[i], &client_events[1]);
                        send(client_fds[i], msgWelcome, strlen(msgWelcome), 0);
                        break;
                    }
                }
            }else{
                // If client fds have ready, that means the clients want to send messages
                byteRead = read(readyEvents[i].data.fd, buffer, MAX_TRANSFER);
                if (byteRead == 0 ){
                    printf("Lost connection\n");
                    epoll_ctl(epfd, EPOLL_CTL_DEL, readyEvents[i].data.fd, &client_events[1]);
                }else{
                    buffer[byteRead] = '\0';
                    printf("[Client] %s\n", buffer);
                }

            }
        }
    }
    // close()
    
}

