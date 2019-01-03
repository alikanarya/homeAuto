#include <QCoreApplication>
#include <QObject>
#include <QtNetwork>
#include <iostream>

#include "globals.h"
#include "startthr.h"
#include "checkclient.h"
#include "gpiods18b20.h"

using namespace std;

QSettings *settings;                // settings: to read/write ini file

Server *serverx;
Client *clientx, *clientForServer1;
checkClient *checkClientX, *checkClientForServer1;
gpioThread *gpioX;
gpioDS18B20 *gpioDS18B20X;
dataThread *dataX;


bool readSettings(){

    if (QFile::exists(INIFILENAME)){

        clientAddress = settings->value("clientAddress", _CLIENT_ADR).toString();
        clientPort = settings->value("clientPort", _CLIENT_PORT).toInt();
        dbName = settings->value("dbName", _DB_NAME).toString();
        dbUser = settings->value("dbUser", _DB_USER).toString();
        dbPass = settings->value("dbPass", _DB_PASS).toString();
        ds18b20_SN1 = settings->value("ds18b20_SN1", _DS18B20_SN1).toString();
        server1Address = settings->value("server1Address", _CLIENT_ADR).toString();
        server1Port = settings->value("server1Port", _SERVER1_PORT).toInt();

        //cout << clientAddress.toUtf8().constData() << endl;
        return true;

    } else {
        cout << "ini file not found" << endl;
        return false;

    }
}

int main(int argc, char *argv[]){

    QCoreApplication app(argc, argv);

    serverx = new Server();
    clientx = new Client();
    checkClientX = new checkClient(clientx);
    gpioX = new gpioThread();
    gpioDS18B20X = new gpioDS18B20();
    dataX = new dataThread();
    startThr startX;

    clientForServer1 = new Client();
    checkClientForServer1 = new checkClient(clientForServer1);

    settings = new QSettings(INIFILENAME, QSettings::IniFormat);
    readSettings();
    clientx->setHost(clientAddress, clientPort);
    clientForServer1->setHost(server1Address, server1Port);

    if (argc == 1){

        dataX->fileRecordEnable = false;
        dataX->dbRecordEnable = false;

    } else if (argc == 2){

        if ( std::string(argv[1]) == "fs" ) dataX->fileRecordEnable = true;
        else if ( std::string(argv[1]) == "db" ) dataX->dbRecordEnable = true;
        else goto label;

    } else if (argc == 3){

        if ( std::string(argv[1]) == "fs" ) dataX->fileRecordEnable = true;
        else if ( std::string(argv[1]) == "db" ) dataX->dbRecordEnable = true;
        else goto label;

        if ( std::string(argv[2]) == "fs" ) dataX->fileRecordEnable = true;
        else if ( std::string(argv[2]) == "db" ) dataX->dbRecordEnable = true;
        else goto label;

    } else {
        label:
        printf("Usage: %s recordModes\n",argv[0]);
        printf("mode: 'fs' (file save) AND / OR 'db' (database record) \n");

        if (argc == 2) cout << "p1: " << argv[1] << endl;
        if (argc == 3) cout << "p1: " << argv[1] << " p2: " << argv[2] << endl;

        return 1;
    }

    //cout << gpioX->PWMCHIP0_PATH.toUtf8().constData() << endl;
    //cout << gpioX->PWMCHIP1_PATH.toUtf8().constData() << endl;

    gpioDS18B20X->ds18b20_SN1 = ds18b20_SN1;

    if (dataX->fileRecordEnable) dataX->prepareFiles();
    if (dataX->dbRecordEnable) dataX->connectToDB();

    printf("__Date_____Time___OtO_SLN_BLK_MUT_EYO_CYO_YOD__T1___T2___Tout");
    cout << endl;


    // check client timer
    QTimer *timerCClient = new QTimer();
    QObject::connect(timerCClient, SIGNAL(timeout()), checkClientX, SLOT(connect()));
    QObject::connect(timerCClient, SIGNAL(timeout()), checkClientForServer1, SLOT(connect()));
    timerCClient->start(1000);

    QObject::connect(serverx, SIGNAL(readFinished()), gpioX, SLOT(enableWrite()));
    QObject::connect(gpioX, SIGNAL(gpioOpsFinished()), &startX, SLOT(runRecordData()));
    QObject::connect(gpioX, SIGNAL(gpioOpsOK()), checkClientX, SLOT(transferToTCPServer()));


    // temperature reading
    QObject::connect(gpioDS18B20X, SIGNAL(readOK()), &startX, SLOT(recordTemperature()));
    startX.rungpioDS18B20();

    // gpio timer
    QTimer *timerSec = new QTimer();
    QObject::connect(timerSec, SIGNAL(timeout()), &startX, SLOT(runGPIOops()));
    timerSec->start(1000);

    QTimer *timerTemperature = new QTimer();
    QObject::connect(timerTemperature, SIGNAL(timeout()), &startX, SLOT(rungpioDS18B20()));
    timerTemperature->start(1800000);

    return app.exec();
}

