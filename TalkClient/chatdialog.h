#ifndef CHATDIALOG_H
#define CHATDIALOG_H

#include <QDialog>
#include <QHostAddress>

class QLabel;
class QPushButton;
class QTextEdit;
class QUdpSocket;
class QCloseEvent;

/*
 * Chatting session dialog for sending and receiving chatting messages from peer.
 */
class ChatDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ChatDialog(QUdpSocket* channel, QString objName, QWidget *parent = 0);

    void setPeerAddrAndPort(QHostAddress peerAddr, quint16 peerPort);
    QUdpSocket* getChannel();

private:
    void init();
    void createWidgets();
    void setupLayout();
    void setupEventHandler();
    void processIncomingMsg(const QByteArray& msg, QString peerAddr, quint16 peerPort);
    QString getTimestamp();
    void closeEvent(QCloseEvent* e);

signals:

private slots:
    // On "send" button clicked.
    void sendMsg();

    // On "readyRead" of QUdpSocket event happened.
    void recvMsg();
    void enableSendButton();

    // On "close" button clicked.
    void closeDialog();

private:
    QLabel* mLabelRecv;
    QLabel* mLabelSend;
    QPushButton* mBtnSend;
    QPushButton* mBtnClose;
    QTextEdit* mTxtRecv;
    QTextEdit* mTxtSend;

    QUdpSocket* mChannel;
    QString mName;
    QString mPeerName;
    QHostAddress mPeerAddr;
    quint16 mPeerPort;
};

#endif // CHATDIALOG_H
