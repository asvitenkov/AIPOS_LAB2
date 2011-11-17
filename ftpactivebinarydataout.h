#ifndef FTPACTIVEBINARYDATAOUT_H
#define FTPACTIVEBINARYDATAOUT_H

#include <QTcpSocket>
#include <QHostAddress>
#include <QFile>
#include <QDataStream>

/**
 * @brief Класс обеспечивает приём данных в активном режиме от сервера клиенту
 *
 */
class FTPActiveBinaryDataOut : public QTcpSocket
{
    Q_OBJECT

    static const qint64 blockSize = 8192; /**< TODO  Размер передаваемого блока*/

public:
    /**
     * @brief Конструктор класса. Создаёт объект класса и создаёт TCP подключения к aHostAdress на aHostPor
     *
     * @param aHostAdress адрес клиента
     * @param aHostPort порт клиента
     * @param aLocalAdress локальный адрес сервера
     * @param aLocalPort локальный порт сервера
     * @param parent Родительский объект
     */
    explicit FTPActiveBinaryDataOut(QHostAddress aHostAdress,int aHostPort, QHostAddress aLocalAdress, int aLocalPort, QObject *parent=0);
    /**
     * @brief Деструктор класса. Закрывает все соединения, если они открыты
     *
     */
    ~FTPActiveBinaryDataOut();
    /**
     * @brief Поток для чтения передаваемого файла
     *
     * @param aStream поток
     */
    void sendFile(QDataStream *aStream);
    /**
     * @brief Прерывание соединения
     *
     */
    void abortConnection();

signals:
    /**
     * @brief Сигнал высылается после того, как произошла ошибка во время передачи файла
     *
     */
    void errorSendBinaryDataSignal();
    /**
     * @brief Сигнал высылается после обрыва соединения
     *
     */
    void connectionCloseByClientSignal();
    /**
     * @brief Сигнал высылается после успешной передачи файла клиенту
     *
     */
    void sendBinaryDataSuccessfulSignal();
    /**
     * @brief Сигнал высылается после того, как соединение было успешно прервано
     *
     */
    void abortConnectionSignal();

public slots:
private slots:
    /**
     * @brief Слот для обработки прерывания клиентом соединения
     *
     */
    void connectionCloseByClientSlot();
    /**
     * @brief Слот для продолжения передачи файла после успешного отправления предыдущих данных
     *
     */
    void continueTransferBinaryDataSlot();
    /**
     * @brief Слот для обрыва соединения
     *
     */
    void abortConnectionSlot();

private:
    QDataStream *outStream; /**< TODO  Поток для чтения передаваемых данных */

};

#endif // FTPACTIVEBINARYDATAOUT_H
