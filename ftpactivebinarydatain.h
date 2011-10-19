#ifndef FTPACTIVEBINARYDATAIN_H
#define FTPACTIVEBINARYDATAIN_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QFile>
#include <QDataStream>
#include <QByteArray>

class FTPActiveBinaryDataIn : public QTcpSocket
{
    Q_OBJECT
public:
    explicit FTPActiveBinaryDataIn(QHostAddress aLocalAdress, int aLocalPort,QObject *parent = 0);
    ~FTPActiveBinaryDataIn();
    bool setSaveFile(QFile *aFile);
    void abortConnection();

signals:
    void errorStoreDataInFileSignal();
    void storeDataInFileCompleteSuccessfulSignal();
    void connectionCloseByClientSignal();
    void abortCommandCompleteSignal();
public slots:

private slots:
    void readIncomingData();
    void connectionCloseByClientSlot();
    void abortConnectionSlot();

private:
    QFile *file;
    QDataStream *writeStream;
};

#endif // FTPACTIVEBINARYDATAIN_H
