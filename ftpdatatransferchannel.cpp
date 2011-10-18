#include "ftpdatatransferchannel.h"
#include <QMutex>

FTPDataTransferChannel::FTPDataTransferChannel(QHostAddress _localAdress, int _localPort, QObject *parent): QTcpServer(parent)
{
    textData.clear();
    transferText = false;
    transferFile = false;
    localAdress = _localAdress;
    listen(QHostAddress::Any,_localPort);
    transferSocket = NULL;
    sendTextDataFinished = false;
}


void FTPDataTransferChannel::incomingConnection(int handle){
    qDebug()<<"FTPDataTransferChannel::incomingConnection(int handle)";
    transferSocket = new QTcpSocket(this);
    transferSocket->setSocketDescriptor(handle);
    connect(transferSocket,SIGNAL(bytesWritten(qint64)),this,SLOT(sendTextDataSlot()));
    sendTextDataSlot();
    if(parent()!=NULL){
        connect(transferSocket,SIGNAL(disconnected()),this,SLOT(disconnectedSlot()));
    }

}


void FTPDataTransferChannel::setTextData(QString _data){
    transferText = true;
    textData = _data;
}

void FTPDataTransferChannel::setFileData(QString _fileName){
    transferFile = true;
    fileName = _fileName;
    //fileData = _file;
}

void FTPDataTransferChannel::closeConnectionByClientSlot(){
    qDebug()<<"FTPDataTransferChannel::connectionClosedByClientSlot()";
}


void FTPDataTransferChannel::sendTextData(QString aData){
//    if(transferSocket==NULL){
//        qDebug()<<"FTPDataTransferChannel::sendTextData transferSocket==NULL";
//        return;
//    }
//    if((transferSocket->write(aData.toLocal8Bit()))==-1){
//        qDebug()<<"emit errorTransferTexdData()";
//        emit errorTransferTexdData();
//    }
    textData+=aData;
}

void FTPDataTransferChannel::disconnectedSlot(){
    qDebug()<<"FTPDataTransferChannel::disconnectedSlot()";
    connect(transferSocket,SIGNAL(disconnected()),this,SLOT(disconnectedSlot()));
    qDebug()<<"closeConnectionByClient()";
    emit closeConnectionByClient();
    waitForNewConnection(30000);

}

void FTPDataTransferChannel::errorTransferBinaryDataSlot(){

}

void FTPDataTransferChannel::errorTransferTexdDataSlot(){

}


void FTPDataTransferChannel::closeTransferConnection(){
    qDebug()<<"FTPDataTransferChannel::closeTransferConnection()";
    if(parent()!=NULL){
        disconnect(transferSocket,SIGNAL(disconnected()),this,SLOT(disconnectedSlot()));
        connect(transferSocket,SIGNAL(disconnected()),this,SLOT(transferSuccessfulSlot()));
    }
    transferSocket->disconnectFromHost();
    transferSocket->deleteLater();
    transferSocket = NULL;
}


void FTPDataTransferChannel::abortTransferConnection(){
    transferSocket->abort();
    transferSocket->deleteLater();
    transferSocket = NULL;
}



void FTPDataTransferChannel::transferSuccessfulSlot(){
    qDebug()<<"FPDataTransferChannel::transferSuccessfulSlot()";
    qDebug()<<"emit transferSuccessful()";
    emit transferSuccessful();
}



void FTPDataTransferChannel::sendTextDataSlot(){
    qDebug()<<"FTPDataTransferChannel::sendTextDataSlot()";
    if(textData.isEmpty()) return;
    QMutex mutex;
    mutex.lock();
    if(transferSocket->write(textData.toLocal8Bit()) == -1){
        qDebug()<<"emit errorTransferTexdData()";
        emit errorTransferTexdData();
    }
    textData.clear();
    mutex.unlock();
    if(sendTextDataFinished)
        emit transferSuccessful();
}


void FTPDataTransferChannel::endTextData(){
    sendTextDataFinished = true;
}
