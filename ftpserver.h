#ifndef FTPSERVER_H
#define FTPSERVER_H

#include <QTcpServer>
#include "ftpsession.h"

class FTPServer : public QTcpServer
{
public:
    FTPServer(QHostAddress _serverAdress, int port,QObject *parent = 0 );

private:
    void incomingConnection(int socketID);
    QHostAddress serverAdress;
};

#endif // FTPSERVER_H
