#include "friendlist.h"
//#include "chatwidget.h"

FriendList::FriendList(QWidget *parent):
    QListWidget(parent)
{
    setFocusPolicy(Qt::NoFocus);       // 去除item选中时的虚线边框

}

FriendList::~FriendList()
{
    qDebug() << "delete FriendList";
}

void FriendList::addBuddy(MyListWidgetItem *newItem, FriendBuddy * newBuddy)
{
    newItem->setSizeHint(QSize(54, 54));
    this->insertItem(this->count(), newItem);
    this->setItemWidget(newItem, newBuddy);
}


