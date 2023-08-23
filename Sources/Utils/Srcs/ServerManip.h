#ifndef SERVER_MANIP_H
#define SERVER_MANIP_H
#include <SocketManip.h>
#include <thread>
#include <sys/epoll.h>
#include <vector>
namespace skt{
    #define MAX_TRANSFER 4096
    #define MAX_CLIENT   15
    /**
     * @brief This is a wrapper for server instance
     * 
     */
    class Server: public skt::Socket{
        private:
            int  &serverFd = fd;
            bool m_runningPool = true;
            char buffer[MAX_TRANSFER];
            size_t structLength = sizeof(s_addr);
            struct sockaddr_in addr;
            int epfd;
            size_t byteRead;
            size_t readys;
            std::string msg;
            const char* msgWelcome = "Hello from server";
                // int client_fds[MAX_CLIENT];
            struct TrackingClientFd{
                TrackingClientFd(const int& _fd, const bool& _sent)
                    :fd{_fd},sent{_sent}
                {

                }
                int fd;
                bool sent = false;
            };
            std::vector<TrackingClientFd> client_fds;
            struct epoll_event client_events[MAX_CLIENT + 1],
                               rEvents[MAX_CLIENT + 1],
                               serverEvent[2];                 // Server event will include the listener on the brand new connection and the input from server
        private:
            void poll();
            bool isConnectionExisted(const int& fd);
            u_char findFdIndex(const int& fd);
            int  newConnection;
            size_t clientSize;
            u_char  lostConnectionIndex;
            bool isSendToAllClients();
            void resetClientState();
        public:
            explicit Server();
            void sendMsg() override final;
            void readMsg() override final;
            void pollingClients();
    };
};
#endif 