#include "ftpactivetextdataout.h"

FTPActiveTextDataOut::FTPActiveTextDataOut(QHostAddress aHostAdress, int aHostPort, QHostAddress aLocalAdress, int aLocalPort, QObject *parent)
    :QTcpSocket(parent)
{
    setLocalPort(aLocalPort);
    setLocalAddress(aLocalAdress);
    connect(this,SIGNAL(disconnected()),this,SLOT(connectionClosedSlot()));
    connectToHost(aHostAdress,aHostPort,QIODevice::ReadWrite);
}

void FTPActiveTextDataOut::sendTextData(QString aData){
    //qDebug()<<"FTPActiveTextDataOut::sendTextData";
    if(write(aData.toLocal8Bit())==-1){
        qDebug()<<"FTPActiveTextDataOut emit errorSendTexdDataSignal()";
        emit errorSendTexdDataSignal();
    }
}

void FTPActiveTextDataOut::connectionClosedSlot(){
    //qDebug()<<"FTPActiveTextDataOut::connectionClosedSlot()";
    if(bytesToWrite()!=0){
        // идёт запись а соединение закрыто
        emit connectionCloseByClientSignal();
    }
    else{
        // записи нет и соединение закрывается
        //qDebug()<<"FTPActiveTextDataOut emit sendTextDataSuccessfulSignal()";
        emit sendTextDataSuccessfulSignal();
    }
}
