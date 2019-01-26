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
extern const int aInpSize_R1 = 3; // Boiler Temp, Wifi Qual, Boiler Set Temp
extern const int aOutSize_R1 = 0;
int dataBufferSizeMax_R1 = 31;     // 1{A}+Time{hhmmss)+dInpSize+dOutSize+aInpSize*5{A+4digit}+1{Z}
int dataBufferSizeMin_R1 = 22;     // 1{A}+Time{hhmmss)+dInpSize+dOutSize+aInpSize*2{A+1digit}+1{Z}
char dInpArr_R1[dInpSize_R1+1];
bool dInpArr_R1_bool[dInpSize_R1];
bool dInpArr_R1_bool_prev[dInpSize_R1];
int aInpArr_R1[aInpSize_R1];
double boilerTemp = 0;
double boilerTempDB = 0;
double boilerTempPrev = 0;
double boilerTempDelta = 0.5;
double boilerTempSetLow = 30;
double boilerTempSetNormal = 35;
double boilerTempSetHigh = 40;
QString rs1Hour =  "";
QString rs1Min = "";
QString rs1Sec = "";
bool showIncomingMessage = false;


#endif // GLOBALS_H
