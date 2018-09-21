#ifndef QQCLIENT_H
#define QQCLIENT_H

#include <QWidget>
#include <QLineEdit>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QDebug>
#include <QListWidget>
#include "friendpanel.h"
//#include "friendlist.h"

#ifdef Q_CC_MSVC
#pragma execution_character_set("utf-8")
#endif

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = 0);
    ~Widget();
    void createFriendPanel();

signals:

private slots:
    void onloginButtonClicked();

private:
    QLineEdit *m_userNameEdit = nullptr;
    QLineEdit *m_ipEdit = nullptr;
    Friendpanel *m_friendPanelWidget = nullptr;
};

#endif // QQCLIENT_H
