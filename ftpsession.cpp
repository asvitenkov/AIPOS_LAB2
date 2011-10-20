#include "ftpsession.h"
#include <QDebug>
#include <QHostAddress>
#include <QStringList>
#include <QSysInfo>
#include <QRegExp>
#include <QDateTime>
#include <QAbstractSocket>
#include "ftpserver.h"
#include <QDir>
#include <QFile>
#include <QMutex>



QString FTPSession::getAbsolutePuth(QString aDirName){
    QString puth = "";
    if(!aDirName.isEmpty()){
        // ��� ������������ ����� ��� ���� �� ������
        if(aDirName[0]=='/'){
            // ���������� ����
            puth = userDir.replace("/","")+aDirName;
        }
        else{
            // ������������� ����
            puth = QString(currentDirectory.absolutePath()+"/"+aDirName).replace("//","/");
        }
    }
    return puth;
}

QString getArgument(QString aStr){
    QString argument="";
    int pos = aStr.indexOf(" ");
    if(pos != -1){
        argument = aStr.mid(pos+1,aStr.length()-pos);
    }

    return argument;
}

QString getCommand(QString aStr){
    QString cmd="";
    int pos = aStr.indexOf(" ");
    if(pos!=-1){
        cmd = aStr.mid(0,pos);
        return cmd;
    }
    return aStr;
}

FTPSession::FTPSession(QObject *parent):QTcpSocket(parent)
{
    connect(this,SIGNAL(readyRead()),this,SLOT(readClient()));
    passIsOk = false;
    loginIsOk = false;
    isAnonymous = false;
    type = "A";
    passiveMode = false;
    activMode = false;
    pasvPort.push_back(0);
    pasvPort.push_back(0);
    setUserDir("D:/");
    currentDirectory.setFilter(QDir::Dirs | QDir::Files );
    renameIsActive = false;
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
    QString cmd = getCommand(query); //qDebug()<<"CMD "<<cmd;
    QString argument = getArgument(query); //qDebug()<<"ARG "<<argument;
    QStringList list = query.split(" ");
    bool cmdIsUnderstoot = false;


    // ����� �٨ �� ����� ������ ������� �������������� ����� � ���������� ���������� ������ ����� ����� ���������� ��� � ���


    if(cmd=="USER"){
        cmdIsUnderstoot = true;
        if(!loginIsOk){
            // ����������� ��� �� ����
            if (checkUserName(argument)){
                loginIsOk = true;
                userName = argument;
                sendToClient("331 Password required for " + userName+" need password");
            }

        }
        else{
            // ����������� ��� ����, ������� ���������������� ������ ���
            sendToClient("503 user "+userName+" is already authorized");
        }
        return;
    }
    if(cmd=="PASS"){
        cmdIsUnderstoot = true;
        if(loginIsOk){
            // ������� ������� pass ����� ������� user
            if(!argument.isEmpty())
                if(checkUserPassword(argument)){
                    // ������ ������
                    sendToClient("230 user "+userName+" logged in");
                    passIsOk = true;
                }
                else{
                    // ������ �� ������
                    sendToClient("530 Not logged in, user or password incorrect!");
                }
            else{
                sendToClient("501 Error in parameters or arguments");
            }
        }
        else{
            //  ������� ������� pass ��� ������� user
            sendToClient("503 First enter a user name");
        }
        return;
    }

    if(!loginIsOk || !passIsOk){
        sendToClient("530 Not logged in, user or password incorrect!");
        return;
    }

    if(cmd=="SYST"){
        cmdIsUnderstoot = true;
        sendToClient("215 UNIX");
        return;
    }
    if(cmd=="FEAT"){
        cmdIsUnderstoot = true;
        sendToClient("211-Extensions supported");
        sendToClient("SIZE");
        sendToClient("211 end");
        return;
    }
    if(cmd=="PWD" || cmd=="XPWD"){
        cmdIsUnderstoot = true;
        sendToClient(QString("257 \"/%1\" is a current directory").arg(getUserWorkDir(userName)).replace("//","/"));
        return;
    }


//    if(cmd=="PASV"){
//        cmdIsUnderstoot = true;
//        passiveMode =true;
//        //             ����� �������� ����� � ����
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

    if(cmd=="TYPE"){
        cmdIsUnderstoot = true;
        if(argument=="I" || argument=="A"){
            type = argument;
            sendToClient("200 Type set to "+type);
        }
        else{
            sendToClient("501 Error in parameters or arguments");
        }
        return;
    }
    if(cmd=="PORT"){
        cmdIsUnderstoot = true;
        QRegExp texp("[0-9]{1,3}\\,[0-9]{1,3}\\,[0-9]{1,3}\\,[0-9]{1,3}\\,[0-9]{1,3}\\,[0-9]{1,3}");
        if(argument.contains(texp)){
            // ���������� ��������
            activMode = true;
            QList<int> lst;
            QStringList listAdress = (argument).split(",");
            for(int i=0; i<listAdress.size();i++) lst.push_back((listAdress[i].toInt()));
            activPort = lst;
            QString strAddr = "%1.%2.%3.%4";
            strAddr=strAddr.arg(activPort[0]).arg(activPort[1]).arg(activPort[2]).arg(activPort[3]);
            if(QHostAddress(strAddr).isNull() || activPort[4]>255 || activPort[5]>255){
                sendToClient("501 Error in parameters or arguments: wrong port");
                activPort.clear();
                return;
            }
            sendToClient("200 Port command successful");
        }
        else{
            // ������������ ��������
            sendToClient("501 Error in parameters or arguments");
        }
        return;
    }
    if(cmd=="LIST"){

        // ����� �������� ������ ������ � �����������
        // ��� ������ ����� ���������������� ������������ ����������
        // ���� ���� ��������, �� ��� ����� ���������� �� ���������

        // ����� ���������, � ����� �� �� ��������?
        // ��������� �� ������� ���� ��� ����ר� ��������� �����?
        // #######################################################
        cmdIsUnderstoot = true;

        if(!activPort.isEmpty() && activMode && activPort.size()==6){
            // �������� ����� � ����
            QString strAddr = "%1.%2.%3.%4";
            strAddr=strAddr.arg(activPort[0]).arg(activPort[1]).arg(activPort[2]).arg(activPort[3]);
//            if(QHostAddress(strAddr).isNull()){
//                activPort.clear();
//                sendToClient("550 Wrong PORT");
//                return;
//            }
//            else{
//                activPort.clear();
//                sendToClient("503 incorrect sequence of commands");
//                return;
//            }
            activeTextDataOut = new FTPActiveTextDataOut(QHostAddress(strAddr),activPort[4]*256+activPort[5],localAddress(),20,this);
            connect(activeTextDataOut,SIGNAL(errorSendTexdDataSignal()),this,SLOT(activeTransferTextDataErrorSlot()));
            connect(activeTextDataOut,SIGNAL(connectionCloseByClientSignal()),this,SLOT(activeTransferTextDataConnectionCloseByClientSlot()));
            connect(activeTextDataOut,SIGNAL(sendTextDataSuccessfulSignal()),this,SLOT(activeTransferTextDataSuccessfulSlot()));
            activPort.clear();
        }
        else if(passiveMode && pasvPort.size()==2){
            // ��������� ����� � ��������� ����
        }
        else{
            //������������ ������������������ ������
            sendToClient("503 incorrect command sequence");
            return;
        }


        QString listStr;
        if(argument.isEmpty()){
            // ��� ���������
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
                else ;
            }
            if(activMode){
                activMode = false;
                activeTextDataOut->disconnectFromHost();
            }
            if(passiveMode){
                passiveMode = false;
            }


        }
        else{
            sendToClient("501 Error in parameters or arguments");

        }
        return;
    }
    if(cmd=="CWD"){
        cmdIsUnderstoot = true;
        QString dirName=argument.replace("\\","/").replace("//","/");
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
            // ������� � �����
            setUserDir(userDir);
            sendToClient((QString("250 Command successful \"/%1\" is a current directory").arg(getUserWorkDir(userName).replace("//","/"))));
            return;
        }

        dirName = getAbsolutePuth(dirName).replace("\\","/").replace("\\","/").replace("//","/").replace("//","/");
        qDebug();
        QDir tmpDir(dirName);
        if(tmpDir.exists()){
            // ����� ���������� ���� ���������, ���� �� � ��� ������
            if(tmpDir.isReadable()){
                // ������� ��������
                currentDirectory=tmpDir;
                sendToClient((QString("250 Command successful \"%1\" is a current directory").arg(getUserWorkDir(userName).replace("\\","/").replace("\\","/").replace("//","/").replace("//","/"))));
                return;
            }
            else{
                // ������� ����������
                sendToClient(QString("550 CWD failed \"%1\" directory is not readable").arg(dirName));
                return;
            }
        }
        else{
            // ����� ���������� �� ����������
            sendToClient(QString("550 CWD failed \"%1\" directory not found").arg(dirName));
            return;
        }
        return;
    }
    if(cmd=="MKD" || cmd=="XMKD"){
        cmdIsUnderstoot = true;
        if(!checkPermissionCreateDir(userName)){
            sendToClient("550 Can't create directory. Permission denied");
            return;
        }
        QString dirName=argument.replace("\\","/").replace("//","/");
        dirName = getAbsolutePuth(dirName);


        if(dirName.isEmpty()){
            sendToClient("501 Error in parameters or arguments");
            return;
        }

        QDir tempDir(dirName);
        if(!tempDir.exists()){
            // ������ �������� ���
            // ������� ��� �������
            if(currentDirectory.mkpath(dirName)){
                // ������� ������
                sendToClient(QString("257 \"%1\" created successfully").arg(dirName).replace(userDir,""));
            }
            else{
                // ������� �� ������
                sendToClient(QString("550 \"/%1\" directory not exist").arg(dirName));
            }
        }
        else{
            // ����� ������� ���������� ���
            sendToClient("550 Directory alredy exists");
        }
        return;
    }
    if(cmd=="CDUP"){
        if(currentDirectory.absolutePath()!=userDir)currentDirectory.cdUp();
        sendToClient((QString("250 Command successful \"/%1\" is a current directory").arg(getUserWorkDir(userName).replace("//","/").replace("//","/"))));
        return;
    }
    if(cmd=="QUIT"){
        cmdIsUnderstoot = true;
        sendToClient("220 Good Bye");
        emit sessionClose(this->socketDescriptor());
        return;
    }
    if(cmd=="RMD" || cmd=="XRMD"){
        cmdIsUnderstoot = true;

        QString dirName=argument.replace("\\","/").replace("//","/");
        //QString tempName
        dirName = getAbsolutePuth(dirName);
        if(dirName.isEmpty()){
            sendToClient("501 Error in parameters or arguments");
            return;
        }
        // ����� ���������, ���������� �� �����
        QDir tmpDir(dirName);
        if(tmpDir.exists()){
            // ���������� ����������
            // ���������, ���� �� ������� � ����������
            if(tmpDir.isReadable()){
                // ������ � �������� ����
                // �������� ������� �������
                if(tmpDir.rmdir(dirName)){
                    // ������� �����
                    sendToClient(QString("250 %1 command seccussful").arg(cmd));
                }
                else{
                    // ������� �� �����
                    // �����, ���� �� ������� ���� ������ � ��� �� ������
                    if((tmpDir.entryList(QStringList("*"),QDir::Dirs| QDir::Files)).isEmpty()){
                        // ������� ������
                        sendToClient(QString("550 Command %1 is not executed").arg(cmd));
                    }
                    else{
                        //������� �� ������
                        sendToClient(QString("550 Command %1 is not executed: directory %2 is not empty").arg(cmd).arg(tmpDir.absolutePath().replace(userDir,"").replace("//","/")));
                    }
                }
            }
            else{
                // ������� � �������� ���
                sendToClient(QString("550 CWD failed \"%1\" directory is not readable").arg(dirName).replace("//","/"));
            }
        }
        else{
            // ���������� �� ����������
            sendToClient(QString("550 CWD failed \"%1\" directory not found").arg(dirName).replace("//","/"));
        }
        return;
    }
    if(cmd=="DELE"){
        cmdIsUnderstoot = true;

        QString filePath=argument.replace("\\","/").replace("//","/");
        //QString tempName
        //filePath = getAbsolutePuth(dirName);
        if(filePath.isEmpty()){
            sendToClient("501 Error in parameters or arguments");
            return;
        }
        QString tempName = getAbsolutePuth(filePath);

        QFile delFile(tempName);
        // ���������, ���������� �� ����
        if(delFile.exists()){
            // ���� ����������
            // ���������, ���� �� � ���� ������
            if(delFile.remove()){
                sendToClient(QString("250 %1 command successful").arg(cmd));
            }
            else{
                //qDebug()<<delFile.errorString();
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
            // ����� ���� �� ����������
            sendToClient(QString("550 \"/%1\" file not exist").arg(filePath).replace("//","/"));
        }

        return;
    }
    if(cmd=="RNFR"){
        cmdIsUnderstoot = true;
        QString targetName=argument.replace("\\","/").replace("//","/");;

        if(targetName.isEmpty()){
            // �������� ����
            // �������� ������
            sendToClient("501 Error in parameters or arguments");
            return;
        }


        // �������� �� ������
        QString tempName = getAbsolutePuth(targetName);

        QFileInfo tmpFileInfo(tempName);
        // ����� ��������, ���� �� ����� ����� ��� ����
        if(tmpFileInfo.exists()){
            // ������ ����
            QString targetObj;
            if(tmpFileInfo.isFile())targetObj="File";
            else targetObj="Directory";
            sendToClient(QString("350 "+targetObj+" exists, ready for destination name"));
            renameIsActive = true;
            renameObj = tmpFileInfo;
        }
        else{
            // ������� ���
            sendToClient("550 file/directory not found");
        }
    }
    if(cmd=="RNTO"){
        cmdIsUnderstoot =true;
        if(!renameIsActive){
            // ������������ ������������������ ������
            sendToClient("503 incorrect sequence of commands");
            return;
        }
        // ���� ���� ��� �������������
        // �������� ��������
        QString targetName=argument.replace("\\","/").replace("//","/");

        if(targetName.isEmpty()){
            // �������� ������
            sendToClient("501 Error in parameters or arguments");
            renameIsActive = false;
            return;
        }
        QString tempName=getAbsolutePuth(targetName);

        // ���������, ����� ���� ��� ���������� �� ������� ����� �������������
        QFileInfo file(tempName);
        if(file.exists()){
            // ��� ����������
            QString targetObj;
            if(renameObj.isFile())targetObj="File";
            else targetObj="Directory";
            sendToClient("553 "+targetObj+" alredy exists");
            renameIsActive = false;
        }
        else{
            // �� ����������
            // ������� �������������

            QDir tmpDir;
            if(tmpDir.rename(renameObj.absoluteFilePath(),tempName)){
                // ������� �������������
                QString targetObj;
                if(renameObj.isFile())targetObj="File";
                else targetObj="Directory";
                sendToClient(QString("250 "+targetObj+" renamed successfully"));
                renameIsActive = false;
                renameObj.setFile(userDir);
            }
            else{
                // ������������� �� �������
                sendToClient("450 Internal error renaming the file");
                renameIsActive = false;
            }
        }
        return;
    }
    if(cmd=="RETR"){
        cmdIsUnderstoot = true;

        QString fileName=argument.replace("\\","/").replace("//","/");
        QString filePath;
        filePath = QString(currentDirectory.absolutePath()+"/"+fileName).replace("//","/");
        QFile *file = new QFile(filePath);

        if(!file->exists()){
            sendToClient(QString("550 RETR failied: file %1 not found").arg(fileName));
            return;
        }
        QString strAddr;
        if(activMode){
            activMode = false;
            if(activPort.size()==6){
                strAddr = "%1.%2.%3.%4";
                strAddr=strAddr.arg(activPort[0]).arg(activPort[1]).arg(activPort[2]).arg(activPort[3]);
                if(QHostAddress(strAddr).isNull()){
                    activPort.clear();
                    sendToClient("550 Wrong PORT");
                    return;
                }
            }
            else{
                activPort.clear();
                sendToClient("503 incorrect sequence of commands");
                return;
            }
            activeBinaryDataOut = new FTPActiveBinaryDataOut(QHostAddress(strAddr),activPort[4]*256+activPort[5],localAddress(),20,this);
            activPort.clear();
            sendToClient("150 Opening data channel for file transfer");
            if(file->open(QFile::ReadOnly)){
                if(file->isReadable()){
                    // ���� ����� ���������
                    connect(activeBinaryDataOut,SIGNAL(errorSendBinaryDataSignal()),this,SLOT(activeTransferBinaryDataErrorSlot()));
                    connect(activeBinaryDataOut,SIGNAL(connectionCloseByClientSignal()),this,SLOT(activeTransferBinaryDataConnectionCloseByClientSlot()));
                    connect(activeBinaryDataOut,SIGNAL(sendBinaryDataSuccessfulSignal()),this,SLOT(activeTransferBinaryDataSuccessfulSlot()));
                    connect(activeBinaryDataOut,SIGNAL(abortConnectionSignal()),this,SLOT(activeTransferBinaryDataAbortSlot()));
                    QDataStream *tmpStream = new QDataStream(file);
                    activeBinaryDataOut->sendFile(tmpStream);
                }
                else{
                    sendToClient("550 can't access file");
                    file->deleteLater();
                }
            }
            else{
                file->deleteLater();
                sendToClient("550 can't access file");
            }
        }
        if(passiveMode){
            passiveMode = false;
        }

        return;
    }
    if(cmd.indexOf("ABOR")!=-1){
        cmdIsUnderstoot = true;
        sendToClient("426 ");
        if(activeTextDataOut){
            activeTextDataOut->abort();
        }
        if(activeBinaryDataOut){
            activeBinaryDataOut->abortConnection();
        }
        if(activeBinaryDataIn){
            activeBinaryDataIn->abortConnection();
        }

    }
    if(cmd=="STOR"){
        cmdIsUnderstoot = true;

        QString targetName=argument.replace("\\","/").replace("//","/");
        QString tempName;
        if(targetName.isEmpty()){
            sendToClient("501 Error in parameters or arguments");
            return;
        }
        tempName = getAbsolutePuth(targetName);

        if(activMode){
            activMode = false;
            // ����� ���������, ���� �� � ��� ���� ��� ���
            QString strAddr="";
            if(activPort.size()==6){
                strAddr = "%1.%2.%3.%4";
                strAddr=strAddr.arg(activPort[0]).arg(activPort[1]).arg(activPort[2]).arg(activPort[3]);
                if(QHostAddress(strAddr).isNull()){
                    activPort.clear();
                    sendToClient("550 Wrong PORT");
                    return;
                }
            }
            else{
                activPort.clear();
                sendToClient("503 incorrect sequence of commands");
                return;
            }
            // ���� ���� ����� ������������� ����������

            QFile *file = new QFile(tempName);
            if(file->open(QIODevice::WriteOnly)){
                // ���� �������� �� ������
                activeBinaryDataIn = new FTPActiveBinaryDataIn(localAddress(),localPort(),this);
                activeBinaryDataIn->setSaveFile(file);
                activeBinaryDataIn->connectToHost(QHostAddress(strAddr),activPort[4]*256+activPort[5]);
                activPort.clear();
                sendToClient("150 Opening data channel for file transfer");

                connect(activeBinaryDataIn,SIGNAL(errorStoreDataInFileSignal()),this,SLOT(activeStoreBinaryDataErrorSlot()));
                connect(activeBinaryDataIn,SIGNAL(storeDataInFileCompleteSuccessfulSignal()),this,SLOT(activeStoreBinaryDataCompleteSuccessfulSlot()));
                connect(activeBinaryDataIn,SIGNAL(connectionCloseByClientSignal()),this,SLOT(activeStoreBinaryDataConnectionCloseByClientSlot()));
                connect(activeBinaryDataIn,SIGNAL(abortCommandCompleteSignal()),this,SLOT(activeStoreBinaryDataAbortSlot()));

                return;

            }
            else{
                // ���� �� �������� �� ������
                activPort.clear();
                sendToClient("450 Internal error error open file");
                return;
            }

        }
        else if(passiveMode){
            passiveMode = false;
        }
    else{
        // ������
        qDebug()<<"ne vklyuchen ne aktivnyi ne passivnyi rejim";
    }



}
    if(cmd=="SIZE"){
        cmdIsUnderstoot  = true;

        QString targetName="";

        for(int i=1;i<list.size();i++) targetName =targetName + " " + list[i];
        targetName = targetName.trimmed().replace("\\","/");
        while(targetName.indexOf("//")!=-1){
            targetName = targetName.replace("//","/");
        }
        if(!targetName.isEmpty()){
            // �������� �� ������

            QString tempName;
            if(targetName[0]=='/'){
                // ����������
                tempName = QString(userDir + targetName).replace("//","/");
            }
            else{
                // �������������
                QDir tmpDir(currentDirectory);
                tmpDir.cdUp();
                tempName = QString(currentDirectory.absolutePath()+"/"+targetName).replace("//","/");
            }
            QFileInfo fileInfo(tempName);
            if(fileInfo.exists()){
                // ����� ���� ���� � ��� ����� ��������
                sendToClient("213 "+QString::number(fileInfo.size()));
            }
            else{
                sendToClient("550 File not found");
            }
        }
        else{
            // �������� ������
            sendToClient("501 Error in parameters or arguments");
        }
        return;
    }
    if(!cmdIsUnderstoot) sendToClient("500 Command is not recognized");

}





void FTPSession::sendToClient(QString data){
    qDebug()<<QTime::currentTime().toString("hh:mm:ss")+" Server: "<<data.replace("\r\n","");
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
    // ��� � ������ �����-�� �������� �� ���� ��� ������=)
    return true;
}

void FTPSession::activeTransferTextDataConnectionCloseByClientSlot(){
    //qDebug()<<"FTPSession::activeTransferTextDataConnectionCloseByClient()";
    activeTextDataOut = NULL;
    sender()->deleteLater();

}

void FTPSession::activeTransferTextDataErrorSlot(){
    //qDebug()<<"FTPSession::activeTransferTextDataErrorSlot()";
    sendToClient("451 Local error, operation aborted");
    activeTextDataOut = NULL;
    sender()->deleteLater();
}

void FTPSession::activeTransferTextDataSuccessfulSlot(){
    //qDebug()<<"FTPSession::activeTransferTextDataSuccessfulSlot()";
    sendToClient("226 Transfer complete");
    activeTextDataOut = NULL;
    sender()->deleteLater();
}

void FTPSession::activeTransferTextDataAbortSlot(){
    //qDebug()<<"void FTPSession::activeTransferTextDataAbortSlot()";
    sendToClient("226 ");
    activeTextDataOut = NULL;
    sender()->deleteLater();
}

void FTPSession::activeTransferBinaryDataConnectionCloseByClientSlot(){
    //qDebug()<<"FTPSession::activeTransferBinaryDataConnectionCloseByClientSlot";
    sender()->deleteLater();
    activeTextDataOut = NULL;

}

void FTPSession::activeTransferBinaryDataErrorSlot(){
    //qDebug()<<"FTPSession::activeTransferBinaryDataErrorSlot";
    sendToClient("451 Local error, operation aborted");
    sender()->deleteLater();
    activeBinaryDataOut = NULL;

}

void FTPSession::activeTransferBinaryDataSuccessfulSlot(){
    //qDebug()<<"FTPSession::activeTransferBinaryDataSuccessfulSlot";
    sendToClient("226 Transfer complete");
    activeBinaryDataOut = NULL;
    sender()->deleteLater();
}


void FTPSession::activeTransferBinaryDataAbortSlot(){
    //qDebug()<<"FTPSession::activeTransferBinaryDataAbortSlot";
    sendToClient("226 Abort command successful");
    activeBinaryDataOut = NULL;
    sender()->deleteLater();
}


void FTPSession::activeStoreBinaryDataAbortSlot(){
    //qDebug()<<"FTPSession::activeStoreBinaryDataAbortSlot()";
    activeBinaryDataIn = NULL;
    sender()->deleteLater();
    sendToClient("226 Abort command successful");
}

void FTPSession::activeStoreBinaryDataCompleteSuccessfulSlot(){
    //qDebug()<<"FTPSession::activeStoreBinaryDataCompleteSuccessfulSlot()";
    activeBinaryDataIn = NULL;
    sender()->deleteLater();
    sendToClient("226 Transfer OK");
}

void FTPSession::activeStoreBinaryDataConnectionCloseByClientSlot(){
    activeBinaryDataIn = NULL;
    //qDebug()<<"FTPSession::activeStoreBinaryDataConnectionCloseByClientSlot()";
    sender()->deleteLater();
}

void FTPSession::activeStoreBinaryDataErrorSlot(){
    activeBinaryDataIn = NULL;
    //qDebug()<<"FTPSession::activeStoreBinaryDataErrorSlot()";
    sendToClient("451 Local error, operation aborted");
    sender()->deleteLater();
}



FTPSession::~FTPSession(){
    if(activeTextDataOut) activeTextDataOut->deleteLater();
    if(activeBinaryDataOut) activeBinaryDataOut->deleteLater();
    if(activeBinaryDataIn) activeBinaryDataIn->deleteLater();
}
