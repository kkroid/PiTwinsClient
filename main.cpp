#include "ZMQController.h"

using namespace std;


int main() {
    auto *pZmqController = new ZMQController();
    // pZmqController->connect("tcp://127.0.0.1:5555", "tcp://127.0.0.1:5556");
    pZmqController->connect("tcp://127.0.0.1:15555", "tcp://127.0.0.1:15556");
    return 0;
}