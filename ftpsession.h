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
    bool isAnonymous;
    QString userName;
    QString userPassword;
    void parsingQuery(QString);
    void sendToClient(QString);
    bool checkUserName(QString);
    bool checkUserPassword(QString);
    QString getUserWorkDir(QString);
    QString type;


};

#endif // FTPSESSION_H
