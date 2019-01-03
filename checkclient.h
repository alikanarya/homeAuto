#ifndef CHECKCLIENT_H
#define CHECKCLIENT_H

#include <QThread>
#include "client.h"

class checkClient : public QThread {

    Q_OBJECT

public:

    Client *clientPtr;

    checkClient(Client *_clientPtr);
    ~checkClient();

    void stop();

public slots:

    void connect();
    void transferToTCPServer();

protected:

    void run();

private:

    volatile bool stopped;

signals:

    void Connected();
    void notConnected();

};

#endif // CHECKCLIENT_H
