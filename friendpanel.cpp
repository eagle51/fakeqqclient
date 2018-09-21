#include "friendpanel.h"

Friendpanel::Friendpanel(QString name, QWidget *parent) : m_userName(name),QWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    m_friendNameLabel = new QLabel(this);
    m_friendNameLabel->setText(m_userName);

    QLineEdit *m_searchEdit = new QLineEdit(this);
    m_searchEdit->setPlaceholderText("搜索在线用户");
    QLabel *m_groupNameLabel = new QLabel(this);
    m_groupNameLabel->setText("好友列表");

    m_friendList = new FriendList(this);

    QVBoxLayout *m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->addWidget(m_friendNameLabel);
    m_mainLayout->addWidget(m_searchEdit);
    m_mainLayout->addWidget(m_groupNameLabel);
    m_mainLayout->addWidget(m_friendList);
    resize(250, 600);

    connect(m_friendList, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
            this, SLOT(onItemDoubleclicked(QListWidgetItem*)));
    receiveBroadcast();
    sendBroadcast();
}

Friendpanel::~Friendpanel()
{
    int i = 0;
    if(!m_listItemToChatWidget.empty())
    {
        for(auto iter = m_listItemToChatWidget.begin(); iter != m_listItemToChatWidget.end(); iter++)
        {
            qDebug() << "panel delete chatwidget";
            delete iter.value();
            i++;
            qDebug() << i;
        }
    }
    else
    {
        qDebug() << "list is empty!";
    }
}

void Friendpanel::closeEvent(QCloseEvent *m_event)
{
    int ret = 0;
    if(m_chatWidgetToListItem.empty())
    {
        if((ret = QMessageBox::question(this, "退出fakeqq", "确定退出？",
                                        QMessageBox::Yes | QMessageBox::No, QMessageBox::No)) == QMessageBox::No)
        {
            m_event->ignore();
        }
        else
        {
            sendCloseBroadcast();
        }
    }
    else
    {
        if((ret = QMessageBox::question(this, "退出fakeqq", "当前有聊天窗口未关闭，确定退出？",
                                        QMessageBox::Yes | QMessageBox::No, QMessageBox::No)) == QMessageBox::No)
        {
            m_event->ignore();
        }
        else
        {
            sendCloseBroadcast();
        }
    }
}

void Friendpanel::addChatWidget(QString name, QHostAddress m_hostAddress)
{
    MyListWidgetItem *m_unnameItem = new MyListWidgetItem(name,m_hostAddress, m_friendList);
    m_hostAddressToItem[m_hostAddress.toString()] = m_unnameItem;
    FriendBuddy *m_newBuddy = new FriendBuddy(this);
    m_newBuddy->setPath("../fakeqqclient/touxiang.jpg");
    m_newBuddy->setName(name);
    m_newBuddy->setIp(m_hostAddress.toString().mid(7));
    m_friendList->addBuddy(m_unnameItem, m_newBuddy);
}

void Friendpanel::sendBroadcast()
{
    qDebug() << "sendBroadcast";
    m_udpServer = new QUdpSocket(this);
    qDebug() << "消息头:" << makeHead(ChatWidget::EnumMessageType::enOnline);
    m_udpServer->writeDatagram(makeHead(ChatWidget::EnumMessageType::enOnline).toUtf8(), QHostAddress::Broadcast, 9109);
}

void Friendpanel::onItemDoubleclicked(QListWidgetItem *m_item)
{
    MyListWidgetItem* item = dynamic_cast<MyListWidgetItem* >(m_item);
    qDebug() << "双击打开聊天窗口";

    if(m_listItemToChatWidget.find(item) == m_listItemToChatWidget.end())
    {
        ChatWidget *m_chatform = new ChatWidget(getUserName(),
                                                QNetworkInterface().allAddresses().at(1),
                                                item->getHostName(), item->getHostAddress());//chatWidget的析构在friendpanel的析构函数中统一析构

        foreach (QString msg, m_messageToReadMap[item->getHostAddress().toString()])//读取未读消息
        {
            m_chatform->receiveChat(msg);
        }
        m_messageToReadMap.erase(m_messageToReadMap.find(item->getHostAddress().toString()));//发送完消息删除未读消息map中该IP对应的那一条

        m_hostAddressToChatWidget[item->getHostAddress().toString()] = m_chatform;
        m_chatWidgetToHostAddress[m_chatform] = item->getHostAddress().toString();
        m_listItemToChatWidget[item] = m_chatform;
        m_chatWidgetToListItem[m_chatform] = item;

        connect(m_chatform, SIGNAL(childClosed()), this, SLOT(onchildClosed()));
        connect(m_chatform, SIGNAL(sendChat(ChatWidget::EnumMessageType, QString, QHostAddress)),
                this, SLOT(sendMessage(ChatWidget::EnumMessageType, QString, QHostAddress)));

        m_chatform->show();
        m_chatform = nullptr;
    }

    FriendBuddy *temp = dynamic_cast<FriendBuddy*>(item->listWidget()->itemWidget(item));
    if(temp->getTwinkleFlag() == true)
    {
        temp->toggleTwinkle();
    }
    item = nullptr;
    temp = nullptr;
}

void Friendpanel::onchildClosed()
{
    ChatWidget* chatWidgetSender = qobject_cast<ChatWidget*>(sender());
    m_messageToReadMap.erase(m_messageToReadMap.find(chatWidgetSender->getChatIp().toString()));
    m_listItemToChatWidget.erase(m_listItemToChatWidget.find(m_chatWidgetToListItem[chatWidgetSender]));
    m_chatWidgetToListItem.erase(m_chatWidgetToListItem.find(chatWidgetSender));
    m_hostAddressToChatWidget.erase(m_hostAddressToChatWidget.find(m_chatWidgetToHostAddress[chatWidgetSender]));
    m_chatWidgetToHostAddress.erase(m_chatWidgetToHostAddress.find(chatWidgetSender));
    delete chatWidgetSender;
    chatWidgetSender = nullptr;
}

void Friendpanel::receiveBroadcast()
{
    qDebug() << "开始监听上线广播";
    m_udpClient = new QUdpSocket(this);
    m_udpClient->bind(9109, QUdpSocket::ShareAddress);
    connect(m_udpClient, SIGNAL(readyRead()), this, SLOT(onReceiverhand()));
}

void Friendpanel::sendCloseBroadcast()
{
    for(auto temp : onlineIpv4Set)
    {
        m_udpServer->writeDatagram(makeHead(ChatWidget::EnumMessageType::enOffline).toUtf8(), QHostAddress(temp), 9109);
    }
}

QString &Friendpanel::getUserName()
{
    return m_userName;
}

QString Friendpanel::makeHead(ChatWidget::EnumMessageType messageType)
{
    return QString::number(messageType, 10)+ "-" + m_userName + "-" + QString("::ffff:" +QNetworkInterface().allAddresses().at(1).toString()) + "-";
}

void Friendpanel::onReceiverhand()
{
    qDebug() << "in onReceivehand";
    QHostAddress* hostAddress = new QHostAddress;
    quint16 port = 0;
    m_udpmessage.resize(m_udpClient->pendingDatagramSize());
    m_udpClient->readDatagram(m_udpmessage.data(), m_udpmessage.size(), hostAddress, &port);
    QString ipv4 = hostAddress->toString();

    qDebug() << "收到来自IP: " << ipv4 << "的消息";
    QStringList msgList = QString::fromUtf8(m_udpmessage).split("-");//分割消息包，最前面是消息类型，然后是对方用户名，然后是对方IP，最后面是消息内容
    switch(msgList.at(0).toInt())
    {
    case ChatWidget::EnumMessageType::enMessage:
    {
        qDebug() << "收到聊天消息";
        if(m_hostAddressToChatWidget.find(msgList.at(2)) == m_hostAddressToChatWidget.end())
        {
            qDebug() << "发送未读消息";
            //根据host和listitem的映射获取到item的buddy,重写buddy的paintEvent使头像闪烁，同时将未读消息存在hostAddress和消息数组映射的map里，在new chatwidget的时候将消息显示到窗口里。
            FriendBuddy* temp = dynamic_cast<FriendBuddy*>(m_hostAddressToItem[msgList.at(2)]->listWidget()->itemWidget(m_hostAddressToItem[msgList.at(2)]));
            m_messageToReadMap[msgList.at(2)].push_back(msgList.at(3));
            if((temp->getName() != m_userName) && (m_flashFlag == false))
            {
                m_flashFlag  = true;
                if(false == temp->getTwinkleFlag())
                {
                    temp->toggleTwinkle();
                }
            }
            temp = nullptr;
        }
        else
        {
            //如果host已经有对应的chatwidget则将消息显示到chatWidget里
            ChatWidget* chatwidget = m_hostAddressToChatWidget.find(msgList.at(2)).value();
            chatwidget->receiveChat(msgList.at(3));
            chatwidget = nullptr;
        }
        break;
    }
    case ChatWidget::EnumMessageType::enOnline:
    {
        qDebug() << "收到上线消息";
        if(!onlineIpv4Set.contains(msgList.at(2)))
        {
            qDebug() << "userName: " << msgList.at(1)<< "userIp: " << msgList.at(2);
            addChatWidget(msgList.at(1), QHostAddress(msgList.at(2)));
            onlineIpv4Set.insert(msgList.at(2));
            m_udpClient->writeDatagram(makeHead(ChatWidget::EnumMessageType::enOnline).toUtf8(), QHostAddress(msgList.at(2)), 9109);
        }

        break;
    }
    case ChatWidget::EnumMessageType::enOffline:
    {
        qDebug() << "收到下线消息";
        qDebug() << "remove item";
        MyListWidgetItem *itemForDelete = m_hostAddressToItem[msgList.at(2)];
        m_friendList->takeItem(m_friendList->row(itemForDelete));
        onlineIpv4Set.erase(onlineIpv4Set.find(msgList.at(2)));
        m_hostAddressToItem.erase(m_hostAddressToItem.find(msgList.at(2)));
        delete itemForDelete;
        break;
    }
    case ChatWidget::EnumMessageType::enFile:
    {
        qDebug() << "收到发送文件消息";
        if(m_hostAddressToChatWidget.find(msgList.at(2)) != m_hostAddressToChatWidget.end())
        {
            //begin new Thread
            SendFileWidget* fileReceive = new SendFileWidget("",
                                                             m_hostAddressToChatWidget[msgList.at(2)]->getChatName(),
                    QHostAddress(msgList.at(2)));
            m_hostAddressToChatWidget[msgList.at(2)]->getSendFileWidgetSet().insert(fileReceive);
            fileReceive->setWindowTitle("文件接收");
            connect(fileReceive, SIGNAL(sendFileClose()), m_hostAddressToChatWidget[msgList.at(2)], SLOT(onSendFileWidgetClose()));
            fileReceive->show();

            QThread *receiveThread = new QThread(this);
            ReceiveManager* receiveManager = new ReceiveManager;
            receiveManager->moveToThread(receiveThread);
            connect(this, SIGNAL(readFile()), receiveManager, SLOT(receiveWork()));
            connect(receiveManager, SIGNAL(updateProgressSignal(int)), fileReceive, SLOT(updateWidget(int)));
            connect(receiveManager, SIGNAL(updateFileName(QString)), fileReceive, SLOT(updateFileNameSlot(QString)));
            connect(receiveManager, SIGNAL(received()), fileReceive, SLOT(finishedMessage()));
            connect(receiveManager, SIGNAL(received()), receiveThread, SLOT(quit()));
            connect(receiveThread, SIGNAL(finished()), receiveManager, SLOT(deleteLater()));
            connect(receiveManager, SIGNAL(sendClose()), fileReceive, SLOT(peerClose()));
            connect(fileReceive, SIGNAL(destroyThread()), receiveThread, SLOT(quit()));

            receiveThread->start();
            emit readFile();

            fileReceive = nullptr;
        }
        break;
    }
    default:
        break;
    }
    m_udpmessage.resize(0);
    delete hostAddress;
    hostAddress = nullptr;
}

void Friendpanel::sendMessage(ChatWidget::EnumMessageType type, QString chatMessage, QHostAddress chatIp)
{
    qDebug() << "in sendMessage";
    switch(type)
    {
    case ChatWidget::EnumMessageType::enMessage:
    {
        m_udpClient->writeDatagram((makeHead(ChatWidget::EnumMessageType::enMessage) + chatMessage).toUtf8(), chatIp, 9109);
        break;
    }
    case ChatWidget::EnumMessageType::enFile:
    {
        m_udpClient->writeDatagram(makeHead(ChatWidget::EnumMessageType::enFile).toUtf8(), chatIp, 9109);
        break;
    }
    default:
        break;
    }
}
