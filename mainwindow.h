#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ftpserver.h"
#include <QMainWindow>

namespace Ui {
    class MainWindow;
}

/**
 * @brief Class provide GUI interface for ftp server
 *
 */
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
/**
 * @brief Create MainWindow object
 *
 * @param parent Parent widget
 */
    MainWindow(QWidget *parent = 0);
    /**
     * @brief destroy MainWindow object
     *
     */
    ~MainWindow();

protected:

private:
    Ui::MainWindow *ui; /**< TODO UI*/

    FTPServer *server; /**< TODO FTP server*/

private slots:
    /**
     * @brief Add new record in log window
     *
     * @param aRecord message
     */
    void addRecordInLog(QString aRecord);
    /**
     * @brief Run FTP server for listen 21 port
     *
     */
    void runServerSlot();
    /**
     * @brief Stop FTP server
     *
     */
    void stopServerSlot();

<<<<<<< HEAD
    /**
     * @brief Clean log window
     *
     */
=======
>>>>>>> 446b87200f44303072fb3040ffcf4e0cd889aa72
    void clearLogSlot();
};

#endif // MAINWINDOW_H
