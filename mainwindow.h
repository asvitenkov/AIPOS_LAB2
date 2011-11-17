#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ftpserver.h"
#include <QMainWindow>

namespace Ui {
    class MainWindow;
}

/**
 * @brief Класс-интерфейс сервера
 *
 */
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
/**
 * @brief Создаёт окно сервера
 *
 * @param parent родительский виджет
 */
    MainWindow(QWidget *parent = 0);
    /**
     * @brief деструктор класса
     *
     */
    ~MainWindow();

protected:

private:
    Ui::MainWindow *ui; /**< TODO переменные интерфейса*/

    FTPServer *server; /**< TODO объект сервера*/

private slots:
    /**
     * @brief Добавляет новую запись в лог
     *
     * @param aRecord сообщение
     */
    void addRecordInLog(QString aRecord);
    /**
     * @brief Запускает сервер
     *
     */
    void runServerSlot();
    /**
     * @brief останавливает сервер
     *
     */
    void stopServerSlot();

    /**
     * @brief очищает лог сервера
     *
     */
    void clearLogSlot();
};

#endif // MAINWINDOW_H
