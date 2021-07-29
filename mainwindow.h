#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ctpl_stl.h"
#include <QKeyEvent>
#include <QCloseEvent>
#include "MsgGen.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    Ui::MainWindow *ui;
    ctpl::thread_pool *threadPool;
private slots:
    void on_connectBtn_clicked();
    void keyPressEvent(QKeyEvent *keyEvent) override;
    void closeEvent(QCloseEvent *event) override;

    /**
     * @param status disconnected = 0, connecting = 1, connected = 2, disconnecting = 3
     */
    void onMsgServerConnectionChanged(int status);
    /**
     * @param status disconnected = 0, connecting = 1, connected = 2, disconnecting = 3
     */
    void onVideoServerConnectionChanged(int status);

    void onMsgReceived(const std::string& cmd);

    void onVideoFrameReceived(const std::vector<unsigned char>& frameData);

};
#endif // MAINWINDOW_H
