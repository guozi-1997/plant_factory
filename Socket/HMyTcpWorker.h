#ifndef HMYTCPWORKER_H
#define HMYTCPWORKER_H

#include <QObject>
#include "HMyTcpSocket.h"
#include <QSignalMapper>
#include "ui_mainwindow.h"
#include <QTimer>
#include <QTime>
class HMyTcpWorker : public QObject
{
    Q_OBJECT
public:
    explicit HMyTcpWorker(quintptr socketHandle, Ui::MainWindow *cui, QObject *parent = 0);
    ~HMyTcpWorker();
    void connection();
    void ShowDO(QByteArray rst);

private:
    void msgHandle(QByteArray rst);
    void cuiInit();
    HMyTcpSocket *m_tcpSocket;
    quintptr m_nID;
    QSignalMapper *map_relay;
    QList<QPushButton *> btn_relay;
    Ui::MainWindow *cui;
    QString m_strHostName;
    QTimer *m_heartTimer;
    int count_num;
    QByteArray heartbeat; //心跳数据
    int m_port;
    int m_timeout;
    bool flag_num[10] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

signals:
    void sigSendData2TcpSocket(QString data, int id);
    void sigDisConnectTcpSocket(int id);

    void sigResult(int id);
    void sigDisconnect();
    void sigRevDataFromServer(QString data); //收到服务端发来数据

public slots:
    void sltRevMessageFromServer(); //服务端发来数据处理
    void sltServerDisConnect();     //服务端发来断开信号
    void buttonClicked_relay(int flag);
    void sltSendData(QString data, int id); //向服务端发送数据
    void sltDisConnect();                   //客户端主动断开
    void sltConnectServer(QString hostName, int port, int timeout);
    void SensorRequest();
private slots:
    void sltHeartTimer();
};

#endif // HMYTCPWORKER_H
