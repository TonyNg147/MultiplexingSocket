#include "ServerManip.h"
#define SERVER_FD_INDEX    (0)
#define SERVER_INPUT_INDEX (1)
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
        pollThread.detach();
    }
    void Server::pollingClients(){
        // Check if whether poll thread is started, then set running flag to true
        epfd = epoll_create1(0);     // Register new poll instance
        serverEvent[SERVER_FD_INDEX].events       = EPOLLIN;                          // Check when the fd is ready to be read
        serverEvent[SERVER_FD_INDEX].data.fd      = serverFd;
        epoll_ctl(epfd, EPOLL_CTL_ADD, serverFd, &serverEvent[SERVER_FD_INDEX]);      // Add listener to server fd
        serverEvent[SERVER_INPUT_INDEX].events  = EPOLLIN;
        serverEvent[SERVER_INPUT_INDEX].data.fd = STDIN_FILENO;
        epoll_ctl(epfd, EPOLL_CTL_ADD, STDIN_FILENO, &serverEvent[SERVER_INPUT_INDEX]);   // Add listener to input of server   
    }
    void Server::poll(){
        while(m_runningPool){
            // Return the number of ready file for registered events
            readys = epoll_wait(epfd, rEvents, MAX_CLIENT, -1);
            for (int i =0 ;i < readys; ++i){
                    // Check if the event is dedicated for READ
                    if(rEvents[i].events & EPOLLIN){
                        // If the EPOLLIN event is happened on the server fd, that means 
                        // the new connection has just established
                        if (rEvents[i].data.fd == serverFd){
                                newConnection = accept(fd, (struct sockaddr*)&addr, (socklen_t*)&structLength);
                                if (newConnection == INVALID_SOCKET){
                                    printf("Create new connection error %s\n", strerror(errno));
                                    exit(1);
                                }
                                printf("Connect new client at %s with port %d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
                                client_fds.emplace_back(newConnection, false);
                                clientSize = client_fds.size();
                                client_events[clientSize - 1].events  = EPOLLIN | EPOLLOUT;
                                client_events[clientSize - 1].data.fd = newConnection; 
                                epoll_ctl(epfd, EPOLL_CTL_ADD, newConnection, &client_events[clientSize - 1]);
                                msg = msgWelcome;
                                // send(newConnection, msgWelcome, strlen(msgWelcome), 0);
                        }else if (rEvents[i].data.fd == STDIN_FILENO){
                            byteRead = read(rEvents[i].data.fd, buffer, MAX_TRANSFER);
                            buffer[byteRead] = '\0';
                            msg = buffer;
                            resetClientState();
                        }else{
                            byteRead = read(rEvents[i].data.fd, buffer, MAX_TRANSFER);
                            if (byteRead == 0 ){
                                if ((lostConnectionIndex = findFdIndex(rEvents[i].data.fd)) != UINT8_MAX){
                                    printf("Lost connection\n");
                                    epoll_ctl(epfd, EPOLL_CTL_DEL, rEvents[i].data.fd, &client_events[lostConnectionIndex]);
                                    auto iter = client_fds.begin();
                                    std::advance(iter, lostConnectionIndex);
                                    client_fds.erase(iter);
                                    close(rEvents[i].data.fd);
                                }
                                
                            }else{
                                buffer[byteRead] = '\0';
                                // msg.reserve(MAX_TRANSFER);
                                // printf("Address is %ld", msg.c_str());
                                // char *message = const_cast<char*>(msg.c_str());
                                // printf("Address is %ld", message);
                                // sprintf(message,"[Client %d] %s",rEvents[i].data.fd,buffer);
                                printf("[Client %d] %s",rEvents[i].data.fd ,buffer);
                            }
                        }

                    }
                    else{
                        // EPOLLOUT event means that the fd has enough buffer to send data
                        if (rEvents[i].data.fd != STDIN_FILENO && rEvents[i].data.fd != serverFd){
                            if (!isSendToAllClients()){
                                int index  = findFdIndex(rEvents[i].data.fd);
                                if (!client_fds[index].sent){
                                    send(client_fds[index].fd,msg.c_str(), msg.size(), 0);
                                    client_fds[index].sent = true;
                                }
                            }else{
                                msg = "";
                            }
                        }
                    }
            }
        };
    }

    void Server::sendMsg(){

    };
    void Server::readMsg(){

    };

    bool Server::isConnectionExisted(const int& newFd){
        for (u_char idx = 0; idx < client_fds.size(); ++idx){
            if (client_fds[idx].fd == newFd) return true;
        }
        return false;
    }
    u_char Server::findFdIndex(const int& fd){
        for (u_char idx = 0; idx < client_fds.size(); ++idx){
            if (client_fds[idx].fd == fd) return idx;
        }
        return UINT8_MAX;
    }
    bool Server::isSendToAllClients(){
        bool status = true;
        for (u_char idx = 0; idx < client_fds.size(); ++idx){
            status&= client_fds[idx].sent;
        }
        return status;
    }
    void Server::resetClientState(){
        for (u_char idx = 0; idx < client_fds.size(); ++idx){
            client_fds[idx].sent = false;
        }
    }
};