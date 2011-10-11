#ifndef FTPSERVER_H
#define FTPSERVER_H

#include <QTcpServer>
#include "ftpsession.h"
#include <QHash>
#include <QtNetwork>

class FTPServer : public QTcpServer
{
    Q_SLOTS
public:
    FTPServer(QHostAddress _serverAdress, int port,QObject *parent = 0 );
    void sessionClose(int _descriptor);

private:
    void incomingConnection(int socketID);
    QHostAddress serverAdress;
    QHash< int, FTPSession *> sessionsList;



public slots:
    void sessionCloseSLOT(int _descriptor);

};

#endif // FTPSERVER_H
