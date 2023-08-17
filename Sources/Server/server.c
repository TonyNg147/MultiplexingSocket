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
#define MAX_TRANSFER 4096
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
    int client_sockets[30], maximum_connections=30, fd, nSelect;
    int id=0;
    fd_set readfds;
    for (id=0;id<maximum_connections;++id){
        client_sockets[id]=0;
    }
    while(1){
        FD_ZERO(&readfds);
        FD_SET(server_fd,&readfds);
        maximum_connections = server_fd;
        for (id=0;id<maximum_connections;++id){
            if(client_sockets[id] > 0) FD_SET(client_sockets[id],&readfds);
            if (maximum_connections < client_sockets[id]) maximum_connections = client_sockets[id];
        } 
		printf("heer 1\n");
        nSelect = select(maximum_connections + 1,&readfds,NULL,NULL,NULL);
		printf("heer 2\n");
        if (FD_ISSET(server_fd,&readfds)){
            if ((fd = accept(server_fd, (struct sockaddr*)&address,(socklen_t*)&structLength)) < 0){
                throwError("Accept error\n");
            }
            printf("Connected with address %s at port %d\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));
            for (id=0;id<maximum_connections;++id){
                if(client_sockets[id] == 0) {
                    client_sockets[id]  = fd;
                    send(fd,msgWelcome,strlen(msgWelcome),0);
                }
            } 
        }
        for (id=0;id<maximum_connections;++id){
            fd = client_sockets[id];
            if (FD_ISSET(fd,&readfds)){
                if ((nByteTransferred = read(fd,buffer,4096)) == 0){
                    printf("Lost connection in %s port %d\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));
                    client_sockets[id] = 0;
                    close(fd);
                }else{
                    printf("Message: %s\n", buffer);
                }
            }
        } 
    }
}

