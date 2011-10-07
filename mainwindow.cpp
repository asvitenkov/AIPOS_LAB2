#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    FTPServer *tmp = new FTPServer;
    if (!tmp->listen(QHostAddress::Any, 21)) {
        qDebug()<<"Erorr open socet";
        exit(1);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
