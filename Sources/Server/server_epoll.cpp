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
#include <ServerManip.h>
#define MAX_TRANSFER 4096
#define MAX_CLIENT   15
void throwError(const char* errorMessage){
    perror(errorMessage);
    exit(EXIT_FAILURE);
}
int main(){
    skt::Server server;
    while(1);
}

