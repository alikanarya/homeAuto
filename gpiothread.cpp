#include "gpiothread.h"
#include "client.h"
//#include "globals.h"

#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

extern Client *clientx;
extern bool firstRun;

gpioThread::gpioThread(){

    for (int i = 0; i < dInpSize; i++) dInpArr[i] = '0';
    dInpArr[dInpSize] = '\0';

    for (int i = 0; i < dInpSize; i++) dInpArrPrev[i] = '0';
    dInpArrPrev[dInpSize] = '\0';

    for (int i = 0; i < dOutSize; i++) dOutArr[i] = '1';
    dOutArr[dOutSize] = '\0';

    for (int i = 0; i < dOutSize; i++) dOutReadArr[i] = '0';
    dOutReadArr[dOutSize] = '\0';

    for (int i = 0; i < aInpSize; i++) aInpArr[i] = 0;

    bool exportStatus = false;
    for (int i = 0; i < dInpSize; i++) {
        if (pinRead(dInpGpioMap[i], false) != -1) {
            exportStatus = true;
            //cout << "pin in already exported" << endl;
        }
        if (!exportStatus) {
            //cout << "pin in will be exported" << endl;
            pinExport( "/sys/class/gpio/export", dInpGpioMap[i] );
        }
        exportStatus = false;
    }

    exportStatus = false;
    for (int i = 0; i < dOutSize; i++) {
        if (pinRead(dOutGpioMap[i], false) != -1) {
            exportStatus = true;
            //cout << "pin out already exported" << endl;
        }
        if (!exportStatus) {
            //cout << "pin out will be exported" << endl;
            pinExport( "/sys/class/gpio/export", dOutGpioMap[i] );
        }
        exportStatus = false;
    }



    for (int i = 0; i < dInpSize; i++) {
        pinDirection( dInpGpioMap[i], "in" );
    }

    for (int i = 0; i < dOutSize; i++) {
        pinDirection( dOutGpioMap[i], "out" );
    }

    // relays work as inverted
    for (int i = 0; i < dOutSize; i++) {
        char *ch = &dOutArr[i];
        pinWrite( dOutGpioMap[i], ch );
    }

    pwmchip0 = consoleCMD(PWM48302200);
    pwmchip1 = consoleCMD(PWM48304200);

    PWMCHIP0_PATH.append(pwmchip0);                                                         // /sys/class/pwm/pwmchip3
    PWMCHIP0_EXPORT.append(PWMCHIP0_PATH).append("/export");                                // /sys/class/pwm/pwmchip3/export
    pwmchip0_No = pwmchip0.toUtf8()[pwmchip0.toUtf8().size()-1];                            // 3
    PWMCHIP0_PATH.append("/pwm-"+pwmchip0_No+":0");                                       // /sys/class/pwm/pwmchip3/pwm-3\:0
    PWMCHIP0_P0_ENABLE.append(PWMCHIP0_PATH).append("/enable");                             // /sys/class/pwm/pwmchip3/pwm-3\:0/enable
    PWMCHIP0_P0_PERIOD.append(PWMCHIP0_PATH).append("/period");                             // /sys/class/pwm/pwmchip3/pwm-3\:0/period
    PWMCHIP0_P0_DUTYCYCLE.append(PWMCHIP0_PATH).append("/duty_cycle");                      // /sys/class/pwm/pwmchip3/pwm-3\:0/duty_cycle
    PWMCHIP0_P1_ENABLE.append(PWMCHIP0_PATH).append("/enable").replace("0","1");            // /sys/class/pwm/pwmchip3/pwm-3\:1/enable
    PWMCHIP0_P1_PERIOD.append(PWMCHIP0_PATH).append("/period").replace("0","1");            // /sys/class/pwm/pwmchip3/pwm-3\:1/period
    PWMCHIP0_P1_DUTYCYCLE.append(PWMCHIP0_PATH).append("/duty_cycle").replace("0","1");     // /sys/class/pwm/pwmchip3/pwm-3\:1/duty_cycle

    PWMCHIP1_PATH.append(pwmchip1);                                                         // /sys/class/pwm/pwmchip6
    PWMCHIP1_EXPORT.append(PWMCHIP1_PATH).append("/export");                                // /sys/class/pwm/pwmchip6/export
    pwmchip1_No = pwmchip1.toUtf8()[pwmchip1.toUtf8().size()-1];                            // 6
    PWMCHIP1_PATH.append("/pwm-"+pwmchip1_No+":0");                                       // /sys/class/pwm/pwmchip6/pwm-6\:0
    PWMCHIP1_P0_ENABLE.append(PWMCHIP1_PATH).append("/enable");                             // /sys/class/pwm/pwmchip6/pwm-6\:0/enable
    PWMCHIP1_P0_PERIOD.append(PWMCHIP1_PATH).append("/period");                             // /sys/class/pwm/pwmchip6/pwm-6\:0/period
    PWMCHIP1_P0_DUTYCYCLE.append(PWMCHIP1_PATH).append("/duty_cycle");                      // /sys/class/pwm/pwmchip6/pwm-6\:0/duty_cycle
    PWMCHIP1_P1_ENABLE.append(PWMCHIP1_PATH).append("/enable").replace("0","1");            // /sys/class/pwm/pwmchip6/pwm-6\:1/enable
    PWMCHIP1_P1_PERIOD.append(PWMCHIP1_PATH).append("/period").replace("0","1");            // /sys/class/pwm/pwmchip6/pwm-6\:1/period
    PWMCHIP1_P1_DUTYCYCLE.append(PWMCHIP1_PATH).append("/duty_cycle").replace("0","1");     // /sys/class/pwm/pwmchip6/pwm-6\:1/duty_cycle

    /* Debian 8 kernel overlay
    PWMCHIP0_PATH.append(pwmchip0);
    PWMCHIP0_EXPORT.append(PWMCHIP0_PATH);
    PWMCHIP0_EXPORT.append("/export");

    PWMCHIP0_P0_ENABLE.append(PWMCHIP0_PATH);
    PWMCHIP0_P0_ENABLE.append("/pwm0/enable");
    PWMCHIP0_P0_PERIOD.append(PWMCHIP0_PATH);
    PWMCHIP0_P0_PERIOD.append("/pwm0/period");
    PWMCHIP0_P0_DUTYCYCLE.append(PWMCHIP0_PATH);
    PWMCHIP0_P0_DUTYCYCLE.append("/pwm0/duty_cycle");

    PWMCHIP0_P1_ENABLE.append(PWMCHIP0_PATH);
    PWMCHIP0_P1_ENABLE.append("/pwm1/enable");
    PWMCHIP0_P1_PERIOD.append(PWMCHIP0_PATH);
    PWMCHIP0_P1_PERIOD.append("/pwm1/period");
    PWMCHIP0_P1_DUTYCYCLE.append(PWMCHIP0_PATH);
    PWMCHIP0_P1_DUTYCYCLE.append("/pwm1/duty_cycle");

    PWMCHIP1_PATH.append(pwmchip1);
    PWMCHIP1_EXPORT.append(PWMCHIP1_PATH);
    PWMCHIP1_EXPORT.append("/export");

    PWMCHIP1_P0_ENABLE.append(PWMCHIP1_PATH);
    PWMCHIP1_P0_ENABLE.append("/pwm0/enable");
    PWMCHIP1_P0_PERIOD.append(PWMCHIP1_PATH);
    PWMCHIP1_P0_PERIOD.append("/pwm0/period");
    PWMCHIP1_P0_DUTYCYCLE.append(PWMCHIP1_PATH);
    PWMCHIP1_P0_DUTYCYCLE.append("/pwm0/duty_cycle");

    PWMCHIP1_P1_ENABLE.append(PWMCHIP1_PATH);
    PWMCHIP1_P1_ENABLE.append("/pwm1/enable");
    PWMCHIP1_P1_PERIOD.append(PWMCHIP1_PATH);
    PWMCHIP1_P1_PERIOD.append("/pwm1/period");
    PWMCHIP1_P1_DUTYCYCLE.append(PWMCHIP1_PATH);
    PWMCHIP1_P1_DUTYCYCLE.append("/pwm1/duty_cycle");
    */

    pinExport( PWMCHIP0_EXPORT.toUtf8().constData(), 0 );
    pinExport( PWMCHIP0_EXPORT.toUtf8().constData(), 1 );
    pinExport( PWMCHIP1_EXPORT.toUtf8().constData(), 0 );
    pinExport( PWMCHIP1_EXPORT.toUtf8().constData(), 1 );

    for (int i = 0; i < 2; i++)
        for (int j = 0; j < 2; j++){
            pwmPeriod(i, j, defaultPeriod);
            pwmDutyCycle(i, j, defaultDutyCycle);
        }

    pinExport( PWMCHIP0_P0_ENABLE.toUtf8().constData(), 1 );    // enable output
    pinExport( PWMCHIP0_P1_ENABLE.toUtf8().constData(), 1 );    // enable output
    pinExport( PWMCHIP1_P0_ENABLE.toUtf8().constData(), 1 );    // enable output
    pinExport( PWMCHIP1_P1_ENABLE.toUtf8().constData(), 1 );    // enable output

    stopped = false;
}

gpioThread::~gpioThread(){
}

void gpioThread::run(){

    //if (!stopped){    }
    //stopped = false;
    gpioOps();
}


void gpioThread::gpioOps(){

    if (writeEnable) {

        for (int i = 0; i < dOutSize; i++) {
            char *ch = &dOutArr[i];
            pinWrite( dOutGpioMap[i], ch );
        }


        int dutyCycle = 0;
        int c = 0;

        for (int i = 0; i < 2; i++)
            for (int j = 0; j < 2; j++){
                dutyCycle = defaultPeriod * aOutArr[c] / 10.0;
                pwmDutyCycle(i, j, dutyCycle);
                c++;
            }

        writeEnable = false;
        //cout << "write to BBB" << endl;   //DBG
    }
/* ***
    if (clientx->clientSocket.state() != QAbstractSocket::ConnectingState &&
        clientx->clientSocket.state() != QAbstractSocket::ConnectedState )
            clientx->start();
*/
    clientx->datagram.clear();

    for (int i = 0; i < dInpNum; i++) {
        pinRead( dInpGpioMap[i] );
        dInpArrPrev[i] = dInpArr[i];
        dInpArr[i] = setValue[0];
        clientx->datagram.append( setValue[0] );
    }

/*
    for (int i = 0; i < dOutSize; i++) {
        pinRead( dOutGpioMap[i] );
        dOutReadArr[i] = setValue[0];
        clientx->datagram.append( setValue[0] );
    }
*/

    int val = 0;

    for (int i = 0; i < aInpNum; i++){
        val = readAnalog(i);
        aInpArr[i] = val;
        //cout << "ADC" << i <<": " << val << "...";  //DBG
        clientx->datagram.append( 'A' );
        char charVal[MAX];
        sprintf(charVal, "%d", val);
        clientx->datagram.append( charVal );
        //clientx->datagram.append( 'Z' );
    }
    clientx->datagram.append( 'Z' );
    //cout << endl;   //DBG

    //cout << clientx->datagram.data() << endl;     //DBG

    //if (clientx->clientSocket.state() == QAbstractSocket::ConnectedState) clientx->startTransfer();

    //readDS18B20(0);

    if ( checkDInputChange() || firstRun)
        emit gpioOpsFinished();

    emit gpioOpsOK();

    //firstRun = false;

}

void gpioThread::enableWrite(){

    writeEnable = true;

}

void gpioThread::stop(){

    stopped = true;
}

int gpioThread::pinExport(QString path, int pinNo){

    // Export the pin
    if ((buttonHandle = fopen(path.toUtf8().constData(), "ab")) == NULL) {
//    if ((buttonHandle = fopen("/sys/class/gpio/export", "ab")) == NULL) {
        printf("Cannot export the GPIO pin.\n");
        return 1;
    }
    sprintf(GPIOString, "%d", pinNo);
    strcpy(setValue, GPIOString);
    fwrite(&setValue, sizeof(char), 3, buttonHandle);
    fclose(buttonHandle);
    return 0;
}

int gpioThread::pinDirection(int pinNo, QString pinDir){

    // Set the direction of the pin
    sprintf(GPIODirection, "/sys/class/gpio/gpio%d/direction", pinNo);
    strcpy(setValue, pinDir.toLatin1().constData());

    if ((buttonHandle = fopen(GPIODirection, "rb+")) == NULL ){
        printf("Cannot open direction handle:pinDirection\n");
        return 1;
    }
    fwrite(&setValue, sizeof(char), 3, buttonHandle);
    fclose(buttonHandle);
    return 0;
}

int gpioThread::pinRead(int pinNo, bool showMsg){

    //Read in the value of the pin
    sprintf(GPIOValue, "/sys/class/gpio/gpio%d/value", pinNo);

    if ((buttonHandle = fopen(GPIOValue, "rb+")) == NULL ){
        if (showMsg) printf("Cannot open value handle:pinRead\n");
        return -1;
    }
    fread(&setValue, sizeof(char), 1, buttonHandle);
    fclose(buttonHandle);
    return ( (int) setValue[0] % 48 );
}

int gpioThread::pinWrite(int pinNo, char *value){

    //Write the value to the pin
    sprintf(GPIOValue, "/sys/class/gpio/gpio%d/value", pinNo);
    if ((buttonHandle = fopen(GPIOValue, "wb+")) == NULL ){
        printf("Cannot open value handle:pinWrite\n");
        return 1;
    }
    strcpy(setValue, value);
    //strcpy(setValue, value.toLatin1().constData());
    fwrite(&setValue, sizeof(char), 1, buttonHandle);
    fclose(buttonHandle);
    //cout << "pinWrite" << endl;
    return 0;
}

int gpioThread::pwmPeriod(int chip, int pinNo, int value){

    if (chip == 0 && pinNo == 0)
        sprintf(GPIOValue, PWMCHIP0_P0_PERIOD.toUtf8().constData());
    else if (chip == 0 && pinNo == 1)
        sprintf(GPIOValue, PWMCHIP0_P1_PERIOD.toUtf8().constData());
    else if (chip == 1 && pinNo == 0)
        sprintf(GPIOValue, PWMCHIP1_P0_PERIOD.toUtf8().constData());
    else if (chip == 1 && pinNo == 1)
        sprintf(GPIOValue, PWMCHIP1_P1_PERIOD.toUtf8().constData());
    else
        return -1;

    if ((buttonHandle = fopen(GPIOValue, "wb+")) == NULL ){
        printf("Cannot open value handle:pwmPeriod\n");
        return 1;
    }

    strcpy(setValue, QString::number(value).toUtf8().constData());
    fwrite(&setValue, sizeof(char), 10, buttonHandle);
    fclose(buttonHandle);
    //cout << GPIOValue << endl;
    return 0;
}

int gpioThread::pwmDutyCycle(int chip, int pinNo, int value){

    if (chip == 0 && pinNo == 0)
        sprintf(GPIOValue, PWMCHIP0_P0_DUTYCYCLE.toUtf8().constData());
    else if (chip == 0 && pinNo == 1)
        sprintf(GPIOValue, PWMCHIP0_P1_DUTYCYCLE.toUtf8().constData());
    else if (chip == 1 && pinNo == 0)
        sprintf(GPIOValue, PWMCHIP1_P0_DUTYCYCLE.toUtf8().constData());
    else if (chip == 1 && pinNo == 1)
        sprintf(GPIOValue, PWMCHIP1_P1_DUTYCYCLE.toUtf8().constData());
    else
        return -1;

    if ((buttonHandle = fopen(GPIOValue, "wb+")) == NULL ){
        printf("Cannot open value handle:pwmDutyCycle\n");
        return 1;
    }

    strcpy(setValue, QString::number(value).toUtf8().constData());
    fwrite(&setValue, sizeof(char), 10, buttonHandle);
    fclose(buttonHandle);
    //cout << GPIOValue << endl;
    return 0;
}

int gpioThread::readAnalog(int adcPin){

    stringstream ss;
    ss << LDR_PATH << adcPin << "_raw";
    fstream fs;
    fs.open(ss.str().c_str(), fstream::in);
    fs >> adcPin;
    fs.close();
    return adcPin;
}
/*
float gpioThread::readDS18B20(int sensor){

    stringstream ss;
    ss << DS18B20_PATH << DS18B20_READ;
    fstream fs;
    fs.open(ss.str().c_str(), fstream::in);
    //streambuf output;
    char * buffer = new char [100];
    fs.read(buffer, 100);
    QString str(buffer);

    qDebug() << buffer;
    fs.close();

    QRegExp rx("(\\d+)");
    QStringList list;
    int pos = 0;

    while ((pos = rx.indexIn(str, pos)) != -1) {
        list << rx.cap(1);
        pos += rx.matchedLength();
    }

    qDebug() << list.last();

    return 0;
}
*/
QString gpioThread::consoleCMD(QString cmd){

    char buf[9];

    FILE *ptr;
    QString result;

    if ((ptr = popen(cmd.toUtf8().constData(), "r")) != NULL) {
         if (fgets(buf, 9, ptr) != NULL) {
             buf[8]='\0';
             //result = new QString(buf);
             result = buf;
         }
         fclose(ptr);
    }

    return result;
}

bool gpioThread::checkDInputChange(){

    for (int i = 0; i < dInpNum; i++)
        if ( dInpArr[i] != dInpArrPrev[i] )
            return true;

    return false;
}
