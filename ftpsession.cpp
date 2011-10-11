#include "ftpsession.h"
#include <QDebug>
#include <QHostAddress>
#include <QStringList>
#include <QSysInfo>
#include <QRegExp>
#include <QDateTime>
#include <QAbstractSocket>


FTPSession::FTPSession(QObject *parent):QTcpSocket(parent)
{
    connect(this,SIGNAL(readyRead()),this,SLOT(readClient()));
    passIsOk = false;
    loginIsOk = false;
    isAnonymous = false;
    type = "I";
    passiveMode = false;
    activMode = false;
    pasvPort.push_back(0);
    pasvPort.push_back(0);
    setCurrentDirectory("D:\\");
    currentDirectory.setFilter(QDir::Dirs | QDir::Files );

}


void FTPSession::readClient(){
    QString query = QString::fromUtf8(this->readAll()).replace("\r\n","");
    parsingQuery(query);
}


void FTPSession::parsingQuery(QString query){
    qDebug()<<"Client: "<<query;
    query = query.replace("\r\n","");
    QStringList list = query.split(" ");
    bool cmdIsUnderstoot = false;

    if(list[0]=="USER"){
        cmdIsUnderstoot = true;
        if(!loginIsOk){
            // авторизации ещё не было
            if (checkUserName(list[1])){
                loginIsOk = true;
                userName = list[1];
                sendToClient("331 Password required for " + userName+" need password");
            }

        }
        else{
            // авторизация уже была, попытка авторизироваться второй раз
            sendToClient("503 user "+userName+" is already authorized");
        }
        return;
    }
    if(list[0]=="PASS"){
        cmdIsUnderstoot = true;
        if(loginIsOk){
            // принята команда pass после команды user
            if(checkUserPassword(list[1])){
                // пароль принят
                sendToClient("230 user "+userName+" logged in");
                passIsOk = true;
            }
            else{
                // пароль не принят
                sendToClient("530 Not logged in, user or password incorrect!");
            }
        }
        else{
            //  принята команда pass без команды user
            sendToClient("503 First enter a user name");
        }
        return;
    }

    if(!loginIsOk || !passIsOk){
        sendToClient("530 Not logged in, user or password incorrect!");
        return;
    }

    if(list[0]=="SYST"){
        cmdIsUnderstoot = true;
        sendToClient("215 UNIX");
        return;
    }
    if(list[0]=="FEAT"){
        cmdIsUnderstoot = true;
        sendToClient("211-Extensions supported");
        sendToClient("SIZE");
        sendToClient("211 end");
        return;
    }
    if(list[0]=="PWD"){
        cmdIsUnderstoot = true;
        sendToClient("257 " + QString("\"\\")+getUserWorkDir(userName)+"\" is a current directory");
        return;
    }

    if(list[0]=="PASV"){
        cmdIsUnderstoot = true;
        passiveMode =true;
        // нужно отослать адрес и порт
//        pasvPort[0]=qrand()%255+1;
//        pasvPort[1]=qrand()%255;
//        sendToClient("227 Entering Passive mode ("+peerAddress().toString().replace(".",",")+
//                     ","+QString::number(pasvPort[0])+","+QString::number(pasvPort[1])+")");

        pasvPort[0]=200;
        pasvPort[1]=128;
        sendToClient("227 Entering Passive mode ("+peerAddress().toString().replace(".",",")+
                     ","+QString::number(pasvPort[0])+","+QString::number(pasvPort[1])+")");

        return;
    }

    if(list[0]=="TYPE"){
        cmdIsUnderstoot = true;
        if(list[1]=="A" || list[1]=="I") type = list[1];
        sendToClient("200 Type set to "+type);
        return;
    }
    if(list[0]=="PORT"){
        cmdIsUnderstoot = true;
        QRegExp texp("[0-9]{1,3}\\,[0-9]{1,3}\\,[0-9]{1,3}\\,[0-9]{1,3}\\,[0-9]{1,3}\\,[0-9]{1,3}");
        if((list[1]).contains(texp)){
            // корректный аргумент
            activMode = true;
            QList<int> lst;
            QStringList listAdress = (list[1]).split(",");
            for(int i=0; i<listAdress.size();i++) lst.push_back((listAdress[i].toInt()));
            activPort = lst;
            qDebug()<<"PORT "<< activPort;
            sendToClient("200 Port command successful");
        }
        else{
            // некорректный аргумент
            sendToClient("501 Error in parameters or arguments");
        }
        return;
    }
    if(list[0]=="LIST"){

        // нужно передать список файлов с информацией
        // для начала нужно сконфигурировать отправляемую информацию
        // если есть аргумент, то нам нужна информация по аргументу

        // НУЖНО ПРОВЕРИТЬ, А МОЖЕМ ЛИ МЫ ПЕРЕДАТЬ?
        // ВЫПОЛНЕНА ЛИ КОМАНДА ПОРТ ИЛИ ВКЛЮЧЁН ПАССИВНЫЙ РЕЖИМ?
        // #######################################################
        cmdIsUnderstoot = true;

        FTPDataOut *pDataOut;

        if(!activPort.isEmpty() && activMode && activPort.size()==6){
            // активные режим и порт
            QString strAddr = "%1.%2.%3.%4";
            strAddr=strAddr.arg(activPort[0]).arg(activPort[1]).arg(activPort[2]).arg(activPort[3]);
            FTPDataOut *dataOut = new FTPDataOut(QHostAddress(strAddr),activPort[4]*256+activPort[5],20,this);
            pDataOut = dataOut;
        }
        else if(passiveMode && pasvPort.size()==2){
            // пассивный режим и отосланый порт
            qDebug()<<"PASV"<<pasvPort;
            FTPDataOut *dataOut = new FTPDataOut(QHostAddress("172.31.5.51"),this->peerPort(),pasvPort[0]*256+pasvPort[1],this);
            pDataOut = dataOut;
        }
        else{
            //неправильная последовательность команд
            sendToClient("503 incorrect command sequence");
        }

        sendToClient("150 Open ASCII mode data connection for directory list");
        QString listStr;
        if(list.size()==2){
            // есть аргумент
            QFileInfoList fileInfoList = currentDirectory.entryInfoList();
            QFileInfo fileInfo;
            QDateTime date;




            for(int i=0;i<fileInfoList.size();i++){
                listStr.clear();
                fileInfo=fileInfoList[i];
                if(fileInfo.isDir()) listStr+=("d");
                else listStr+="-";
                if(fileInfo.permission(QFile::ReadOwner)) listStr+="r";
                else listStr+="-";
                if(fileInfo.permission(QFile::WriteOwner)) listStr+="w";
                else listStr+="-";
                if(fileInfo.permission(QFile::ExeOwner)) listStr+="x";
                else listStr+="-";
                listStr+="------";
                listStr+=" 1 user group ";
                listStr+=QString::number(fileInfo.size());
                date=fileInfo.created();
                listStr+=" Aug 01 15:59 ";
                listStr+=fileInfo.fileName();
                //listStr+="\r\n";
                //sendToClient(listStr);
                //tmp->write(QByteArray(listStr.toUtf8()));
            }
        pDataOut->sendTextData("end_of_file\r\n");
        pDataOut->close();
        qDebug()<<"Transfer complete";
        sendToClient("226 Transfer complete");

        }
        else if(list.size()==1){
            // без аргумента

            QFileInfoList fileInfoList = currentDirectory.entryInfoList();
            QFileInfo fileInfo;
            QDateTime date;

            for(int i=0;i<fileInfoList.size();i++){
                listStr.clear();
                fileInfo=fileInfoList[i];
                if(fileInfo.isDir()) listStr+=("d");
                else listStr+="-";
                if(fileInfo.permission(QFile::ReadOwner)) listStr+="r";
                else listStr+="-";
                if(fileInfo.permission(QFile::WriteOwner)) listStr+="w";
                else listStr+="-";
                if(fileInfo.permission(QFile::ExeOwner)) listStr+="x";
                else listStr+="-";
                listStr+="------";
                listStr+=" 1 user group ";
                listStr+=QString::number(fileInfo.size());
                date=fileInfo.created();
                listStr+=" Aug 01 15:59 ";
                listStr+=fileInfo.fileName();
                listStr+="\r\n";
                //QString
                pDataOut->sendTextData(listStr);
            }

        pDataOut->sendTextData("end_of_file\r\n");
        pDataOut->close();
        qDebug()<<"Transfer complete";
        sendToClient("226 Transfer complete");

        }
        else{
            sendToClient("501 Error in parameters or arguments");
            return;
        }
        return;
    }
    if(!cmdIsUnderstoot) sendToClient("500 Command is not recognized");

}




void FTPSession::sendToClient(QString data){
    qDebug()<<"Server: "<<data;
    data+="\r\n";
    //write(QByteArray(data.toUtf8()));
    write(QByteArray(data.toUtf8()));
}



bool FTPSession::checkUserName(QString name){
    if(name=="anonymous")
        isAnonymous = true;
    return true;
}

bool FTPSession::checkUserPassword(QString pass){
    if(isAnonymous){
        if(pass.contains("@") && pass.contains(".") && pass.length()>3)
            return true;
        else return false;
    }
    if(pass=="1234") return true;
    return false;
}



QString FTPSession::getUserWorkDir(QString user){
    //return userName+"_work_directory";
    return currentDirectory.dirName();
}


bool FTPSession::setCurrentDirectory(QString dirName){
    currentDirectory.setPath(dirName);
    if(currentDirectory.isReadable())
        return true;
    else return false;
}
