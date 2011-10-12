#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileInfoList>
#include <QFileInfo>
#include <Qdir>
#include <QDateTime>
#include <QLocale>
#include <QDate>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    FTPServer *server = new FTPServer(this);
    server->listen(QHostAddress::Any,21);
    server->setServerAdress(QHostAddress("172.31.5.51"));




    QApplication::setStyle("Cleanlooks");
    QApplication::setPalette(QApplication::style()->standardPalette());
//    QDir dir("D:\\");
//    QFileInfoList fileInfoList = dir.entryInfoList();
//    QFileInfo fileInfo;
//    for(int i=0;i<fileInfoList.size();i++){
//        fileInfo=fileInfoList[i];
//        qDebug()<<fileInfo.created().toString("MMM dd hh:mm")<<fileInfo.fileName();
//    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
