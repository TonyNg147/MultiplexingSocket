#ifndef SERVER_MANIP_H
#define SERVER_MANIP_H
#include <SocketManip.h>
#include <thread>
#include <sys/epoll.h>
namespace skt{
    #define MAX_TRANSFER 4096
    #define MAX_CLIENT   15
    class Server: public skt::Socket{
        private:
            bool m_runningPool = true;
            char buffer[MAX_TRANSFER];
            size_t structLength = sizeof(s_addr);
            struct sockaddr_in addr;
            int epfd;
            size_t byteRead;
            size_t readys;
            const char* msgWelcome = "Hello from server";
                int client_fds[MAX_CLIENT];
            struct epoll_event client_events[MAX_CLIENT + 1], readyEvents[MAX_CLIENT + 1];
        private:
            void poll();
        public:
            explicit Server();
            void sendMsg() override final;
            void readMsg() override final;
            void pollingClients();
    };
};
#endif 