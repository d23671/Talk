#ifndef CLIENTWINDOW_H
#define CLIENTWINDOW_H

#include <QMainWindow>
#include <QByteArray>

class QUdpSocket;
class AddrSelectionDlg;
class RegSettingsDlg;
class QPushButton;
class QTableWidget;
class ChatDialog;

/*
 * Client window. Shows the registry pool on server (self omitted). Manages
 * chatting session dialog.
 */
class ClientWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit ClientWindow(QWidget *parent = 0);
    ~ClientWindow();

private:
    void init();
    void setLayout();
    // Selection of local "listen" address.
    void listen();

    // Setup client's nickname, server's address and port.
    void setup();

    void createWidgets();
    void setupEventHandler();
    void processSubscribeData(const QByteArray& datagram);

    // Processes chat req, chat resp
    void processData(const QByteArray& datagram, QString listenAddr, quint16 listenPort);

    void initTableWidget();
    void clearBuddyTable();
    ChatDialog* createChatDialog(QString objName);

signals:

private slots:
    // On the event of AddrSelectionDlg object saves address.
    void getAddr(QString address);

    // On the event of RegSettingsDlg object saves settings.
    void getSettings(QString name, QString srvAddr, quint16 srvPort);

    // On "Reigster" button clicked.
    void registerClient();

    // On "Deregister" button clicked.
    void deregisterClient();

    // On the event of server publishing registry pool.
    void readSubscribedData();

    // On the event of chat req, chat resp comming from "listen" port.
    void readData();

    // To initiate a chat session.
    void initSession(int row, int column);

private:
    QUdpSocket* mChannelSubscribe;
    QUdpSocket* mChannelListen;

    AddrSelectionDlg* mAddrDlg;
    RegSettingsDlg* mSettingsDlg;
    QString mAddr;

    QString mName;
    QString mSrvAddr;
    quint16 mSrvPort;

    QPushButton* mBtnReg;
    QPushButton* mBtnDereg;
    QTableWidget* mTblBuddies;
};

#endif // CLIENTWINDOW_H
