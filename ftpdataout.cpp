#include "ftpdataout.h"
#include <QDebug>
#include <QFtp>

FTPDataOut::FTPDataOut(QHostAddress _hostAdress, int _hostPort, int _localPort, QObject *parent) :
    QTcpSocket(parent), hostAdress(_hostAdress), hostPort(_hostPort)
{
    connect(this,SIGNAL(disconnected()),this,SLOT(clientCloseConnectionSlot()));
    setLocalPort(_localPort);
    setLocalAddress(_hostAdress);
    connectToHost(hostAdress,hostPort,QIODevice::ReadWrite);

    transferIsActive = false;
    setSocketOption(QAbstractSocket::LowDelayOption,0);
}



bool FTPDataOut::setPort(int _port){
    setLocalPort(_port);
    return true;
}


void FTPDataOut::sendTextData(QString _data){
    if(write(_data.toLocal8Bit())==-1){
        qDebug()<<"FTPDataOut emit errorTransferTextData()";
        emit errorTransferTextData();
    }


}

//void FTPDataOut::sendTextDataSlot(){
//    write(QByteArray(data.toUtf8()));
//}

void FTPDataOut::sendBinaryData(QFile *file){
    if(file) writtenFile = file;
    else{
        // сигнал о ошибке
    }
    connect(this,SIGNAL(bytesWritten(qint64)),this,SLOT(binaryDataWrittenSlot(qint64)));
    transferIsActive = true;
    binaryDataWrittenSlot(0);
}

void FTPDataOut::binaryDataWrittenSlot(qint64 bytes){
    if(!transferIsActive) return;
    char buffer[blockSize];
    if(!writtenFile->atEnd()){
        qint64 sizeRead = writtenFile->read(buffer, blockSize);
        if (sizeRead > 0)
        {
            qint64 sizeWrite = 0;
            do
            {
                qint64 size = write(buffer + sizeWrite, sizeRead - sizeWrite);

                if (size < 0){
                    // ошибка, сигнал об ошибке
                    qDebug()<<"FTPDataOut emit errorTransferBinaryData();";
                    emit errorTransferBinaryData();
                    return;
                }
                sizeWrite += size;
            } while (sizeWrite < sizeRead);
        }
    }
    else{
        disconnect(this,SIGNAL(bytesWritten(qint64)),this,SLOT(binaryDataWrittenSlot(qint64)));
        transferIsActive = false;
        qDebug()<<"FTPDataOut emit tansferFileCompleteSeccessfulSignal()";
        emit tansferFileCompleteSeccessfulSignal();

    }
}



void FTPDataOut::clientCloseConnectionSlot(){
    qDebug()<<"FTPDataOut emit closeConnectionByClient()";
    emit closeConnectionByClient();
}
