/**
  *@brief 聊天窗口界面文件
  */
#ifndef CHATWIDGET_H
#define CHATWIDGET_H

#include <QWidget>
#include <QTextBrowser>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSplitter>
#include <QHostAddress>
#include <QFileDialog>
#include <QDate>
#include <QTime>
#include <QDateTime>
#include <QSet>
#include <QThread>
#include <QTcpSocket>
#include "sendmanager.h"
#include "sendfilewidget.h"

#ifdef Q_CC_MSVC
#pragma execution_character_set("utf-8")
#endif

/**
 * @brief The ChatWidget class
 */
class ChatWidget : public QWidget
{
    Q_OBJECT
public:
    enum EnumMessageType
    {
        enMessage = 1,
        enOnline      = 2,
        enOffline     = 3,
        enFile        = 4
    };

    explicit ChatWidget(QString myName, QHostAddress myHostAddress, QString m_chatName, QHostAddress m_chatIp, QWidget *parent = nullptr);
    ~ChatWidget();
    void closeEvent(QCloseEvent *event);
    QString &getChatName();
    QHostAddress &getChatIp();
    QSet<SendFileWidget *> &getSendFileWidgetSet();
    QString &getMyName();
    QHostAddress &getMyHostAddress();

    /**
     * @brief receiveChat
     * @param receiveMessage 显示接收到的消息内容到聊天窗口
     */
    void receiveChat(QString receiveMessage);

signals:

    /**
     * @brief 聊天窗口关闭时提醒好友面板，好友面板更新好友列表
     */
    void childClosed();
    void sendChat(ChatWidget::EnumMessageType type, QString chatMessage, QHostAddress chatIP);
    void writeFile();
    void sendSocketClose();

private slots:
    void onSendButtonClicked();
    void onCloseButtonClicked();
    void onSendFileButtonClicked();
    void onSendFileWidgetClose();

private:
    QTextBrowser *m_chatBrowser; //显示聊天消息框
    QTextEdit *m_chatEdit;//输入聊天信息框
    QPushButton *m_sendButton;//发送聊天消息按钮
    QPushButton *m_closeButton;//关闭聊天窗口按钮
    QPushButton* m_fileSendButton;//发送文件按钮
    QSet<SendFileWidget*> m_sendFileWidgetSet;//存放发送文件窗口的Set，便于统一析构

    QString m_myName;//自己的用户名
    QHostAddress m_myHostAddress;//自己的IP
    QString chatName;//对方的用户名
    QHostAddress m_ChatIp;//对方的IP

    bool m_respondingFlag = true;//是否已回复消息的标志
    bool m_receiveFlag = true;//接收到对方回复的标志

};

#endif // CHATWIDGET_H
