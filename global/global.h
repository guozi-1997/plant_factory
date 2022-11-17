#ifndef GLOBAL_H
#define GLOBAL_H
#include <QByteArray>
#include <QString>
#include <stdlib.h>
#include <stdio.h>
struct global_context
{
    float tmp;      //温度
    float humidity; //湿度
    float elec;     //电导率
};
extern global_context sensor_struct[1];
QByteArray sensor_analysisRcv(QByteArray src);
unsigned int calc_crc16(QByteArray snd, unsigned char num);
QByteArray soilParameterRq();
QByteArray weatherParameterRq();
#endif // GLOBAL_H
