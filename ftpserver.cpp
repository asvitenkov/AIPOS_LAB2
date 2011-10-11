#include "ftpserver.h"
#include <QTcpSocket>

FTPServer::FTPServer(QHostAddress _serverAdress, int port, QObject *parent): QTcpServer(parent), serverAdress(_serverAdress)
{
    this->listen(QHostAddress::Any,port);
    // нужна проверка, а не занят ли этот порт!!!!1
//    $$$$$$$$$$$
//    $$$$$$$$$$$
//    $$$$$$$$$$$
//            $$$$$$$$$$$
//            $$$$$$$$$$$
//            $$$$$$$$$$$
}

void FTPServer::incomingConnection(int socketID){
    FTPSession *session = new FTPSession(this);
    session->setSocketDescriptor(socketID);
    session->write(QByteArray(QString("200 SveT FTP Service\r\n").toUtf8()));
}
