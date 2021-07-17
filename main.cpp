#define USE_ZMQ 0
#if USE_ZMQ

#include "ZMQController.h"

#else

#include <Server.h>
#include <Snowflak.h>
#include "Client.h"
#include "utils.h"
#include <opencv2/opencv.hpp>
#include <future>

#define BASE_CAMERA_CMD R"({"tid":%lu,"type":0,"payload":{"cmd":%d,"param":%d}})"

#endif

using namespace std;


int main() {
#if USE_ZMQ
    auto *pZmqController = new ZMQController();
    // pZmqController->connect("tcp://192.168.3.100:5555", "tcp://192.168.3.100:5556");
    pZmqController->connect("tcp://127.0.0.1:15555", "tcp://127.0.0.1:15556");
#else
    Client::getCMDClient().init([](const evpp::TCPConnPtr &connPtr) {
        if (connPtr->IsConnected()) {
            Snowflake snowflake;
            std::string msg = format(BASE_CAMERA_CMD, snowflake.nextId(), 0, 0);
            connPtr->Send(msg);
            sleep(1);
            spdlog::info("start to connect video server");
            cv::namedWindow("PiTwinsClient");
            Client::getVideoClient().init(nullptr, [](const evpp::TCPConnPtr &connPtr, evpp::Buffer *buffer) {
                auto *ucharData = buffer->data();
                vector<unsigned char> data;
                for (int i = 0, n = buffer->length(); i < n; i++) {
                    data.push_back(ucharData[i]);
                }
                cv::Mat matFrame = cv::imdecode(data, cv::IMREAD_COLOR);
                if (matFrame.empty() || matFrame.rows == 0) {
                    matFrame.release();
                    buffer->Reset();
                    spdlog::info("Invalid frame:{}, {}, {}, ignored", buffer->length(),
                                 matFrame.rows, matFrame.cols);
                    return;
                }
                // spdlog::info("Client Received A Frame:{}, {}, {}", buffer->length(),
                //              matFrame.rows, matFrame.cols);
                cv::imshow("PiTwinsClient", matFrame);
                cv::waitKey(1);
                matFrame.release();
                buffer->Reset();
            });
            Client::getVideoClient().connect();
        }
    }, [](const evpp::TCPConnPtr &connPtr, evpp::Buffer *buffer) {
        spdlog::info("Client Received A Message:[{}]", buffer->ToString());

    });
    Client::getCMDClient().connect();
#endif
    return 0;
}