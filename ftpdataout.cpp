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


void FTPDataOut::sendBinaryData(QFile *file){
    //file.open(QFile::ReadOnly);
        QDataStream read(file);
        int lBytes = 0;
        char * ch;
        ch = (char*)malloc(sizeof(char) * 1024);
        ch[1023] = '\0';
        while(!read.atEnd()){
          int l = read.readRawData(ch, sizeof(char)*1023);
          QByteArray ba(ch, sizeof(char)*l);

          lBytes += write(ba, sizeof(char)*l);
          //flush();
          if (-1 == lBytes){
            qWarning() << "Error";
            //close(); //Закрываем устройство сокета
            return;
          }
          //float procentage = ((float)lBytes / package.filelength) * 100;
          //emit setProcentage((int)procentage);
        }//while(!readEnd())
        free((void*)ch);
        close();
        qDebug()<<"FILE END";
}
