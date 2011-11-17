#ifndef FTPACTIVEBINARYDATAIN_H
#define FTPACTIVEBINARYDATAIN_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QFile>
#include <QDataStream>
#include <QByteArray>

/**
 * @brief ����� ������������ �������� �������� ������ � �������� ������ �� ������� �������
 *
 */
class FTPActiveBinaryDataIn : public QTcpSocket
{
    Q_OBJECT
public:
    /**
     * @brief ����������� ������
     *
     * @param aLocalAdress ����� ���������� ������
     * @param aLocalPort ���� ���������� ������
     * @param parent ������������ ������
     */
    explicit FTPActiveBinaryDataIn(QHostAddress aLocalAdress, int aLocalPort,QObject *parent = 0);
    /**
     * @brief ���������� ������
     *
     */
    ~FTPActiveBinaryDataIn();
    /**
     * @brief ������������� ����, � ������� ���������� ��������� �������� ������
     *
     * @param aFile ����
     */
    bool setSaveFile(QFile *aFile);
    /**
     * @brief ���������� ����������
     *
     */
    void abortConnection();

signals:
    /**
     * @brief ������ ���������� ����� ����, ��� ��������� ������ �� ����� ���������� �����
     *
     */
    void errorStoreDataInFileSignal();
    /**
     * @brief ������ ���������� ����� ��������� ����� � ���������� �����
     *
     */
    void storeDataInFileCompleteSuccessfulSignal();
    /**
     * @brief ������ ���������� ����� ������ ����������
     *
     */
    void connectionCloseByClientSignal();
    /**
     * @brief ������ ���������� ����� ����, ��� ���������� ���� ������� ��������
     *
     */
    void abortCommandCompleteSignal();
public slots:

private slots:
    /**
     * @brief ���� ��� ������ �������� ������
     *
     */
    void readIncomingData();
    /**
     * @brief ���� ��� ��������� �������� �������� ���������� ��������
     *
     */
    void connectionCloseByClientSlot();
    /**
     * @brief ���� ��� ��������� �������� ������ ���������� ��������
     *
     */
    void abortConnectionSlot();

private:
    QFile *file; /**< TODO ����������� ���� */
    QDataStream *writeStream; /**< TODO ����� ��� ���������� */
};

#endif // FTPACTIVEBINARYDATAIN_H
