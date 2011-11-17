#ifndef FTPACTIVETEXTDATAOUT_H
#define FTPACTIVETEXTDATAOUT_H

#include <QTcpSocket>
#include <QHostAddress>

/**
 * @brief Класс для передачи текстовой информации от сервера клиенту
 *
 */
class FTPActiveTextDataOut : public QTcpSocket
{
    Q_OBJECT
public:
/**
 * @brief Конструктор класса. Создаёт TCP соединение к  aHostAdress на aHostPort
 *
 * @param aHostAdress Адрес клиента
 * @param aHostPort Порт клиента
 * @param aLocalAdress Адрес локального сокета
 * @param aLocalPort порт локального сокета
 * @param parent Родительский объект
 */
    FTPActiveTextDataOut(QHostAddress aHostAdress,int aHostPort, QHostAddress aLocalAdress, int aLocalPort, QObject *parent=0);
    /**
     * @brief Передача текствой информации aData клиенту
     *
     * @param aData Текстовая информация
     */
    void sendTextData(QString aData);

private slots:
    /**
     * @brief Слот для обработки закрытия соединения
     *
     */
    void connectionClosedSlot();

signals:
    /**
     * @brief Сигнал высылается после ошибки передачи данных
     *
     */
    void errorSendTexdDataSignal();
    /**
     * @brief Сигнал высылается после обрыва или закрытия соединения клиентом
     *
     */
    void connectionCloseByClientSignal();
    /**
     * @brief Сигнал высылается после успешной передачи информации
     *
     */
    void sendTextDataSuccessfulSignal();
};

#endif // FTPACTIVETEXTDATAOUT_H
