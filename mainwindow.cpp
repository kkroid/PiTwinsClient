#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "Snowflak.h"
#include "Client.h"
#include "utils.h"
#include <opencv2/opencv.hpp>
#include "json.hpp"

using namespace std;


MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    ui->cmdAddr->setText("127.0.0.1:15556");
    ui->videoAddr->setText("127.0.0.1:15555");
    threadPool = new ctpl::thread_pool(2);
    spdlog::info("MainWindow created");
}

MainWindow::~MainWindow() {
    delete ui;
    threadPool->clear_queue();
    delete threadPool;
}

void MainWindow::on_connectBtn_clicked() {
    auto msgServer = ui->cmdAddr->text().toStdString();
    ui->cmdAddr->clearFocus();
    ui->videoAddr->clearFocus();
    ui->Frame->setFocus();
    spdlog::info("Connect button clicked, msgServer = {}", msgServer);

    threadPool->push([this, msgServer](int id) {
        spdlog::info("start to launch cmd server");
        Client::getMsgClient().init(msgServer, "cmd server", [this](const evpp::TCPConnPtr &connPtr) {
            onMsgServerConnectionChanged(connPtr->status());
        }, [this](const evpp::TCPConnPtr &connPtr, evpp::Buffer *buffer) {
            onMsgReceived(buffer->ToString());
        });
        Client::getMsgClient().connect();
    });
}

void MainWindow::keyPressEvent(QKeyEvent *keyEvent) {
    spdlog::info("{} key pressed", keyEvent->text().toUtf8());
}

void MainWindow::closeEvent(QCloseEvent *event) {
    spdlog::info("window onClose called");
    Client::getMsgClient().disconnect();
    Client::getVideoClient().disconnect();
}

void MainWindow::onMsgServerConnectionChanged(int status) {
    if (status == 2) {
        std::string msg = MessageGenerator::gen(0, MessageGenerator::format(R"({"cmd":%d,"param":%d})", 0, 0));
        Client::getMsgClient().Send(msg);
        QPalette pe;
        pe.setColor(QPalette::WindowText, Qt::green);
        ui->cmdLabel->setPalette(pe);
    } else {
        QPalette pe;
        pe.setColor(QPalette::WindowText, Qt::black);
        ui->cmdLabel->setPalette(pe);
    }
}

void MainWindow::onVideoServerConnectionChanged(int status) {
    if (status == 2) {
        QPalette pe;
        pe.setColor(QPalette::WindowText, Qt::green);
        ui->videoLabel->setPalette(pe);
    } else {
        QPalette pe;
        pe.setColor(QPalette::WindowText, Qt::black);
        ui->videoLabel->setPalette(pe);
    }
}

void MainWindow::onMsgReceived(const std::string &cmd) {
    spdlog::info("Client Received A Message:[{}]", cmd);
    nlohmann::json obj = nlohmann::json::parse(cmd);
    int type = obj["type"];
    if (type == TYPE_CAMERA_OPENED) {
        auto videoServer = ui->videoAddr->text().toStdString();
        threadPool->push([this, videoServer](int id) {
            spdlog::info("start to launch video server");
            Client::getVideoClient().init(videoServer, "video server", [this](const evpp::TCPConnPtr &connPtr) {
                onVideoServerConnectionChanged(connPtr->status());
            }, [this](const evpp::TCPConnPtr &connPtr, evpp::Buffer *buffer) {
                auto *ucharData = buffer->data();
                vector<unsigned char> data(ucharData, ucharData + buffer->size());
                onVideoFrameReceived(data);
                buffer->Reset();
            });
            Client::getVideoClient().connect();
        });
    }
}

void MainWindow::onVideoFrameReceived(const std::vector<unsigned char> &frameData) {
    cv::Mat matFrame = cv::imdecode(frameData, cv::IMREAD_UNCHANGED);
    if (matFrame.empty() || matFrame.rows == 0) {
        spdlog::warn("Invalid frame:{}, {}, {}, ignored", frameData.size(), matFrame.rows, matFrame.cols);
        matFrame.release();
        return;
    }
    // spdlog::info("Valid frame:{}, {}, {}", frameData.size(), matFrame.rows, matFrame.cols);

    QImage img = QImage((const uchar *) (matFrame.data),  //(const unsigned char*)
                        matFrame.cols, matFrame.rows,
                        matFrame.cols * matFrame.channels(),   //new add
                        QImage::Format_RGB888);
    ui->Frame->setPixmap(QPixmap::fromImage(img));
    ui->Frame->resize(ui->Frame->pixmap()->size());
    ui->Frame->show();
    matFrame.release();
}
