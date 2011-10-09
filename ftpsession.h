#ifndef FTPSESSION_H
#define FTPSESSION_H

#include <QTcpSocket>
#include <QDataStream>
#include <QByteArray>
#include <QString>

class FTPSession : public QTcpSocket
{
    Q_OBJECT

public:
    FTPSession(QObject *parent = 0);

private slots:
    void readClient();

private:
    bool loginIsOk;
    bool passIsOk;

};

#endif // FTPSESSION_H
