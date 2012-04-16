#include "protocolparser.h"

ProtocolParser::ProtocolParser(const QByteArray& datagram):
        mData(datagram)
{
    mList = mData.split(DELIMITER);
}

ProtocolParser::~ProtocolParser()
{
}


QString ProtocolParser::toString()
{
    return QString::fromAscii(mData.constData());
}

QString ProtocolParser::getCliString()
{
    return QString("%1:%2:%3").arg(getName()).arg(getAddr()).arg(getPort());
}

int ProtocolParser::getCmd()
{
    bool ok = false;
    int cmd = mList.at(POS_CMD).toInt(&ok);
    if (!ok)
        cmd = CMD_ERROR;
    return cmd;
}

QString ProtocolParser::getName()
{
    return QString::fromAscii(mList.at(POS_NAME).constData());
}

QString ProtocolParser::getAddr()
{
    return QString::fromAscii(mList.at(POS_ADDR).constData());
}

quint16 ProtocolParser::getPort()
{
    bool ok = false;
    quint16 port = mList.at(POS_PORT).toUShort(&ok);
    if (!ok)
        port = 0xFFFF;
    return port;
}

QString ProtocolParser::getMsg()
{
    // Semi-colon is allowed in message, so QString::split() should not be used.
    QString str = toString();
    int pos = 0;
    int msgPos = POS_MSG;
    while (msgPos--)
    {
        pos = str.indexOf(DELIMITER);
        if (pos >=0)
        {
            ++pos;
            str = str.right(str.length() - pos);
        }
    }
    return str;
}

QString ProtocolParser::generateCliString(QString name, QHostAddress addr, quint16 port)
{
    return QString("%1:%2:%3")
            .arg(name)
            .arg(addr.toString())
            .arg(port);
}

QByteArray ProtocolParser::generateDatagram(int cmd, QString name, QHostAddress addr, quint16 port)
{
    return QString("%1:%2:%3:%4")
            .arg(cmd)
            .arg(name)
            .arg(addr.toString())
            .arg(port)
            .toAscii();
}

QByteArray ProtocolParser::generateDatagram(int cmd, QString name, QHostAddress addr, quint16 port, QString msg)
{
    return QString("%1:%2:%3:%4:%5")
            .arg(cmd)
            .arg(name)
            .arg(addr.toString())
            .arg(port)
            .arg(msg)
            .toAscii();
}


ClientStringParser::ClientStringParser(const QString& cliStr)
{
    mList = cliStr.split(DELIMITER);
}

QString ClientStringParser::getName()
{
    if (mList.count() == 3)
        return mList.at(CLI_POS_NAME);
    else
        return QString();
}

QString ClientStringParser::getAddr()
{
    if (mList.count() == 3)
        return mList.at(CLI_POS_ADDR);
    else
        return QString();
}

quint16 ClientStringParser::getPort()
{
    if (mList.count() == 3)
    {
        bool ok = false;
        quint16 port = mList.at(CLI_POS_PORT).toUShort(&ok);
        if (ok)
            return port;
    }
    return 0xFFFF;
}

