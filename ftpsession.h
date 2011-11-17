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

#include "ftpactivetextdataout.h"
#include "ftpactivebinarydataout.h"
#include "ftpactivebinarydatain.h"

class FTPServer;

/**
 * @brief The class is intended for management of session of client on FTP server
 *
 */
class FTPSession : public QTcpSocket
{

    Q_OBJECT

public:
/**
 * @brief Creates a FTPSession object
 *
 * @param parent The parent object
 */
    FTPSession(QObject *parent = 0);
    /**
     * @brief Destroys the client session
     *
     */
    ~FTPSession();
    /**
     * @brief Set current user work directory
     *
     * @param QString absolute file puth
     */
    bool setCurrentDirectory(QString);
    /**
     * @brief Set user work directory
     *
     * @param userDirName absolute file puth
     */
    void setUserDir(QString userDirName);



private slots:
    /**
     * @brief Slot for read incoming data
     *
     */
    void readClient();

private:
    bool loginIsOk; /**< TODO */
    bool passIsOk; /**< TODO */
    bool isAnonymous; /**< TODO */
    QString userName; /**< TODO */
    QString userPassword; /**< TODO */
    /**
     * @brief Parse client query
     *
     * @param QString Query
     */
    void parsingQuery(QString);
    /**
     * @brief Send text data to client
     *
     * @param QString text data
     */
    void sendToClient(QString);
    /**
     * @brief Check user name
     *
     * @param QString user name
     */
    bool checkUserName(QString);
    /**
     * @brief Check user password
     *
     * @param QString user password
     */
    bool checkUserPassword(QString);
    /**
     * @brief Chech permission create directory
     *
     * @param QString user name
     */
    bool checkPermissionCreateDir(QString);
    /**
     * @brief Return user work directory
     *
     * @param QString user name
     */
    QString getUserWorkDir(QString);
    /**
     * @brief Return absolute path of user work directory
     *
     * @param aDirName directory name
     */
    QString getAbsolutePuth(QString aDirName);
    bool passiveMode; /**< TODO */
    bool activMode; /**< TODO */
    QString type; /**< TODO */
    QList<int> pasvPort; /**< TODO */
    QList<int> activPort; /**< TODO */
    QDir currentDirectory; /**< TODO */
    QString userDir; /**< TODO */
    bool renameIsActive; /**< TODO */
    QFileInfo renameObj; /**< TODO */

    FTPActiveTextDataOut *activeTextDataOut; /**< TODO */
    FTPActiveBinaryDataOut *activeBinaryDataOut; /**< TODO */
    FTPActiveBinaryDataIn *activeBinaryDataIn; /**< TODO */


signals:
    /**
     * @brief Signal is emitted after close connection by client
     *
     * @param int Socket descriptor
     */
    void sessionClose(int);
<<<<<<< HEAD
    /**
     * @brief Signal is emitted when necessary send information to log
     *
     * @param aData Text message
     */
=======
>>>>>>> 446b87200f44303072fb3040ffcf4e0cd889aa72
    void sendDataToLog(QString aData);

public slots:
    /**
     * @brief Slot caused after successful transfer text data
     *
     */
    void activeTransferTextDataSuccessfulSlot();
    /**
     * @brief Slot caused after error transfer text data
     *
     */
    void activeTransferTextDataErrorSlot();
    /**
     * @brief Slot caused after closed socket for text data transfer
     *
     */
    void activeTransferTextDataConnectionCloseByClientSlot();
    /**
     * @brief Slot caused after abort socket for text data transfer
     *
     */
    void activeTransferTextDataAbortSlot();

    /**
     * @brief Slot caused after successful transfer binary data
     *
     */
    void activeTransferBinaryDataSuccessfulSlot();
    /**
     * @brief Slot caused after error transfer binary data
     *
     */
    void activeTransferBinaryDataErrorSlot();
    /**
     * @brief Slot caused after closed socket for binary data transfer
     *
     */
    void activeTransferBinaryDataConnectionCloseByClientSlot();
    /**
     * @brief Slot caused after abort socket for binary data transfer
     *
     */
    void activeTransferBinaryDataAbortSlot();

    //void activeReceptionBinaryDataErrorOpenFileSlot();
    /**
     * @brief Slot caused after successful storing binary data
     *
     */
    void activeStoreBinaryDataCompleteSuccessfulSlot();
    /**
     * @brief Slot caused after closed socket for binary data storing
     *
     */
    void activeStoreBinaryDataConnectionCloseByClientSlot();
    /**
     * @brief Slot caused after error storing binary data
     *
     */
    void activeStoreBinaryDataErrorSlot();
    /**
     * @brief Slot caused after abort socket for binary data storing
     *
     */
    void activeStoreBinaryDataAbortSlot();


};

#endif // FTPSESSION_H
