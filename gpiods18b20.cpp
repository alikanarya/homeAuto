#include "gpiods18b20.h"

#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

gpioDS18B20::gpioDS18B20(){

}

gpioDS18B20::~gpioDS18B20(){

}

void gpioDS18B20::run(){

    //if (!stopped){    }
    //stopped = false;
    sensor1val = readDS18B20(1);
    emit readOK();
}

void gpioDS18B20::stop(){

    stopped = true;
}

float gpioDS18B20::readDS18B20(int sensor){

    QString sensorSN = "";

    switch (sensor){
        case 1: sensorSN = ds18b20_SN1;
                break;
    }

    QString cmd = DS18B20_PATH + sensorSN + DS18B20_READ;
    //qDebug() << cmd;

    stringstream ss;
    ss << cmd.toUtf8().constData();

    fstream fs;
    fs.open(ss.str().c_str(), fstream::in);
    char * buffer = new char [80];
    fs.read(buffer, 80);
    QString str(buffer);
    //qDebug() << buffer;
    fs.close();

    QRegExp rxd("\\b(t)\\b");
    int posd = rxd.indexIn(str);

    //QRegExp rx("(\\d{5})");
    QRegExp rx("(\\-?\\d+)");

    QStringList list;
    int pos = 0;
    while ((pos = rx.indexIn(str, pos)) != -1) {
        list << rx.cap(1);
        pos += rx.matchedLength();
        if (pos > posd)
            break;

    }

    QString valstr = "";
    float val = -1;

    if (!list.isEmpty()){
        //qDebug() << list.last();
        valstr = list.last();
        val = valstr.toInt() / 1000.0;
    }

    qDebug() << val;

    return val;
}

