#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QString>
#include <QByteArray>
#include <QHostAddress>
#include <QList>
#include <QStringList>

const QString REG_STR = "Register";
const QString DEREG_STR = "Deregister";

/*
 * +------------------------------------------------+
 * | Semi-colon is not supported in client's name.  |
 * +------------------------------------------------+
 * Protocol: semi-colon seperated fields.
 *   Cmd:ClientName:HostAddress:Port[:Msg]
 * Cmd:
 *   Register: 0xAA
 *   Deregister: 0x55
 *   Msg: 0x88
 *   chat req: 0x66
 *   chat resp: 0x99
 *   error: 0xFF
 */
const int CMD_REGISTER = 0xAA;
const int CMD_DEREGISTER = 0x55;
const int CMD_MESSAGE = 0x88;
const int CMD_PEER_REQ = 0x66;
const int CMD_PEER_RESP = 0x99;
const int CMD_ERROR = 0xFF;
const int POS_CMD = 0;
const int POS_NAME = 1;
const int POS_ADDR = 2;
const int POS_PORT = 3;
const int POS_MSG = 4;
const char DELIMITER = ':';

/*
 * Client String (registry item):
 *   UniqueName:SubscribeAddr:SubscribePort
 */
const int CLI_POS_NAME = 0;
const int CLI_POS_ADDR = 1;
const int CLI_POS_PORT = 2;

/*
 * Chat session object name:
 *   PeerName:PeerAddr:PeerPort-SelfName:SelfAddr:SelfPort
 */
const QString FMT_OBJ_NAME = "%1:%2:%3-%4:%5:%6";

class ProtocolParser
{
public:
    explicit ProtocolParser(const QByteArray& datagram);
    ~ProtocolParser();

public:
    QString toString();
    QString getCliString();
    int getCmd();
    QString getName();
    QString getSenderName();
    QString getAddr();
    quint16 getPort();
    QString getMsg();
    static QString generateCliString(QString name, QHostAddress addr, quint16 port);
    static QByteArray generateDatagram(int cmd, QString name, QHostAddress addr, quint16 port);
    static QByteArray generateDatagram(int cmd, QString name, QHostAddress addr, quint16 port, QString msg);

private:
    QByteArray mData;
    QList<QByteArray> mList;
};

class ClientStringParser
{
public:
    explicit ClientStringParser(const QString& cliStr);

    QString getName();
    QString getAddr();
    quint16 getPort();
private:
    QStringList mList;
};

#endif
