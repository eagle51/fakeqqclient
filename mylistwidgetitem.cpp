#include "mylistwidgetitem.h"

MyListWidgetItem::MyListWidgetItem(QString hostName, QHostAddress m_hostAddress, QListWidget *view, int type):m_hostName(hostName),m_hostAddress(m_hostAddress),QListWidgetItem(view)
{
    Q_UNUSED(type);
}

MyListWidgetItem::~MyListWidgetItem()
{
    qDebug() << "delete MyListWidgetItem";
}

QString &MyListWidgetItem::getHostName()
{
    return m_hostName;
}

QHostAddress &MyListWidgetItem::getHostAddress()
{
    return m_hostAddress;
}
