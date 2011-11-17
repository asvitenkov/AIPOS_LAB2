#ifndef FTPACTIVEBINARYDATAOUT_H
#define FTPACTIVEBINARYDATAOUT_H

#include <QTcpSocket>
#include <QHostAddress>
#include <QFile>
#include <QDataStream>

/**
 * @brief ����� ������������ ���� ������ � �������� ������ �� ������� �������
 *
 */
class FTPActiveBinaryDataOut : public QTcpSocket
{
    Q_OBJECT

    static const qint64 blockSize = 8192; /**< TODO  ������ ������������� �����*/

public:
    /**
     * @brief ����������� ������. ������ ������ ������ � ������ TCP ����������� � aHostAdress �� aHostPor
     *
     * @param aHostAdress ����� �������
     * @param aHostPort ���� �������
     * @param aLocalAdress ��������� ����� �������
     * @param aLocalPort ��������� ���� �������
     * @param parent ������������ ������
     */
    explicit FTPActiveBinaryDataOut(QHostAddress aHostAdress,int aHostPort, QHostAddress aLocalAdress, int aLocalPort, QObject *parent=0);
    /**
     * @brief ���������� ������. ��������� ��� ����������, ���� ��� �������
     *
     */
    ~FTPActiveBinaryDataOut();
    /**
     * @brief ����� ��� ������ ������������� �����
     *
     * @param aStream �����
     */
    void sendFile(QDataStream *aStream);
    /**
     * @brief ���������� ����������
     *
     */
    void abortConnection();

signals:
    /**
     * @brief ������ ���������� ����� ����, ��� ��������� ������ �� ����� �������� �����
     *
     */
    void errorSendBinaryDataSignal();
    /**
     * @brief ������ ���������� ����� ������ ����������
     *
     */
    void connectionCloseByClientSignal();
    /**
     * @brief ������ ���������� ����� �������� �������� ����� �������
     *
     */
    void sendBinaryDataSuccessfulSignal();
    /**
     * @brief ������ ���������� ����� ����, ��� ���������� ���� ������� ��������
     *
     */
    void abortConnectionSignal();

public slots:
private slots:
    /**
     * @brief ���� ��� ��������� ���������� �������� ����������
     *
     */
    void connectionCloseByClientSlot();
    /**
     * @brief ���� ��� ����������� �������� ����� ����� ��������� ����������� ���������� ������
     *
     */
    void continueTransferBinaryDataSlot();
    /**
     * @brief ���� ��� ������ ����������
     *
     */
    void abortConnectionSlot();

private:
    QDataStream *outStream; /**< TODO  ����� ��� ������ ������������ ������ */

};

#endif // FTPACTIVEBINARYDATAOUT_H
