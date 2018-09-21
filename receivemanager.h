#ifndef RECEIVEMANAGER_H
#define RECEIVEMANAGER_H

#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>
#include <QFile>
#include <QDataStream>
#include <QDebug>

#ifdef Q_CC_MSVC
#pragma execution_character_set("utf-8")
#endif

class ReceiveManager : public QObject
{
    Q_OBJECT
public:
    explicit ReceiveManager(QObject *parent = nullptr);
    ~ReceiveManager();

signals:
    void updateProgressSignal(int);
    void updateFileName(QString);
    void received();
    void sendClose();

private slots:
    void newConnectionSlot();
    void receiveWork();
    void startReceive();
    void onSendClose();

private:
    QTcpServer* m_receiveServer;//接收文件的tcpserver
    QTcpSocket* m_receiveSocket;

    qint64 m_totalRecvBytes = 0; //总共需接收的字节数
    qint64 m_bytesReceived = 0; //已接收字节数
    quint16 m_fileNameSize = 0;//文件名大小
    QString  m_receivedFileName; //文件名

    QFile* m_receivedFile;//接收文件指针
    QByteArray m_inBlock;//每次接收的数据块大小
    int m_proBarValue;//接收文件的进度条值
};

#endif // RECEIVEMANAGER_H
