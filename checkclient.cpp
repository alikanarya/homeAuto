#include "checkclient.h"

#include <iostream>

using namespace std;

//extern Client *clientx;

checkClient::checkClient(Client *_clientPtr){

    stopped = false;
    clientPtr = _clientPtr;
}

void checkClient::run(){

    if (!stopped){    }
    stopped = false;
    //connect();
}

void checkClient::connect(){

    //cout << "checkClient::connect() " << clientPtr->clientSocket.state() << endl;    //DBG
    if (clientPtr->clientSocket.state() != QAbstractSocket::ConnectingState &&
        clientPtr->clientSocket.state() != QAbstractSocket::ConnectedState ){
        //cout << "checkClient: no connection" << endl;    //DBG
        clientPtr->start();
    }

    if (clientPtr->clientSocket.state() == QAbstractSocket::ConnectedState )
        emit Connected();
    else
        emit notConnected();

}

void checkClient::transferToTCPServer(){

    if (clientPtr->clientSocket.state() == QAbstractSocket::ConnectedState)
        clientPtr->startTransfer();
}

void checkClient::transferToTCPServer(QByteArray _datagram){

    if (clientPtr->clientSocket.state() == QAbstractSocket::ConnectedState)
        clientPtr->startTransfer(_datagram);
}

void checkClient::stop(){

    stopped = true;

}

checkClient::~checkClient(){
}

