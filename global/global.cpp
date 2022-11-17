#include "global.h"

unsigned int calc_crc16(QByteArray snd, unsigned char num)
{
    unsigned char i, j;
    unsigned int c, crc = 0xFFFF;
    for (i = 0; i < num; i++)
    {
        c = snd[i] & 0x00FF;
        crc ^= c;
        for (j = 0; j < 8; j++)
        {
            if (crc & 0x0001)
            {
                crc >>= 1;
                crc ^= 0xA001;
            }
            else
            {
                crc >>= 1;
            }
        }
    }
    return crc;
}
/*
 *Soil parameters
 *主机请求：01 04 00 00 00 03 B0 0B （8 个字节）
 *传感器响应：01 04 06 08 90 0E 93 02 4E D2 57 （11个字节）
 */
QByteArray soilParameterRq()
{
    QByteArray src;
    src[0] = 0x02;
    src[1] = 0x04;
    src[2] = 0x00;
    src[3] = 0x00;
    src[4] = 0x00;
    src[5] = 0x03;
    unsigned int crc16 = 0;
    crc16 = calc_crc16(src, 6);
    src[6] = crc16 % 256;
    src[7] = crc16 / 256;
    return src;
}

/*
 *Automatic weather station feeds information to the computer.
 *功能码	起始地址	数据长度	校验码低位	校验码高位
 *主百叶箱传感器读取命令：01 03 00 00 00 09 85 CC （8 个字节）地址码
 *地址码	功能码	有效字节数	湿度值	温度值	校验码低位	校验码高位
 *传感器响应：01 03 12 00 17 00 1D 01 92 00 E3 00 00 00 6B 00 00 00 00 01 9E 54 D4（23个字节）
 */
QByteArray weatherParameterRq()
{
    QByteArray src;
    src[0] = 0x01;
    src[1] = 0x03;
    src[2] = 0x00;
    src[3] = 0x00;
    src[4] = 0x00;
    src[5] = 0x09;
    unsigned int crc16 = 0;
    crc16 = calc_crc16(src, 6);
    src[6] = crc16 % 256;
    src[7] = crc16 / 256;
    return src;
}

/****************************室外气象站传感器***************************************/

/*
 *CO2 PM2.5 PM10 TSP
 */
QByteArray exterParameterRq1()
{
    QByteArray src;
    src[0] = 0x8a;
    src[1] = 0x03;
    src[2] = 0x00;
    src[3] = 0x00;
    src[4] = 0x00;
    src[5] = 0x04;
    unsigned int crc16 = 0;
    crc16 = calc_crc16(src, 6);
    src[6] = crc16 % 256;
    src[7] = crc16 / 256;
    return src;
}
/*
 *温度 湿度 大气压 噪声
 */
QByteArray exterParameterRq2()
{
    QByteArray src;
    src[0] = 0x66;
    src[1] = 0x03;
    src[2] = 0x00;
    src[3] = 0x00;
    src[4] = 0x00;
    src[5] = 0x04;
    unsigned int crc16 = 0;
    crc16 = calc_crc16(src, 6);
    src[6] = crc16 % 256;
    src[7] = crc16 / 256;
    return src;
}

/*
 *光照
 */
QByteArray exterParameterRq3()
{
    QByteArray src;
    src[0] = 0xcc;
    src[1] = 0x03;
    src[2] = 0x00;
    src[3] = 0x03;
    src[4] = 0x00;
    src[5] = 0x01;
    unsigned int crc16 = 0;
    crc16 = calc_crc16(src, 6);
    src[6] = crc16 % 256;
    src[7] = crc16 / 256;
    return src;
}

/*
 *风速 风向
 */
QByteArray exterParameterRq4()
{
    QByteArray src;
    src[0] = 0xc8;
    src[1] = 0x03;
    src[2] = 0x00;
    src[3] = 0x03;
    src[4] = 0x00;
    src[5] = 0x02;
    unsigned int crc16 = 0;
    crc16 = calc_crc16(src, 6);
    src[6] = crc16 % 256;
    src[7] = crc16 / 256;
    return src;
}

/*
 *雨量
 */
QByteArray exterParameterRq5()
{
    QByteArray src;
    src[0] = 0xca;
    src[1] = 0x03;
    src[2] = 0x00;
    src[3] = 0x48;
    src[4] = 0x00;
    src[5] = 0x04;
    unsigned int crc16 = 0;
    crc16 = calc_crc16(src, 6);
    src[6] = crc16 % 256;
    src[7] = crc16 / 256;
    return src;
}

QByteArray sensor_analysisRcv(QByteArray src)
{
    QByteArray dst;

    if (src[0] == 0x01 && src[1] == 0x03) //室内
    {
        if (calc_crc16(src, 23) == 0)
        {
            for (int i = 0; i < 18; i++)
                dst[i] = src[3 + i];
            return dst;
        }
    }
    else if (src[0] == 0x02 && src[1] == 0x04) //土壤
    {
        if (calc_crc16(src, 11) == 0)
        {
            dst[0] = src[3];
            dst[1] = src[4];
            dst[2] = src[5];
            dst[3] = src[6];
            dst[4] = src[7];
            dst[5] = src[8];
            return dst;
        }
    }
    else if (src[0] == 0x8a && src[1] == 0x03)
    {
        if (calc_crc16(src, 13) == 0)
        {
            for (int i = 0; i < src[2]; i++)
                dst[i] = src[3 + i];
            return dst;
        }
    }
    else if (src[0] == 0x66 && src[1] == 0x03)
    {
        if (calc_crc16(src, 13) == 0)
        {
            for (int i = 0; i < src[2]; i++)
                dst[i] = src[3 + i];
            return dst;
        }
    }
    else if (src[0] == 0xc8 && src[1] == 0x03)
    {
        if (calc_crc16(src, 9) == 0)
        {
            for (int i = 0; i < src[2]; i++)
                dst[i] = src[3 + i];
            return dst;
        }
    }
    else if (src[0] == 0xcc && src[1] == 0x03)
    {
        if (calc_crc16(src, 9) == 0)
        {
            for (int i = 0; i < src[2]; i++)
                dst[i] = src[3 + i];
            return dst;
        }
    }
    else if (src[0] == 0xca && src[1] == 0x03)
    {
        if (calc_crc16(src, 9) == 0)
        {
            for (int i = 0; i < src[2]; i++)
                dst[i] = src[3 + i];
            return dst;
        }
    }

    return dst;
}
