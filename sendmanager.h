#ifndef SENDMANAGER_H
#define SENDMANAGER_H

#include <QObject>
#include <QTcpSocket>
#include <QFile>
#include <QMessageBox>
#include <QHostAddress>
#include <QDebug>
#include <QFileInfo>
#include <QtGlobal>

#ifdef Q_CC_MSVC
#pragma execution_character_set("utf-8")
#endif

class SendManager : public QObject
{
    Q_OBJECT
public:
    explicit SendManager(QString fileName, QHostAddress sendIp, QObject *parent = nullptr);
    ~SendManager();

signals:
    void updateProgressSignal(int);
    void sendFinished();
    void receiveHaveClosed();

private slots:
    void sendWork();
    void startSend();
    void sendContinue(qint64 numBytes);
    void resumeSend();
    void onSendPauseSlot();
    void onSendContinueSlot();
    void onSendSocketClose();
    void receiveClose();

private:
    QString m_fileName;//发送文件的文件名
    QHostAddress m_sendIp;//接收方的IP
    QTcpSocket* m_sendSocket;

    qint64 m_totalBytes = 0;//发送文件的总大小
    qint64 m_bytesWritten = 0;//已发送文件的大小
    qint64 m_bytesToWrite = 0;//还剩下未发送的文件大小
    QByteArray m_outBlock;//发送的数据块
    QFile* m_sendFile;
    qint64 m_filePos = 0;//断点续传的位置

    int m_proBarValue = 0;
    const qint64 BLOCKSIZE = 40960;//发送数据块大小设置为40k，这样减少了文件发送进程向主进程的发送窗口发送信号的频率。
};

#endif // SENDMANAGER_H
