#include "serverwindow.h"
#include "addrselectiondlg.h"
#include "protocolparser.h"
#include <QLabel>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QByteArray>
#include <QMessageBox>
#include <QNetworkInterface>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QList>
#include <QHeaderView>
#include <QApplication>

ServerWindow::ServerWindow(QWidget *parent) :
        QMainWindow(parent), mAddrDlg(0), mSrvChannel(0),
        mLabelSrvAddr(0), mLabelOutput(0),
        mLabelRegistry(0), mTxtOutput(0), mTableRegisty(0)
{
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    listen();

    loadWidgets();
    init();
    setupEventHandlers();
    setLayout();
}

ServerWindow::~ServerWindow()
{
    if (mSrvChannel != NULL && mSrvChannel->isOpen())
        mSrvChannel->close();
}

void ServerWindow::listen()
{
    mAddrDlg = new AddrSelectionDlg(this);
    QObject::connect(mAddrDlg, SIGNAL(addrSaved(QString)),
                     this, SLOT(getAddr(QString)));
    mAddrDlg->exec();

    if (mServAddr.isEmpty())
        QApplication::exit(-1);
}

void ServerWindow::loadWidgets()
{
    mLabelSrvAddr = new QLabel(this);
    mLabelRegistry = new QLabel(tr("Registered clients:"), this);
    mLabelOutput = new QLabel(tr("Incoming requests:"), this);

    mTableRegisty = new QTableWidget(0, 1, this);
    QTableWidgetItem* item = new QTableWidgetItem(tr("Name @ Addr:Port"));
    mTableRegisty->setHorizontalHeaderItem(0, item);
    mTableRegisty->setShowGrid(true);
    mTableRegisty->horizontalHeader()->setResizeMode(QHeaderView::Stretch);

    mTxtOutput = new QTextEdit(this);
}

void ServerWindow::init()
{
    // "Listen" on server port.
    mSrvChannel = new QUdpSocket(this);
    QHostAddress srvAddr(mServAddr);
    mSrvChannel->open(QIODevice::ReadWrite);
    if (!mSrvChannel->bind(srvAddr, 0, QUdpSocket::ShareAddress))
    {
        QMessageBox::critical(this, tr("Error"), mSrvChannel->errorString());
        QApplication::exit(-1);
    }
    mLabelSrvAddr->setText(tr("Server address: %1 : %2")
                          .arg(mSrvChannel->localAddress().toString())
                          .arg(mSrvChannel->localPort()));
}

void ServerWindow::setLayout()
{
    QVBoxLayout* vLayout = new QVBoxLayout(this);
    vLayout->addWidget(mLabelSrvAddr);
    vLayout->addWidget(mLabelRegistry);
    vLayout->addWidget(mTableRegisty);
    vLayout->addWidget(mLabelOutput);
    vLayout->addWidget(mTxtOutput);
    centralWidget()->setLayout(vLayout);
}

void ServerWindow::setupEventHandlers()
{
    QObject::connect(mSrvChannel, SIGNAL(readyRead()), this, SLOT(readData()));
}

void ServerWindow::readData()
{
    // Data on "listen" channel.
    while (mSrvChannel->hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(mSrvChannel->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;
        mSrvChannel->readDatagram(datagram.data(),
                                  datagram.size(),
                                  &sender,
                                  &senderPort);

        processTheDatagram(&datagram, sender, senderPort);
    }
}

void ServerWindow::processTheDatagram(QByteArray* datagram, QHostAddress& sender, quint16 senderPort)
{
    ProtocolParser pp(*datagram);

    // 'switch' statement is not good choice. Command pattern is better.
    switch (pp.getCmd())
    {
    case CMD_REGISTER:
        // Registration of client.
        addRegistryItem(ProtocolParser::generateCliString(pp.getName(), sender, senderPort),
                        pp.getCliString());
        addTableItem(pp.getCliString());
        mTxtOutput->append(tr("%1: %2 @ %3:%4")
                         .arg(REG_STR)
                         .arg(pp.getName())
                         .arg(sender.toString())
                         .arg(senderPort));
        break;

    case CMD_DEREGISTER:
        // Deregistration of client.
        removeRegistryItem(ProtocolParser::generateCliString(pp.getName(), sender, senderPort),
                           pp.getCliString());
        removeTableItem(pp.getCliString());
        mTxtOutput->append(tr("%1: %2 @ %3:%4")
                         .arg(DEREG_STR)
                         .arg(pp.getName())
                         .arg(sender.toString())
                         .arg(senderPort));
        break;

    case CMD_ERROR:

    default:
        mOutputMsg.append(tr("Protocol error: %1 @ %2:%3\n")
                         .arg(pp.getName())
                         .arg(sender.toString())
                         .arg(senderPort));
        mTxtOutput->setText(mOutputMsg);
        break;
    }
}

void ServerWindow::addTableItem(QString cliStr)
{
    QTableWidgetItem* item = new QTableWidgetItem(cliStr);
    int curRow = mTableRegisty->rowCount();
    mTableRegisty->insertRow(curRow);
    mTableRegisty->setItem(curRow, 0, item);
}

void ServerWindow::addRegistryItem(QString subscribeStr, QString listenStr)
{
    mSubscribeRegistry << subscribeStr;
    mCliRegistry << listenStr;
    publishListenClients();
}

void ServerWindow::removeRegistryItem(QString subscribeStr, QString listenStr)
{
    mSubscribeRegistry.removeAll(subscribeStr);
    mCliRegistry.removeAll(listenStr);
    publishListenClients();
}

void ServerWindow::removeTableItem(QString cliStr)
{
    // 'cliStr' is unique
    int i = 0;
    for (; i < mTableRegisty->rowCount(); ++i)
    {
        if (mTableRegisty->item(i, 0)->text().compare(cliStr) == 0)
            break;
    }
    mTableRegisty->removeRow(i);
}

void ServerWindow::getAddr(QString addr)
{
    mServAddr = addr;
}

void ServerWindow::publishListenClients()
{
    // Through subscribe channel of each client,
    foreach (QString subscriber, mSubscribeRegistry)
    {
        ClientStringParser csp(subscriber);
        QString addr = csp.getAddr();
        quint16 port = csp.getPort();

        if (port != 0xFFFF)
        {
            // tell them the change to registry pool.
            foreach (QString listener, mCliRegistry)
                mSrvChannel->writeDatagram(listener.toAscii(), QHostAddress(addr), port);
        }
    }
}
