#ifndef FTPACTIVEBINARYDATAOUT_H
#define FTPACTIVEBINARYDATAOUT_H

#include <QTcpSocket>
#include <QHostAddress>
#include <QFile>
#include <QDataStream>

/**
 * @brief The class gives transfer of the binary data under FTP protocol from the server to the client
 *
 */
class FTPActiveBinaryDataOut : public QTcpSocket
{
    Q_OBJECT

    static const qint64 blockSize = 8192; /**< TODO  Size of transfer block*/

public:
    /**
     * @brief Creates a FTPActiveBinaryDataOut object and attempts to make a connection to adress aHostAdress on port aHostPort
     *
     * @param aHostAdress Peer address
     * @param aHostPort Peer port
     * @param aLocalAdress The address of a local socket
     * @param aLocalPort The port of a local socet
     * @param parent The parent object
     */
    explicit FTPActiveBinaryDataOut(QHostAddress aHostAdress,int aHostPort, QHostAddress aLocalAdress, int aLocalPort, QObject *parent=0);
    /**
     * @brief Destroys the local socket, closing the connection if necessary
     *
     */
    ~FTPActiveBinaryDataOut();
    /**
     * @brief Stream of reading of a sent file
     *
     * @param aStream file stream
     */
    void sendFile(QDataStream *aStream);
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
    void errorSendBinaryDataSignal();
    /**
     * @brief This signal is emitted after the socket has been closed on the remote terminal
     *
     */
    void connectionCloseByClientSignal();
    /**
     * @brief This signal is emitted after the successful transfer
     *
     */
    void sendBinaryDataSuccessfulSignal();
    /**
     * @brief This signal is emitted after abort() connection
     *
     */
    void abortConnectionSignal();

public slots:
private slots:
    /**
     * @brief Slot for processing a situation of aborting of connection
     *
     */
    void connectionCloseByClientSlot();
    /**
     * @brief Slot for continuation of data transmission after sending of the previous package
     *
     */
    void continueTransferBinaryDataSlot();
    /**
     * @brief for continuation of data transmission after sending of the previous package
     *
     */
    void abortConnectionSlot();

private:
    QDataStream *outStream; /**< TODO  Stream for reading data */

};

#endif // FTPACTIVEBINARYDATAOUT_H
