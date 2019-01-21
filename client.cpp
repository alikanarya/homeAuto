#include "client.h"

#include <iostream>
#include <unistd.h>

using namespace std;

extern QString MSG_CLIENT_CONN;

// remote station 1
extern const int dInpSize_R1;
extern const int dOutSize_R1;
extern const int aInpSize_R1;
extern const int aOutSize_R1;
extern int dataBufferSizeMax_R1;
extern int dataBufferSizeMin_R1;
extern char dInpArr_R1[];
extern bool dInpArr_R1_bool[];
extern bool dInpArr_R1_bool_prev[];
extern int aInpArr_R1[];
extern double boilerTemp;
extern double boilerTempDB;
extern double boilerTempPrev;
extern double boilerTempDelta;

Client::Client(QObject* parent): QObject(parent){

    int enableKeepAlive = 1;
    int fd = clientSocket.socketDescriptor();
    setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &enableKeepAlive, sizeof(enableKeepAlive));

    int maxIdle = 5; // seconds
    setsockopt(fd, IPPROTO_TCP, TCP_KEEPIDLE, &maxIdle, sizeof(maxIdle));

    int count = 2;  // send up to 3 keepalive packets out, then disconnect if no response
    setsockopt(fd, SOL_TCP, TCP_KEEPCNT, &count, sizeof(count));

    int interval = 2;   // send a keepalive packet out every 2 seconds (after the 5 second idle period)
    setsockopt(fd, SOL_TCP, TCP_KEEPINTVL, &interval, sizeof(interval));

    connect(&clientSocket, SIGNAL(connected()), this, SLOT(connectionEstablished()));
    connect(&clientSocket, SIGNAL(readyRead()), this, SLOT(readMessage()));

    for (int i = 0; i < dInpSize_R1; i++) {
        dInpArr_R1_bool_prev[i] = false;
        dInpArr_R1_bool[i] = false;
    }

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

void Client::startTransfer(QByteArray _datagram){

    if (clientSocket.state() == QAbstractSocket::ConnectedState)
        //client.write(MSG_HI.toLatin1().constData(), 9);
        clientSocket.write(_datagram);
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

    readCount = 0;
    cout <<  datagram.data() << endl;
    emit messageGot(datagram);

    // A 1 1 1 1 1 1 1 A 1 2 3 4 Z
    // 0 1 2 3 4 5 6 7 8 910111213
    bool validData = !datagram.isEmpty() && datagram.size() >= dataBufferSizeMin_R1 && datagram.size() <= dataBufferSizeMax_R1;

    if (datagram.size() >=2 )
        validData = (datagram.at(datagram.size()-2) == 'Z');
    else
        validData = false;

    // DI (&& DO) buffer check
    if (validData) {
        for (int k=1; k<=dInpSize_R1; k++) {   // +dOutSizeUsed
            if ( !QChar(datagram.at(k)).isDigit() ) {
                validData = false;
                cout << datagram.at(k);
            }
        }
    }

    if (validData) {

        //emit dataValid();

        for (int i = 0; i < dInpSize_R1; i++) {
            dInpArr_R1[i] = datagram.data()[i+1];
            dInpArr_R1_bool_prev[i] = dInpArr_R1_bool[i];
            dInpArr_R1_bool[i] = (dInpArr_R1[i] == '0') ? false : true;
        }

        //for (int i = 0; i < dOutSize; i++) { dOutReadArr[i] = datagram.data()[i + dInpSize]; }

        int pos = dInpSize_R1+1;         //+dOutSizeUsed;
        char ch = datagram.at(pos);
        //cout << ch << endl;       //DBG
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
                if ( x < aInpSize_R1 ) {
                    aInpArr_R1[x] = atoi(temp);
                    if (x==0) {
//                        float tempt = QString::number(aInpArr_R1[0]/100.0,'f',1).toFloat();
                        double tempt = aInpArr_R1[0]/100.0;
                        if ( abs(tempt - boilerTempPrev) >= boilerTempDelta ) {
                            boilerTempPrev = boilerTemp;
                            boilerTemp = tempt;
                            boilerTempDB = boilerTemp;
                            emit recordBoilerTemperature();
                        } else {
                            boilerTemp = tempt;
                        }
                    }
                    //cout << " " << aInpArr[x];
                } else
                    break;
                x++;
            }
        }

        if ( checkInputChange() )
            emit messageDecrypted();

        //cout << endl;
        //emit this->remote1IO();
    } else {
        if (!datagram.isEmpty()) {
            cout << "invalid data-remote1: " << datagram.size() << datagram.data() << endl;      //DBG
            //emit dataInValid();
        }
    }


}

bool Client::checkInputChange(){

    for (int i = 0; i < dInpSize_R1; i++)
        if ( dInpArr_R1[i] != dInpArr_R1_bool_prev[i] )
            return true;

    return false;
}
