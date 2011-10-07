#ifndef FTPSERVER_H
#define FTPSERVER_H

#include <QTcpServer>
#include "ftpsession.h"

class FTPServer : public QTcpServer
{
public:
    FTPServer(QObject *parent = 0 );

private:
    void incomingConnection(int socketID);
};

#endif // FTPSERVER_H
