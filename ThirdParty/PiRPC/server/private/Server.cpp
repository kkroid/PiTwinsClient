//
// Created by Will Zhang on 2021/7/10.
//

#include "Server.h"

#include <utility>


void Server::init(const std::string& addr, const evpp::ConnectionCallback &ccb, const evpp::MessageCallback& mcb) {
    loop = new evpp::EventLoop();
    server = new evpp::TCPServer(loop, addr, "PiServer", DEFAULT_THREAD_NUM);
    server->SetConnectionCallback(ccb);
    server->SetMessageCallback(mcb);
}

void Server::run() {
    if (nullptr != server) {
        server->Init();
        server->Start();
        loop->Run();
    }
}
