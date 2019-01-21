#ifndef CLIENT_H
#define CLIENT_H

#include <QtNetwork>
#include <QObject>
#include <QString>
#include <QTcpSocket>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

class Client: public QObject{

    Q_OBJECT

public:

    Client(QObject* parent = 0);
    ~Client();
    void setHost(QString hostAddress, quint16 hostPort);
    void start();
    bool checkInputChange();


    QTcpSocket clientSocket;
    quint16 clientPort = 8888;
    QString clientAddress = "locahost";
    QHostAddress host;

    bool connected = false;
    QByteArray datagram;
    int readCount = 0;
    bool autoDisconnect = false;

public slots:

    void startTransfer();
    void startTransfer(QByteArray _datagram);
    void connectionEstablished();
    void readMessage();

private:


signals:

    void clientConnected();
    void messageGot(QByteArray);
    void messageDecrypted();
    void recordBoilerTemperature();

};

#endif // CLIENT_H
