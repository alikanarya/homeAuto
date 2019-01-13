#include "server.h"
#include "gpiothread.h"

#include <iostream>
#include <unistd.h>

using namespace std;

extern gpioThread *gpioX;
extern QString MSG_SERVER_INIT;


Server::Server(QObject* parent): QObject(parent){

    connect(&server, SIGNAL(newConnection()), this, SLOT(acceptConnection()));
    server.listen(QHostAddress::Any, serverPort);
    cout << MSG_SERVER_INIT.toUtf8().constData() << endl;
}

Server::~Server(){

    server.close();
}

void Server::acceptConnection(){

    //cout <<"acceptConnection" << endl;    //DBG
    client = server.nextPendingConnection();
    connect(client, SIGNAL(readyRead()), this, SLOT(startRead()));
}

void Server::startRead(){

    datagram.clear();
    while (client->bytesAvailable())
        datagram.append(client->readAll());

    cout <<  datagram.data() << endl;
    emit this->toServer1(datagram);

/*
    for (int i = 0; i < gpioX->dOutSize; i++) {
        gpioX->dOutArr[i] = datagram.data()[i];
        //cout << gpioX->dOutArr[i] << ".";   //DBG
    }


    cout <<  datagram.data() ;//<< endl;

    int pos = gpioX->dOutSize;
    char ch = datagram.at(pos);
    //cout << ch << endl;      //DBG

    int j = 0, x = 0;
    while (ch != 'Z') {
        //x = 0;
        if (ch == 'A') {
            char temp[16];
            j = -1;
            do {
                pos++;
                ch = datagram.at(pos);
                //cout << " " << ch;
                if (ch == 'Z') break;
                if (ch == 'A') break;
                j++;
                temp[j] = ch;

            } while ( ch != 'Z' );

            temp [j+1] = '\0';
            gpioX->aOutArr[x] = atoi(temp);
            cout << " " << gpioX->aOutArr[x];
            x++;

        }
    }
    cout << endl;
*/

    emit this->readFinished();
    //client->close();
}
