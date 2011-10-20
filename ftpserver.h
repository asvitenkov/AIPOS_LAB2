#ifndef FTPSERVER_H
#define FTPSERVER_H

#include <QTcpServer>
#include "ftpsession.h"
#include <QHash>

class FTPServer : public QTcpServer
{

    Q_OBJECT

public:
    //FTPServer(QHostAddress _serverAdress, int port,QObject *parent = 0 );
    FTPServer(QObject *parent = 0 );
    //void sessionClose(int _descriptor);
    void setServerAdress(QHostAddress _adress);
    void startServer();

private:
    void incomingConnection(int socketID);
    QHostAddress serverAdress;
    QHash< int, FTPSession *> sessionsList;



public slots:
    void sessionCloseSLOT(int _descriptor);
    void addRecordToLogSlot(QString aData);


signals:
    void addRecordToLogSignal(QString aData);
};

#endif // FTPSERVER_H
