#ifndef SENDFILEWIDGET_H
#define SENDFILEWIDGET_H

#include <QWidget>
#include <QDebug>
#include <QProgressBar>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFile>
#include <QHostAddress>

#ifdef Q_CC_MSVC
#pragma execution_character_set("utf-8")
#endif

class SendFileWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SendFileWidget(QString filename, QString hostName, QHostAddress hostAddress, QWidget *parent = nullptr);
    ~SendFileWidget();

    void closeEvent(QCloseEvent *event);
signals:
    void sendFileClose();
    void destroyThread();
    void sendPause();
    void sendContinue();

private slots:
    void updateWidget(int barValue);
    void onPauseButtonClicked();
    void onContinueButtonClicked();
    void updateFileNameSlot(QString receiveFileName);
    void finishedMessage();
    void peerClose();
    void receiveClosed();

private:
    QString m_fileName;
    QString m_hostName;
    QHostAddress m_hostAddress;
    QProgressBar* m_pSendProgressBar;
    QPushButton* m_pPauseButton;
    QPushButton* m_pContinueButton;
    QLabel* m_pProgressLabel;
    QLabel* m_pFilePathLabel;

    QFile* m_sendFile;
};

#endif // SENDFILEWIDGET_H
