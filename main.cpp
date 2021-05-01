#include "ZMQController.h"

using namespace std;


int main() {
    auto *pZmqController = new ZMQController();
    // pZmqController->connect("tcp://39.97.236.69:7003", "tcp://39.97.236.69:7004");
    // pZmqController->connect("tcp://cn8.frp.cool:11005", "tcp://cn8.frp.cool:11006");
    // pZmqController->connect("tcp://192.168.3.100:5555", "tcp://192.168.3.100:5556");
    pZmqController->connect("tcp://127.0.0.1:15555", "tcp://127.0.0.1:15556");
    return 0;
}