#ifndef HMYTCPSOCKET_H
#define HMYTCPSOCKET_H

/*
 * ----------------------------------------
 * | 8 bit  |   data                      |
 * ----------------------------------------
 * 对数据进行了简单的封包，前面8位表示后面数据的长度，读取，写入时全按这格式进行；
 */
#include <QObject>
#include <QTcpSocket>

class HMyTcpSocket : public QTcpSocket
{
    Q_OBJECT
public:
    explicit HMyTcpSocket(quintptr socketDescriptor, QObject *parent = 0);
    ~HMyTcpSocket();

private:
    quintptr socketID; //保存id，

signals:

public slots:
    void sendData(QString data, const int); //发送数据（进行了简单的封包）
    void disConnectTcp(int id);             //断开连接
};

#endif // HMYTCPSOCKET_H
