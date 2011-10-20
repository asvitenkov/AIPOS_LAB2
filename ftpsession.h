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
//#include "ftpdataout.h"
//#include "ftpdatatransferchannel.h"

#include "ftpactivetextdataout.h"
#include "ftpactivebinarydataout.h"
#include "ftpactivebinarydatain.h"

class FTPServer;

class FTPSession : public QTcpSocket
{

    Q_OBJECT

public:
    FTPSession(QObject *parent = 0);
    ~FTPSession();
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
    QString getAbsolutePuth(QString aDirName);
    bool passiveMode;
    bool activMode;
    QString type;
    QList<int> pasvPort;
    QList<int> activPort;
    QDir currentDirectory;
    QString userDir;
    bool renameIsActive;
    QFileInfo renameObj;

    FTPActiveTextDataOut *activeTextDataOut;
    FTPActiveBinaryDataOut *activeBinaryDataOut;
    FTPActiveBinaryDataIn *activeBinaryDataIn;


signals:
    void sessionClose(int);

public slots:
    void activeTransferTextDataSuccessfulSlot();
    void activeTransferTextDataErrorSlot();
    void activeTransferTextDataConnectionCloseByClientSlot();
    void activeTransferTextDataAbortSlot();

    void activeTransferBinaryDataSuccessfulSlot();
    void activeTransferBinaryDataErrorSlot();
    void activeTransferBinaryDataConnectionCloseByClientSlot();
    void activeTransferBinaryDataAbortSlot();

    //void activeReceptionBinaryDataErrorOpenFileSlot();
    void activeStoreBinaryDataCompleteSuccessfulSlot();
    void activeStoreBinaryDataConnectionCloseByClientSlot();
    void activeStoreBinaryDataErrorSlot();
    void activeStoreBinaryDataAbortSlot();
};

#endif // FTPSESSION_H
