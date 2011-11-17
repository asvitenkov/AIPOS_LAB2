#ifndef FTPACTIVEBINARYDATAIN_H
#define FTPACTIVEBINARYDATAIN_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QFile>
#include <QDataStream>
#include <QByteArray>

/**
 * @brief Класс обеспечивает передачу бинарных данных в активном режиме от клиента серверу
 *
 */
class FTPActiveBinaryDataIn : public QTcpSocket
{
    Q_OBJECT
public:
    /**
     * @brief Конструктор класса
     *
     * @param aLocalAdress адрес локального сокета
     * @param aLocalPort порт локального сокета
     * @param parent родительский объект
     */
    explicit FTPActiveBinaryDataIn(QHostAddress aLocalAdress, int aLocalPort,QObject *parent = 0);
    /**
     * @brief Деструктор класса
     *
     */
    ~FTPActiveBinaryDataIn();
    /**
     * @brief Устанавливает файл, в который необходимо сохранить принятые данные
     *
     * @param aFile Файл
     */
    bool setSaveFile(QFile *aFile);
    /**
     * @brief Прерывание соединения
     *
     */
    void abortConnection();

signals:
    /**
     * @brief Сигнал высылается после того, как произошла ошибка во время сохранения файла
     *
     */
    void errorStoreDataInFileSignal();
    /**
     * @brief Сигнал высылается после успешного приёма и сохранения файла
     *
     */
    void storeDataInFileCompleteSuccessfulSignal();
    /**
     * @brief Сигнал высылается после обрыва соединения
     *
     */
    void connectionCloseByClientSignal();
    /**
     * @brief Сигнал высылается после того, как соединение было успешно прервано
     *
     */
    void abortCommandCompleteSignal();
public slots:

private slots:
    /**
     * @brief Слот для чтения входящих данных
     *
     */
    void readIncomingData();
    /**
     * @brief слот для обработки ситуации закрытия соединения клиентом
     *
     */
    void connectionCloseByClientSlot();
    /**
     * @brief слот для обработки ситуации обрыва соединения сервером
     *
     */
    void abortConnectionSlot();

private:
    QFile *file; /**< TODO Сохраняемый файл */
    QDataStream *writeStream; /**< TODO Поток для сохранения */
};

#endif // FTPACTIVEBINARYDATAIN_H
