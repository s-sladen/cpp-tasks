#include "Chat.h"

#include <stdexcept>

enum class MessageType : char {
    CONNECT,
    MESSAGE,
    I_AM_HERE,
    DISCONNECT
};

Chat::Chat() {}

Chat::~Chat() {
    if (socket) {
        QByteArray message;
        message.append(static_cast<char>(MessageType::DISCONNECT));
        socket->writeDatagram(message, QHostAddress::Broadcast, PORT);
    }
}

QString Chat::getNickname() {
    return nickname;
}

void Chat::login(QString nickname) {
    this->nickname = nickname;
    socket.reset(new QUdpSocket(this));
    if (!socket->bind(PORT, QUdpSocket::ReuseAddressHint)) {
        throw std::runtime_error("cannot bind port");
    }
    connect(socket.data(), SIGNAL(readyRead()), this, SLOT(readMessages()));

    QByteArray message;
    message.append(static_cast<char>(MessageType::CONNECT));
    message.append(nickname);
    socket->writeDatagram(message, QHostAddress::Broadcast, PORT);
}

void Chat::readMessages() {
    while (socket->hasPendingDatagrams()) {
        QByteArray buffer(socket->pendingDatagramSize(), 0);
        QHostAddress fromAddress;
        quint16 fromPort;
        socket->readDatagram(buffer.data(), buffer.size(), &fromAddress, &fromPort);
        QString from = fromAddress.toString() + ":" + fromPort;
        MessageType type = static_cast<MessageType>(buffer.at(0));
        QString data = QString::fromUtf8(buffer.data() + 1, buffer.size() - 1);
        switch (type) {
            case MessageType::CONNECT:
                emit loggedIn(data);
                buffer.clear();
                buffer.append(static_cast<char>(MessageType::I_AM_HERE));
                buffer.append(nickname);
                socket->writeDatagram(buffer, QHostAddress::Broadcast, PORT);
                break;
            case MessageType::MESSAGE:
                emit messageCame(users[from], data);
                break;
            case MessageType::DISCONNECT:
                emit loggedOut(users[from]);
                users.remove(from);
                emit refreshUsers();
                break;
            case MessageType::I_AM_HERE:
                users[from] = data;
                emit refreshUsers();
                break;
        }
    }
}

void Chat::sendMessage(QString message) {
    QByteArray buffer;
    buffer.append(static_cast<char>(MessageType::MESSAGE));
    buffer.append(message);
    socket->writeDatagram(buffer, QHostAddress::Broadcast, PORT);
}

QStringList Chat::getUsers() {
    return users.values();
}
