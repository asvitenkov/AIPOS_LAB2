#ifndef FTPDATAOUT_H
#define FTPDATAOUT_H

#include <QTcpSocket>
#include <QHostAddress>
#include <QString>
#include <QByteArray>
#include <QFile>

#include "ftpactivebinarydataout.h"

class FTPDataOut : public QTcpSocket
{
    Q_OBJECT


    static const qint64 blockSize = 8192;

public:
    FTPDataOut(QHostAddress _hostAdress, int _hostPort, int _localPort,QObject *parent = 0);
    //FTPDataOut(QObject *parent = 0);
    void sendTextData(QString _data);
    bool setPort(int _port);
    void sendBinaryData(QFile *file);


signals:
    void tansferFileCompleteSeccessfulSignal();
    void transferTextCompleteSeccussful();
    void errorTransferBinaryData();
    void errorTransferTextData();
    void closeConnectionByClient();

public slots:
//    void sendTextDataSlot();

private slots:
    void binaryDataWrittenSlot(qint64);
    void clientCloseConnectionSlot();

private:
    QHostAddress hostAdress;
    int hostPort;
    QString data;
    bool bytesAlredyWritten;
    QFile *writtenFile;
    bool transferIsActive;


};

#endif // FTPDATAOUT_H
