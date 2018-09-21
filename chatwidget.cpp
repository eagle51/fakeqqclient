#include "chatwidget.h"

ChatWidget::ChatWidget(QString myName, QHostAddress myHostAddress, QString m_chatName, QHostAddress m_chatIp, QWidget *parent)
    :m_myName(myName), m_myHostAddress(myHostAddress), chatName(m_chatName),m_ChatIp(m_chatIp),QWidget(parent)
{

    resize(600,600);
    setWindowTitle(chatName + m_ChatIp.toString().mid(7));

    m_chatBrowser = new QTextBrowser(this);
    m_chatEdit = new QTextEdit(this);
    m_chatEdit->setMaximumHeight(150);

    m_sendButton = new QPushButton(this);
    m_sendButton->setText("发送");
    m_closeButton = new QPushButton(this);
    m_closeButton->setText("关闭");
    m_fileSendButton = new QPushButton(this);
    m_fileSendButton->setText("发送文件");

    QVBoxLayout *m_mainLayout = new QVBoxLayout;
    QHBoxLayout *m_buttonLayout = new QHBoxLayout;
    QSplitter* m_splitter = new QSplitter;

    m_splitter->addWidget(m_chatBrowser);
    m_splitter->addWidget(m_chatEdit);
    m_splitter->setOrientation(Qt::Vertical);
    m_mainLayout->addWidget(m_splitter);
    m_buttonLayout->addStretch();
    m_buttonLayout->addWidget(m_fileSendButton);
    m_buttonLayout->addWidget(m_closeButton);
    m_buttonLayout->addWidget(m_sendButton);
    m_mainLayout->addLayout(m_buttonLayout);

    setLayout(m_mainLayout);
    connect(m_sendButton, SIGNAL(clicked()), this, SLOT(onSendButtonClicked()));
    connect(m_closeButton, SIGNAL(clicked()), this, SLOT(onCloseButtonClicked()));
    connect(m_fileSendButton, SIGNAL(clicked()), this, SLOT(onSendFileButtonClicked()));
}

ChatWidget::~ChatWidget()
{
    qDebug() << "delete chatwidget";
    int i = 0;
    if(!m_sendFileWidgetSet.empty())
    {
        for(SendFileWidget* temp : m_sendFileWidgetSet)
        {
            delete temp;
            i++;
            qDebug() << i;
        }
    }
}

void ChatWidget::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    emit childClosed();
}

QString &ChatWidget::getChatName()
{
    return chatName;
}

QHostAddress &ChatWidget::getChatIp()
{
    return m_ChatIp;
}

QSet<SendFileWidget *> &ChatWidget::getSendFileWidgetSet()
{
    return m_sendFileWidgetSet;
}

QString &ChatWidget::getMyName()
{
    return m_myName;
}

QHostAddress &ChatWidget::getMyHostAddress()
{
    return m_myHostAddress;
}

void ChatWidget::receiveChat(QString receiveMessage)
{
    qDebug() << "in receiveChat";
    qDebug() << "receiveMessage" << receiveMessage;
    qDebug() << "chatip: " << getChatIp().toString();
    m_respondingFlag = !m_respondingFlag;
    if(m_receiveFlag)
    {
        m_receiveFlag = !m_receiveFlag;
        m_chatBrowser->append(getChatName() + "(" + getChatIp().toString().mid(7) + ")" + QDateTime::currentDateTime().toString("yy/MM/dd hh:mm:ss"));
    }
    m_chatBrowser->append(receiveMessage);
}

void ChatWidget::onSendFileButtonClicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    "Open file", "../fakeqqclient");


    if(fileName != "")
    {
        emit sendChat(EnumMessageType::enFile, "", getChatIp());
        SendFileWidget* fileSend = new SendFileWidget(fileName, getChatName(), getChatIp());
        m_sendFileWidgetSet.insert(fileSend);
        connect(fileSend, SIGNAL(sendFileClose()), this, SLOT(onSendFileWidgetClose()));
        fileSend->show();

        QThread* sendThread = new QThread(this);
        SendManager* sendManager = new SendManager(fileName, getChatIp());
        sendManager->moveToThread(sendThread);
        connect(fileSend, SIGNAL(sendPause()), sendManager, SLOT(onSendPauseSlot()));
        connect(fileSend, SIGNAL(sendContinue()), sendManager, SLOT(onSendContinueSlot()));
        connect(sendManager, SIGNAL(updateProgressSignal(int)), fileSend, SLOT(updateWidget(int)));
        connect(sendManager, SIGNAL(sendFinished()), sendThread, SLOT(quit()));
        connect(sendManager, SIGNAL(receiveHaveClosed()), fileSend, SLOT(receiveClosed()));
        connect(fileSend, SIGNAL(destroyThread()), sendThread, SLOT(quit()));
        connect(this, SIGNAL(writeFile()), sendManager, SLOT(sendWork()));
        connect(this, SIGNAL(sendSocketClose()), sendManager, SLOT(onSendSocketClose()));
        connect(sendManager, SIGNAL(sendFinished()), fileSend, SLOT(finishedMessage()));
        connect(sendThread, SIGNAL(finished()), sendManager, SLOT(deleteLater()));

        sendThread->start();
        emit writeFile();
        fileSend = nullptr;
    }
}

void ChatWidget::onSendFileWidgetClose()
{
    SendFileWidget* sendWidgetForClose = qobject_cast<SendFileWidget*>(sender());
    //    delete sendWidgetForClose;在聊天窗口的析构函数中中统一析构
//    m_sendFileWidgetSet.erase(m_sendFileWidgetSet.find(sendWidgetForClose));
    emit sendSocketClose();
    sendWidgetForClose = nullptr;
}

void ChatWidget::onSendButtonClicked()
{
    qDebug() << "点击发送键";
    if(m_respondingFlag)
    {
        m_receiveFlag = !m_receiveFlag;
        m_respondingFlag = !m_respondingFlag;
        m_chatBrowser->append(getMyName() + "(" + getMyHostAddress().toString() + ")" + QDateTime::currentDateTime().toString("yy/MM/dd hh:mm:ss"));
        qDebug() << "我的ip" << getMyHostAddress().toString();
    }
    m_chatBrowser->append(m_chatEdit->toPlainText());
    emit sendChat(EnumMessageType::enMessage, this->m_chatEdit->toPlainText(), getChatIp());
    m_chatEdit->clear();
}

void ChatWidget::onCloseButtonClicked()
{
    this->close();
}
