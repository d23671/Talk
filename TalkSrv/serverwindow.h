#ifndef SERVERWINDOW_H
#define SERVERWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>
#include <QStringList>

class QLabel;
class QTextEdit;
class QByteArray;
class QTableWidget;
class AddrSelectionDlg;

class ServerWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit ServerWindow(QWidget *parent = 0);
    ~ServerWindow();
public:

private:
    void loadWidgets();

    // Selection of "listen" address.
    void listen();
    void init();
    void setLayout();
    void setupEventHandlers();

    // Processes incoming datagrams, namely, register/deregister requests.
    void processTheDatagram(QByteArray* datagram, QHostAddress& sender, quint16 senderPort);

    void addTableItem(QString cliStr);
    void addRegistryItem(QString subscribeStr, QString listenStr);
    void removeRegistryItem(QString subscribeStr, QString listenStr);
    void removeTableItem(QString cliStr);

    // Tell all clients about the change to registry pool.
    void publishListenClients();

signals:

private slots:
    // On "readyRead" event of QUdpSocket.
    void readData();

    // On AddrSelectionDlg object saves address.
    void getAddr(QString addr);

private:
    AddrSelectionDlg* mAddrDlg;
    QUdpSocket* mSrvChannel;

    QLabel* mLabelSrvAddr;
    QLabel* mLabelOutput;
    QLabel* mLabelRegistry;
    QTextEdit* mTxtOutput;

    QTableWidget* mTableRegisty;

    QString mOutputMsg;

    QStringList mCliRegistry;
    QStringList mSubscribeRegistry;

    QString mServAddr;
};

#endif // SERVERWINDOW_H
