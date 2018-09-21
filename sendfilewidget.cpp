#include "sendfilewidget.h"

SendFileWidget::SendFileWidget(QString filename, QString hostName, QHostAddress hostAddress, QWidget *parent) :
    m_fileName(filename), m_hostName(hostName), m_hostAddress(hostAddress), QWidget(parent)
{
    resize(300,150);
    setWindowTitle("文件传送");
    m_pSendProgressBar = new QProgressBar(this);
    m_pSendProgressBar->setMaximum(100);
    m_pSendProgressBar->setMinimum(0);

    m_pPauseButton = new QPushButton("暂停", this);
    m_pContinueButton = new QPushButton("继续", this);
    m_pProgressLabel  = new QLabel("进度", this);
    m_pFilePathLabel = new QLabel(filename,this);
    m_sendFile = new QFile(m_fileName,this);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    QHBoxLayout* topLayout = new QHBoxLayout;
    QHBoxLayout* midLayout = new QHBoxLayout;

    topLayout->addWidget(m_pPauseButton);
    topLayout->addWidget(m_pContinueButton);

    midLayout->addWidget(m_pProgressLabel);
    midLayout->addWidget(m_pSendProgressBar);

    mainLayout->addLayout(topLayout);
    mainLayout->addStretch(1);
    mainLayout->addLayout(midLayout);
    mainLayout->addStretch(1);
    mainLayout->addWidget(m_pFilePathLabel);
    setLayout(mainLayout);
    connect(m_pPauseButton, SIGNAL(clicked()), this, SLOT(onPauseButtonClicked()));
    connect(m_pContinueButton, SIGNAL(clicked()), this, SLOT(onContinueButtonClicked()));
}

SendFileWidget::~SendFileWidget()
{
    qDebug() << "sendfilewidget delete";
    emit destroyThread();
}

void SendFileWidget::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    emit sendFileClose();
    emit destroyThread();
}

void SendFileWidget::updateWidget(int barValue)
{
    qDebug() << "in updateWiget";
    m_pSendProgressBar->setValue(barValue);
}

void SendFileWidget::onPauseButtonClicked()
{
    emit sendPause();
}

void SendFileWidget::onContinueButtonClicked()
{
    emit sendContinue();
}

void SendFileWidget::updateFileNameSlot(QString receiveFileName)
{
    qDebug() << "in udpateFileName";
    m_pFilePathLabel->setText(receiveFileName);
}

void SendFileWidget::finishedMessage()
{
    QMessageBox::information(this,"通知", "传输结束，是否关闭窗口？", QMessageBox::Yes);
    close();
}

void SendFileWidget::peerClose()
{
    QMessageBox::information(this,"注意", "发送端关闭", QMessageBox::Yes);
    close();
}

void SendFileWidget::receiveClosed()
{
    QMessageBox::information(this,"注意", "接收端关闭", QMessageBox::Yes);
    close();
}
