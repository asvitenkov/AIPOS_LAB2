#include "ftpsession.h"
#include <QDebug>
#include <QHostAddress>
#include <QStringList>
#include <QSysInfo>
#include <QRegExp>
#include <QDateTime>
#include <QAbstractSocket>
#include "ftpserver.h"
#include "ftpdatatransferchannel.h"
#include <QDir>
#include <QFile>
#include <QMutex>




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
    currentDirectory.setFilter(QDir::Dirs | QDir::Files );
    renameIsActive = false;
    activeTextOut = NULL;
    activBinaryOut = NULL;
    passiveTextOut = NULL;
    passiveBinaryOut = NULL;

    activeTextDataOut = NULL;
    activeBinaryDataOut = NULL;
    activeBinaryDataIn = NULL;


}


void FTPSession::readClient(){
    QString query;
    QByteArray array;
    array = this->readAll();
    parsingQuery(QString::fromLocal8Bit(array));
}


void FTPSession::parsingQuery(QString query){
    qDebug()<<QTime::currentTime().toString("hh:mm:ss:zzz")+" Client: "<<query.replace("\r\n","");
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
        sendToClient(QString("257 \"/%1\" is a current directory").arg(getUserWorkDir(userName)).replace("//","/"));
        //sendToClient(QString("257 ") + "\"/\" is a current directory");
        return;
    }


//    if(list[0]=="PASV"){
//        cmdIsUnderstoot = true;
//        passiveMode =true;
//        //             нужно отослать адрес и порт
////        pasvPort[0]=qrand()%255+1;
////        pasvPort[1]=qrand()%255;
////        sendToClient("227 Entering Passive mode ("+peerAddress().toString().replace(".",",")+
////                     ","+QString::number(pasvPort[0])+","+QString::number(pasvPort[1])+")");

//        pasvPort[0]=250;
//        pasvPort[1]=128;
//        sendToClient("227 Entering Passive mode ("+peerAddress().toString().replace(".",",")+
//                     ","+QString::number(pasvPort[0])+","+QString::number(pasvPort[1])+")");
//        passiveTransfer = new FTPDataTransferChannel(localAddress(),pasvPort[0]*256+pasvPort[1],this);
//        return;
//    }

    if(list[0]=="TYPE"){
        cmdIsUnderstoot = true;
        if(list[1]=="I"){
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
        QMutex mutex;
        mutex.lock();
        // нужно передать список файлов с информацией
        // для начала нужно сконфигурировать отправляемую информацию
        // если есть аргумент, то нам нужна информация по аргументу

        // НУЖНО ПРОВЕРИТЬ, А МОЖЕМ ЛИ МЫ ПЕРЕДАТЬ?
        // ВЫПОЛНЕНА ЛИ КОМАНДА ПОРТ ИЛИ ВКЛЮЧЁН ПАССИВНЫЙ РЕЖИМ?
        // #######################################################
        cmdIsUnderstoot = true;

        if(!activPort.isEmpty() && activMode && activPort.size()==6){
            // активные режим и порт
            QString strAddr = "%1.%2.%3.%4";
            strAddr=strAddr.arg(activPort[0]).arg(activPort[1]).arg(activPort[2]).arg(activPort[3]);
            activeTextDataOut = new FTPActiveTextDataOut(QHostAddress(strAddr),activPort[4]*256+activPort[5],localAddress(),20,this);
            connect(activeTextDataOut,SIGNAL(errorSendTexdDataSignal()),this,SLOT(activeTransferTextDataErrorSlot()));
            connect(activeTextDataOut,SIGNAL(connectionCloseByClientSignal()),this,SLOT(activeTransferTextDataConnectionCloseByClientSlot()));
            connect(activeTextDataOut,SIGNAL(sendTextDataSuccessfulSignal()),this,SLOT(activeTransferTextDataSuccessfulSlot()));
        }
        else if(passiveMode && pasvPort.size()==2){
            // пассивный режим и отосланый порт
        }
        else{
            //неправильная последовательность команд
            sendToClient("503 incorrect command sequence");
            return;
        }


        QString listStr;
        if(list.size()==1){
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
                if(activMode)activeTextDataOut->sendTextData(listStr);
                else passiveTextOut->sendTextData(listStr);
            }
            if(activMode){
                activeTextDataOut->disconnectFromHost();
            }
            if(passiveMode){
            }


        }
        else{
            sendToClient("501 Error in parameters or arguments");

        }
//        mutex.unlock();
        return;
    }
    if(list[0]=="CWD"){
        cmdIsUnderstoot = true;
        QString dirName="";
        for(int i=1;i<list.size();i++) dirName =dirName + " " + list[i];
        dirName = dirName.trimmed().replace("\\","/");
        while(dirName.indexOf("//")!=-1){
            dirName = dirName.replace("//","/");
        }
        if(dirName.isEmpty()){
            sendToClient("501 Error in parameters or arguments");
            return;
        }
        if(dirName==".."){
            currentDirectory.cdUp();
            sendToClient((QString("250 Command successful \"/%1\" is a current directory").arg(getUserWorkDir(userName).replace("//","/"))));
            return;
        }
        if(dirName=="/"){
            // переход к корню
            //currentDirectory.setPath("D:");
            setUserDir(userDir);
            sendToClient((QString("250 Command successful \"/%1\" is a current directory").arg(getUserWorkDir(userName).replace("//","/"))));
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
                    sendToClient((QString("250 Command successful \"/%1\" is a current directory").arg(getUserWorkDir(userName).replace("//","/"))));
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
        if(tmpDir.exists()){
            // такая директория есть проверяем, есть ли к ней доступ
            if(tmpDir.isReadable()){
                // переход возможен
                currentDirectory=tmpDir;
                sendToClient(QString("250 Command successful \"/%1\" is a current directory").arg(getUserWorkDir(userName).replace("//","/")));
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
        //qDebug()<<"DO "<<
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
                tempName = QString(currentDirectory.absolutePath()+"/"+dirName).replace("//","/");
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
        sendToClient((QString("250 Command successful \"/%1\" is a current directory").arg(getUserWorkDir(userName).replace("//","/"))));
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
                tempName = QString(currentDirectory.absolutePath()+"/"+dirName).replace("//","/");
            }

            // нужно проверить, существует ли папка

            QDir tmpDir(tempName);
            if(tmpDir.exists()){
                // директория существует
                // проверить, есть ли досутуп к директории
                if(tmpDir.isReadable()){
                    // доступ к каталогу есть
                    // пытаемся удалить каталог
                    if(tmpDir.rmdir(tempName)){
                        // каталог удалён
                        sendToClient(QString("250 %1 command seccussful").arg(list[0]));
                    }
                    else{
                        // каталог не удалён
                        // узнаём, пуст ли каталог либо ошибка в чём то другом
                        if((tmpDir.entryList(QStringList("*"),QDir::Dirs| QDir::Files)).isEmpty()){
                            // каталог пустой
                            sendToClient(QString("550 Command %1 is not executed").arg(list[0]));
                        }
                        else{
                            //каталог не пустой
                            sendToClient(QString("550 Command %1 is not executed: directory %2 is not empty").arg(list[0]).arg(tmpDir.absolutePath().replace(userDir,"").replace("//","/")));
                        }
                    }
                }
                else{
                    // доступа к каталогу нет
                    sendToClient(QString("550 CWD failed \"%1\" directory is not readable").arg(dirName).replace("//","/"));
                }

            }
            else{
                // директория не существует
                sendToClient(QString("550 CWD failed \"%1\" directory not found").arg(dirName).replace("//","/"));
            }
        }
        else{
            // аргумент пуст
            sendToClient("501 Error in parameters or arguments");
        }
        return;

    }
    if(list[0]=="DELE"){
        cmdIsUnderstoot = true;

        QString filePath="";
        for(int i=1;i<list.size();i++) filePath =filePath + " " + list[i];
        filePath = filePath.trimmed().replace("\\","/");
        while(filePath.indexOf("//")!=-1){
            filePath = filePath.replace("//","/");
        }
        QString tempName;
        if(!filePath.isEmpty()){
            // аргумент не пуст
            // проверить, абсолютный или относительный путь используется
            if(filePath[0]=='/'){
                // абсолютный путь
                tempName = userDir.replace("/","")+filePath;
            }
            else{
                // относительный путь
                tempName = QString(currentDirectory.absolutePath()+"/"+filePath).replace("//","/");
            }
            QFile delFile(tempName);
            // проверяем, существует ли файл
            if(delFile.exists()){
                // файл существует
                // проверяем, есть ли к нему доступ
                //delFile.isReadable()
                if(delFile.remove()){
                    sendToClient(QString("250 %1 command successful").arg(list[0]));
                }
                else{
                    //qDebug()<<delFile.error();
                    qDebug()<<delFile.errorString();
                    switch(delFile.error()){
                    case QFile::RemoveError:
                        sendToClient(QString("500 Failed to delete file %1: file could not be removed").arg(filePath).replace("//","/"));
                        break;
                    case QFile::FatalError:
                        sendToClient(QString("500 Failed to delete file %1: A fatal error").arg(filePath).replace("//","/"));
                        break;
                    default:
                        sendToClient(QString("500 Failed to delete file %1: An unspecified error").arg(filePath).replace("//","/"));
                        break;
                    }
                }

            }
            else{
                // такой файл не существует
                sendToClient(QString("550 \"/%1\" file not exist").arg(filePath).replace("//","/"));
            }

        }
        else{
            // аргумент пустой
            sendToClient("501 Error in parameters or arguments");
        }
        return;
    }
    if(list[0]=="RNFR"){
        cmdIsUnderstoot = true;
        QString targetName="";
        for(int i=1;i<list.size();i++) targetName =targetName + " " + list[i];
        targetName = targetName.trimmed().replace("\\","/");
        while(targetName.indexOf("//")!=-1){
            targetName = targetName.replace("//","/");
        }

        if(!targetName.isEmpty()){
            // аргумент не пустой
            // смотрим, путь относительный или абсолютный
            QString tempName;
            if(targetName[0]=='/'){
                // абсолютный
                tempName = QString(userDir + targetName).replace("//","/");
            }
            else{
                // относительный
                tempName = QString(currentDirectory.absolutePath()+"/"+targetName).replace("//","/");
            }
            //QDir tmpDir();
            QFileInfo tmpFileInfo(tempName);
            // нужно проверит, есть ли такая папка или файл
            if(tmpFileInfo.exists()){
                // объект есть
                QString targetObj;
                if(tmpFileInfo.isFile())targetObj="File";
                else targetObj="Directory";
                sendToClient(QString("350 "+targetObj+" exists, ready for destination name"));
                renameIsActive = true;
                renameObj = tmpFileInfo;
            }
            else{
                // объекта нет
                sendToClient("550 file/directory not found");
            }

        }
        else{
            // аргумент пуст
            // отсылаем ошибку
            sendToClient("501 Error in parameters or arguments");
        }

    }
    if(list[0]=="RNTO"){
        cmdIsUnderstoot =true;
        if(renameIsActive){
            // есть файл для реобразования
            // получаем аргумент
            QString targetName="";
            for(int i=1;i<list.size();i++) targetName =targetName + " " + list[i];
            targetName = targetName.trimmed().replace("\\","/");
            while(targetName.indexOf("//")!=-1){
                targetName = targetName.replace("//","/");
            }
            if(!targetName.isEmpty()){
                // аргумент не пустой

                QString tempName;
                if(targetName[0]=='/'){
                    // абсолютный
                    tempName = QString(userDir + targetName).replace("//","/");
                }
                else{
                    // относительный
                    QDir tmpDir(currentDirectory);
                    tmpDir.cdUp();
                    tempName = QString(currentDirectory.absolutePath()+"/"+targetName).replace("//","/");
                }

                // проверить, может файл уже существует на который хотят переименовать
                QFileInfo file(tempName);
                if(file.exists()){
                    // уже существует
                    QString targetObj;
                    if(renameObj.isFile())targetObj="File";
                    else targetObj="Directory";
                    sendToClient("553 "+targetObj+" alredy exists");
                    renameIsActive = false;
                }
                else{
                    // не существует
                    // пробуем переименовать

                    QDir tmpDir;
                    if(tmpDir.rename(renameObj.absoluteFilePath(),tempName)){
                        // успешно переименовали
                        QString targetObj;
                        if(renameObj.isFile())targetObj="File";
                        else targetObj="Directory";
                        sendToClient(QString("250 "+targetObj+" renamed successfully"));
                        renameIsActive = false;
                        renameObj.setFile(userDir);
                    }
                    else{
                        // переименовать не удалось
                        sendToClient("450 Internal error renaming the file");
                        renameIsActive = false;
                    }
                }
            }
            else{
                // аргумент пустой
                sendToClient("501 Error in parameters or arguments");
                renameIsActive = false;
            }
        }
        else{
            // неправильный порядок следовани команд
            sendToClient("503 incorrect sequence of commands");
        }
        return;
    }
    if(list[0]=="RETR"){
        QMutex mutex;
        mutex.lock();
        cmdIsUnderstoot = true;
        QString strAddr = "%1.%2.%3.%4";
        strAddr=strAddr.arg(activPort[0]).arg(activPort[1]).arg(activPort[2]).arg(activPort[3]);
        activeBinaryDataOut = new FTPActiveBinaryDataOut(QHostAddress(strAddr),activPort[4]*256+activPort[5],localAddress(),20,this);

        QString fileName="";
        for(int i=1;i<list.size();i++) fileName =fileName + " " + list[i];
        fileName = fileName.trimmed().replace("\\","/");
        while(fileName.indexOf("//")!=-1){
            fileName = fileName.replace("//","/");
        }

        QString filePath;
        filePath = QString(currentDirectory.absolutePath()+"/"+fileName).replace("//","/");
        QFile *file = new QFile(filePath);
        sendToClient("150 Opening data channel for file transfer");
        if(file->open(QFile::ReadOnly))
            if(file->isReadable()){
                // файл можно прочитать
                connect(activeBinaryDataOut,SIGNAL(errorSendBinaryDataSignal()),this,SLOT(activeTransferBinaryDataErrorSlot()));
                connect(activeBinaryDataOut,SIGNAL(connectionCloseByClientSignal()),this,SLOT(activeTransferBinaryDataConnectionCloseByClientSlot()));
                connect(activeBinaryDataOut,SIGNAL(sendBinaryDataSuccessfulSignal()),this,SLOT(activeTransferBinaryDataSuccessfulSlot()));
                activeBinaryDataOut->sendFile(file);
            }
            else{
                sendToClient("550 can't access file");
                file->deleteLater();
            }
        else{
            file->deleteLater();
            sendToClient("550 can't access file");
        }
        mutex.unlock();
        return;
    }
    if(list[0].indexOf("ABOR")!=-1){
        cmdIsUnderstoot = true;
        sendToClient("426 ");
        QMutex  mutex;
        mutex.lock();
        if(activeTextDataOut){
            disconnect(activeTextDataOut,SIGNAL(connectionCloseByClientSignal()),this,SLOT(activeTransferTextDataConnectionCloseByClientSlot()));
            connect(activeTextDataOut,SIGNAL(connectionCloseByClientSignal()),this,SLOT(activeTransferTextDataAbortSlot()));
            activeTextDataOut->abort();
        }
        if(activeBinaryDataOut){
            disconnect(activeBinaryDataOut,SIGNAL(connectionCloseByClientSignal()),this,SLOT(activeTransferBinaryDataConnectionCloseByClientSlot()));
            connect(activeBinaryDataOut,SIGNAL(connectionCloseByClientSignal()),this,SLOT(activeTransferBinaryDataAbortSlot()));
            activeBinaryDataOut->abort();
        }
        mutex.unlock();

    }
    if(list[0]=="STOR"){
        cmdIsUnderstoot = true;

        QString targetName="";
        for(int i=1;i<list.size();i++) targetName =targetName + " " + list[i];
        targetName = targetName.trimmed().replace("\\","/");
        while(targetName.indexOf("//")!=-1){
            targetName = targetName.replace("//","/");
        }
        QString tempName;
        if(!targetName.isEmpty()){
            // аргумент не пустой
            if(targetName[0]=='/'){
                // абсолютный
                tempName = QString(userDir + targetName).replace("//","/");
            }
            else{
                // относительный
                QDir tmpDir(currentDirectory);
                tmpDir.cdUp();
                tempName = QString(currentDirectory.absolutePath()+"/"+targetName).replace("//","/");
            }
        }
        else{
            // аргумент пустой
            sendToClient("501 Error in parameters or arguments");
            return;
        }

        if(activMode){
            // нужно проверить, есть ли у нас порт или нет
            if(activPort.size()==6){
                // порт есть можно устанавливать соединение
                QString strAddr = "%1.%2.%3.%4";
                strAddr=strAddr.arg(activPort[0]).arg(activPort[1]).arg(activPort[2]).arg(activPort[3]);

                QFile *file = new QFile(tempName);
                if(file->open(QIODevice::WriteOnly)){
                    // файл открылся на запись
                    activeBinaryDataIn = new FTPActiveBinaryDataIn(localAddress(),localPort(),this);
                    activeBinaryDataIn->setSaveFile(file);
                    activeBinaryDataIn->connectToHost(QHostAddress(strAddr),activPort[4]*256+activPort[5]);

                    sendToClient("150 Opening data channel for file transfer");

                    connect(activeBinaryDataIn,SIGNAL(errorStoreDataInFileSignal()),this,SLOT(activeStoreBinaryDataErrorSlot()));
                    connect(activeBinaryDataIn,SIGNAL(storeDataInFileCompleteSuccessfulSignal()),this,SLOT(activeStoreBinaryDataCompleteSuccessfulSlot()));
                    connect(activeBinaryDataIn,SIGNAL(connectionCloseByClientSignal()),this,SLOT(activeStoreBinaryDataConnectionCloseByClientSlot()));
                    connect(activeBinaryDataIn,SIGNAL(abortCommandCompleteSignal()),this,SLOT(activeStoreBinaryDataAbortSlot()));

                    return;

                }
                else{
                    // файл не открылся на запись
                    sendToClient("450 Internal error error open file");
                    return;
                }

            }
            else{
                // нет порта неправильная последовательность команд
                sendToClient("503 incorrect sequence of commands");
                return;
            }
        }
        else if(passiveMode){

        }
        else{
            // ошибка
            qDebug()<<"ne vklyuchen ne aktivnyi ne passivnyi rejim";
        }



    }
    if(list[0]=="SIZE"){
        cmdIsUnderstoot  = true;

        QString targetName="";

        for(int i=1;i<list.size();i++) targetName =targetName + " " + list[i];
        targetName = targetName.trimmed().replace("\\","/");
        while(targetName.indexOf("//")!=-1){
            targetName = targetName.replace("//","/");
        }
        if(!targetName.isEmpty()){
            // аргумент не пустой

            QString tempName;
            if(targetName[0]=='/'){
                // абсолютный
                tempName = QString(userDir + targetName).replace("//","/");
            }
            else{
                // относительный
                QDir tmpDir(currentDirectory);
                tmpDir.cdUp();
                tempName = QString(currentDirectory.absolutePath()+"/"+targetName).replace("//","/");
            }
            QFileInfo fileInfo(tempName);
            if(fileInfo.exists()){
                // такой файл есть и его можно прочесть
                sendToClient("213 "+QString::number(fileInfo.size()));
            }
            else{
                sendToClient("550 File not found");
            }
        }
        else{
            // аргумент пустой
            sendToClient("501 Error in parameters or arguments");
        }
        return;
    }
    if(!cmdIsUnderstoot) sendToClient("500 Command is not recognized");

}





void FTPSession::sendToClient(QString data){
    qDebug()<<QTime::currentTime().toString("hh:mm:ss:zzz")+" Server: "<<data.replace("\r\n","");
    data+="\r\n";
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

void FTPSession::activeTransferTextDataConnectionCloseByClientSlot(){
    qDebug()<<"FTPSession::activeTransferTextDataConnectionCloseByClient()";
    activeTextDataOut = NULL;
    sender()->deleteLater();

}

void FTPSession::activeTransferTextDataErrorSlot(){
    qDebug()<<"FTPSession::activeTransferTextDataErrorSlot()";
    sendToClient("451 Local error, operation aborted");
    activeTextDataOut = NULL;
    sender()->deleteLater();
}

void FTPSession::activeTransferTextDataSuccessfulSlot(){
    qDebug()<<"FTPSession::activeTransferTextDataSuccessfulSlot()";
    sendToClient("226 Transfer complete");
    activeTextDataOut = NULL;
    sender()->deleteLater();
}

void FTPSession::activeTransferTextDataAbortSlot(){
    qDebug()<<"void FTPSession::activeTransferTextDataAbortSlot()";
    sendToClient("226 ");
    activeTextDataOut = NULL;
    sender()->deleteLater();
}

void FTPSession::activeTransferBinaryDataConnectionCloseByClientSlot(){
    qDebug()<<"FTPSession::activeTransferBinaryDataConnectionCloseByClientSlot";
    sender()->deleteLater();
    activeTextDataOut = NULL;

}

void FTPSession::activeTransferBinaryDataErrorSlot(){
    qDebug()<<"FTPSession::activeTransferBinaryDataErrorSlot";
    sendToClient("451 Local error, operation aborted");
    sender()->deleteLater();
    activeBinaryDataOut = NULL;

}

void FTPSession::activeTransferBinaryDataSuccessfulSlot(){
    qDebug()<<"FTPSession::activeTransferBinaryDataSuccessfulSlot";
    sendToClient("226 Transfer complete");
    activeBinaryDataOut = NULL;
    sender()->deleteLater();
}


void FTPSession::activeTransferBinaryDataAbortSlot(){
    qDebug()<<"FTPSession::activeTransferBinaryDataAbortSlot";
    sendToClient("226 Abort command successful");
    activeBinaryDataOut = NULL;
    sender()->deleteLater();
}


void FTPSession::activeStoreBinaryDataAbortSlot(){
    qDebug()<<"FTPSession::activeStoreBinaryDataAbortSlot()";
    sender()->deleteLater();
    sendToClient("226 Abort command successful");
}

void FTPSession::activeStoreBinaryDataCompleteSuccessfulSlot(){
    qDebug()<<"FTPSession::activeStoreBinaryDataCompleteSuccessfulSlot()";
    sender()->deleteLater();
    sendToClient("226 Transfer OK");
}

void FTPSession::activeStoreBinaryDataConnectionCloseByClientSlot(){
    qDebug()<<"FTPSession::activeStoreBinaryDataConnectionCloseByClientSlot()";
    sender()->deleteLater();
}

void FTPSession::activeStoreBinaryDataErrorSlot(){
    qDebug()<<"FTPSession::activeStoreBinaryDataErrorSlot()";
    sendToClient("451 Local error, operation aborted");
    sender()->deleteLater();
}
