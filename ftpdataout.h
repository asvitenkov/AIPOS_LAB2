#ifndef FTPDATAOUT_H
#define FTPDATAOUT_H

#include <QTcpSocket>
#include <QHostAddress>
#include <QString>
#include <QByteArray>

class FTPDataOut : public QTcpSocket
{
    Q_OBJECT
public:
    FTPDataOut(QHostAddress _hostAdress, int _hostPort, int _localPort,QObject *parent = 0);
    //FTPDataOut(QObject *parent = 0);
    void sendTextData(QString _data);
    bool setPort(int _port);


signals:


public slots:
    void sendTextDataSlot();

private:
    QHostAddress hostAdress;
    int hostPort;
    QString data;
};

#endif // FTPDATAOUT_H
