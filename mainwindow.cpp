#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileInfoList>
#include <QFileInfo>
#include <Qdir>
#include <QDateTime>
#include <QLocale>
#include <QDate>
#include <QErrorMessage>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->editServerLog->setReadOnly(true);
    ui->serverAdressLine->setText("192.168.38.105");
    QApplication::setStyle("Cleanlooks");
    QApplication::setPalette(QApplication::style()->standardPalette());

    server = NULL;
    connect(ui->buttonRunServer,SIGNAL(clicked()),this,SLOT(runServerSlot()));
    connect(ui->clearLogButton,SIGNAL(clicked()),this,SLOT(clearLogSlot()));








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


void MainWindow::runServerSlot(){

    QHostAddress hostAdress(ui->serverAdressLine->text().replace(",","."));
    if(hostAdress.isNull()){
        //адрес некорректный
        QMessageBox::StandardButton reply;
        reply = QMessageBox::information(this, QString::fromLocal8Bit("Некорректный адрес сервера"),QString::fromLocal8Bit("Введите корректный адрес сервера"));
        return;
    }
    server = new FTPServer(this);
    server->setServerAdress(hostAdress);
    if(server->listen(QHostAddress::Any,21)){
        // сервер стартовал
        addRecordInLog(QString::fromLocal8Bit("Сервер запущен"));
        connect(server,SIGNAL(addRecordToLogSignal(QString)),this,SLOT(addRecordInLog(QString)));
    }
    else{
        // сервер не смог стартовать
        addRecordInLog(QString::fromLocal8Bit("Сервер не запущен ")+server->errorString());

    }

}


void MainWindow::addRecordInLog(QString aRecord)
{
    ui->editServerLog->append(aRecord);
}


void MainWindow::stopServerSlot(){

}


void MainWindow::clearLogSlot(){
    ui->editServerLog->clear();
}
