#ifndef MAINDIALOG_H
#define MAINDIALOG_H

#include <QDialog>
#include <QTcpServer>
#include <QFile>
#include <QtNetwork>
#include <QMessageBox>
#include <QPainter>
#include <QDateTime>

QT_BEGIN_NAMESPACE
namespace Ui { class MainDialog; }
QT_END_NAMESPACE

class MainDialog : public QDialog
{
    Q_OBJECT

public:
    MainDialog(QWidget *parent = nullptr);
    ~MainDialog();

private:
    Ui::MainDialog *ui;


private:
    QTcpServer *m_tcpServer;                    // TCP服务器

    QTcpSocket *m_tcpTextSocket;                // 聊天消息文本数据Socket
    QTcpSocket *m_tcpFileSocket;                // 传输文件数据socket

    qint64 m_totalBytes;                        // 存放文件总大小数据
    qint64 m_bytesReceived;                     // 已收到文件数据的大小
    qint64 m_fileNameSize;                      // 文件名的大小信息

    QString m_fileName;                         // 存放文件名称
    QFile *m_localFile;                         // 本地文件
    QByteArray m_inBlock;

    QString getLocalIP();                       // 获取本机IP地址
    void paintEvent(QPaintEvent *ev);           // 更改窗口背景图片



protected:
    void closeEvent(QCloseEvent *event);        // 关闭窗口时，则服务器端停止监听


private slots:
    void onNewConnection();                     // QTcpServer的newConnection()信号
    void onClientConnected();                   // 文本数据 Socket connected
    void onClientDisconnected();                // C文本lient Socket disconnected
    void onSocketReadyRead();                   // 读取文本socket传入的数据


    void onFileClientConnected();               // 文件socket
    void onFileClientDisconnected();            // 文件socket


    void updateServerProgress();                // 更新进度条，接收数据
    void displayError(QAbstractSocket::SocketError socketError); // 显示错误



    void on_pushButton_StartListening_clicked();
    void on_pushButton_StopListening_clicked();
    void on_pushButton_SendData_clicked();
    void on_pushButton_ClearMsg_clicked();
    void on_pushButton_ExitSystem_clicked();



};
#endif // MAINDIALOG_H
