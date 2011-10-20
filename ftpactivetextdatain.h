#ifndef FTPACTIVETEXTDATAIN_H
#define FTPACTIVETEXTDATAIN_H

#include <QTcpSocket>

class FTPActiveTextDataIn : public QTcpSocket
{
    Q_OBJECT
public:
    explicit FTPActiveTextDataIn(QObject *parent = 0);

signals:

public slots:

};

#endif // FTPACTIVETEXTDATAIN_H
