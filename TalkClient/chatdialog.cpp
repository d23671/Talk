#include "chatdialog.h"
#include "protocolparser.h"
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSizePolicy>
#include <QUdpSocket>
#include <QHostAddress>
#include <QByteArray>
#include <QMessageBox>
#include <QDate>
#include <QTime>
#include <QCloseEvent>

const static QString TALK_WITH = "Conversation with \"%1\" (%2)";
const static QString RECV = "Reveived messages";
const static QString SEND = "Message to be sent";
const static QString OK = "Send";
const static QString CLOSE = "Close";
const static QString FMT_CLI_MSG = "[%1] %2@%3:%4 says:\n %5";
const static QString FMT_CLI_MSG_SELF = "[%1] You say:\n %2";

ChatDialog::ChatDialog(QUdpSocket* channel, QString objName, QWidget *parent) :
    QDialog(parent),
    mLabelRecv(0),
    mLabelSend(0),
    mBtnSend(0),
    mBtnClose(0),
    mTxtRecv(0),
    mTxtSend(0),
    mChannel(channel)
{
    this->setObjectName(objName);

    init();

    int pos = objName.indexOf('-');
    QString peer = objName.left(pos);

    ClientStringParser csp(peer);
    mPeerName = csp.getName();
    ClientStringParser csp1(objName.right(objName.length() - pos -1));
    mName = csp1.getName();

    this->setWindowTitle(TALK_WITH.arg(peer).arg(mName));
}

void ChatDialog::init()
{
    createWidgets();
    setupLayout();
    setupEventHandler();
}

void ChatDialog::createWidgets()
{
    mLabelRecv = new QLabel(RECV, this);
    mLabelSend = new QLabel(SEND, this);
    mBtnSend = new QPushButton(OK, this);
    mBtnSend->setEnabled(false);
    mBtnClose = new QPushButton(CLOSE, this);
    mTxtRecv = new QTextEdit(this);
    mTxtSend = new QTextEdit(this);

    mTxtSend->setFixedHeight(50);
    this->setFixedWidth(500);
}

void ChatDialog::setupLayout()
{
    QWidget* group = new QWidget(this);
    QHBoxLayout* hLayout = new QHBoxLayout(this);
    hLayout->addWidget(mBtnSend);
    hLayout->addWidget(mBtnClose);
    group->setLayout(hLayout);

    QVBoxLayout* vLayout = new QVBoxLayout(this);
    vLayout->addWidget(mLabelRecv);
    vLayout->addWidget(mTxtRecv);
    vLayout->addWidget(mLabelSend);
    vLayout->addWidget(mTxtSend);
    vLayout->addWidget(group);
    this->setLayout(vLayout);
}

void ChatDialog::setupEventHandler()
{
    QObject::connect(mBtnSend, SIGNAL(clicked()), this, SLOT(sendMsg()));
    QObject::connect(mBtnClose, SIGNAL(clicked()), this, SLOT(closeDialog()));
    QObject::connect(mChannel, SIGNAL(readyRead()), this, SLOT(recvMsg()));
    QObject::connect(mTxtSend, SIGNAL(textChanged()), this, SLOT(enableSendButton()));
//    QObject::
}


void ChatDialog::setPeerAddrAndPort(QHostAddress peerAddr, quint16 peerPort)
{
    mPeerAddr = peerAddr;
    mPeerPort = peerPort;
}

QUdpSocket* ChatDialog::getChannel()
{
    return mChannel;
}

void ChatDialog::sendMsg()
{
    // Generates the datagram.
    QByteArray msg =
            ProtocolParser::generateDatagram(CMD_MESSAGE,
                                             mName,
                                             mChannel->localAddress(),
                                             mChannel->localPort(),
                                             mTxtSend->toPlainText());
    // Writes to peer.
    if (mChannel->writeDatagram(msg, mPeerAddr, mPeerPort) == -1)
    {
        mTxtRecv->append(tr("** ERROR! **\nSend msg to %1@%2:%3 failed!")
                              .arg(mPeerName)
                              .arg(mPeerAddr.toString())
                              .arg(mPeerPort));
    }

    // Displays in recv area.
    mTxtRecv->append(FMT_CLI_MSG_SELF
                     .arg(getTimestamp())
                     .arg(mTxtSend->toPlainText()));
    // Clears the send area for next msg to be sent.
    mTxtSend->clear();
}

void ChatDialog::recvMsg()
{
    while (mChannel->hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(mChannel->pendingDatagramSize());
        QHostAddress peerAddr;
        quint16 peerPort;
        mChannel->readDatagram(datagram.data(),
                                        datagram.size(),
                                        &peerAddr,
                                        &peerPort);

        processIncomingMsg(datagram, peerAddr.toString(), peerPort);
    }
}

void ChatDialog::closeEvent(QCloseEvent* e)
{
    mTxtRecv->clear();
    e->accept();
}

void ChatDialog::closeDialog()
{
    mTxtRecv->clear();
    this->close();
}

void ChatDialog::processIncomingMsg(const QByteArray& msg, QString peerAddr, quint16 peerPort)
{
    // Chat dialog is restored so that the recv area should be cleaned.
//    if (this->isHidden())
//    {
//        mTxtRecv->clear();
        this->show();
//    }

    // Displays in recv area.
    ProtocolParser pp(msg);
    mTxtRecv->append(FMT_CLI_MSG
                     .arg(getTimestamp())
                     .arg(mPeerName)
                     .arg(peerAddr)
                     .arg(peerPort)
                     .arg(pp.getMsg()));
}

void ChatDialog::enableSendButton()
{
     mBtnSend->setEnabled(mTxtSend->toPlainText().length() > 0);
}

QString ChatDialog::getTimestamp()
{
    QDate today = QDate::currentDate();
    QTime now = QTime::currentTime();
    QString dateTime = "%1/%2/%3 %4:%5";
    return dateTime.arg(today.year()).arg(today.month()).arg(today.day()).arg(now.hour()).arg(now.minute());
}
