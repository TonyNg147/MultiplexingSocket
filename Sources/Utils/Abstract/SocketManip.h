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
#include <string>
#include <errno.h>
namespace skt{
    static constexpr const int INVALID_SOCKET = -1;
    static void handleError(const std::string& error){
        fprintf(stderr, "%s", error.c_str());
        printf("error no is %s\n",strerror(errno));
        exit(1);
    }
    
    static const std::string errorType[] = {
        "Failed to create socket\n",
        "Failed to bind infor to socket\n",
        "Failed to listen on socket\n",
        "Failed to accept\n",
    };
    enum class SocketAction{
        CREATE=0,
        BIND,
        LISTEN,
        ACCEPT
    };
    static bool validate(const int& fd, const SocketAction& action){
        if (action < SocketAction::CREATE || action > SocketAction::ACCEPT) return false;
        if (fd == INVALID_SOCKET)  handleError(errorType[static_cast<uint8_t>(action)]);
        return true;
    };
    typedef struct sockaddr_in s_addrin;
    typedef struct sockaddr     s_addr;
    class Socket{
        protected:
            int domain = AF_INET,
            type   = SOCK_STREAM;
            int fd;
            size_t fdCounts = 0;
            int** fdController, **iter;
            int** subSockets;
            // s_addrin addr;
            
        protected:
            inline void addFd(int* fd){
                iter = fdController;
                for (int i = 0 ; i < fdCounts; ++i){
                    ++iter;
                }
                (*iter) = fd;
                ++fdCounts;
            }
            /**
             * @brief Close session after the Socket Instance is destroyed
             *        This function is intended to be called in destructor
             * 
             */
            inline void closeSession(){
                int i = 0;
                int fdVal = -1;
                // Loop through each socket files which has been added to socket controller
                // If the val differs INVALID, then close it
                while(i < fdCounts){
                    if ((fdVal = **fdController) != INVALID_SOCKET){
                        close(fdVal);
                    }
                    ++i;
                }
            };
        public:
            /**
             * @brief Init the socket 
             * 
             */
            inline void init(){
                fd = socket(domain, type, 0);
                if (validate(fd,SocketAction::CREATE)){
                    // addFd(&fd);
                }
            }
            virtual void sendMsg() = 0;
            virtual void readMsg() = 0;
    };
    class SocketUtil{
        private:

    };
    class EpollUtil{

    };
};
#endif 