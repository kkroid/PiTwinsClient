//
// Created by Will Zhang on 2021/7/12.
//
#include "Client.h"

void Client::init(const char *address, char *name, const evpp::ConnectionCallback &ccb, const evpp::MessageCallback &mcb) {
    addr = address;
    loop = new evpp::EventLoop();
    client = new evpp::TCPClient(loop, addr, name);
    setConnectionCallback(ccb);
    setMessageCallback(mcb);
}

void Client::setConnectionCallback(const evpp::ConnectionCallback &ccb) {
    if (client) {
        client->SetConnectionCallback([this, ccb](const evpp::TCPConnPtr &connPtr) {
            if (connPtr->IsConnected()) {
                spdlog::info("{} connected to {}", client->name(), connPtr->remote_addr());
            } else if (connPtr->IsDisconnected()) {
                spdlog::info("{} disconnected to {}", client->name(), connPtr->remote_addr());
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
    if (client) {
        client->conn()->Close();
        client->Disconnect();
        loop->Stop();
    }
}
