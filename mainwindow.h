#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ftpserver.h"
#include <QMainWindow>

namespace Ui {
    class MainWindow;
}

/**
 * @brief �����-��������� �������
 *
 */
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
/**
 * @brief ������ ���� �������
 *
 * @param parent ������������ ������
 */
    MainWindow(QWidget *parent = 0);
    /**
     * @brief ���������� ������
     *
     */
    ~MainWindow();

protected:

private:
    Ui::MainWindow *ui; /**< TODO ���������� ����������*/

    FTPServer *server; /**< TODO ������ �������*/

private slots:
    /**
     * @brief ��������� ����� ������ � ���
     *
     * @param aRecord ���������
     */
    void addRecordInLog(QString aRecord);
    /**
     * @brief ��������� ������
     *
     */
    void runServerSlot();
    /**
     * @brief ������������� ������
     *
     */
    void stopServerSlot();

    /**
     * @brief ������� ��� �������
     *
     */
    void clearLogSlot();
};

#endif // MAINWINDOW_H
