#include "friendbuddy.h"

FriendBuddy::FriendBuddy(QWidget *parent) : QWidget(parent)
{
    initUI();
}

FriendBuddy::~FriendBuddy()
{
    qDebug() << "delete FriendBuddy";
}

void FriendBuddy::initUI()
{
    m_head = new QWidget(this);
    m_name = new QLabel(this);
    m_ip = new QLabel(this);

    m_head->setFixedSize(40, 40);

    QPalette m_color;
    m_color.setColor(QPalette::Text, Qt::gray);
    m_ip->setPalette(m_color);

    m_head->move(7, 7);
    m_name->move(54,10);
    m_ip->move(54, 27);

    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(onTimerUpdate()));

    m_head->installEventFilter(this);
}

void FriendBuddy::toggleTwinkle()
{
    if(false == b_twinkleFlag)
    {
        m_timer->start(500);
    }
    else
    {
        m_timer->stop();
        m_timerFlag = true;
        update();
    }
    b_twinkleFlag = !b_twinkleFlag;
}

bool FriendBuddy::getTwinkleFlag()
{
    return b_twinkleFlag;
}

void FriendBuddy::setName(QString buddyname)
{
    m_name->setText(buddyname);
}

QString FriendBuddy::getName()
{
    return m_name->text();
}

void FriendBuddy::setIp(QString buddyip)
{
    m_ip->setText(buddyip);
}

void FriendBuddy::setPath(QString headpath)
{
    m_headPath = headpath;
}

bool FriendBuddy::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == m_head)
    {
        if(event->type() == QEvent::Paint)
        {
            if(m_timerFlag == true)
            {
                QPainter m_painter(m_head);
                m_painter.drawPixmap(m_head->rect(),QPixmap(m_headPath));
            }
        }
    }
    return QWidget::eventFilter(watched,event);
}

void FriendBuddy::onTimerUpdate()
{
    m_timerFlag = !m_timerFlag;
    update();
}
