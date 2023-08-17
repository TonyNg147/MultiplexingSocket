#ifndef SOCKET_UTIL_H
#define SOCKET_UTIL_H
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
#define VALID_SOCKET(fd) (fd != 0)
namespace socketUtil{
    class Server{
        private:
            int server_fd, client_fd[30], fdCount = 30;
        public:
            Server(){
                for (int i=0;i<fdCount;++i){
                    if (VALID_SOCKET(client_fd[i])){
                        close(server_fd);
                    }
                }
            }
            ~Server(){
                if (VALID_SOCKET(server_fd)){
                    close(server_fd);
                }
                for (int i=0;i<fdCount;++i){
                    if (VALID_SOCKET(client_fd[i])){
                        close(server_fd);
                    }
                }
            }
    };
};
#endif 