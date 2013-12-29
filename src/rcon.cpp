#include "rconwindow.h"
#include "rcon.h"
#include "huffman.h"

/* Skulltag protocol definitions */
const int PROTOCOL_VERSION = 3;
const int PONG_INTERVAL = 5000;
const int TIMEDOUT_INTERVAL = 10000;

/* For debug */
QString straddress;
quint16 strport;

/* Client packet types */
enum
{
    CLRC_BEGINCONNECTION = 52,
    CLRC_PASSWORD,
    CLRC_COMMAND,
    CLRC_PONG,
    CLRC_DISCONNECT
};

/* Server packet types */
enum
{
    SVRC_OLDPROTOCOL = 32, // This set of enumerations starts at 32
    SVRC_BANNED,
    SVRC_SALT,
    SVRC_LOGGEDIN,
    SVRC_INVALIDPASSWORD,
    SVRC_MESSAGE,
    SVRC_UPDATE
};

/* Server update types */
enum
{
    SVRCU_PLAYERDATA = 0,
    SVRCU_ADMINCOUNT,
    SVRCU_MAP
};


/* Rcon class implemenation */
Rcon::Rcon(QObject *parent) :
    QObject(parent), state(Disconnected)
{
    QObject::connect(&socket, SIGNAL(readyRead()), this, SLOT(readPendingPackets()));
    QObject::connect(&keepaliveTimer, SIGNAL(timeout()), this, SLOT(sendKeepAlive()));
}

Rcon::~Rcon()
{
    disconnect();
}

void Rcon::ConnectTimer()
{
    while (state == Connecting)
    {
        qDebug() << straddress << "Timed out";
        abort();
        emit error(TimedOut);
    }
}

bool Rcon::checkCancelEvent()
{
    if ((state == Connecting) || (state == SendingPassword))
    {
        qDebug() << "Canceled" << straddress;
        abort();
        setState(Canceled);
        return true;
    } else
        return false;
}

void Rcon::connectToHost(QHostAddress address, quint16 port, QString password)
{
    this->address = address;
    this->port = port;
    this->password = password;

    straddress = address.toString();
    strport = port;
    qDebug() << "Connecting to" << straddress << "with port" << strport;

    setState(Connecting);
    QTimer::singleShot(TIMEDOUT_INTERVAL, this, SLOT(ConnectTimer()));
    socket.connectToHost(address, port);

    /* Send first packet */
    QByteArray packet;
    packet.append(CLRC_BEGINCONNECTION);
    packet.append(PROTOCOL_VERSION);
    socket.write(huffmanEncode(packet));
}

/* Shortcut to reconnect */
void Rcon::reconnect()
{
    qDebug() << "Reconnecting to" << straddress << "with port" << strport;

    setState(Connecting);
    QTimer::singleShot(TIMEDOUT_INTERVAL, this, SLOT(ConnectTimer()));
    socket.connectToHost(address, port);

    /* Send first packet */
    QByteArray packet;
    packet.append(CLRC_BEGINCONNECTION);
    packet.append(PROTOCOL_VERSION);
    socket.write(huffmanEncode(packet));
    emit message("\n"); // make a space
}

/* Properly disconnect from the server */
void Rcon::disconnect()
{
    if (state == Disconnected)
        return;

    qDebug() << "Disconnecting from" << straddress;
    QByteArray packet;
    packet.append(CLRC_DISCONNECT);
    socket.write(huffmanEncode(packet));

    socket.disconnect();
    abort();
}

/* Disconnect without notifying the server */
void Rcon::abort()
{
    setState(Disconnected);
    socket.abort();
}

/* Sends password packet */
void Rcon::sendPassword(QByteArray salt)
{
    QCryptographicHash md5(QCryptographicHash::Md5);
    md5.addData(salt + password.toLatin1());

    QByteArray packet;
    packet.append(CLRC_PASSWORD);
    packet.append(md5.result().toHex());
    packet.append('\0');

    for(unsigned int i = 0;i < 100;i++)
        socket.write(huffmanEncode(packet));
}

/* Sends a command to the server */
void Rcon::sendCommand(QString command)
{
    QByteArray packet;
    packet.append(CLRC_COMMAND);
    packet.append(command.toLatin1());
    packet.append('\0');
    socket.write(huffmanEncode(packet));
}

/* Sends "pong" packet to notify the server that we are still alive */
void Rcon::sendKeepAlive()
{
    QByteArray packet;
    packet.append(CLRC_PONG);
    socket.write(huffmanEncode(packet));
}

/* Reads RCON string from packet */
QString readString(QBuffer &packet)
{
    char c;
    QString str;
    while (packet.getChar(&c) && c != 0)
        str += c;

    return str;
}

/* Login without receiving SVRC_LOGGEDIN packet */
void Rcon::forceloggedin()
{
    qDebug() << "You're FORCED logged in!";
    setState(LoggedIn);
    keepaliveTimer.start(PONG_INTERVAL); // Start sending keepalive packets.
    emit message("\\cgWARNING: server name and server log has lost! \\n\\c-All information will be shown on next update!");

}

/* Main packet handling function */
void Rcon::processPacket(QBuffer &packet)
{
    char type = 0;
    packet.getChar(&type);

    switch (type)
    {
    case SVRC_OLDPROTOCOL:
        qDebug() << "Version mismatch between server and Rcon.";
        abort();
        emit error(InvalidVersion);
        break;
    case SVRC_BANNED:
        qDebug() << "You're banned on this server!";
        abort();
        emit error(Banned);
        break;
    case SVRC_INVALIDPASSWORD:
        qDebug() << "Invalid password.";
        abort();
        emit error(InvalidPassword);
        break;
    case SVRC_SALT:
    {
        qDebug() << "Server is OK! Sending password...";
        setState(SendingPassword);

        /* Read salt string */
        QString salt = readString(packet);
        sendPassword(salt.toLatin1());
        break;
    }
    case SVRC_LOGGEDIN:
    {
        qDebug() << "You're logged in!";
        setState(LoggedIn);
        keepaliveTimer.start(PONG_INTERVAL); // Start sending keepalive packets.

        /* Read server name */
        emit servernamereceived(readString(packet));

        /* Read all updates */
        char numupdates;
        packet.getChar(&numupdates);
        for (int i = 0; i < numupdates; i++)
            processUpdate(packet);

        /* Read chat log */
        char numlogmessages;
        packet.getChar(&numlogmessages);
        for (int i = 0; i < numlogmessages; i++) emit serverlog(readString(packet));
        break;
    }
    case SVRC_MESSAGE:
        emit message(readString(packet));
        break;
    case SVRC_UPDATE:
        processUpdate(packet);
        break;

    default:
        qDebug() << "Unknown packet type: " << (quint8)type;
    }
}

/* Process update packet */
void Rcon::processUpdate(QBuffer &packet)
{
    char type;
    packet.getChar(&type);

    switch (type)
    {
    case SVRCU_ADMINCOUNT:
    {
        char admincount;
        packet.getChar(&admincount);
        emit adminCountChanged(admincount);
        break;
    }

    case SVRCU_MAP:
    {
        emit mapChanged(readString(packet));
        break;
    }

    case SVRCU_PLAYERDATA:
    {
        char playercount;
        packet.getChar(&playercount);
        emit playerCountChanged(playercount);

        QList<PlayerInfo> players;
        for (int i = 0; i < playercount; i++)
        {
            PlayerInfo info;
            info.name = " " + readString(packet);

            players.append(info);
        }

        emit playerListChanged(players);
        break;
    }
    }
}

/* Reads all pending packets */
void Rcon::readPendingPackets()
{
    while (socket.hasPendingDatagrams())
    {
        QByteArray packet = socket.read(socket.pendingDatagramSize());
        QByteArray decoded = huffmanDecode(packet);
        QBuffer buffer(&decoded);
        buffer.open(QIODevice::ReadWrite);
        processPacket(buffer);
    }
}
