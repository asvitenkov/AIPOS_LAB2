#ifndef FTPSERVER_H
#define FTPSERVER_H

#include <QTcpServer>
#include "ftpsession.h"
#include <QHash>

/**
 * @brief ����� FTP �������
 *
 */
class FTPServer : public QTcpServer
{

    Q_OBJECT

public:
/**
 * @brief ����������� ������
 *
 * @param parent ������������ ������
 */
    FTPServer(QObject *parent = 0 );
    /**
     * @brief ������������� ��������� ����� �������
     *
     * @param _adress ����� �������
     */
    void setServerAdress(QHostAddress _adress);
    /**
     * @brief ��������� ������. ������ ������������ 21 ����
     *
     */
    void startServer();

private:
    /**
     * @brief ����� ���������� ����� ��������� ������ ����������� � �������
     *
     * @param socketID ���������� ����������
     */
    void incomingConnection(int socketID);
    QHostAddress serverAdress; /**< TODO  ����� �������*/
    QHash< int, FTPSession *> sessionsList; /**< TODO ������ ���������� �������*/



public slots:
    /**
     * @brief ���� ��� ��������� �������� ���������� ��������
     *
     * @param ���������� ����������
     */
    void sessionCloseSLOT(int _descriptor);
    /**
     * @brief ���� ��� ���������� ���������� � ���
     *
     * @param aData ���������� ��� ����������
     */
    void addRecordToLogSlot(QString aData);


signals:
    /**
     * @brief ������ ���������� ��� ���������� ���������� � ���
     *
     * @param aData ���������� ��� ����������
     */
    void addRecordToLogSignal(QString aData);
};

#endif // FTPSERVER_H
