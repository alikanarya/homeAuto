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




#endif // GLOBALS_H
