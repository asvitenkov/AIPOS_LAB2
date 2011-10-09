#include "ftpsession.h"
#include <QDebug>
#include <QHostAddress>
#include <QStringList>


FTPSession::FTPSession(QObject *parent):QTcpSocket(parent)
{
    connect(this,SIGNAL(readyRead()),this,SLOT(readClient()));
    passIsOk = false;
    loginIsOk = false;
    isAnonymous = false;

}


void FTPSession::readClient(){
    QString query = QString::fromUtf8(this->readAll()).replace("\r\n","");
    parsingQuery(query);
}


void FTPSession::parsingQuery(QString query){
    query = query.replace("\r\n","");
    QStringList list = query.split(" ");
    if(list.size()==1){
        //������ ��� ���������

    }
    else if(list.size()==2){
        // ������ � ����������
        qDebug()<<list[0]<<" " <<list[1]<<endl;
        if(list[0]=="USER"){
            if(!loginIsOk){
                // ����������� ��� �� ����
                if (checkUserName(list[1])){
                    loginIsOk = true;
                    userName = list[1];
                    if(!isAnonymous)
                        sendToClient("331 Password required for " + userName+" need password");
                    else sendToClient("331 Annonumous access allowed, send identity (e-mail name) as a password");
                }
                else{
                    // ������ ����� ���
//                    �����������������
//                    �����������������
//                    �����������������
//                    ATTENTION
                    // ��� �� ����� ���� ��, ���� �� �������� ����� � ������, � ����� ��� �������� �����������
                    // �.�. ���� �������� ������ ����� ����� ������, �� ����� ����� ��������� ��� �����
                    sendToClient("530 access denied");
                }

//                else if(list[1]=="anonymous"){
//                    // ���������� ������
//                    isAnonymous = true;
//                    userName = list[1];
//                    sendToClient("331 Annonumous access allowed, send identity (e-mail name) as a password");
//                }
            }
            else{
                // ����������� ��� ����, ������� ���������������� ������ ���
                sendToClient("503 user "+userName+" is already authorized");

            }
        }
        if(list[0]=="PASS"){
            if(loginIsOk){
                // ������� ������� pass ����� ������� user
                if(checkUserPassword(list[1])){
                    // ������ ������
                }
                else{
                    // ������ �� ������
                }
            }
            else{
                //  ������� ������� pass ��� ������� user
            }
        }

    }
    else{
        // ���������� ������ ������
    }


}




void FTPSession::sendToClient(QString data){
    data+="\r\n";
    write(QByteArray(data.toUtf8()));
}



bool FTPSession::checkUserName(QString name){
    if(name=="svet") return true;
    if(userName=="anonymous"){
        isAnonymous = true;
        return true;
    }
    return false;
}

bool FTPSession::checkUserPassword(QString pass){
    if(userName=="anonymous") return true;
    if(pass=="1234") return true;
    return false;
}
