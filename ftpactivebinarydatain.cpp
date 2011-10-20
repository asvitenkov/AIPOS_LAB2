#include "ftpactivebinarydatain.h"

FTPActiveBinaryDataIn::FTPActiveBinaryDataIn(QHostAddress aLocalAdress, int aLocalPort, QObject *parent):
    QTcpSocket(parent)
{
    setLocalPort(aLocalPort);
    setLocalAddress(aLocalAdress);
    connect(this,SIGNAL(readyRead()),this,SLOT(readIncomingData()));
    connect(this,SIGNAL(disconnected()),this,SLOT(connectionCloseByClientSlot()));
    file = NULL;
    writeStream = NULL;
}



void FTPActiveBinaryDataIn::readIncomingData(){
    //qDebug()<<"FTPActiveBinaryDataIn::readIncomingData()";
    if(state()==QAbstractSocket::ClosingState) return;
    //bytesAvailable()
    QByteArray tmp = readAll();
    if(writeStream->writeRawData(tmp.data(),tmp.size())==-1){
        // ошибка записи
        qDebug()<<"    emit errorStoreDataInFileSignal()";
        emit errorStoreDataInFileSignal();
    }
}

bool FTPActiveBinaryDataIn::setSaveFile(QFile *aFile){
    qDebug()<<"FTPActiveBinaryDataIn::setSaveFile";
    if(aFile!=NULL){
        file = aFile;
        writeStream = new QDataStream(file);
        return true;
    }
    else{
        return false;
    }
}



FTPActiveBinaryDataIn::~FTPActiveBinaryDataIn(){
    if(file){
        file->close();
        file->deleteLater();
    }
    if(writeStream) delete writeStream;
}


void FTPActiveBinaryDataIn::connectionCloseByClientSlot(){
    qDebug()<<"FTPActiveBinaryDataIn::connectionCloseByClientSlot()";
    //qDebug()<<"    "<<state();
    //disconnectFromHost();
    if(!bytesAvailable()){
        // информации на запись нет, передача закончилась успешно
        qDebug()<<"    emit storeDataInFileCompleteSuccessfulSignal()";
        file->close();
        emit storeDataInFileCompleteSuccessfulSignal();
    }
    else{
        qDebug()<<"    emit connectionCloseByClientSignal()";
        emit connectionCloseByClientSignal();
    }
}


void FTPActiveBinaryDataIn::abortConnection(){
    qDebug()<<"FTPActiveBinaryDataIn::abortConnection()";
    disconnect(this,SIGNAL(disconnected()),this,SLOT(connectionCloseByClientSlot()));
    connect(this,SIGNAL(disconnected()),this,SLOT(abortConnectionSlot()));
    abort();
}


void FTPActiveBinaryDataIn::abortConnectionSlot(){
    qDebug()<<"FTPActiveBinaryDataIn::abortConnectionSlot()";
    qDebug()<<"    emit abortCommandCompleteSignal()";
    emit abortCommandCompleteSignal();
}
