#include "ftpsession.h"
#include <QDebug>
#include <QHostAddress>
#include <QStringList>
#include <QSysInfo>
#include <QRegExp>


FTPSession::FTPSession(QObject *parent):QTcpSocket(parent)
{
    connect(this,SIGNAL(readyRead()),this,SLOT(readClient()));
    passIsOk = false;
    loginIsOk = false;
    isAnonymous = false;
    type = "I";
    passiveMode = false;
    pasvPort.push_back(0);
    pasvPort.push_back(0);
    setCurrentDirectory("D:\\");

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
    // но есть команды которые могут содержать аргументы а могут их и не содержать
    if(list.size()==1){
        //qDebug()<<list[0];
        //запрос без агрумента
        if(list[0]=="SYST"){
            cmdIsUnderstoot = true;
            sendToClient("215 UNIX");
        }
        if(list[0]=="FEAT"){
            cmdIsUnderstoot = true;
            sendToClient("211-Extensions supported");
            sendToClient("SIZE");
            sendToClient("211 end");
        }
        if(list[0]=="PWD"){
            cmdIsUnderstoot = true;
            sendToClient("257 " + QString("\"\\")+getUserWorkDir(userName)+"\" is a current directory");
        }
        if(list[0]=="PASV"){
            cmdIsUnderstoot = true;
            passiveMode =true;
            // нужно отослать адрес и порт
            pasvPort[0]=qrand()%255+1;
            pasvPort[1]=qrand()%255;
            sendToClient("227 Entering Passive mode ("+peerAddress().toString().replace(".",",")+
                         ","+QString::number(pasvPort[0])+","+QString::number(pasvPort[1])+")");
        }
        if(!cmdIsUnderstoot) sendToClient("500 Command is not recognized");
    }
    else if(list.size()==2){
        // запрос с аргументом
        //qDebug()<<list[0]<<" " <<list[1];
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
        }
        if(list[0]=="PASS"){
            cmdIsUnderstoot = true;
            if(loginIsOk){
                // принята команда pass после команды user
                if(checkUserPassword(list[1])){
                    // пароль принят
                    sendToClient("230 user "+userName+" logged in");
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
        }
        if(list[0]=="TYPE"){
            cmdIsUnderstoot = true;
            if(list[1]=="A" || list[1]=="I") type = list[1];
            sendToClient("200 Type set to "+type);
        }
        if(list[0]=="PORT"){
            cmdIsUnderstoot = true;
            QRegExp texp("[0-9]{1,3}\\,[0-9]{1,3}\\,[0-9]{1,3}\\,[0-9]{1,3}\\,[0-9]{1,3}\\,[0-9]{1,3}");
            if((list[1]).contains(texp)){
                // корректный аргумент
                QList<int> lst;
                QStringList listAdress = (list[1]).split(",");
                for(int i=0; i<listAdress.size();i++) lst.push_back((listAdress[i].toInt()));
            }
            else{
                // некорректный аргумент
                sendToClient("501 Error in parameters or arguments");
            }
        }
        if(!cmdIsUnderstoot) sendToClient("500 Command is not recognized");
    }
    else{
        // непонятный запрос ошибка
        sendToClient("501 Error in parameters or arguments");
    }


}




void FTPSession::sendToClient(QString data){
    qDebug()<<"Server: "<<data;
    data+="\r\n";
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
    return userName+"_work_directory";
}


bool FTPSession::setCurrentDirectory(QString dirName){
    currentDirectory.setPath(dirName);
    if(currentDirectory.isReadable())
        return true;
    else return false;
}
