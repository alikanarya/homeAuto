#ifndef GLOBALS_H
#define GLOBALS_H

#include "server.h"
#include "client.h"
#include "datathread.h"
#include "gpiothread.h"

#define INIFILENAME         "settings.ini"
#define _CLIENT_ADR         "localhost"
#define _CLIENT_PORT        8888
#define _DB_NAME            "homeAutoDB"
#define _DB_USER            "root"
#define _DB_PASS            "reyhan"
#define _DS18B20_SN1        "28-800000xxxxxx"
#define _SERVER1_PORT       23

QString MSG_SERVER_INIT = "Server is listening...";
QString MSG_CLIENT_CONN = "Client connected";
QString MSG_HI = "Hi by BBB";

QString clientAddress = "";
quint16 clientPort = 0;
QString dbName;
QString dbUser;
QString dbPass;
QString ds18b20_SN1;
QString server1Address = "";    // wemos 1
quint16 server1Port = 0;

bool firstRun = true;
bool firstRunR1 = true; // remote station 1

// remote station 1
extern const int dInpSize_R1 = 7;
extern const int dOutSize_R1 = 0;
extern const int aInpSize_R1 = 2; // Boiler Temp, Wifi Qual
extern const int aOutSize_R1 = 0;
int dataBufferSizeMax_R1 = 20;     // 1{A}+dInpSize+dOutSize+aInpSize*5{A+4digit}+1{Z}
int dataBufferSizeMin_R1 = 13;     // 1{A}+dInpSize+dOutSize+aInpSize*2{A+1digit}+1{Z}
char dInpArr_R1[dInpSize_R1+1];
bool dInpArr_R1_bool[dInpSize_R1];
bool dInpArr_R1_bool_prev[dInpSize_R1];
int aInpArr_R1[aInpSize_R1];
float boilerTemp = 0;
float boilerTempPrev = 0;
float boilerTempDelta = 0.1;
float boilerTempSetLow = 30;
float boilerTempSetNormal = 35;
float boilerTempSetHigh = 40;


#endif // GLOBALS_H
