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
    bool checkPermissionCreateDir(QString);
    QString getUserWorkDir(QString);
    bool passiveMode;
    bool activMode;
    QString type;
    QList<int> pasvPort;
    QList<int> activPort;
    QDir currentDirectory;
    QString userDir;
    bool renameIsActive;
    QFileInfo renameObj;
    FTPDataOut *textOut;
    FTPDataOut *binaryOut;

signals:
    void sessionClose(int);

public slots:
    void transferFileCompleteSuccessfulSlot();
    void transferTextDataCompleteSuccessfulSlot();
    void errorTransferFileSlot();
    void errorTransferTextDataSlot();
    void transferTexdDataSocketClosedByUserSlot();
    void transferBinaryDataSocketClosedByUserSlot();

};

#endif // FTPSESSION_H
