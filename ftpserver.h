#ifndef FTPSERVER_H
#define FTPSERVER_H

#include <QTcpServer>
#include "ftpsession.h"
#include <QHash>

/**
 * @brief Класс FTP сервера
 *
 */
class FTPServer : public QTcpServer
{

    Q_OBJECT

public:
/**
 * @brief Конструктор класса
 *
 * @param parent Родительский объект
 */
    FTPServer(QObject *parent = 0 );
    /**
     * @brief Устанавливает локальный адрес сервера
     *
     * @param _adress Адрес сервера
     */
    void setServerAdress(QHostAddress _adress);
    /**
     * @brief Запускает сервер. Сервер прослушивает 21 порт
     *
     */
    void startServer();

private:
    /**
     * @brief Метод вызывается после появления нового подключения к серверу
     *
     * @param socketID дескриптор соединения
     */
    void incomingConnection(int socketID);
    QHostAddress serverAdress; /**< TODO  Адрес сервера*/
    QHash< int, FTPSession *> sessionsList; /**< TODO Список соединения сервера*/



public slots:
    /**
     * @brief Слот для обработки закрытия соединения клиентом
     *
     * @param дескриптор соединения
     */
    void sessionCloseSLOT(int _descriptor);
    /**
     * @brief слот для добавления информации в лог
     *
     * @param aData Информация для добавления
     */
    void addRecordToLogSlot(QString aData);


signals:
    /**
     * @brief Сигнал высылается для добавления информации в лог
     *
     * @param aData Информация для добавления
     */
    void addRecordToLogSignal(QString aData);
};

#endif // FTPSERVER_H
