#include "qqclient.h"
//#include "global.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    this->setFixedSize(350, 200);
    m_userNameEdit = new QLineEdit(this);
    m_ipEdit = new QLineEdit(this);
    QLabel *m_userNameLabel = new QLabel(this);
    m_userNameLabel->setText("Username:");
    QLabel *m_ipLabel = new QLabel(this);
    m_ipLabel->setText("Sex:");
    QPushButton *m_loginButton = new QPushButton(this);
    m_loginButton->setText("登录");
    m_loginButton->setDefault(true);

    QGridLayout *m_topLayout = new QGridLayout(this);
    m_topLayout->addWidget(m_userNameLabel, 0, 0);
    m_topLayout->addWidget(m_userNameEdit, 0, 1);
    m_topLayout->addWidget(m_ipLabel, 1, 0);
    m_topLayout->addWidget(m_ipEdit, 1, 1);
    m_topLayout->addWidget(m_loginButton,2,0,1,2);

    connect(m_loginButton, SIGNAL(clicked()), this, SLOT(onloginButtonClicked()));
    connect(m_userNameEdit, SIGNAL(returnPressed()), m_loginButton, SLOT(click()));

}

Widget::~Widget()
{
    qDebug() << "in qqclient delete";
    m_friendPanelWidget = nullptr;
}

void Widget::createFriendPanel()
{
    m_friendPanelWidget = new Friendpanel(m_userNameEdit->text());
    qDebug() << "input userName" << m_userNameEdit->text();
    m_friendPanelWidget->show();
}

void Widget::onloginButtonClicked()
{
    if(!(m_userNameEdit->text().isEmpty()))
    {
        this->hide();
        this->createFriendPanel();
    }
    else
    {
        QMessageBox::warning(this, "输入有误", "请输入用户名!", QMessageBox::Ok, QMessageBox::Ok);
    }
}
