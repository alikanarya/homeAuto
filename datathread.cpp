#include "datathread.h"
#include "gpiothread.h"
#include "gpiods18b20.h"
#include "client.h"

#include <string.h>
#include <unistd.h>
#include <time.h>
#include <string>
#include <math.h>

#define MAX 64

extern gpioThread *gpioX;
extern QString dbName;
extern QString dbUser;
extern QString dbPass;
extern bool firstRun;
extern bool firstRunR1;
extern gpioDS18B20 *gpioDS18B20X;
//extern Client *clientForServer1;
// remote station 1
extern const int dInpSize_R1;
extern bool dInpArr_R1_bool[];
extern bool dInpArr_R1_bool_prev[];
extern char dInpArr_R1[];
extern int aInpArr_R1[];
extern double boilerTemp;
extern double boilerTempDB;
extern double boilerTempPrev;
extern double boilerTempDelta;
extern int boilerTempSet;

time_t firstTime, prevTime, currentTime;
struct tm *prevTimeInfo, *currentTimeInfo = new tm();
int timeDiff = 0, timeTotal = 0;
char line[MAX];
char dateInfo[MAX];
char timeInfo[MAX];

void timeString(){
    sprintf (line, "%02d/%02d/%02d, %02d:%02d:%02d, ", 	currentTimeInfo->tm_mday,
                                                (currentTimeInfo->tm_mon+1),
                                                (currentTimeInfo->tm_year-100),
                                                currentTimeInfo->tm_hour,
                                                currentTimeInfo->tm_min,
                                                currentTimeInfo->tm_sec);
    sprintf (dateInfo, "%02d/%02d/%02d", currentTimeInfo->tm_mday, (currentTimeInfo->tm_mon+1), (currentTimeInfo->tm_year-100));
    sprintf (timeInfo, "%02d:%02d:%02d", currentTimeInfo->tm_hour, currentTimeInfo->tm_min, currentTimeInfo->tm_sec);
}

dataThread::dataThread(){

    time (&currentTime);
    currentTimeInfo = localtime (&currentTime);
    firstTime = prevTime = currentTime;
    timeString();

}

dataThread::~dataThread(){
}

void dataThread::prepareFiles(){

    if (fileRecordEnable) {

        for (int i = 0; i < 8; i++){

            if ( !fileExists( zoneNames[i].toUtf8().constData() ) ){

                zones[i].open( zoneNames[i].toUtf8().constData(), ios::out | ios::app );

                if (zones[i].is_open()){
                    zones[i] << fileHeaders[i].toUtf8().constData() << endl;
                    //timeString(); zoneAll << line << "-, -, -, -, -, -, -, -, -" << endl;
                }
            } else {

                zones[i].open( zoneNames[i].toUtf8().constData(), ios::out | ios::app );

                if (zones[i].is_open()){
                    zones[i] << fileSeperators[i].toUtf8().constData() << endl;
                }
            }
        }
    }
}

void dataThread::connectToDB(){

    db.setHostName("localhost");
    db.setDatabaseName(dbName);
    db.setUserName(dbUser);
    db.setPassword(dbPass);

    if (!db.open()) {

        qDebug() <<  db.lastError().text();

    } else {

        qDebug() <<  "db connection established";

        QSqlQuery qry;
        QString cmd;

        cmd = QString( "INSERT INTO %1 (date, time, oto, sln, blk, mut, eyo, cyo, yod) VALUES ('%2', '%3', '*', '*', '*', '*', '*', '*', '*')").arg(tableNames[0]).arg(dateInfo).arg(timeInfo);

        qry.prepare( cmd );

        if( !qry.exec() )   qDebug() << qry.lastError();


        for (int i = 0; i < gpioX->dInpNum; i++) {

            cmd = QString( "INSERT INTO %1 (date, time, state) VALUES ('%2', '%3', '*')" ).arg(tableNames[i+1]).arg(dateInfo).arg(timeInfo);

            qry.prepare( cmd );

            if( !qry.exec() )   qDebug() << qry.lastError();

        }

        cmd = QString( "INSERT INTO %1 (date, time, temp) VALUES ('%2', '%3', -99)" ).arg(tableNames[8]).arg(dateInfo).arg(timeInfo);
        //qDebug() << cmd.toUtf8().constData();

        qry.prepare( cmd );

        if( !qry.exec() )   qDebug() << qry.lastError();

    }
}

void dataThread::run(){

    //if (!stopped){    }
    //stopped = false;

    if (cmdRecordData) {
        recordData();
        cmdRecordData = false;
    }

    if (cmdRecordTemperature) {
        recordTemperature();
        cmdRecordTemperature = false;
    }

    if (cmdRecordDataR1) {
        recordDataR1();
        cmdRecordDataR1 = false;
    }

    if (cmdRecordBoilerTemperature) {
        recordBoilerTemperature();
        cmdRecordBoilerTemperature = false;
    }

    if (cmdRecordBoilerSetTemperature) {
        recordBoilerSetTemperature();
        cmdRecordBoilerSetTemperature = false;
    }
}

void dataThread::stop(){

    stopped = true;
}

void dataThread::recordData(){

    time (&currentTime);
    currentTimeInfo = localtime (&currentTime);
    //timeDiff = difftime(currentTime, prevTime);
    //timeTotal = difftime(currentTime, firstTime);
    //prevTime = currentTime;
    timeString();

    //cout << line;
    cout << dateInfo << " " << timeInfo << "  ";
    for (int i = 0; i < gpioX->dInpNum; i++)
          cout << gpioX->dInpArr[i] << "   ";

    //cout << gpioX->aInpArr[0] << " " << gpioX->aInpArr[1] << " " << gpioDS18B20X->sensor1val;
    cout << QString::number(gpioX->aInpArr[0]/22.755555,'f',1).toUtf8().constData() << " "
         << QString::number(gpioX->aInpArr[1]/22.755555,'f',1).toUtf8().constData() << " "
         << QString::number(gpioDS18B20X->sensor1val,'f',1).toUtf8().constData();

    cout << endl;

    if (fileRecordEnable) {

        if (zones[0].is_open()){

            zones[0] << line;
            for (int i = 0; i < gpioX->dInpNum; i++)
                zones[0] << gpioX->dInpArr[i] << ", " ;
            zones[0] << gpioX->aInpArr[0] << ", " << gpioX->aInpArr[1];
            zones[0]<< endl;
        }

        for (int i = 0; i < gpioX->dInpNum; i++) {

            if ( gpioX->dInpArr[i] != gpioX->dInpArrPrev[i] ) {

                if ( zones[i+1].is_open() ){
                    zones[i+1] << line << gpioX->dInpArr[i] << endl;
                }
            }
        }
    }


    if (dbRecordEnable && db.open()) {

        QSqlQuery qry;
        QString cmd;


        cmd = QString( "INSERT INTO %1 (date, time, oto, sln, blk, mut, eyo, cyo, yod) VALUES ('%2', '%3', %4, %5, %6, %7, %8, %9, %10)").arg(tableNames[0]).arg(dateInfo).arg(timeInfo).arg(gpioX->dInpArr[0]).arg(gpioX->dInpArr[1]).arg(gpioX->dInpArr[2]).arg(gpioX->dInpArr[3]).arg(gpioX->dInpArr[4]).arg(gpioX->dInpArr[5]).arg(gpioX->dInpArr[6]);
        //qDebug() << cmd.toUtf8().constData();

        qry.prepare( cmd );

        if( !qry.exec() )
          qDebug() << qry.lastError();


        for (int i = 0; i < gpioX->dInpNum; i++) {

            if ( (gpioX->dInpArr[i] != gpioX->dInpArrPrev[i]) || firstRun ) {

                cmd = QString( "INSERT INTO %1 (date, time, state) VALUES ('%2', '%3', %4)").arg(tableNames[i+1]).arg(dateInfo).arg(timeInfo).arg(gpioX->dInpArr[i]);
                //qDebug() << cmd.toUtf8().constData();

                qry.prepare( cmd );

                if( !qry.exec() )
                  qDebug() << qry.lastError();
            }
        }

    }

    firstRun = false;

}

void dataThread::recordTemperature(){

    time (&currentTime);
    currentTimeInfo = localtime (&currentTime);
    timeString();

    if (dbRecordEnable && db.open()) {

        QSqlQuery qry;
        QString cmd;
        cmd = QString( "INSERT INTO %1 (date, time, temp) VALUES ('%2', '%3', %4)").arg(tableNames[8]).arg(dateInfo).arg(timeInfo).arg(gpioDS18B20X->sensor1val);
        //qDebug() << cmd.toUtf8().constData();
        qry.prepare( cmd );
        if( !qry.exec() )   qDebug() << qry.lastError();
    }
}

void dataThread::recordDataR1(){

    time (&currentTime);
    currentTimeInfo = localtime (&currentTime);
    //timeDiff = difftime(currentTime, prevTime);
    //timeTotal = difftime(currentTime, firstTime);
    //prevTime = currentTime;
    timeString();
/*
    //cout << line;
    cout << dateInfo << " " << timeInfo << "  ";
    for (int i = 0; i < gpioX->dInpNum; i++)
          cout << gpioX->dInpArr[i] << "   ";

    //cout << gpioX->aInpArr[0] << " " << gpioX->aInpArr[1] << " " << gpioDS18B20X->sensor1val;
    cout << QString::number(gpioX->aInpArr[0]/22.755555,'f',1).toUtf8().constData() << " "
         << QString::number(gpioX->aInpArr[1]/22.755555,'f',1).toUtf8().constData() << " "
         << QString::number(gpioDS18B20X->sensor1val,'f',1).toUtf8().constData();

    cout << endl;
*/
    if (fileRecordEnable) {
    }


    if (dbRecordEnable && db.open()) {

        QSqlQuery qry;
        QString cmd;

        if (firstRunR1) {

            cmd = QString( "INSERT INTO %1 (date, time, ch, dhw, flame) VALUES ('%2', '%3', '*', '*', '*')").arg(tableNames[9]).arg(dateInfo).arg(timeInfo);
            qry.prepare( cmd );
            if( !qry.exec() )   qDebug() << qry.lastError();

            cmd = QString( "INSERT INTO %1 (date, time, settemp) VALUES ('%2', '%3', -99)").arg(tableNames[12]).arg(dateInfo).arg(timeInfo);
            qry.prepare( cmd );
            if( !qry.exec() )   qDebug() << qry.lastError();

            firstRunR1 = false;
        }


        // ot_status TABLE

        bool state = false;
        for (int i = 0; i <= 3; i++)
            if ( dInpArr_R1_bool[i] != dInpArr_R1_bool_prev[i] )    state = true;

        if (state) {
            cmd = QString( "INSERT INTO %1 (date, time, success, none, invalid, timeout) VALUES ('%2', '%3', %4, %5, %6, %7)").arg(tableNames[10]).arg(dateInfo).arg(timeInfo).arg(dInpArr_R1_bool[0]).arg(dInpArr_R1_bool[1]).arg(dInpArr_R1_bool[2]).arg(dInpArr_R1_bool[3]);
            //qDebug() << cmd.toUtf8().constData();
            qry.prepare( cmd );
            if( !qry.exec() )   qDebug() << qry.lastError();
        }

        // boiler_status TABLE

        state = false;
        for (int i = 4; i < dInpSize_R1; i++)
            if ( dInpArr_R1_bool[i] != dInpArr_R1_bool_prev[i] && dInpArr_R1[i]!='9' )    state = true;

        if (state) {
            cmd = QString( "INSERT INTO %1 (date, time, ch, dhw, flame) VALUES ('%2', '%3', %4, %5, %6)").arg(tableNames[9]).arg(dateInfo).arg(timeInfo).arg(dInpArr_R1_bool[4]).arg(dInpArr_R1_bool[5]).arg(dInpArr_R1_bool[6]);
            //qDebug() << cmd.toUtf8().constData();
            qry.prepare( cmd );
            if( !qry.exec() )   qDebug() << qry.lastError();
        }

    }
}

void dataThread::recordBoilerTemperature(){

    time (&currentTime);
    currentTimeInfo = localtime (&currentTime);
    timeString();

    if (dbRecordEnable && db.open()) {

        QSqlQuery qry;
        QString cmd;
        cmd = QString( "INSERT INTO %1 (date, time, temp) VALUES ('%2', '%3', %4)").arg(tableNames[11]).arg(dateInfo).arg(timeInfo).arg(QString::number(boilerTempDB,'f',1).toFloat());
        //qDebug() << cmd.toUtf8().constData();
        qry.prepare( cmd );
        if( !qry.exec() )   qDebug() << qry.lastError();
    }
}

void dataThread::recordBoilerSetTemperature(){

    time (&currentTime);
    currentTimeInfo = localtime (&currentTime);
    timeString();

    if (dbRecordEnable && db.open()) {

        QSqlQuery qry;
        QString cmd;
        cmd = QString( "INSERT INTO %1 (date, time, settemp) VALUES ('%2', '%3', %4)").arg(tableNames[12]).arg(dateInfo).arg(timeInfo).arg(QString::number(boilerTempSet));
        //qDebug() << cmd.toUtf8().constData();
        qry.prepare( cmd );
        if( !qry.exec() )   qDebug() << qry.lastError();
    }
}
