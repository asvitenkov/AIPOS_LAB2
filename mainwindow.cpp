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
    ui->serverAdressLine->setText("127.0.0.1");
    QApplication::setStyle("Cleanlooks");
    QApplication::setPalette(QApplication::style()->standardPalette());

    server = NULL;
    connect(ui->buttonRunServer,SIGNAL(clicked()),this,SLOT(runServerSlot()));








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
        //����� ������������
        QMessageBox::StandardButton reply;
        reply = QMessageBox::information(this, QString::fromLocal8Bit("������������ ����� �������"),QString::fromLocal8Bit("������� ���������� ����� �������"));
        return;
    }
    server = new FTPServer(this);
    server->setServerAdress(hostAdress);
    if(server->listen(QHostAddress::Any,21)){
        // ������ ���������
        addRecordInLog(QString::fromLocal8Bit("������ �������"));
    }
    else{
        // ������ �� ���� ����������
        addRecordInLog(QString::fromLocal8Bit("������ �� ������� ")+server->errorString());

    }

}


void MainWindow::addRecordInLog(QString aRecord)
{
    ui->editServerLog->append(aRecord);
}


void MainWindow::stopServerSlot(){

}
