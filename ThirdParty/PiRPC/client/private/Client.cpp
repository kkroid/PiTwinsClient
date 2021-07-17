//
// Created by Will Zhang on 2021/7/12.
//
#include "Client.h"

void Client::init(const evpp::ConnectionCallback &ccb, const evpp::MessageCallback &mcb) {
    loop = new evpp::EventLoop();
    client = new evpp::TCPClient(loop, addr, "PiClient");
    setConnectionCallback(ccb);
    setMessageCallback(mcb);
}

void Client::setConnectionCallback(const evpp::ConnectionCallback &ccb) {
    if (client) {
        client->SetConnectionCallback([ccb](const evpp::TCPConnPtr &connPtr) {
            if (connPtr->IsConnected()) {
                spdlog::info("connected to {}", connPtr->remote_addr());
            } else if (connPtr->IsDisconnected()) {
                spdlog::info("disconnected to {}", connPtr->remote_addr());
            }
            if (ccb) {
                ccb(connPtr);
            }
        });
    }
}

void Client::setMessageCallback(const evpp::MessageCallback &mcb) {
    if (client) {
        client->SetMessageCallback([mcb](const evpp::TCPConnPtr &connPtr, evpp::Buffer *buffer) {
            if (mcb) {
                mcb(connPtr, buffer);
            }
        });
    }
}

void Client::connect() {
    client->Connect();
    client->set_auto_reconnect(true);
    loop->Run();
}

void Client::disconnect() {
    client->Disconnect();
}
