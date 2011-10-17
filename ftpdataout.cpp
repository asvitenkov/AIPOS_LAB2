#include "ftpdataout.h"
#include <QDebug>
#include <QFtp>

FTPDataOut::FTPDataOut(QHostAddress _hostAdress, int _hostPort, int _localPort, QObject *parent) :
    QTcpSocket(parent), hostAdress(_hostAdress), hostPort(_hostPort)
{

    setLocalPort(_localPort);
    setLocalAddress(_hostAdress);
    connectToHost(hostAdress,hostPort,QIODevice::ReadWrite);
    bytesAlredyWritten = false;
}



bool FTPDataOut::setPort(int _port){
    setLocalPort(_port);
    return true;
}


void FTPDataOut::sendTextData(QString _data){
    write(_data.toLocal8Bit());

}

void FTPDataOut::sendTextDataSlot(){
    write(QByteArray(data.toUtf8()));
}

void FTPDataOut::sendBinaryData(QFile *file){
    if(file) writtenFile = file;
    connect(this,SIGNAL(bytesWritten(qint64)),this,SLOT(binaryDataWrittenSlot(qint64)));
    binaryDataWrittenSlot(0);
}

void FTPDataOut::binaryDataWrittenSlot(qint64 bytes){
    qDebug()<<bytesToWrite();
    char buffer[blockSize];
    if(!writtenFile->atEnd()){
        qint64 sizeRead = writtenFile->read(buffer, blockSize);
        if (sizeRead > 0)
        {
            qint64 sizeWrite = 0;
            do
            {
                qint64 size = write(buffer + sizeWrite, sizeRead - sizeWrite);

                if (size < 0)
                    return;
                sizeWrite += size;
            } while (sizeWrite < sizeRead);
        }
    }
    else{
        disconnect(this,SIGNAL(bytesWritten(qint64)),this,SLOT(binaryDataWrittenSlot(qint64)));
        emit tansferFileCompleteSeccessfulSignal();
        close();
    }
}
