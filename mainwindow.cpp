#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "Server.h"
#include "Snowflak.h"
#include "Client.h"
#include "utils.h"
#include <opencv2/opencv.hpp>
#include <future>

#define BASE_CAMERA_CMD R"({"tid":%lu,"type":0,"payload":{"cmd":%d,"param":%d}})"

using namespace std;


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->cmdAddr->setText("127.0.0.1:15556");
    ui->videoAddr->setText("127.0.0.1:15555");
    threadPool = new ctpl::thread_pool(2);
    spdlog::info("MainWindow created");
}

MainWindow::~MainWindow()
{
    delete ui;
    threadPool->clear_queue();
    delete threadPool;
}

void MainWindow::on_connect_clicked() {
    auto cmdServer = ui->cmdAddr->text().toUtf8();
    auto videoServer = ui->videoAddr->text().toUtf8();
    ui->cmdAddr->clearFocus();
    ui->videoAddr->clearFocus();
    ui->Frame->setFocus();
    spdlog::info("Connect button clicked, cmdServer = {}, videoServer = {}", cmdServer, videoServer);

    threadPool->push([this, cmdServer, videoServer](int id) {
        spdlog::info("start to launch cmd server");
        Client::getCMDClient().init(cmdServer, "cmd server", [this, videoServer](const evpp::TCPConnPtr &connPtr) {
            spdlog::info("cmd server connection status:{}", connPtr->status());
            if (connPtr->IsConnected()) {
                Snowflake snowflake;
                std::string msg = format(BASE_CAMERA_CMD, snowflake.nextId(), 0, 0);
                connPtr->Send(msg);
                sleep(1);
                threadPool->push([this, videoServer](int id) {
                    spdlog::info("start to launch video server");
                    Client::getVideoClient().init(videoServer, "video server", [](const evpp::TCPConnPtr &connPtr) {
                        spdlog::info("video server connection status:{}", connPtr->status());
                    }, [this](const evpp::TCPConnPtr &connPtr, evpp::Buffer *buffer) {
                        auto *ucharData = buffer->data();
                        vector<unsigned char> data;
                        for (int i = 0, n = buffer->length(); i < n; i++) {
                            data.push_back(ucharData[i]);
                        }
                        cv::Mat matFrame = cv::imdecode(data, cv::IMREAD_UNCHANGED);
                        if (matFrame.empty() || matFrame.rows == 0) {
                            spdlog::info("Invalid frame:{}, {}, {}, ignored", buffer->capacity(), matFrame.rows, matFrame.cols);
                            matFrame.release();
                            buffer->Reset();
                            return;
                        }
                        spdlog::info("Valid frame:{}, {}, {}", buffer->capacity(), matFrame.rows, matFrame.cols);

                        QImage img = QImage((const uchar*)(matFrame.data),  //(const unsigned char*)
                                             matFrame.cols,matFrame.rows,
                                             matFrame.cols*matFrame.channels(),   //new add
                                             QImage::Format_RGB888);
                        ui->Frame->setPixmap(QPixmap::fromImage(img));
                        ui->Frame->resize(ui->Frame->pixmap()->size());
                        ui->Frame->show();
                        matFrame.release();
                        buffer->Reset();
                    });
                    Client::getVideoClient().connect();
                });
            }
        }, [](const evpp::TCPConnPtr &connPtr, evpp::Buffer *buffer) {
            spdlog::info("Client Received A Message:[{}]", buffer->ToString());
        });
        Client::getCMDClient().connect();
    });
}

void MainWindow::keyPressEvent(QKeyEvent *keyEvent) {
    spdlog::info("{} key pressed", keyEvent->text().toUtf8());
}

void MainWindow::closeEvent(QCloseEvent *event) {
    spdlog::info("window onClose called");
    Client::getCMDClient().disconnect();
    Client::getVideoClient().disconnect();
}
