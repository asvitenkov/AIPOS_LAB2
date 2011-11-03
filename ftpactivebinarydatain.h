#ifndef FTPACTIVEBINARYDATAIN_H
#define FTPACTIVEBINARYDATAIN_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QFile>
#include <QDataStream>
#include <QByteArray>

/**
 * @brief The class provides reception of the binary data under FTP protocol from the client to the server
 *
 */
class FTPActiveBinaryDataIn : public QTcpSocket
{
    Q_OBJECT
public:
    /**
     * @brief Creates a FTPActiveBinaryDataIn object in state UnconnectedState
     *
     * @param aLocalAdress The address of a local socket
     * @param aLocalPort The port of a local socet
     * @param parent The parent object
     */
    explicit FTPActiveBinaryDataIn(QHostAddress aLocalAdress, int aLocalPort,QObject *parent = 0);
    /**
     * @brief Destroys the local socket, closing the connection if necessary
     *
     */
    ~FTPActiveBinaryDataIn();
    /**
     * @brief Set a file in which it is necessary to write down the entering data
     *
     * @param aFile Output file
     */
    bool setSaveFile(QFile *aFile);
    /**
     * @brief Aborts the current connection and resets the socket
     *
     */
    void abortConnection();

signals:
    /**
     * @brief This signal is emitted after an error occurred
     *
     */
    void errorStoreDataInFileSignal();
    /**
     * @brief This signal is emitted after the successful termination of data recording in a file
     *
     */
    void storeDataInFileCompleteSuccessfulSignal();
    /**
     * @brief This signal is emitted after the socket has been closed on the remote terminal
     *
     */
    void connectionCloseByClientSignal();
    /**
     * @brief This signal is emitted after performance abort() conenction
     *
     */
    void abortCommandCompleteSignal();
public slots:

private slots:
    /**
     * @brief Slot for reading of the entering data
     *
     */
    void readIncomingData();
    /**
     * @brief Slot for processing a situation of closing of connection by the client
     *
     */
    void connectionCloseByClientSlot();
    /**
     * @brief Slot for processing a situation of aborting of connection
     *
     */
    void abortConnectionSlot();

private:
    QFile *file; /**< TODO Kept file */
    QDataStream *writeStream; /**< TODO Stream for storing */
};

#endif // FTPACTIVEBINARYDATAIN_H
