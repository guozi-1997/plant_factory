#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QVector>
#include <QSignalMapper>
#include "Socket/HMyTcpClient.h"
#include <QTimer>
#include <QTime>

QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    QTcpSocket *tcpsocket;
    Ui::MainWindow *ui;
    ~MainWindow();

private slots:
    void Maininterface_ctl(int flag);
    void on_pushButton_clicked();
    void sltHeartTimer();

private:
    void uiInit();
    QSignalMapper *Maininterface_map;
    QList<QPushButton *> Maininterface_btn;
    HMyTcpClient *relay_tcpClient;
    HMyTcpClient *sensor_tcpClient;
    HMyTcpClient *led_tcpClient;
    QTimer *m_heartTimer;
    int i_but = 0;
};

#endif // MAINWINDOW_H
