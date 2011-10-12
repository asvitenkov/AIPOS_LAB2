#ifndef FTPSESSION_H
#define FTPSESSION_H

#include <QTcpSocket>
#include <QDataStream>
#include <QByteArray>
#include <QString>
#include <QList>
#include <QDir>
#include <QFileInfo>
#include <QFileInfoList>
#include "ftpdataout.h"

//#include "ftpserver.h"

class FTPServer;

class FTPSession : public QTcpSocket
{

    Q_OBJECT

public:
    FTPSession(QObject *parent = 0);
    bool setCurrentDirectory(QString);
    void setUserDir(QString userDirName);


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
    bool passiveMode;
    bool activMode;
    QString type;
    QList<int> pasvPort;
    QList<int> activPort;
    QDir currentDirectory;
    QString userDir;


signals:
    void sessionClose(int);


};

#endif // FTPSESSION_H
