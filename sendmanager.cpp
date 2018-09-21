#include "sendmanager.h"

SendManager::SendManager(QString fileName, QHostAddress sendIp, QObject *parent) : m_fileName(fileName), m_sendIp(sendIp), QObject(parent)
{

}

SendManager::~SendManager()
{
    qDebug() << "delete SendManager";
}

void SendManager::sendWork()
{

    qDebug() << "in sendWork";

    m_sendSocket = new QTcpSocket(this);
    m_sendSocket->connectToHost(m_sendIp, 9211);
    connect(m_sendSocket, SIGNAL(connected()), this, SLOT(startSend()));
    connect(m_sendSocket, SIGNAL(disconnected()), this, SLOT(receiveClose()));
    connect(m_sendSocket, SIGNAL(readyRead()), this, SLOT(resumeSend()));

}

void SendManager::startSend()
{
    m_outBlock.resize(0);
    qDebug() << "in startSend";

    m_sendFile = new QFile(m_fileName, this);
    QFileInfo sendFileInfo(m_fileName);
    qDebug() << sendFileInfo.fileName();

    if(!m_sendFile->open( QIODevice::ReadOnly))
    {
        QMessageBox::warning(NULL, tr("WARNING"),
                             tr("cannot open file %1:\n%2.").arg(m_fileName).arg(m_sendFile->errorString()));
    }
    m_totalBytes = m_sendFile->size();
    qDebug() << "totalbytes" << m_totalBytes;

    QDataStream sendOut(&m_outBlock,QIODevice::WriteOnly);
    sendOut.setVersion(QDataStream::Qt_5_9);

    //发送类型，数据大小，文件类型，文件名大小，文件名
    sendOut <<  qint64(0) << quint16(0) << sendFileInfo.fileName();
//    TotalBytes += outBlock.size();//outBlock代表sendOut目前的大小

    sendOut.device()->seek(0);
    sendOut << m_totalBytes << quint16(m_outBlock.size() - sizeof(qint64) - sizeof(quint16));
    m_sendSocket->write(m_outBlock); //writedata函数可以制定最大发送的大小，大于65535，但是需要重写writedata虚函数。

//    bytesToWrite = TotalBytes - outBlock.size();
    m_bytesToWrite = m_totalBytes;
    m_outBlock.resize(0);
}

void SendManager::sendContinue(qint64 numBytes)
{
    qDebug() << "in updateProgress";
    if(m_totalBytes == 0)
        return;
    m_bytesWritten += numBytes;
    m_proBarValue = (int)(m_bytesWritten*100/m_totalBytes);//传递进度条的百分比
    qDebug() << "proBarValue" << m_proBarValue;
    emit updateProgressSignal(m_proBarValue);
    if(m_bytesToWrite > 0)
    {
        m_outBlock = m_sendFile->read(qMin(m_bytesToWrite, BLOCKSIZE));
        m_bytesToWrite -= m_sendSocket->write(m_outBlock);
        m_outBlock.resize(0);
    }
    else
    {
        m_sendFile->close();
        delete m_sendFile;
        m_sendFile = nullptr;
        qDebug() << "文件发送结束";
        emit sendFinished();
    }
}

void SendManager::onSendPauseSlot()
{
    qDebug() << "onSendPause";
    disconnect(m_sendSocket, SIGNAL(m_bytesWritten(qint64)), this, SLOT(sendContinue(qint64)));
}

void SendManager::onSendContinueSlot()
{
    qDebug() << "onSendContinue";
    connect(m_sendSocket, SIGNAL(m_bytesWritten(qint64)), this, SLOT(sendContinue(qint64)));
    sendContinue(m_totalBytes - m_bytesToWrite - m_bytesWritten);
}

void SendManager::onSendSocketClose()
{
    m_sendSocket->disconnectFromHost();
}

void SendManager::receiveClose()
{
    emit receiveHaveClosed();
}

void SendManager::resumeSend()
{
    qDebug() << "in resumeSend";
    QDataStream inFilePos(m_sendSocket);
    inFilePos.setVersion(QDataStream::Qt_5_9);
    connect(m_sendSocket, SIGNAL(bytesWritten(qint64)), this, SLOT(sendContinue(qint64)));
    inFilePos >> m_filePos;
    qDebug() << "filePos " << m_filePos;
    m_sendFile->seek(m_filePos);
    m_bytesToWrite -= m_filePos;

    sendContinue(m_filePos);
}
