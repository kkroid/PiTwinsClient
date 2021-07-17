//
// Created by Will Zhang on 2021/7/10.
//

#include "Server.h"


void Server::run() {
    if (nullptr != server) {
        server->Init();
        server->Start();
        loop->Run();
    }
}

void Server::release() {
    spdlog::info("call {} stop", name);
    server->Stop([this]() {
        spdlog::info("{} stopped", name);
    });
    loop->Stop();
    delete loop;
    loop = nullptr;
    delete server;
    server = nullptr;
}

void Server::setConnectionCallback(const evpp::ConnectionCallback &ccb) {
    if (nullptr != server) {
        server->SetConnectionCallback([this, ccb](const evpp::TCPConnPtr &connPtr) {
            if (connPtr->IsConnected()) {
                clientCount++;
                spdlog::info("got a connection:{}, count:{}", connPtr->remote_addr(), clientCount);
                // if (tcpConnPtr == nullptr) {
                    tcpConnPtr = connPtr;
                    spdlog::info("save a connection:{}", connPtr->remote_addr());
                // }
            } else if (connPtr->IsDisconnected()) {
                spdlog::info("delete a connection:{}", connPtr->remote_addr());
                tcpConnPtr = nullptr;
                clientCount--;
            }
            if (ccb) {
                ccb(connPtr);
            }
        });
    }
}

void Server::setMessageCallback(const evpp::MessageCallback &mcb) {
    if (nullptr != server) {
        server->SetMessageCallback([this, mcb](const evpp::TCPConnPtr &connPtr, evpp::Buffer *buffer) {
            if (mcb) {
                mcb(connPtr, buffer);
            }
        });
    }
}
