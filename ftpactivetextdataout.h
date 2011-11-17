#ifndef FTPACTIVETEXTDATAOUT_H
#define FTPACTIVETEXTDATAOUT_H

#include <QTcpSocket>
#include <QHostAddress>

/**
 * @brief ����� ��� �������� ��������� ���������� �� ������� �������
 *
 */
class FTPActiveTextDataOut : public QTcpSocket
{
    Q_OBJECT
public:
/**
 * @brief ����������� ������. ������ TCP ���������� �  aHostAdress �� aHostPort
 *
 * @param aHostAdress ����� �������
 * @param aHostPort ���� �������
 * @param aLocalAdress ����� ���������� ������
 * @param aLocalPort ���� ���������� ������
 * @param parent ������������ ������
 */
    FTPActiveTextDataOut(QHostAddress aHostAdress,int aHostPort, QHostAddress aLocalAdress, int aLocalPort, QObject *parent=0);
    /**
     * @brief �������� �������� ���������� aData �������
     *
     * @param aData ��������� ����������
     */
    void sendTextData(QString aData);

private slots:
    /**
     * @brief ���� ��� ��������� �������� ����������
     *
     */
    void connectionClosedSlot();

signals:
    /**
     * @brief ������ ���������� ����� ������ �������� ������
     *
     */
    void errorSendTexdDataSignal();
    /**
     * @brief ������ ���������� ����� ������ ��� �������� ���������� ��������
     *
     */
    void connectionCloseByClientSignal();
    /**
     * @brief ������ ���������� ����� �������� �������� ����������
     *
     */
    void sendTextDataSuccessfulSignal();
};

#endif // FTPACTIVETEXTDATAOUT_H
