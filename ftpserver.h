#ifndef FTPSERVER_H
#define FTPSERVER_H

#include <QTcpServer>
#include "ftpsession.h"
#include <QHash>

/**
 * @brief The class provides a FTP TCP-based server
 *
 */
class FTPServer : public QTcpServer
{

    Q_OBJECT

public:
    //FTPServer(QHostAddress _serverAdress, int port,QObject *parent = 0 );
/**
 * @brief Constructs a FTPServer object
 *
 * @param parent The parent object
 */
    FTPServer(QObject *parent = 0 );
    //void sessionClose(int _descriptor);
    /**
     * @brief Set the server address
     *
     * @param _adress Server address
     */
    void setServerAdress(QHostAddress _adress);
    /**
     * @brief Run FTP server for listening 21 port
     *
     */
    void startServer();

private:
    /**
     * @brief Function is called by FTPServer when a new connection is available
     *
     * @param socketID The native socket descriptor for the accepted connection
     */
    void incomingConnection(int socketID);
    QHostAddress serverAdress; /**< TODO  Server address*/
    QHash< int, FTPSession *> sessionsList; /**< TODO List of connections*/



public slots:
    /**
     * @brief Slot for frocessing closing session
     *
     * @param _descriptor Descriptor of the session socket
     */
    void sessionCloseSLOT(int _descriptor);
<<<<<<< HEAD
    /**
     * @brief Slot for adding information for servers log file
     *
     * @param aData information for adding
     */
=======
>>>>>>> 446b87200f44303072fb3040ffcf4e0cd889aa72
    void addRecordToLogSlot(QString aData);


signals:
<<<<<<< HEAD
    /**
     * @brief This signal is emmited when new information for log avaliable
     *
     * @param aData log information
     */
=======
>>>>>>> 446b87200f44303072fb3040ffcf4e0cd889aa72
    void addRecordToLogSignal(QString aData);
};

#endif // FTPSERVER_H
