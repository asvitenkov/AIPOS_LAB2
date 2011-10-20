#include "ftpserver.h"
#include <QTcpSocket>

//FTPServer::FTPServer(QHostAddress _serverAdress, int port, QObject *parent): QTcpServer(parent), serverAdress(_serverAdress)
FTPServer::FTPServer(QObject *parent): QTcpServer(parent)
{
    sessionsList.clear();
}

void FTPServer::incomingConnection(int socketID){
    FTPSession *session = new FTPSession(this);
    connect(session,SIGNAL(sessionClose(int)),this,SLOT(sessionCloseSLOT(int)));
    connect(session,SIGNAL(sendDataToLog(QString)),this,SLOT(addRecordToLogSlot(QString)));
    session->setSocketDescriptor(socketID);
    //qDebug()<<socketID;
    session->write(QByteArray(QString("200 SveT FTP Service\r\n").toUtf8()));
    sessionsList.insert(socketID,session);

}


void FTPServer::sessionCloseSLOT(int _descriptor){
    if(sessionsList.contains(_descriptor))
       delete sessionsList[_descriptor];
    qDebug()<<"Session with descr "<<_descriptor<<" close";
}


void FTPServer::setServerAdress(QHostAddress _adress){
    serverAdress = _adress;
}


void FTPServer::startServer(){

}


void FTPServer::addRecordToLogSlot(QString aData){
    emit addRecordToLogSignal(aData.replace("\r\n",""));
}
