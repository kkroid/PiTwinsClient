#define USE_ZMQ 0
#if USE_ZMQ

#include "ZMQController.h"

#else

#include <Server.h>
#include <Snowflak.h>
#include "Client.h"
#include "utils.h"
#include <opencv2/opencv.hpp>

#define BASE_CAMERA_CMD R"({"tid":%lu,"type":0,"payload":{"cmd":%d,"param":%d}})"

#endif

using namespace std;


int main() {
#if USE_ZMQ
    auto *pZmqController = new ZMQController();
    // pZmqController->connect("tcp://192.168.3.100:5555", "tcp://192.168.3.100:5556");
    pZmqController->connect("tcp://127.0.0.1:15555", "tcp://127.0.0.1:15556");
#else
    Client::getInstance().init("127.0.0.1:15555", [](const evpp::TCPConnPtr &connPtr) {
        if (connPtr->IsConnected()) {
            spdlog::info("Server {} Connected", connPtr->remote_addr());
            Snowflake snowflake;
            std::string msg = format(BASE_CAMERA_CMD, snowflake.nextId(), 0, 0);
            connPtr->Send(msg);
        } else {
            spdlog::info("Server {} Disconnected", connPtr->remote_addr());
        }
    }, [](const evpp::TCPConnPtr &connPtr, evpp::Buffer *buffer) {
        spdlog::info("Client Received A Message");
        auto *ucharData = buffer->data();
        vector<unsigned char> data;
        for (int i = 0, n = buffer->length(); i < n; i++) {
            data.push_back(ucharData[i]);
        }
        cv::Mat matFrame = cv::imdecode(data, cv::IMREAD_COLOR);
        if (matFrame.empty() || matFrame.rows == 0) {
            matFrame.release();
            buffer->Reset();
            return;
        }
        spdlog::info("Client Received A Message:{}, {}, {}", buffer->length(), matFrame.rows, matFrame.cols);
        cv::waitKey(1);
        cv::imshow("PiTwinsClient", matFrame);
        matFrame.release();
        buffer->Reset();
        // spdlog::info("Client Received A Message:[{}]", buffer->ToString());
    });
    Client::getInstance().connect();
#endif
    return 0;
}