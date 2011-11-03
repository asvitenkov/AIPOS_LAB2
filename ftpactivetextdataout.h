#ifndef FTPACTIVETEXTDATAOUT_H
#define FTPACTIVETEXTDATAOUT_H

#include <QTcpSocket>
#include <QHostAddress>

/**
 * @brief The class gives transfer of the text data under FTP protocol from the server to the client
 *
 */
class FTPActiveTextDataOut : public QTcpSocket
{
    Q_OBJECT
public:
/**
 * @brief Creates a FTPActiveTextDataOut object and attempts to make a connection to adress aHostAdress on port aHostPort
 *
 * @param aHostAdress Peer address
 * @param aHostPort Peer port
 * @param aLocalAdress The address of a local socket
 * @param aLocalPort The port of a local socet
 * @param parent The parent object
 */
    FTPActiveTextDataOut(QHostAddress aHostAdress,int aHostPort, QHostAddress aLocalAdress, int aLocalPort, QObject *parent=0);
    /**
     * @brief Sending text data aData
     *
     * @param aData Text data
     */
    void sendTextData(QString aData);

private slots:
    /**
     * @brief Slot for processing a situation after closed connection
     *
     */
    void connectionClosedSlot();

signals:
    /**
     * @brief This signal is emitted after an error occurred
     *
     */
    void errorSendTexdDataSignal();
    /**
     * @brief This signal is emitted after the socket has been closed on the remote terminal
     *
     */
    void connectionCloseByClientSignal();
    /**
     * @brief This signal is emitted after the successful transfer
     *
     */
    void sendTextDataSuccessfulSignal();
};

#endif // FTPACTIVETEXTDATAOUT_H
