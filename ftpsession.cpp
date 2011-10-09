#include "ftpsession.h"
#include <QDebug>
#include <QHostAddress>


FTPSession::FTPSession(QObject *parent):QTcpSocket(parent)
{
    connect(this,SIGNAL(readyRead()),this,SLOT(readClient()));
    passIsOk = false;
    loginIsOk = false;
}


void FTPSession::readClient(){
    //QDataStream in(this);
    //in.setVersion(QDataStream::Qt_4_7);
    //qDebug()<<bytesAvailable();
    //in.setVersion();
    QByteArray array;
    //array=readAll();
    QString str = QString::fromUtf8(this->readAll()).replace("\r\n","");
    //in>>str;

    qDebug()<<str<<endl;
    //close();
}
