#ifndef FTPACTIVETEXTDATAOUT_H
#define FTPACTIVETEXTDATAOUT_H

#include <QTcpSocket>
#include <QHostAddress>

class FTPActiveTextDataOut : public QTcpSocket
{
    Q_OBJECT
public:
    FTPActiveTextDataOut(QHostAddress aHostAdress,int aHostPort, QHostAddress aLocalAdress, int aLocalPort, QObject *parent=0);
    void sendTextData(QString aData);

private slots:
    void connectionClosedSlot();

signals:
    void errorSendTexdDataSignal();
    void connectionCloseByClientSignal();
    void sendTextDataSuccessfulSignal();
};

#endif // FTPACTIVETEXTDATAOUT_H
