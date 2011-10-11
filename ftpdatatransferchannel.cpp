#include "ftpdatatransferchannel.h"

FTPDataTransferChannel::FTPDataTransferChannel(QObject *parent): QTcpServer(parent)
{
    textData.clear();
    transferText = false;
    transferFile = false;
    hostAdress = QHostAddress::LocalHost;
    hostPort = 0;
}


void FTPDataTransferChannel::incomingConnection(int handle){
    if(transferText && transferFile){
        qDebug()<<"WARNIBG TRANSFER FILE AND TEXT AT THE SAME TIME ERROR";
        return;
    }
    if(!transferText && !transferFile){
        qDebug()<<"WARNIBG NOT TRANSFER FILE AND NOT TRANSFER TEXT AT THE SAME TIME ERROR";
        return;
    }
    if(transferText){

        return;
    }
    if(transferFile){

        return;
    }
    return;

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
