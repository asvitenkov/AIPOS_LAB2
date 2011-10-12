#include "ftpdataout.h"
#include <QDebug>

FTPDataOut::FTPDataOut(QHostAddress _hostAdress, int _hostPort, int _localPort, QObject *parent) :
    QTcpSocket(parent), hostAdress(_hostAdress), hostPort(_hostPort)
{

    setLocalPort(_localPort);
    setLocalAddress(_hostAdress);
    connectToHost(hostAdress,hostPort,QIODevice::ReadWrite);
}



bool FTPDataOut::setPort(int _port){
    setLocalPort(_port);
    return true;
}


void FTPDataOut::sendTextData(QString _data){
    write(_data.toLocal8Bit());

}

void FTPDataOut::sendTextDataSlot(){
    //qDebug()<<"FTPDataOut::sendTextDataSlot()";
    write(QByteArray(data.toUtf8()));
}
