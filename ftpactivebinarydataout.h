#ifndef FTPACTIVEBINARYDATAOUT_H
#define FTPACTIVEBINARYDATAOUT_H

#include <QTcpSocket>
#include <QHostAddress>
#include <QFile>

class FTPActiveBinaryDataOut : public QTcpSocket
{
    Q_OBJECT

    static const qint64 blockSize = 8192;

public:
    explicit FTPActiveBinaryDataOut(QHostAddress aHostAdress,int aHostPort, QHostAddress aLocalAdress, int aLocalPort, QObject *parent=0);
    void sendFile(QFile *aFile);

signals:
    void errorSendBinaryDataSignal();
    void connectionCloseByClientSignal();
    void sendBinaryDataSuccessfulSignal();

public slots:
private slots:
    void connectionCloseByClientSlot();
    void continueTransferBinaryDataSlot();

private:
    QFile *file;

};

#endif // FTPACTIVEBINARYDATAOUT_H
