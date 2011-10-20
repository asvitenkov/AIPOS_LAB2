#ifndef FTPACTIVEBINARYDATAOUT_H
#define FTPACTIVEBINARYDATAOUT_H

#include <QTcpSocket>
#include <QHostAddress>
#include <QFile>
#include <QDataStream>

class FTPActiveBinaryDataOut : public QTcpSocket
{
    Q_OBJECT

    static const qint64 blockSize = 8192;

public:
    explicit FTPActiveBinaryDataOut(QHostAddress aHostAdress,int aHostPort, QHostAddress aLocalAdress, int aLocalPort, QObject *parent=0);
    ~FTPActiveBinaryDataOut();
    void sendFile(QDataStream *aStream);
    void abortConnection();

signals:
    void errorSendBinaryDataSignal();
    void connectionCloseByClientSignal();
    void sendBinaryDataSuccessfulSignal();
    void abortConnectionSignal();

public slots:
private slots:
    void connectionCloseByClientSlot();
    void continueTransferBinaryDataSlot();
    void abortConnectionSlot();

private:
    QDataStream *outStream;

};

#endif // FTPACTIVEBINARYDATAOUT_H
