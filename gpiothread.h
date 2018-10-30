#ifndef GPIOTHREAD_H
#define GPIOTHREAD_H

#define MAX 64
#define LDR_PATH "/sys/bus/iio/devices/iio:device0/in_voltage"
#define PWM48302200 "ls /sys/devices/platform/ocp/48302000.epwmss/48302200.pwm/pwm"
#define PWM48304200 "ls /sys/devices/platform/ocp/48304000.epwmss/48304200.pwm/pwm"
//#define DS18B20_PATH "/sys/devices/w1_bus_master1/28-80000026a828"
//#define DS18B20_READ "/w1_slave"

#include <QThread>

class gpioThread : public QThread {

    Q_OBJECT

public:

    gpioThread();
    ~gpioThread();

    void stop();
    int pinExport(QString path, int pinNo);
    int pinDirection(int pinNo, QString pinDir);
    int pinRead(int pinNo, bool showMsg = true);
    int pinWrite(int pinNo, char *value);
    int pwmPeriod(int chip, int pinNo, int value);
    int pwmDutyCycle(int chip, int pinNo, int value);
    int readAnalog(int adcPin);
    //float readDS18B20(int sensor);
    QString consoleCMD(QString cmd);
    bool checkDInputChange();



    FILE *buttonHandle = NULL;
    char setValue[4], GPIOString[4], GPIOValue[MAX], GPIODirection[MAX];

    static const int dInpSize = 8;
    static const int dOutSize = 4;

    static const int aInpSize = 7;
    static const int aOutSize = 4;

    int dInpNum = 7;
    int aInpNum = 2;

    char dInpArr[dInpSize+1];
    char dInpArrPrev[dInpSize+1];

    char dOutArr[dOutSize+1];
    char dOutReadArr[dOutSize+1];

    int aInpArr[aInpSize];
    int aOutArr[dOutSize];

//    int dInpGpioMap[dInpSize] = {45, 44, 26, 47, 46, 27, 65, 61};
    int dInpGpioMap[dInpSize] = {45, 44, 27, 47, 46, 26, 65, 61}; // error in circuit of 27, cable connection was moved to cct of 26
    int dOutGpioMap[dOutSize] = {66, 67, 69, 68};

    bool writeEnable = false;

    QString pwmchip0;
    QString pwmchip0_No;
    QString PWMCHIP0_PATH = "/sys/class/pwm/";
    QString PWMCHIP0_EXPORT = "";
    QString PWMCHIP0_P0_ENABLE = "";
    QString PWMCHIP0_P0_PERIOD = "";
    QString PWMCHIP0_P0_DUTYCYCLE = "";
    QString PWMCHIP0_P1_ENABLE = "";
    QString PWMCHIP0_P1_PERIOD = "";
    QString PWMCHIP0_P1_DUTYCYCLE = "";

    QString pwmchip1;
    QString pwmchip1_No;
    QString PWMCHIP1_PATH = "/sys/class/pwm/";
    QString PWMCHIP1_EXPORT = "";
    QString PWMCHIP1_P0_ENABLE = "";
    QString PWMCHIP1_P0_PERIOD = "";
    QString PWMCHIP1_P0_DUTYCYCLE = "";
    QString PWMCHIP1_P1_ENABLE = "";
    QString PWMCHIP1_P1_PERIOD = "";
    QString PWMCHIP1_P1_DUTYCYCLE = "";

    int defaultPeriod = 1000000;
    int defaultDutyCycle = 0;

    bool stopLoop = false;

public slots:

    void gpioOps();
    void enableWrite();

protected:

    void run();

private:

    volatile bool stopped;

signals:

    void gpioOpsFinished();
    void gpioOpsOK();
};

#endif // GPIOTHREAD_H
