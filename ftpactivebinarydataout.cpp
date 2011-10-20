#include "ftpactivebinarydataout.h"

FTPActiveBinaryDataOut::FTPActiveBinaryDataOut(QHostAddress aHostAdress, int aHostPort, QHostAddress aLocalAdress, int aLocalPort, QObject *parent) :
    QTcpSocket(parent)
{
    setLocalPort(aLocalPort);
    setLocalAddress(aLocalAdress);
    connect(this,SIGNAL(disconnected()),this,SLOT(connectionCloseByClientSlot()));
    connectToHost(aHostAdress,aHostPort,QIODevice::ReadWrite);
    outStream = NULL;
}

void FTPActiveBinaryDataOut::sendFile(QDataStream *aStream){
    //qDebug()<<"FTPActiveBinaryDataOut::sendFile";
    if(aStream!=NULL){
        outStream = aStream;
        connect(this,SIGNAL(bytesWritten(qint64)),this,SLOT(continueTransferBinaryDataSlot()));
        continueTransferBinaryDataSlot();
    }
    else{
        // нулевая ссылка
        // послать ошибку
        //qDebug()<<"FTPActiveBinaryDataOut::sendFile\n    emit errorSendBinaryDataSignal()";
        emit errorSendBinaryDataSignal();
        return;
    }
}



void FTPActiveBinaryDataOut::continueTransferBinaryDataSlot(){
    char buffer[blockSize];
    if(!outStream->atEnd()){
        qint64 sizeRead = outStream->readRawData(buffer, blockSize);
        if (sizeRead > 0)
        {
            qint64 sizeWrite = 0;
            do
            {
                qint64 size = write(buffer + sizeWrite, sizeRead - sizeWrite);

                if (size < 0){
                    // ошибка, сигнал об ошибке
                    //qDebug()<<"FTPActiveBinaryDataOut::continueTransferBinaryDataSlot\n    emit errorTransferBinaryData();";
//                    outStream->device()->close();
//                    outStream->device()->deleteLater();
                    emit errorSendBinaryDataSignal();
                    return;
                }
                sizeWrite += size;
            } while (sizeWrite < sizeRead);
        }
    }
    else{
//        outStream->device()->close();
//        outStream->device()->deleteLater();
        disconnect(this,SIGNAL(bytesWritten(qint64)),this,SLOT(continueTransferBinaryDataSlot()));
        //qDebug()<<"FTPActiveBinaryDataOut::continueTransferBinaryDataSlot\n    emit sendBinaryDataSuccessfulSignal()";
        emit sendBinaryDataSuccessfulSignal();
    }
}


void FTPActiveBinaryDataOut::connectionCloseByClientSlot(){
    //qDebug()<<"FTPActiveBinaryDataOut::connectionCloseByClientSlot";
    disconnect(this,SIGNAL(bytesWritten(qint64)),this,SLOT(continueTransferBinaryDataSlot()));
//    outStream->device()->close();
//    outStream->device()->deleteLater();
    //qDebug()<<"    emit connectionCloseByClientSignal()";
    emit connectionCloseByClientSignal();
}



FTPActiveBinaryDataOut::~FTPActiveBinaryDataOut(){
    if(outStream!=NULL){
        outStream->device()->close();
        outStream->device()->deleteLater();
        delete outStream;
    }
}


void FTPActiveBinaryDataOut::abortConnection(){
    //qDebug()<<"FTPActiveBinaryDataOut::abortConnection()";
    disconnect(this,SIGNAL(disconnected()),this,SLOT(connectionCloseByClientSlot()));
    connect(this,SIGNAL(disconnected()),this,SLOT(abortConnectionSlot()));
    abort();
}


void FTPActiveBinaryDataOut::abortConnectionSlot(){
    emit abortConnectionSignal();
}
