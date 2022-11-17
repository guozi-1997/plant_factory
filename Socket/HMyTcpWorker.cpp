#include "HMyTcpWorker.h"
#include <QDebug>
#include <QXmlStreamReader>
#include "ui_mainwindow.h"
#include "mainwindow.h"
#include <QWidget>
#include <QVector>
#include "global/global.h"
ushort CalculateCrc(QByteArray data, int len);
QByteArray writeID(int io, bool openclose);
QByteArray WriteAllDO(int io, bool openclose);
QByteArray ReadAllDO();
QByteArray GetHexValue(QString str);
QByteArray analysisRcv(QByteArray src);
void Sleep(int msec);
char ConvertHexChar(char ch);
global_context sensor_struct[1];

HMyTcpWorker::HMyTcpWorker(quintptr socketHandle, Ui::MainWindow *cui, QObject *parent)
    : QObject(parent),
      m_nID(socketHandle),
      cui(cui)
{
    m_tcpSocket = NULL;
    cuiInit();
}

HMyTcpWorker::~HMyTcpWorker()
{
    qDebug() << QString::fromLocal8Bit("析构……。……。……。……。……。……………。");
}
//校验码
static const ushort crcTable[] = {
    0X0000, 0XC0C1, 0XC181, 0X0140, 0XC301, 0X03C0, 0X0280, 0XC241,
    0XC601, 0X06C0, 0X0780, 0XC741, 0X0500, 0XC5C1, 0XC481, 0X0440,
    0XCC01, 0X0CC0, 0X0D80, 0XCD41, 0X0F00, 0XCFC1, 0XCE81, 0X0E40,
    0X0A00, 0XCAC1, 0XCB81, 0X0B40, 0XC901, 0X09C0, 0X0880, 0XC841,
    0XD801, 0X18C0, 0X1980, 0XD941, 0X1B00, 0XDBC1, 0XDA81, 0X1A40,
    0X1E00, 0XDEC1, 0XDF81, 0X1F40, 0XDD01, 0X1DC0, 0X1C80, 0XDC41,
    0X1400, 0XD4C1, 0XD581, 0X1540, 0XD701, 0X17C0, 0X1680, 0XD641,
    0XD201, 0X12C0, 0X1380, 0XD341, 0X1100, 0XD1C1, 0XD081, 0X1040,
    0XF001, 0X30C0, 0X3180, 0XF141, 0X3300, 0XF3C1, 0XF281, 0X3240,
    0X3600, 0XF6C1, 0XF781, 0X3740, 0XF501, 0X35C0, 0X3480, 0XF441,
    0X3C00, 0XFCC1, 0XFD81, 0X3D40, 0XFF01, 0X3FC0, 0X3E80, 0XFE41,
    0XFA01, 0X3AC0, 0X3B80, 0XFB41, 0X3900, 0XF9C1, 0XF881, 0X3840,
    0X2800, 0XE8C1, 0XE981, 0X2940, 0XEB01, 0X2BC0, 0X2A80, 0XEA41,
    0XEE01, 0X2EC0, 0X2F80, 0XEF41, 0X2D00, 0XEDC1, 0XEC81, 0X2C40,
    0XE401, 0X24C0, 0X2580, 0XE541, 0X2700, 0XE7C1, 0XE681, 0X2640,
    0X2200, 0XE2C1, 0XE381, 0X2340, 0XE101, 0X21C0, 0X2080, 0XE041,
    0XA001, 0X60C0, 0X6180, 0XA141, 0X6300, 0XA3C1, 0XA281, 0X6240,
    0X6600, 0XA6C1, 0XA781, 0X6740, 0XA501, 0X65C0, 0X6480, 0XA441,
    0X6C00, 0XACC1, 0XAD81, 0X6D40, 0XAF01, 0X6FC0, 0X6E80, 0XAE41,
    0XAA01, 0X6AC0, 0X6B80, 0XAB41, 0X6900, 0XA9C1, 0XA881, 0X6840,
    0X7800, 0XB8C1, 0XB981, 0X7940, 0XBB01, 0X7BC0, 0X7A80, 0XBA41,
    0XBE01, 0X7EC0, 0X7F80, 0XBF41, 0X7D00, 0XBDC1, 0XBC81, 0X7C40,
    0XB401, 0X74C0, 0X7580, 0XB541, 0X7700, 0XB7C1, 0XB681, 0X7640,
    0X7200, 0XB2C1, 0XB381, 0X7340, 0XB101, 0X71C0, 0X7080, 0XB041,
    0X5000, 0X90C1, 0X9181, 0X5140, 0X9301, 0X53C0, 0X5280, 0X9241,
    0X9601, 0X56C0, 0X5780, 0X9741, 0X5500, 0X95C1, 0X9481, 0X5440,
    0X9C01, 0X5CC0, 0X5D80, 0X9D41, 0X5F00, 0X9FC1, 0X9E81, 0X5E40,
    0X5A00, 0X9AC1, 0X9B81, 0X5B40, 0X9901, 0X59C0, 0X5880, 0X9841,
    0X8801, 0X48C0, 0X4980, 0X8941, 0X4B00, 0X8BC1, 0X8A81, 0X4A40,
    0X4E00, 0X8EC1, 0X8F81, 0X4F40, 0X8D01, 0X4DC0, 0X4C80, 0X8C41,
    0X4400, 0X84C1, 0X8581, 0X4540, 0X8701, 0X47C0, 0X4680, 0X8641,
    0X8201, 0X42C0, 0X4380, 0X8341, 0X4100, 0X81C1, 0X8081, 0X4040};

void HMyTcpWorker::sltRevMessageFromServer()
{
    HMyTcpSocket *tcpSocket = (HMyTcpSocket *)QObject::sender();
    if (tcpSocket != m_tcpSocket)
    {
        qDebug() << "unKnown Data";
        return;
    }

    if (tcpSocket->bytesAvailable() <= 0)
    {
        qDebug() << "no data";
        return;
    }
    //解析数据（这部分可以根据自己定义的通信协议及通信的数据结构进行解析
    QByteArray pByte;
    pByte = tcpSocket->readAll();
    msgHandle(pByte);

    // qDebug() << pByte.toHex();
}

void HMyTcpWorker::sltServerDisConnect()
{
    qDebug() << QString::fromLocal8Bit("服务端关闭") << m_nID;
    emit sigDisconnect();
}

void HMyTcpWorker::sltDisConnect()
{
    if (m_tcpSocket)
    {
        qDebug() << QString::fromLocal8Bit("断开与服务端连接") << m_nID;
        emit sigDisConnectTcpSocket(m_nID);
        //        m_tcpSocket->disconnectFromHost();
        //        m_tcpSocket->deleteLater();
        //        m_tcpSocket = NULL;
    }
}

void HMyTcpWorker::sltSendData(QString data, int id)
{
    if (m_tcpSocket)
    {
        if (id == m_nID)
        {
            emit sigSendData2TcpSocket(data, m_nID);
        }
    }
}

void HMyTcpWorker::sltConnectServer(QString hostName, int port, int timeout)
{
    if (NULL == m_tcpSocket)
    {
        m_strHostName = hostName;
        m_port = port;
        m_timeout = timeout;
        m_tcpSocket = new HMyTcpSocket(m_nID);

        connect(m_tcpSocket, SIGNAL(readyRead()), this, SLOT(sltRevMessageFromServer()));
        connect(m_tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(sltServerDisConnect()));
        connect(this, SIGNAL(sigDisConnectTcpSocket(int)), m_tcpSocket, SLOT(disConnectTcp(int)));
        connect(this, SIGNAL(sigSendData2TcpSocket(QString, int)), m_tcpSocket, SLOT(sendData(QString, int)));
    }
    if (m_tcpSocket)
    {
        m_tcpSocket->connectToHost(hostName, port);
        if (!m_tcpSocket->waitForConnected(timeout))
        {
            qDebug() << "conncet to server is timeout";
            emit sigResult(-1); //没有通过
        }
        else
        {
            emit sigResult(0);
            m_tcpSocket->write(ReadAllDO()); //如果重新连接成功，就读取继电器当前状态
        }
    }
}

void HMyTcpWorker::ShowDO(QByteArray rst)
{
    QString str = "继电器";
    if (rst == NULL)
        return;
    uchar status = rst[0];
    for (int i = 0; i < 8; i++)
    {
        if ((status & (1 << i)) == 0x00)
        {
            flag_num[i] = true;

            btn_relay[i]->setStyleSheet("QPushButton{background: #08D9D6;}");
            btn_relay[i]->setText(str + QString::number(i + 1) + "开");
        }
        else
        {
            flag_num[i] = false;
            btn_relay[i]->setStyleSheet("QPushButton{background: #DC143C;}"); //与下面一行的顺序很重要，写反了指针会找不到地址
            btn_relay[i]->setText(str + QString::number(i + 1) + "关");
        }
    }
}

void HMyTcpWorker::msgHandle(QByteArray pByte) //对收到的消息进行处理
{
    QString strData;
    strData = pByte.toHex();
    pByte = GetHexValue(strData); //这两行意味着你可以获取一个strData的字符串，并且还能重新获取pByete
    switch (m_nID)
    {
    case 666: // 666是继电器的id号，这个Id是程序设定的
        if (pByte[1] == 0x01)
            ShowDO(analysisRcv(pByte));
        break;
    case 777: //网口转串口的id号
    {
        QByteArray QBdata;
        QBdata = sensor_analysisRcv(pByte);
        if (pByte[0] == 0x01 && pByte[1] == 0x03)
        {
            //qDebug()<<QBdata.toHex();

            cui->label_9-> setText(QString("%2").arg((QBdata[6] * 256 + QBdata[7])/10.));
            cui->label_10->setText(QString("%2").arg((QBdata[4] * 256 + QBdata[5])/10.));
            cui->label_11->setText(QString("%2").arg((QBdata[10] * 256 + QBdata[11])*10 ));
            cui->label_16->setText(QString("%2").arg((QBdata[0] * 256 + QBdata[1]) ));
            cui->label_17->setText(QString("%2").arg((QBdata[16] * 256 + QBdata[17]) ));

        }

        if (pByte[0] == 0x02 && pByte[1] == 0x04)
        {
            sensor_struct[0].tmp = (QBdata[0] * 256 + QBdata[1]) / 100.;
            sensor_struct[0].humidity = (QBdata[2] * 256 + QBdata[3]) / 100.;
            sensor_struct[0].elec = QBdata[4] * 256 + QBdata[5];
            cui->label_6->setText(QString("%2").arg((QBdata[0] * 256 + QBdata[1]) / 100.));
            cui->label_7->setText(QString("%2").arg((QBdata[2] * 256 + QBdata[3]) / 100.));
            cui->label_8->setText(QString("%2").arg(QBdata[4] * 256 + QBdata[5]));
        }
    }
    break;
    default:
        break;
    }
    emit sigRevDataFromServer(strData);
}

void HMyTcpWorker::cuiInit() //对所有的按钮初始化
{
    m_heartTimer = new QTimer;
    count_num = 0;
    connect(m_heartTimer, SIGNAL(timeout()), this, SLOT(sltHeartTimer())); //建立一个心跳包
    /*
        在新开的心跳包中，尽量做较少的事情，因为心跳包会耗费很大的资源
     */
    heartbeat[0] = 0x01, heartbeat[1] = 0x02, heartbeat[2] = 0x04;
    m_heartTimer->start(300);
    switch (m_nID)
    {
    case 666:
        map_relay = new QSignalMapper();
        /*
        algorithm_relay = new QSignalMapper();
        除了按键控制之外，还应预留控制list用于算法控制
        */
        //btn_relay = (cui->page->findChildren<QPushButton *>());

        btn_relay << cui->switch_1 << cui->switch_2 << cui->switch_3
                  <<cui->switch_4 << cui->switch_5 << cui->switch_6
                 << cui->switch_7 << cui->switch_8 << cui->switch_9
                 << cui->switch_10 ;
        for (int i = 0; i < 10; i++)
        {
            // btn_relay[i] = new QPushButton();
            map_relay->setMapping(btn_relay[i], i);
            connect(btn_relay[i], SIGNAL(clicked(bool)), map_relay, SLOT(map()));
        }
        connect(map_relay, SIGNAL(mapped(int)), this, SLOT(buttonClicked_relay(int)));
        break;
    case 777: //暂时还没想好做好什么事情
              // connect(cui->production_monitoring, SIGNAL(clicked(bool)), this, SLOT(SensorRequest()));
    default:
        break;
    }
}

void HMyTcpWorker::SensorRequest()
{
    connection();
}

void HMyTcpWorker::sltHeartTimer()
{
    connection();
}

void HMyTcpWorker::buttonClicked_relay(int flag) // 第几个继电器，开or关
{
    QString str = "继电器";
    if (flag < 8)
    {
        switch (flag_num[flag])
        {
        case false:
            m_tcpSocket->write(writeID(flag, flag_num[flag]));
            btn_relay[flag]->setStyleSheet("QPushButton{background: #08D9D6;}");
            btn_relay[flag]->setText(str + QString::number(flag + 1) + "开");
            flag_num[flag] = true;
            break;
        case true:
            m_tcpSocket->write(writeID(flag, flag_num[flag]));
            btn_relay[flag]->setStyleSheet("QPushButton{background: #DC143C	;}"); //与下面一行的顺序很重要
            btn_relay[flag]->setText(str + QString::number(flag + 1) + "关");
            flag_num[flag] = false;
            break;
        default:
            break;
        }
    }
    else if (flag == 8)
    {
        switch (flag_num[flag])
        {
        case false:
            m_tcpSocket->write(WriteAllDO(flag, flag_num[flag]));
            btn_relay[flag]->setStyleSheet("QPushButton{background: #08D9D6;}");
            btn_relay[flag]->setText(str + "全开");
            for (int i = 0; i < 8; ++i)
            {
                btn_relay[i]->setStyleSheet("QPushButton{background: #08D9D6;}");
                btn_relay[i]->setText(str + QString::number(i + 1) + "开");
                flag_num[i] = true;
            }
            flag_num[flag] = true;
            break;
        case true:
            m_tcpSocket->write(WriteAllDO(flag, flag_num[flag]));

            btn_relay[flag]->setStyleSheet("QPushButton{background: #DC143C	;}"); //与下面一行的顺序很重要
            btn_relay[flag]->setText(str + "全关");
            for (int i = 0; i < 8; ++i)
            {
                btn_relay[i]->setStyleSheet("QPushButton{background: #DC143C;}"); //与下面一行的顺序很重要
                btn_relay[i]->setText(str + QString::number(i + 1) + "关");
                flag_num[i] = false;
            }
            flag_num[flag] = false;
            break;
        default:
            break;
        }
    }
    else if (flag == 9)
    {
        m_tcpSocket->write(ReadAllDO()); //本指令将获取所有继电器状态，并更改对应按钮的文字及颜色
        // m_tcpSocket->write(abcd);

        // qDebug()<<"pByte.toHex()"<<endl;
    }
}

void HMyTcpWorker::connection() //判断客户端是否连接，没连接就尝试连接，连接上了就当心跳包
{
    if (m_tcpSocket->state() != QAbstractSocket::ConnectedState)
    {
        m_tcpSocket->connectToHost(m_strHostName, m_port);
        if (m_tcpSocket->waitForConnected(m_timeout))
        {
            emit sigResult(0);
        }
        else
        {
            emit sigResult(-1); //没有成功连接
        }
    }
    else
    {
        switch (m_nID)
        {
        case 666:
            if (count_num >= 400)
            {
                m_tcpSocket->write(heartbeat); // 120s去ping一次
                count_num = 0;
            }
            count_num++;
            break;
        case 777:
            m_tcpSocket->write(soilParameterRq()); // 300ms刷新数据
            Sleep(10);                             //
            m_tcpSocket->write(weatherParameterRq());
            break;
        default:
            break;
        }
    }
}

void Sleep(int msec)
{
    QTime dieTime = QTime::currentTime().addMSecs(msec);
    while (QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

QByteArray writeID(int io, bool openclose) //打开or关闭第io个继电器
{
    QByteArray src;
    src[0] = 0xFE;
    src[1] = 0x05;
    src[2] = 0x00;
    src[3] = (uchar)io;
    src[4] = (uchar)((openclose) ? 0xff : 0x00);
    src[5] = 0x00;
    ushort crc = CalculateCrc(src, 6);
    src[6] = (uchar)(crc & 0xff);
    src[7] = (uchar)(crc >> 8);
    return src;
}

QByteArray WriteAllDO(int io, bool openclose) //打开所有继电器
{
    QByteArray src;
    src[0] = 0xFE;
    src[1] = 0x0f;
    src[2] = 0x00;
    src[3] = 0x00;
    src[4] = 0x00;
    src[5] = (uchar)io;
    src[6] = (uchar)(io + 7) / 8;
    src[7] = (uchar)((openclose) ? 0xff : 0x00);
    ushort crc = CalculateCrc(src, 8);
    src[8] = (uchar)(crc & 0xff);
    src[9] = (uchar)(crc >> 8);
    return src;
}

QByteArray ReadAllDO() //读取所有继电器状态
{
    QByteArray src;
    src[0] = 0xFE;
    src[1] = 0x01;
    src[2] = 0x00;
    src[3] = 0x00;
    src[4] = 0x00;
    src[5] = (uchar)8;
    ushort crc = CalculateCrc(src, 6);
    src[6] = (uchar)(crc & 0xff);
    src[7] = (uchar)(crc >> 8);
    return src;
}

ushort CalculateCrc(QByteArray data, int len)
{
    ushort crc = 65535;
    uchar tableIndex;
    for (int i = 0; i < len; i++)
    {
        tableIndex = (uchar)(crc ^ data[i]);
        crc >>= 8;
        crc ^= crcTable[tableIndex];
    }
    return crc;
}

QByteArray GetHexValue(QString str)
{
    QByteArray senddata;
    int hexdata, lowhexdata;
    int hexdatalen = 0;
    int len = str.length();
    senddata.resize(len / 2);
    char lstr, hstr;
    for (int i = 0; i < len;)
    {
        hstr = str[i].toLatin1();
        if (hstr == ' ')
        {
            i++;
            continue;
        }
        i++;
        if (i >= len)
            break;
        lstr = str[i].toLatin1();
        hexdata = ConvertHexChar(hstr);
        lowhexdata = ConvertHexChar(lstr);
        if ((hexdata == 16) || (lowhexdata == 16))
            break;
        else
            hexdata = hexdata * 16 + lowhexdata;
        i++;
        senddata[hexdatalen] = (char)hexdata;
        hexdatalen++;
    }
    senddata.resize(hexdatalen);
    return senddata;
}

char ConvertHexChar(char ch)
{
    if ((ch >= '0') && (ch <= '9')) //限定字符0-9
        return ch - 0x30;
    else if ((ch >= 'A') && (ch <= 'F')) //限定字符A-F
        return ch - 'A' + 10;
    else if ((ch >= 'a') && (ch <= 'f')) //限定字符a-f
        return ch - 'a' + 10;
    else
        return (-1);
}

QByteArray analysisRcv(QByteArray src)
{
    QByteArray dst;
    switch (src[1])
    {
    case 0x01:
        if (CalculateCrc(src, src[2] + 5) == 0x00)
        {

            for (int i = 0; i < src[2]; i++)
                dst[i] = src[3 + i];
            return dst;
        }
        break;
    case 0x02:
        if (CalculateCrc(src, src[2] + 5) == 0x00)
        {

            for (int i = 0; i < src[2]; i++)
                dst[i] = src[3 + i];
            return dst;
        }
        break;
    case 0x04:
        if (CalculateCrc(src, src[2] + 5) == 0x00)
        {
            for (int i = 0; i < src[2]; i++)
                dst[i] = src[3 + i];
            return dst;
        }
        break;
    case 0x05:
        if (CalculateCrc(src, 8) == 0x00)
        {
            dst[0] = src[4];
            return dst;
        }
        break;
    case 0x0f:
        if (CalculateCrc(src, 8) == 0x00)
        {
            dst[0] = 1;
            return dst;
        }
        break;
    case 0x06:
        if (CalculateCrc(src, 8) == 0x00)
        {
            dst[0] = src[2];
            dst[1] = src[3];
            dst[2] = src[4];
            dst[3] = src[5];
            return dst;
        }
        break;
    case 0x10:
        if (CalculateCrc(src, 8) == 0x00)
        {
            dst[0] = src[2];
            dst[1] = src[3];
            dst[2] = src[4];
            dst[3] = src[5];
            return dst;
        }
        break;
    }
    return NULL;
}
