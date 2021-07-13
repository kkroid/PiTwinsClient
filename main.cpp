#define USE_ZMQ 0
#if USE_ZMQ

#include "ZMQController.h"

#else

#include <Server.h>
#include "Client.h"
#include <future>

#endif

using namespace std;


int main() {
#if USE_ZMQ
    auto *pZmqController = new ZMQController();
    // pZmqController->connect("tcp://192.168.3.100:5555", "tcp://192.168.3.100:5556");
    pZmqController->connect("tcp://127.0.0.1:15555", "tcp://127.0.0.1:15556");
#else
    auto serverThread = async(launch::async, []() {
        Server::getInstance().init("0.0.0.0:5555", [](const evpp::TCPConnPtr &connPtr) {
            if (connPtr->IsConnected()) {
                spdlog::info("Client {} Connected", connPtr->remote_addr());
                connPtr->Send("Hello:" + connPtr->remote_addr());
            } else {
                spdlog::info("Client {} Disconnected", connPtr->remote_addr());
            }
        }, [](const evpp::TCPConnPtr &connPtr, evpp::Buffer *buffer) {
            spdlog::info("Server Received A Message:[{}]", buffer->ToString());
        });
        Server::getInstance().run();
    });

    auto clientThread = async(launch::async, []() {
        Client::getInstance().init("127.0.0.1:5555", [](const evpp::TCPConnPtr &connPtr) {
            if (connPtr->IsConnected()) {
                spdlog::info("Server {} Connected", connPtr->remote_addr());
                connPtr->Send("Hello:5555");
            } else {
                spdlog::info("Server {} Disconnected", connPtr->remote_addr());
            }
        }, [](const evpp::TCPConnPtr &connPtr, evpp::Buffer *buffer) {
            spdlog::info("Client Received A Message:[{}]", buffer->ToString());
        });
        Client::getInstance().connect();
    });
    serverThread.wait();
    clientThread.wait();
#endif
    return 0;
}