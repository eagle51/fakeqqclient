#ifndef FRIENDBUDDY_H
#define FRIENDBUDDY_H

#include <QWidget>
#include <QTimer>
#include <QLabel>
#include <QEvent>
#include <QPainter>
#include <QPixmap>
#include <QDebug>

#ifdef Q_CC_MSVC
#pragma execution_character_set("utf-8")
#endif

class FriendBuddy : public QWidget
{
    Q_OBJECT
public:
    explicit FriendBuddy(QWidget *parent = nullptr);
    ~FriendBuddy();
    void initUI();

    void toggleTwinkle();
    bool getTwinkleFlag();
    void setName(QString buddyname);
    QString getName();
    void setIp(QString buddyip);
    void setPath(QString headpath);

protected:
    bool eventFilter(QObject *watched, QEvent *event);

signals:

private slots:
    void onTimerUpdate();

private:
    QWidget* m_head;
    QLabel* m_name;
    QLabel* m_ip;
    QString m_headPath;

    QTimer *m_timer;
    bool m_timerFlag = true;
    bool b_twinkleFlag = false;
};

#endif // FRIENDBUDDY_H
