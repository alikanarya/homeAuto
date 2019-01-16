#ifndef STARTTHR_H
#define STARTTHR_H

#include <QObject>
#include "gpiothread.h"
#include "datathread.h"
#include "gpiods18b20.h"

extern gpioThread *gpioX;
extern dataThread *dataX;
extern gpioDS18B20 *gpioDS18B20X;

class startThr : public QObject {

    Q_OBJECT

public:

    explicit startThr(QObject *parent = 0);

signals:

public slots:

    void runRecordData(){
        dataX->cmdRecordData = true;
        dataX->start();
    }

    void recordTemperature(){
        dataX->cmdRecordTemperature = true;
        dataX->start();
    }

    void runGPIOops(){
        gpioX->start();
    }

    void rungpioDS18B20(){
        gpioDS18B20X->start();
    }

    void runRecordDataR1(){
        dataX->cmdRecordDataR1 = true;
        dataX->start();
    }

};

#endif // STARTTHR_H
