#ifndef FTPDATAOUT_H
#define FTPDATAOUT_H

#include <QTcpSocket>
#include <QHostAddress>
#include <QString>
#include <QByteArray>
#include <QFile>

class FTPDataOut : public QTcpSocket
{
    Q_OBJECT


    static const qint64 blockSize = 8192*2;

public:
    FTPDataOut(QHostAddress _hostAdress, int _hostPort, int _localPort,QObject *parent = 0);
    //FTPDataOut(QObject *parent = 0);
    void sendTextData(QString _data);
    bool setPort(int _port);
    void sendBinaryData(QFile *file);


signals:
    void tansferFileCompleteSeccessfulSignal();

public slots:
    void sendTextDataSlot();

private slots:
    void binaryDataWrittenSlot(qint64);

private:
    QHostAddress hostAdress;
    int hostPort;
    QString data;
    bool bytesAlredyWritten;
    QFile *writtenFile;


};

#endif // FTPDATAOUT_H
