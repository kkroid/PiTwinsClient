//
// Created by Will Zhang on 2021/7/12.
//
#include "Client.h"

#include <utility>

void Client::init(std::string address,
                  std::string clientName,
                  const evpp::ConnectionCallback &ccb,
                  const evpp::MessageCallback &mcb) {
    addr = std::move(address);
    name = std::move(clientName);
    loop = new evpp::EventLoop();
    client = new evpp::TCPClient(loop, addr, name);
    setConnectionCallback(ccb);
    setMessageCallback(mcb);
}

void Client::setConnectionCallback(const evpp::ConnectionCallback &ccb) {
    if (client) {
        client->SetConnectionCallback([this, ccb](const evpp::TCPConnPtr &connPtr) {
            spdlog::info("{} connection status:{}", client->name(), connPtr->status());
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
