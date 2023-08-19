#include "ServerManip.h"
namespace skt{
    Server::Server(){
        Socket::init();
        addr.sin_family        = AF_INET;
        addr.sin_addr.s_addr   = INADDR_ANY;
        addr.sin_port          = htons(5000);
        int res = bind(fd, (struct sockaddr*)&addr, sizeof(addr));
        validate(res, SocketAction::BIND);
        validate(listen(fd, 15), SocketAction::LISTEN);
        pollingClients();
        std::thread pollThread([this](){
            poll();
        });
        for (int i = 0 ; i < MAX_CLIENT ; ++i){
            client_fds[MAX_CLIENT] = 0;
        }
        pollThread.detach();
    }
    void Server::pollingClients(){
        // Check if whether poll thread is started, then set running flag to true
        epfd = epoll_create1(0);
        client_events[0].events  = EPOLLIN;
        client_events[0].data.fd = fd;
        epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &client_events[0]);
    }
    void Server::poll(){
        while(m_runningPool){
            readys = epoll_wait(epfd, readyEvents, MAX_CLIENT, -1);
            for (int i =0 ;i < readys; ++i){
                if (readyEvents[i].data.fd == fd){
                    // If server Socket has ready, that means it has another connections
                    for (int i = 0 ; i < MAX_CLIENT ; ++i){
                        if (client_fds[i] == 0){
                            client_fds[i] = accept(fd, (struct sockaddr*)&addr, (socklen_t*)&structLength);
                            printf("Connect new client at %s with port %d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
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
        };
    }
    void Server::sendMsg(){

    };
    void Server::readMsg(){

    };
};