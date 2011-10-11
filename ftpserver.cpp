#include "ftpserver.h"
#include <QTcpSocket>

FTPServer::FTPServer(QHostAddress _serverAdress, int port, QObject *parent): QTcpServer(parent), serverAdress(_serverAdress)
{
    //sessionsList.clear();
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
    connect(session,SIGNAL(sessionClose(int)),this,SLOT(sessionCloseSLOT(int)));
    session->setSocketDescriptor(socketID);
    qDebug()<<socketID;
    session->write(QByteArray(QString("200 SveT FTP Service\r\n").toUtf8()));
    sessionsList.insert(socketID,session);

}


void FTPServer::sessionClose(int _descriptor){
    if(sessionsList.contains(_descriptor))
       delete sessionsList[_descriptor];
    qDebug()<<"Session with descr "<<_descriptor<<" close";
}
