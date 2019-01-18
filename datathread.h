#ifndef DATATHREAD_H
#define DATATHREAD_H

#include <QThread>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <QtSql/QSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlDriver>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QDebug>

using namespace std;


class dataThread: public QThread{

    Q_OBJECT

public:

    dataThread();
    void prepareFiles();
    void connectToDB();

    ~dataThread();

    void stop();
    inline bool fileExists (const std::string& name) {
        if (FILE *file = fopen(name.c_str(), "r")) {
            fclose(file);
            return true;
        } else {
            return false;
        }
    }

    bool fileRecordEnable = false;
    ofstream zones[8];
    QString zoneNames[8] = {"./data/zones.csv", "./data/z1_oto.csv", "./data/z2_sln.csv", "./data/z3_blk.csv", "./data/z4_mut.csv", "./data/z5_eyo.csv", "./data/z6_cyo.csv", "./data/z7_yod.csv"};
    QString fileHeaders[8] = {"Date, Time, OtO, SLN, BLK, MUT, EYO, CYO, YOD, T1, T2", "Date, Time, OtO", "Date, Time, SLN", "Date, Time, BLK", "Date, Time, MUT",  "Date, Time, EYO", "Date, Time, CYO", "Date, Time, YOD" };
    QString fileSeperators[8] = {"*, *, *, *, *, *, *, *, *, *, *", "*, *, *", "*, *, *", "*, *, *", "*, *, *",  "*, *, *", "*, *, *", "*, *, *" };

    bool dbRecordEnable = false;
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    QString tableNames[12] = {"zones", "oto", "sln", "blk", "mut", "eyo", "cyo", "yod", "tempout", "boiler_status", "ot_status", "boiler_temp"};

    bool cmdRecordData = false;
    bool cmdRecordTemperature = false;
    bool cmdRecordDataR1 = false;
    bool cmdRecordBoilerTemperature = false;



public slots:

    void recordData();
    void recordTemperature();
    void recordDataR1();
    void recordBoilerTemperature();


protected:

    void run();


private:

    volatile bool stopped;

signals:


};
#endif // DATATHREAD_H
