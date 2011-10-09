#include "ftpserver.h"
#include <QTcpSocket>

FTPServer::FTPServer(QObject *parent): QTcpServer(parent)
{
    this->listen(QHostAddress::Any,21);
}

void FTPServer::incomingConnection(int socketID){
    FTPSession *session = new FTPSession(this);
    //QTcpSocket *tmp = new QTcpSocket(this);
    session->setSocketDescriptor(socketID);
}
