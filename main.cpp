#define USE_ZMQ 0
#if USE_ZMQ

#include "ZMQController.h"

#else
#endif

using namespace std;


int main() {
#if USE_ZMQ
    auto *pZmqController = new ZMQController();
    // pZmqController->connect("tcp://192.168.3.100:5555", "tcp://192.168.3.100:5556");
    pZmqController->connect("tcp://127.0.0.1:15555", "tcp://127.0.0.1:15556");
#else

#endif
    return 0;
}