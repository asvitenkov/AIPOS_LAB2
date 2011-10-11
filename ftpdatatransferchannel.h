#ifndef FTPDATATRANSFERCHANNEL_H
#define FTPDATATRANSFERCHANNEL_H

#include <QTcpServer>
#include <QFile>
#include "ftpdataout.h"

class FTPDataTransferChannel : public QTcpServer
{
public:
    FTPDataTransferChannel(QObject *parent = 0);

private:
    void incomingConnection(int handle);
    void setTextData(QString _data);
    void setFileData(QString _fileName);
    QString textData;
    QString fileName;
    QFile fileData;
    bool transferText;
    bool transferFile;
    QHostAddress hostAdress;
    int hostPort;
};

#endif // FTPDATATRANSFERCHANNEL_H
