#ifndef FTPSESSION_H
#define FTPSESSION_H

#include <QTcpSocket>
#include <QDataStream>
#include <QByteArray>
#include <QString>
#include <QList>
#include <QDir>
#include <QFileInfo>
#include <QFileInfoList>

#include "ftpactivetextdataout.h"
#include "ftpactivebinarydataout.h"
#include "ftpactivebinarydatain.h"

class FTPServer;

/**
 * @brief Класс является менеджером для клиентской сессии
 *
 */
class FTPSession : public QTcpSocket
{

    Q_OBJECT

public:
/**
 * @brief Конструктор класса
 *
 * @param parent Родительский объект
 */
    FTPSession(QObject *parent = 0);
    /**
     * @brief Деструктор класса
     *
     */
    ~FTPSession();
    /**
     * @brief Устанавливает текущую рабочую директорию пользователя
     *
     * @param QString Абсолютный путь к директории
     */
    bool setCurrentDirectory(QString);
    /**
     * @brief Устанавливает начальную пользовательскую директорию
     *
     * @param userDirName Абсолютный путь к директории
     */
    void setUserDir(QString userDirName);



private slots:
    /**
     * @brief Слот для чтения данных клиента
     *
     */
    void readClient();

private:
    bool loginIsOk; /**< флаг корректности логина */
    bool passIsOk; /**< флаг корректности пароля */
    bool isAnonymous; /**< флаг анонимной сессии */
    QString userName; /**< Имя пользователя */
    QString userPassword; /**< Пароль пользователя */
    /**
     * @brief Обработка запроса
     *
     * @param QString запрос
     */
    void parsingQuery(QString);
    /**
     * @brief Отправка текстовой информации клиенту
     *
     * @param QString текст
     */
    void sendToClient(QString);
    /**
     * @brief Проверка доступа логина
     *
     * @param QString логин клиента
     */
    bool checkUserName(QString);
    /**
     * @brief Check проверка пароля
     *
     * @param QString пароль
     */
    bool checkUserPassword(QString);
    /**
     * @brief проверка разрешения пользователю создавать директорию
     *
     * @param QString имя пользователя
     */
    bool checkPermissionCreateDir(QString);
    /**
     * @brief Абсолютный путь текущей рабочей директории пользователя
     *
     * @param QString директория
     */
    QString getUserWorkDir(QString);
    /**
     * @brief возвращает строку абсолютного пути к директории
     *
     * @param aDirName имя директории
     */
    QString getAbsolutePuth(QString aDirName);
    bool passiveMode; /**< Флаг пассивного режима */
    bool activMode; /**< Фалг активного режима */
    QString type; /**< тип передаваемых данных */
    QList<int> pasvPort; /**< пассивный адрес и порт */
    QList<int> activPort; /**< активный адрес и порт */
    QDir currentDirectory; /**< текущая директория */
    QString userDir; /**< начальная пользовательская директория */
    bool renameIsActive; /**< флаг действия переименования */
    QFileInfo renameObj; /**< переименовываемый объект */

    FTPActiveTextDataOut *activeTextDataOut; /**< объект для передачи текстовой информации */
    FTPActiveBinaryDataOut *activeBinaryDataOut; /**< объект для передачи бинарной инфорации */
    FTPActiveBinaryDataIn *activeBinaryDataIn; /**< объект для приёма текстовой информации */


signals:
    /**
     * @brief Сигнал высылается после закрытия соеднения клиентом
     *
     * @param int дескриптор соединения
     */
    void sessionClose(int);
    /**
     * @brief Сигнал высылается после необходимости добавить информацию в лог
     *
     * @param aData тестовое сообщение
     */
    void sendDataToLog(QString aData);

public slots:
    /**
     * @brief Слот для обработки успешной передачи текствой информации
     *
     */
    void activeTransferTextDataSuccessfulSlot();
    /**
     * @brief Слот для обработки ошибки во время передачи текстовой информации
     *
     */
    void activeTransferTextDataErrorSlot();
    /**
     * @brief Слот для обработки закрытия соединения передачи текстовой информации клиентом
     *
     */
    void activeTransferTextDataConnectionCloseByClientSlot();
    /**
     * @brief Слот для обработки обрыва соединения передачи текстовой информации
     *
     */
    void activeTransferTextDataAbortSlot();

    /**
     * @brief Слот для обработки успешной передачи бинарной информации
     *
     */
    void activeTransferBinaryDataSuccessfulSlot();
    /**
     * @brief Слот вызывается после ошибки во время передачи бинарной информации
     *
     */
    void activeTransferBinaryDataErrorSlot();
    /**
     * @brief Слот вызывается после закрытия соединения передачи бинарной информации клиентом
     *
     */
    void activeTransferBinaryDataConnectionCloseByClientSlot();
    /**
     * @brief Слот вызывается после обрыва соединения передачи бинарной информации
     *
     */
    void activeTransferBinaryDataAbortSlot();

    /**
     * @brief Слот вызывается после успешного приёма бинарных данных
     *
     */
    void activeStoreBinaryDataCompleteSuccessfulSlot();
    /**
     * @brief Слот вызывается после закрытия клинтом соединения приёма бинарных данных
     *
     */
    void activeStoreBinaryDataConnectionCloseByClientSlot();
    /**
     * @brief Слот вызывается после возникновения ошибки во время приёма бинарных данных
     *
     */
    void activeStoreBinaryDataErrorSlot();
    /**
     * @brief Слот вызывается после обрыва соединения приёма бинарных данных
     *
     */
    void activeStoreBinaryDataAbortSlot();


};

#endif // FTPSESSION_H
