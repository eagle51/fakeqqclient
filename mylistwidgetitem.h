#ifndef MYLISTWIDGETITEM_H
#define MYLISTWIDGETITEM_H
#include <QListWidgetItem>
#include <QHostAddress>

#ifdef Q_CC_MSVC
#pragma execution_character_set("utf-8")
#endif

class MyListWidgetItem : public QListWidgetItem
{
public:
    MyListWidgetItem(QString m_hostName, QHostAddress m_hostAddress, QListWidget *view = Q_NULLPTR, int type = Type);
    ~MyListWidgetItem();
    QString& getHostName();
    QHostAddress& getHostAddress();

private:
    QString m_hostName;//好友Item的用户名
    QHostAddress m_hostAddress;//好友Item的用户IP
};

#endif // MYLISTWIDGETITEM_H
