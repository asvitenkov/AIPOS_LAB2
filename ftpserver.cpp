#include "ftpserver.h"
#include <QTcpSocket>

FTPServer::FTPServer(QObject *parent): QTcpServer(parent)
{
    //this->listen(QHostAddress::LocalHost,21);
}

void FTPServer::incomingConnection(int socketID){
    FTPSession *session = new FTPSession(this);
    session->setSocketDescriptor(socketID);
    session->write(QByteArray(QString("220 Welcome to SveT's FTP Server\r\n").toUtf8()));
    //session->connectToHost(this->);
}
