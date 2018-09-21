#include "receivemanager.h"

ReceiveManager::ReceiveManager(QObject *parent) : QObject(parent)
{
}

ReceiveManager::~ReceiveManager()
{
    qDebug() << "delete ReceiveManager";
}

void ReceiveManager::receiveWork()
{
    qDebug() << "inreceiveWork";
    m_receiveServer = new QTcpServer(this);
    m_receiveServer->listen(QHostAddress::Any,9211);
    connect(m_receiveServer,SIGNAL(newConnection()),this,SLOT(newConnectionSlot()));

}


void ReceiveManager::newConnectionSlot()
{
    m_receiveSocket = m_receiveServer->nextPendingConnection();
    connect(m_receiveSocket, SIGNAL(readyRead()), this, SLOT(startReceive()));
    connect(m_receiveSocket, SIGNAL(disconnected()), this, SLOT(onSendClose()));
}

void ReceiveManager::startReceive()
{
    qDebug() << "in startReceive";
    m_inBlock.resize(0);
    QDataStream in(m_receiveSocket);
    in.setVersion(QDataStream::Qt_5_9);

    if(m_bytesReceived <= (sizeof(qint64) + sizeof(quint16)))
    {
        qDebug() << "receive head";
        if((m_receiveSocket->bytesAvailable() >= qint64(sizeof(qint64)+sizeof(quint16)))
                &&(m_fileNameSize == 0))
        {
            in >> m_totalRecvBytes >> m_fileNameSize;
            qDebug() << "TotalRecvBytes" << m_totalRecvBytes  << "fileNameSize" << m_fileNameSize;
            m_bytesReceived += sizeof(qint64);
            m_bytesReceived += sizeof(quint16);
        }
        else
        {
            qDebug() << "文件头信息错误";
            return;
        }

        if((m_receiveSocket->bytesAvailable() >= m_fileNameSize) &&
                (m_fileNameSize != 0))
        {
            in >> m_receivedFileName;
            emit updateFileName(m_receivedFileName);
            m_bytesReceived += m_fileNameSize;

            m_receivedFile = new QFile(m_receivedFileName);
            if(!m_receivedFile->open(QFile::Append))
            {
                qDebug() << "新建文件失败";
                return;
            }
            else
            {
                //发送已经接收到的文件大小给对端。
                qint64 filePos = m_receivedFile->size();
                m_bytesReceived += filePos;
                QByteArray filePosBlock;
                QDataStream sendFilePos(&filePosBlock,QIODevice::WriteOnly);
                sendFilePos.setVersion(QDataStream::Qt_5_9);

                sendFilePos <<  filePos;
                m_receiveSocket->write(filePosBlock);
                filePosBlock.resize(0);
            }
        }
        else
        {
            return;
        }
    }

    if(m_bytesReceived < m_totalRecvBytes)
    {
        qint64 tmpAvailable = m_receiveSocket->bytesAvailable();
        if(tmpAvailable > 0)
        {
            if(tmpAvailable <= (m_totalRecvBytes - m_bytesReceived))
            {
                qDebug()<< "在上面";
                m_bytesReceived += tmpAvailable;
                //            totalBytesReceived += tmpAvailable;
                m_inBlock = m_receiveSocket->readAll();
                m_receivedFile->write(m_inBlock);
                m_inBlock.resize(0);
                tmpAvailable = 0;
            }
            else
            {
                qDebug()<< "在下面";
                m_inBlock = m_receiveSocket->read(m_totalRecvBytes - m_bytesReceived);
                m_bytesReceived = m_totalRecvBytes;
                m_receivedFile->write(m_inBlock);
                m_inBlock.resize(0);
                tmpAvailable = 0;
            }
        }
    }

    m_proBarValue = (int)(m_bytesReceived*100/m_totalRecvBytes);
    qDebug() << "receive proBarValue" << m_proBarValue;
    emit updateProgressSignal(m_proBarValue);

    if(m_bytesReceived == m_totalRecvBytes)
    {
        qDebug() << "接收总大小" << m_bytesReceived;
        m_bytesReceived = 0;
        m_totalRecvBytes = 0;
        m_fileNameSize = 0;
        m_receivedFile->close();
        delete m_receivedFile;
        qDebug() << "接收文件结束";
        emit received();
    }
}

void ReceiveManager::onSendClose()
{
    if(m_bytesReceived < m_totalRecvBytes)
    {
        emit sendClose();
    }
}
