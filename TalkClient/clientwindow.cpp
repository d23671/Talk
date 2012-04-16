#include "clientwindow.h"
#include "addrselectiondlg.h"
#include "regsettingsdlg.h"
#include "protocolparser.h"
#include "chatdialog.h"
#include <QUdpSocket>
#include <QHostAddress>
#include <QMessageBox>
#include <QByteArray>
#include <QApplication>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QHeaderView>

const static QString BUDDIES = "Buddy List";

ClientWindow::ClientWindow(QWidget *parent) :
        QMainWindow(parent),
        mChannelSubscribe(0),
        mChannelListen(0),
        mAddrDlg(0),
        mSettingsDlg(0),
        mBtnReg(0),
        mBtnDereg(0),
        mTblBuddies(0)
{
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    listen();

    setup();

    this->setWindowTitle(mName);

    init();
    createWidgets();
    setupEventHandler();
    setLayout();
}

void ClientWindow::listen()
{
    mAddrDlg = new AddrSelectionDlg();
    QObject::connect(mAddrDlg, SIGNAL(addrSaved(QString)),
                     this, SLOT(getAddr(QString)));
    mAddrDlg->exec();
    delete mAddrDlg;
    if (mAddr.isEmpty())
        QApplication::exit(-1);
}

void ClientWindow::setup()
{
    mSettingsDlg = new RegSettingsDlg();
    QObject::connect(mSettingsDlg, SIGNAL(settingsSaved(QString, QString, quint16)),
                     this, SLOT(getSettings(QString, QString, quint16)));
    mSettingsDlg->exec();
    delete mSettingsDlg;
    if (mName.isEmpty() || mSrvAddr.isEmpty() || mSrvPort == 0xFFFF)
        QApplication::exit(-1);
}

ClientWindow::~ClientWindow()
{
    if (mBtnDereg->isEnabled())
        deregisterClient();

    if (mChannelSubscribe != NULL && mChannelSubscribe->isOpen())
        mChannelSubscribe->close();

    if (mChannelListen != NULL && mChannelListen->isOpen())
        mChannelListen->close();
}

void ClientWindow::init()
{
    // "Listen" on subscribe channel which subscribing server's registry pool
    // infomation.
    mChannelSubscribe = new QUdpSocket(this);
    mChannelSubscribe->open(QIODevice::ReadWrite);
    if (!mChannelSubscribe->bind(QHostAddress(mAddr), 0, QUdpSocket::ShareAddress))
    {
        QMessageBox::critical(this, tr("Error"), mChannelSubscribe->errorString());
        QApplication::exit(-1);
    }

    // "Listen" on listen channel which waiting for incoming protocol msg.
    mChannelListen = new QUdpSocket(this);
    mChannelListen->open(QIODevice::ReadWrite);
    if (!mChannelListen->bind(QHostAddress(mAddr), 0, QUdpSocket::ShareAddress))
    {
        QMessageBox::critical(this, tr("Error"), mChannelListen->errorString());
        QApplication::exit(-1);
    }
}

void ClientWindow::setLayout()
{
    QWidget* group = new QWidget(this);
    QHBoxLayout* hLayout = new QHBoxLayout(this);
    hLayout->addWidget(mBtnReg);
    hLayout->addWidget(mBtnDereg);
    group->setLayout(hLayout);

    QVBoxLayout* vLayout = new QVBoxLayout(this);
    vLayout->addWidget(group);
    vLayout->addWidget(mTblBuddies);
    centralWidget()->setLayout(vLayout);
}

void ClientWindow::createWidgets()
{
    mBtnReg = new QPushButton(REG_STR, this);
    mBtnDereg = new QPushButton(DEREG_STR, this);
    mBtnDereg->setEnabled(false);

    initTableWidget();
}

void ClientWindow::initTableWidget()
{
    mTblBuddies = new QTableWidget(0, 1, this);
    QTableWidgetItem* item = new QTableWidgetItem(BUDDIES);
    mTblBuddies->setHorizontalHeaderItem(0, item);
    mTblBuddies->setShowGrid(true);
    mTblBuddies->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
}

void ClientWindow::setupEventHandler()
{
    QObject::connect(mBtnReg, SIGNAL(clicked()),
                     this, SLOT(registerClient()));
    QObject::connect(mBtnDereg, SIGNAL(clicked()),
                     this, SLOT(deregisterClient()));
    QObject::connect(mChannelSubscribe, SIGNAL(readyRead()),
                     this, SLOT(readSubscribedData()));
    QObject::connect(mChannelListen, SIGNAL(readyRead()),
                     this, SLOT(readData()));
    QObject::connect(mTblBuddies, SIGNAL(cellDoubleClicked(int,int)),
                     this, SLOT(initSession(int, int)));
}

void ClientWindow::registerClient()
{
    // Datagram for registration.
    QByteArray datagram =
            ProtocolParser::generateDatagram(CMD_REGISTER,
                                             mName,
                                             mChannelListen->localAddress(),
                                             mChannelListen->localPort());

    // Register to server.
    if (mChannelSubscribe->writeDatagram(datagram, datagram.length(), QHostAddress(mSrvAddr), mSrvPort) == -1)
    {
        QMessageBox::critical(this, tr("Error"), tr("Register on server %1:%2 failed")
                              .arg(mSrvAddr)
                              .arg(mSrvPort));
        QApplication::exit(-1);
    }
    mBtnDereg->setEnabled(true);
    mBtnReg->setEnabled(false);
}

void ClientWindow::deregisterClient()
{
    // Datagram for deregistration.
    QByteArray datagram =
            ProtocolParser::generateDatagram(CMD_DEREGISTER,
                                             mName,
                                             mChannelListen->localAddress(),
                                             mChannelListen->localPort());

    // Deregister from server.
    if (mChannelSubscribe->writeDatagram(datagram, datagram.length(), QHostAddress(mSrvAddr), mSrvPort) == -1)
    {
        QMessageBox::critical(this, tr("Error"), tr("Deregister on server %1:%2 failed")
                              .arg(mSrvAddr)
                              .arg(mSrvPort));
        QApplication::exit(-1);
    }
    clearBuddyTable();
    mBtnDereg->setEnabled(false);
    mBtnReg->setEnabled(true);
}

void ClientWindow::getAddr(QString address)
{
    mAddr = address;
}

void ClientWindow::getSettings(QString name, QString srvAddr, quint16 srvPort)
{
    mName = name;
    mSrvAddr = srvAddr;
    mSrvPort = srvPort;
}

void ClientWindow::readSubscribedData()
{
    // Refresh registered clients list.
    clearBuddyTable();
    while (mChannelSubscribe->hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(mChannelSubscribe->pendingDatagramSize());
        mChannelSubscribe->readDatagram(datagram.data(),
                                        datagram.size());

        processSubscribeData(datagram);
    }
}

void ClientWindow::processSubscribeData(const QByteArray& datagram)
{
    QString str = QString::fromAscii(datagram.constData());
    if (tr("%1:%2:%3").arg(mName)
        .arg(mChannelListen->localAddress().toString())
        .arg(mChannelListen->localPort())
        .compare(str) != 0)
        {
        QTableWidgetItem* item = new QTableWidgetItem(str);
        int curRow = mTblBuddies->rowCount();
        mTblBuddies->insertRow(curRow);
        mTblBuddies->setItem(curRow, 0, item);
    }
}

void ClientWindow::clearBuddyTable()
{
    for (int i = 1; i < mTblBuddies->rowCount(); ++i)
    {
        delete mTblBuddies->takeItem(i, 0);
        mTblBuddies->removeRow(i);
    }
    mTblBuddies->clearContents();
    mTblBuddies->setRowCount(0);
}

void ClientWindow::readData()
{
    // Data on "listen" channel.
    while (mChannelListen->hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(mChannelListen->pendingDatagramSize());
        QHostAddress peerAddr;
        quint16 peerPort;
        mChannelListen->readDatagram(datagram.data(),
                                     datagram.size(),
                                     &peerAddr,
                                     &peerPort);

        processData(datagram, peerAddr.toString(), peerPort);
    }
}

void ClientWindow::processData(const QByteArray& datagram, QString listenAddr, quint16 listenPort)
{
    ProtocolParser pp(datagram);

    // To identify a chatting dialog in ClientWindow's object tree.
    QString objName= FMT_OBJ_NAME
                     // PeerName:PeerAddr:PeerListenPort
                     .arg(pp.getName()).arg(listenAddr).arg(listenPort)
                     // SelfName:SelfAddr:SelfListenPort
                     .arg(mName).arg(mAddr).arg(mChannelListen->localPort());

    ChatDialog* chatDlg = this->findChild<ChatDialog*>(objName);
    QByteArray peerMsg;

    // Command pattern should be better.
    switch (pp.getCmd())
    {
    case CMD_PEER_RESP:
        if (chatDlg == 0)
        {
            // No chatting dialog is opened when response received.
            // Indicates an stray CMD_PEER_RESP received.
            QMessageBox::critical(this, tr("Error"), tr("Stray datagram:\n%1").arg(pp.toString()));
            if (mBtnDereg->isEnabled())
                deregisterClient();
            QApplication::exit(-1);
        }
        // Session established.
        chatDlg->setPeerAddrAndPort(QHostAddress(pp.getAddr()), pp.getPort());
        chatDlg->show();
        break;

    case CMD_PEER_REQ:
        if (chatDlg == 0)
            chatDlg = createChatDialog(objName);
        peerMsg =
                ProtocolParser::generateDatagram(CMD_PEER_RESP,
                                                 mName,
                                                 QHostAddress(mAddr),
                                                 chatDlg->getChannel()->localPort());

        // Response the chatting req.
        if (mChannelListen->writeDatagram(peerMsg, QHostAddress(listenAddr), listenPort) == -1)
        {
            QMessageBox::critical(this, tr("Error"), tr("Send response to %1@%2:%3 failed!")
                                  .arg(pp.getName())
                                  .arg(listenAddr)
                                  .arg(listenPort));
            if (mBtnDereg->isEnabled())
                deregisterClient();
            QApplication::exit(-1);
        }
        chatDlg->setPeerAddrAndPort(QHostAddress(pp.getAddr()), pp.getPort());
        chatDlg->show();
        break;

    default:
        QMessageBox::critical(this, tr("Error"), tr("Protocol error:\n %1").arg(pp.toString()));
        if (mBtnDereg->isEnabled())
            deregisterClient();
        QApplication::exit(-1);
        break;
    }
}

ChatDialog* ClientWindow::createChatDialog(QString objName)
{
    QUdpSocket* channel = new QUdpSocket(this);
    channel->open(QIODevice::ReadWrite);
    if (!channel->bind(QHostAddress(mAddr), 0, QUdpSocket::ShareAddress))
    {
        QMessageBox::critical(this, tr("Error"), channel->errorString());
        if (mBtnDereg->isEnabled())
            deregisterClient();
        QApplication::exit(-1);
    }

    ChatDialog* dlg = new ChatDialog(channel, objName, this);
    return dlg;
}

void ClientWindow::initSession(int row, int column)
{
    QTableWidgetItem* item = mTblBuddies->item(row, column);
    if (item != NULL)
    {
        ClientStringParser csp(item->text());

        // To identify a chatting dialog in ClientWindow's object tree.
        QString objName(FMT_OBJ_NAME
                        // PeerName:PeerAddr:PeerListenPort
                        .arg(csp.getName()).arg(csp.getAddr()).arg(csp.getPort())
                        // SelfName:SelfAddr:SelfListenPort
                        .arg(mName).arg(mAddr).arg(mChannelListen->localPort()));

        ChatDialog* chatDlg = this->findChild<ChatDialog*>(objName);
        if (chatDlg == NULL)
            chatDlg = createChatDialog(objName);

        // Send chatting request to peer.
        QByteArray datagram =
                ProtocolParser::generateDatagram(CMD_PEER_REQ, mName, QHostAddress(mAddr), chatDlg->getChannel()->localPort());
        if (mChannelListen->writeDatagram(datagram, QHostAddress(csp.getAddr()), csp.getPort()) == -1)
        {
            QMessageBox::critical(this, tr("Error"), tr("Send request to %1@%2:%3 failed!")
                                  .arg(csp.getName())
                                  .arg(csp.getAddr())
                                  .arg(csp.getPort()));
            if (mBtnDereg->isEnabled())
                deregisterClient();
            QApplication::exit(-1);
        }
    }
}
