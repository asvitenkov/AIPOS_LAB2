#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ftpserver.h"
#include <QMainWindow>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:

private:
    Ui::MainWindow *ui;

    FTPServer *server;

private slots:
    void addRecordInLog(QString aRecord);
    void runServerSlot();
    void stopServerSlot();
};

#endif // MAINWINDOW_H
