#ifndef FTPDATATRANSFERCHANNEL_H
#define FTPDATATRANSFERCHANNEL_H

#include <QTcpServer>
#include <QFile>
#include "ftpdataout.h"

class FTPDataTransferChannel : public QTcpServer
{
    Q_OBJECT

public:
    FTPDataTransferChannel(QHostAddress _localAdress, int _localPort, QObject *parent = 0);
    void sendTextData(QString aData);
    void closeTransferConnection();
    void abortTransferConnection();
    void endTextData();

private:
    void incomingConnection(int handle);
    void setTextData(QString _data);
    void setFileData(QString _fileName);
    QString textData;
    QString fileName;
    QFile fileData;
    bool transferText;
    bool transferFile;
    QHostAddress localAdress;
    QTcpSocket *transferSocket;
    int localPort;
    bool sendTextDataFinished;

signals:
    void disconnected();
    void errorTransferTexdData();
    void errorTransferBinaryData();
    void closeConnectionByClient();
    void transferTextDataCompleteSuccessful();
    void transferBinaryDataCompleteSuccessful();
    void transferSuccessful();

private slots:
    void disconnectedSlot();
    void errorTransferTexdDataSlot();
    void errorTransferBinaryDataSlot();
    void closeConnectionByClientSlot();
//    void transferTextDataCompleteSuccessfulSlot();
//    void transferBinaryDataCompleteSuccessfulSlot();
    void transferSuccessfulSlot();
    void sendTextDataSlot();


};

#endif // FTPDATATRANSFERCHANNEL_H
