#include "client.h"

#include <iostream>
#include <unistd.h>

using namespace std;

extern QString MSG_CLIENT_CONN;

Client::Client(QObject* parent): QObject(parent){

    connect(&clientSocket, SIGNAL(connected()), this, SLOT(connectionEstablished()));
    connect(&clientSocket, SIGNAL(readyRead()), this, SLOT(readMessage()));

}

Client::~Client(){

    clientSocket.close();
}

void Client::setHost(QString hostAddress, quint16 hostPort){

    clientAddress = hostAddress;
    clientPort = hostPort;
    host.setAddress(clientAddress);

}

void Client::start(){

    clientSocket.connectToHost(host, clientPort);
    //clientSocket.waitForConnected(5000);
}

void Client::startTransfer(){

    if (clientSocket.state() == QAbstractSocket::ConnectedState)
        //client.write(MSG_HI.toLatin1().constData(), 9);
        clientSocket.write(datagram);
}

void Client::connectionEstablished(){

    connected = true;
    //emit clientConnected();
    cout << MSG_CLIENT_CONN.toUtf8().constData() << endl;
}

void Client::readMessage() {

    datagram.clear();
    while (this->clientSocket.bytesAvailable())
        datagram.append(this->clientSocket.readAll());

    cout <<  datagram.data() << endl;

}
