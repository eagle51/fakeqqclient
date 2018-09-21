#ifndef FRIENDPANEL_H
#define FRIENDPANEL_H

#include <QWidget>
#include <QLineEdit>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QDebug>
#include <QMessageBox>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMap>
#include <map>
#include <QSet>
#include <QList>
#include <QCloseEvent>
#include <QUdpSocket>
#include <QTcpServer>
#include <QTcpSocket>
#include <QNetworkInterface>
#include "chatwidget.h"
#include "friendlist.h"
//#include "friendbuddy.h"
//#include "mylistwidgetitem.h"
#include "receivemanager.h"

#ifdef Q_CC_MSVC
#pragma execution_character_set("utf-8")
#endif
class Friendpanel : public QWidget
{
    Q_OBJECT
public:
    explicit Friendpanel(QString name = "", QWidget *parent = nullptr);
    ~Friendpanel();
    void closeEvent(QCloseEvent* m_event);
    void addChatWidget(QString name, QHostAddress m_hostAddress);
    void sendBroadcast();
    void receiveBroadcast();
    void sendCloseBroadcast();
    QString &getUserName();
    QString makeHead(ChatWidget::EnumMessageType messageType);

signals:
    void receiveMessage(QString);
    void createChat(MyListWidgetItem *);
    void readFile();

private slots:
    void onItemDoubleclicked(QListWidgetItem* m_item);
    void onchildClosed();

    /**
     * @brief 接收消息的函数，从中分离出上线、下线、发送文件、聊天消息，分发给响应的处理类
     */
    void onReceiverhand();

    /**
     * @brief 将聊天消息转发给聊天窗口显示
     * @param type 消息类型
     * @param chatMessage 消息内容
     * @param chatIp 对方IP
     */
    void sendMessage(ChatWidget::EnumMessageType type, QString chatMessage, QHostAddress chatIp);

private:
    QMap<MyListWidgetItem*, ChatWidget*> m_listItemToChatWidget;//聊天好友列表中的好友Item和聊天窗口的对应关系,管理上线下线
    QMap<ChatWidget*, MyListWidgetItem*> m_chatWidgetToListItem;//反向对应关系，便于管理
    FriendList *m_friendList;//聊天好友列表继承自ListWidget
    QLabel *m_friendNameLabel;
    QMap<QString, QVector<QString>> m_messageToReadMap;//保存未读消息

    QUdpSocket* m_udpServer;//聊天所用到的udpserver
    QUdpSocket* m_udpClient;//聊天用的udpsocket
    QSet<QString> onlineIpv4Set;//已上线的用户IP集合
    QMap<QString, ChatWidget*> m_hostAddressToChatWidget;//IP和聊天窗口的对应关系，用来判断是否打开的聊天窗口，如果没有则将消息保存到未读消息
    QMap<ChatWidget*, QString> m_chatWidgetToHostAddress;//反向对应关系，便于管理
    QMap<QString, MyListWidgetItem*> m_hostAddressToItem;//IP和好友Item的对应关系，用来闪烁头像

    QString m_userName;//自己的用户名
    QByteArray m_udpmessage;//接收聊天消息的ByteArray

    QTcpServer* m_tcpserver;//发送文件的tcpServer,用来接收接收端发送过来的已接收文件大小，完成断点续传
    bool m_flashFlag = false;//用户头像是否闪烁的标志
};

#endif // FRIENDPANEL_H
