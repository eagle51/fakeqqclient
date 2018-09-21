#ifndef FRIENDLIST_H
#define FRIENDLIST_H
#include <QListWidget>
#include "friendbuddy.h"
#include "mylistwidgetitem.h"

#ifdef Q_CC_MSVC
#pragma execution_character_set("utf-8")
#endif


class FriendList : public QListWidget
{
    Q_OBJECT
public:
    FriendList(QWidget *parent = nullptr);
    ~FriendList();
    void addBuddy(MyListWidgetItem *newItem, FriendBuddy *newBuddy);

signals:

private slots:

private:
};

#endif // FRIENDLIST_H
