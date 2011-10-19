#include "ftpactivebinarydataout.h"

FTPActiveBinaryDataOut::FTPActiveBinaryDataOut(QHostAddress aHostAdress, int aHostPort, QHostAddress aLocalAdress, int aLocalPort, QObject *parent) :
    QTcpSocket(parent)
{
    setLocalPort(aLocalPort);
    setLocalAddress(aLocalAdress);
    connect(this,SIGNAL(disconnected()),this,SLOT(connectionCloseByClientSlot()));
    connectToHost(aHostAdress,aHostPort,QIODevice::ReadWrite);
    file = NULL;
}

void FTPActiveBinaryDataOut::sendFile(QFile *aFile){
    qDebug()<<"FTPActiveBinaryDataOut::sendFile";
    if(aFile!=NULL){
        file = aFile;
        connect(this,SIGNAL(bytesWritten(qint64)),this,SLOT(continueTransferBinaryDataSlot()));
        continueTransferBinaryDataSlot();
    }
    else{
        // нулевая ссылка
        // послать ошибку
        qDebug()<<"FTPActiveBinaryDataOut::sendFile emit errorSendBinaryDataSignal()";
        emit errorSendBinaryDataSignal();
        return;
    }
    //connect(this,SIGNAL(bytesWritten(qint64)),this,SLOT(continueTransferBinaryDataSlot()));
    //continueTransferBinaryDataSlot();
}



void FTPActiveBinaryDataOut::continueTransferBinaryDataSlot(){
    char buffer[blockSize];
    if(!file->atEnd()){
        qint64 sizeRead = file->read(buffer, blockSize);
        if (sizeRead > 0)
        {
            qint64 sizeWrite = 0;
            do
            {
                qint64 size = write(buffer + sizeWrite, sizeRead - sizeWrite);

                if (size < 0){
                    // ошибка, сигнал об ошибке
                    qDebug()<<"################";//<<file->errorString();
                    qDebug()<<"FTPActiveBinaryDataOut::continueTransferBinaryDataSlot emit errorTransferBinaryData();";
                    file->close();
                    delete file;
                    emit errorSendBinaryDataSignal();
                    return;
                }
                sizeWrite += size;
            } while (sizeWrite < sizeRead);
        }
    }
    else{
        file->close();
        file->deleteLater();
        //delete file;
        disconnect(this,SIGNAL(bytesWritten(qint64)),this,SLOT(continueTransferBinaryDataSlot()));
        qDebug()<<"FTPActiveBinaryDataOut::continueTransferBinaryDataSlot emit sendBinaryDataSuccessfulSignal()";
        emit sendBinaryDataSuccessfulSignal();
    }
}


void FTPActiveBinaryDataOut::connectionCloseByClientSlot(){
    disconnect(this,SIGNAL(bytesWritten(qint64)),this,SLOT(continueTransferBinaryDataSlot()));
    file->close();
    file->deleteLater();
    //delete file;
    emit connectionCloseByClientSignal();
}
