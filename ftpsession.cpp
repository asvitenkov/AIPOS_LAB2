#include "ftpsession.h"
#include <QDebug>
#include <QHostAddress>
#include <QStringList>
#include <QSysInfo>
#include <QRegExp>
#include <QDateTime>
#include <QAbstractSocket>
#include "ftpserver.h"


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
    setUserDir("D:/");
    //setCurrentDirectory("D:\\TESTFTP");
    currentDirectory.setFilter(QDir::Dirs | QDir::Files );

}


void FTPSession::readClient(){
    //QString query = QString::fromUtf8(this->readAll()).replace("\r\n","");
    //QString query = this->readAll().replace("\r\n","");
    QString query;
    QByteArray array;
    array = this->readAll();
    QString s1,s2,s3,s4,s5,s6,s7,s8;
    s1=array;
    s2=s2.fromAscii(array);
    s3=s3.fromLatin1(array);
    s4=s4.fromLocal8Bit(array);
    s5=s5.fromUtf8(array);
    query = query.fromUtf8(array.data());
    query = query.toLocal8Bit();
    parsingQuery(s4);
}


void FTPSession::parsingQuery(QString query){
    qDebug()<<"Client: "<<query.replace("\r\n","");
    query = query.replace("\r\n","");
    QStringList list = query.split(" ");
    bool cmdIsUnderstoot = false;


    // НУЖНО ЕЩЁ ВО ВРЕМЯ КОМАНД СТИРАТЬ ИСПОЛЬЗОВАННЫЕ ПОРТЫ И ЛОГИЧЕСКИЕ ПЕРЕМЕННЫЙ МЕНЯТЬ НУЖНО ПОТОМ ПОСМОТРЕТЬ ГДЕ И ЧТО


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
            if(list.size()>=2)
                if(checkUserPassword(list[1])){
                    // пароль принят
                    sendToClient("230 user "+userName+" logged in");
                    passIsOk = true;
                }
                else{
                    // пароль не принят
                    sendToClient("530 Not logged in, user or password incorrect!");
                }
            else{
                sendToClient("501 Error in parameters or arguments");
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
    if(list[0]=="PWD" || list[0]=="XPWD"){
        cmdIsUnderstoot = true;
        sendToClient(QString("257 \"/%1\" is a current directory").arg(getUserWorkDir(userName)));
        //sendToClient(QString("257 ") + "\"/\" is a current directory");
        return;
    }


    //    if(list[0]=="PASV"){
    //        cmdIsUnderstoot = true;
    //        passiveMode =true;
    //        // нужно отослать адрес и порт
    ////        pasvPort[0]=qrand()%255+1;
    ////        pasvPort[1]=qrand()%255;
    ////        sendToClient("227 Entering Passive mode ("+peerAddress().toString().replace(".",",")+
    ////                     ","+QString::number(pasvPort[0])+","+QString::number(pasvPort[1])+")");

    //        pasvPort[0]=250;
    //        pasvPort[1]=128;
    //        sendToClient("227 Entering Passive mode ("+peerAddress().toString().replace(".",",")+
    //                     ","+QString::number(pasvPort[0])+","+QString::number(pasvPort[1])+")");

    //        return;
    //    }

    if(list[0]=="TYPE"){
        cmdIsUnderstoot = true;
        if(list[1]=="A" || list[1]=="I"){
            type = list[1];
            sendToClient("200 Type set to "+type);
        }
        else{
            sendToClient("501 Error in parameters or arguments");
        }
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
            FTPDataOut *dataOut = new FTPDataOut(this->peerAddress(),this->peerPort(),pasvPort[0]*256+pasvPort[1],this);
            pDataOut = dataOut;
        }
        else{
            //неправильная последовательность команд
            sendToClient("503 incorrect command sequence");
            return;
        }


        QString listStr;
        if(list.size()==2){
            // есть аргумент
//            QFileInfoList fileInfoList = currentDirectory.entryInfoList();
//            QFileInfo fileInfo;
            QDateTime date;

            QStringList filters;
            filters<<list[1];
            QFileInfoList fileInfoList = currentDirectory.entryInfoList(filters,QDir::Dirs | QDir::Files);
            QFileInfo fileInfo;
            if(!fileInfoList.isEmpty()){
                sendToClient("150 Opening data channel for directory list");
                listStr.clear();
                fileInfo=fileInfoList.takeFirst();
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
                pDataOut->sendTextData(listStr);
                pDataOut->sendTextData("end_of_file\r\n");
                sendToClient("226 Transfer complete");
            }
            else{
                sendToClient(QString("550 Directory \"%1\" is not found").arg(list[1]));
            }
            pDataOut->close();
            return;

        }
        else if(list.size()==1){
            // без аргумента
            sendToClient("150 Opening data channel for directory list");
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
            sendToClient("226 Transfer complete");

        }
        else{
            sendToClient("501 Error in parameters or arguments");
            return;
        }
        return;
    }
    if(list[0]=="NLST"){

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
            FTPDataOut *dataOut = new FTPDataOut(this->peerAddress(),this->peerPort(),pasvPort[0]*256+pasvPort[1],this);
            pDataOut = dataOut;
        }
        else{
            //неправильная последовательность команд
            sendToClient("503 incorrect command sequence");
            return;
        }


        QString listStr;
        if(list.size()==2){
            // есть аргумент
//            QFileInfoList fileInfoList = currentDirectory.entryInfoList();
//            QFileInfo fileInfo;
            QDateTime date;

            QStringList filters;
            filters<<list[1];
            QFileInfoList fileInfoList = currentDirectory.entryInfoList(filters,QDir::Dirs | QDir::Files);
            QFileInfo fileInfo;
            if(!fileInfoList.isEmpty()){
                sendToClient("150 Opening data channel for directory list");
                listStr.clear();
                fileInfo=fileInfoList.takeFirst();
                listStr+=fileInfo.fileName();
                listStr+="\r\n";
                pDataOut->sendTextData(listStr);
                pDataOut->sendTextData("end_of_file\r\n");
                sendToClient("226 Transfer complete");
            }
            else{
                sendToClient(QString("550 Directory \"%1\" is not found").arg(list[1]));
            }
            pDataOut->close();
            return;

        }
        else if(list.size()==1){
            // без аргумента
            sendToClient("150 Opening data channel for directory list");
            QFileInfoList fileInfoList = currentDirectory.entryInfoList();
            QFileInfo fileInfo;
            QDateTime date;

            for(int i=0;i<fileInfoList.size();i++){
                listStr.clear();
                fileInfo=fileInfoList[i];
                listStr+=fileInfo.fileName();
                listStr+="\r\n";
                //QString
                pDataOut->sendTextData(listStr);
            }

            pDataOut->sendTextData("end_of_file\r\n");
            pDataOut->close();
            sendToClient("226 Transfer complete");

        }
        else{
            sendToClient("501 Error in parameters or arguments");
            return;
        }
        return;
    }
    if(list[0]=="CWD"){
        cmdIsUnderstoot = true;
        QString dirName="";
        //if(!list.isEmpty()) dirName = (list[0]).trimmed();
        for(int i=1;i<list.size();i++) dirName =dirName + " " + list[i];
        //dirName = dirName.trimmed().replace("\\","/").replace("//","/");
        dirName = dirName.trimmed().replace("\\","/");
        while(dirName.indexOf("//")!=-1){
            dirName = dirName.replace("//","/");
        }
        qDebug()<<"DIRECTORY "<<dirName;
        if(dirName.isEmpty()){
            sendToClient("501 Error in parameters or arguments");
            return;
        }
        if(dirName==".."){
            currentDirectory.cdUp();
            sendToClient((QString("250 Command successful \"/%1\" is a current directory").arg(getUserWorkDir(userName))));
            return;
        }
        if(dirName=="/"){
            // переход к корню
            //currentDirectory.setPath("D:");
            setUserDir(userDir);
            sendToClient((QString("250 Command successful \"/%1\" is a current directory").arg(getUserWorkDir(userName))));
            return;
        }
        if(dirName[0]=='/'){
            // абсолютный путь
            QDir tmpDir(userDir+dirName);
            if(tmpDir.exists()){
                // такая директория есть проверяем, есть ли к ней доступ
                if(tmpDir.isReadable()){
                    // переход возможен
                    currentDirectory=tmpDir;
                    sendToClient((QString("250 Command successful \"%1\" is a current directory").arg(getUserWorkDir(userName))));
                    return;
                }
                else{
                    // переход невозможен
                    sendToClient(QString("550 CWD failed \"%1\" directory is not readable").arg(dirName));
                    return;
                }
            }
            else{
                // такой директории не существует
                sendToClient(QString("550 CWD failed \"%1\" directory not found").arg(dirName));
                return;
            }
        }
        QDir tmpDir(currentDirectory.absolutePath()+"\\"+dirName);
        //if(tmpDir.isReadable()

//        QStringList filters;
//        filters<<dirName;
//        QFileInfoList fileInfoList = currentDirectory.entryInfoList(filters,QDir::Dirs);
        qDebug()<<tmpDir.absolutePath();
        if(tmpDir.exists()){
            // такая директория есть проверяем, есть ли к ней доступ
            if(tmpDir.isReadable()){
                // переход возможен
                currentDirectory=tmpDir;
                sendToClient(QString("250 Command successful \"%1\" is a current directory").arg(getUserWorkDir(userName)));
                return;
            }
            else{
                // переход невозможен
                sendToClient(QString("550 CWD failed \"%1\" directory is not readable").arg(dirName));
                return;
            }
        }
        else{
            // такой директории не существует
            sendToClient(QString("550 CWD failed \"%1\" directory not found").arg(dirName));
            return;
        }
        return;
    }
    if(list[0]=="MKD" || list[0]=="XMKD"){
        cmdIsUnderstoot = true;
        if(!checkPermissionCreateDir(userName)){
            sendToClient("550 Can't create directory. Permission denied");
            return;
        }
        QString dirName="";
        for(int i=1;i<list.size();i++) dirName =dirName + " " + list[i];
        dirName = dirName.trimmed().replace("\\","/");
        while(dirName.indexOf("//")!=-1){
            dirName = dirName.replace("//","/");
        }
        QString tempName;
        if(!dirName.isEmpty()){
            // имя сосздаваемой папки или пути не пустое
            if(dirName[0]=='/'){
                // абсолютный путь
                tempName = userDir.replace("/","")+dirName;
            }
            else{
                // относительный путь
                tempName = currentDirectory.absolutePath()+dirName;
            }
        }
        else{
            // аргумент пуст
            sendToClient("501 Error in parameters or arguments");
            return;
        }
        QDir tempDir(tempName);
        if(!tempDir.exists()){
            // такого каталога нет
            // пробуем его создать
            if(currentDirectory.mkpath(tempName)){
                // каталог создан
                sendToClient(QString("257 \"%1\" created successfully").arg(tempName).replace(userDir,""));
            }
            else{
                // каталог не создан
                sendToClient(QString("550 \"/%1\" directory not exist").arg(tempName));
            }
        }
        else{
            // такой каталог существует уже
            sendToClient("550 Directory alredy exists");
        }
        return;
    }
    if(list[0]=="CDUP"){
        if(currentDirectory.absolutePath()!=userDir)currentDirectory.cdUp();
        sendToClient((QString("250 Command successful \"/%1\" is a current directory").arg(getUserWorkDir(userName))));
        return;
    }
    if(list[0]=="QUIT"){
        cmdIsUnderstoot = true;
        sendToClient("220 Good Bye");
        //dynamic_cast<FTPServer*>(this->parent())->sessionClose(this->socketDescriptor());
        emit sessionClose(this->socketDescriptor());
        return;
    }

    if(list[0]=="RMD" || list[0]=="XRMD"){
        cmdIsUnderstoot = true;

        QString dirName="";
        for(int i=1;i<list.size();i++) dirName =dirName + " " + list[i];
        dirName = dirName.trimmed().replace("\\","/");
        while(dirName.indexOf("//")!=-1){
            dirName = dirName.replace("//","/");
        }
        QString tempName;
        if(!dirName.isEmpty()){
            // имя сосздаваемой папки или пути не пустое
            if(dirName[0]=='/'){
                // абсолютный путь
                tempName = userDir.replace("/","")+dirName;
            }
            else{
                // относительный путь
                tempName = currentDirectory.absolutePath()+dirName;
            }


            QDir tmpDir(tempName);
        }
        else{
            // аргумент пуст
            sendToClient("501 Error in parameters or arguments");
            return;
        }

    }

    if(!cmdIsUnderstoot) sendToClient("500 Command is not recognized");

}




void FTPSession::sendToClient(QString data){
    qDebug()<<"Server: "<<data.replace("\r\n","");
    data+="\r\n";
    //write(QByteArray(data.toUtf8()));
//    write(QByteArray(data.toUtf8()));
    write(QByteArray(data.toLocal8Bit()));
}



bool FTPSession::checkUserName(QString name){
    if(name=="anonymous")
        isAnonymous = true;
    return true;
}

bool FTPSession::checkUserPassword(QString pass){
    if(isAnonymous){
        if(pass.contains("@") && pass.contains(".") && pass.length()>3){
            passIsOk =true;
            return true;
        }
        else return false;
    }
    if(pass=="1234"){
        passIsOk =true;
        return true;
    }
    return false;
}



QString FTPSession::getUserWorkDir(QString user){
    //return userName+"_work_directory";
    qDebug()<<currentDirectory.absolutePath()<<userDir;
    return currentDirectory.absolutePath().replace(userDir,"");
}


bool FTPSession::setCurrentDirectory(QString dirName){
    currentDirectory.setPath(dirName);
    if(currentDirectory.isReadable())
        return true;
    else return false;
}


void FTPSession::setUserDir(QString userDirName){
    userDir = userDirName;
    currentDirectory.setPath(userDir);
}


bool FTPSession::checkPermissionCreateDir(QString _userName){
    // тут в идеале какая-то проверка но пока нет ничего=)
    return true;
}
