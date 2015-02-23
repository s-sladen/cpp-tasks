#pragma once

#include <QObject>
#include <QString>
#include <QUdpSocket>
#include <QScopedPointer>
#include <QMap>

class Chat : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString nickname READ getNickname)
    Q_PROPERTY(QStringList users READ getUsers NOTIFY refreshUsers)
public:
    Chat();
    ~Chat();

    QString getNickname();
    QStringList getUsers();

signals:
    void loggedIn(QString nickname);
    void messageCame(QString author, QString message);
    void loggedOut(QString nickname);
    void refreshUsers();

public slots:
    void login(QString nickname);
    void sendMessage(QString message);

private slots:
    void readMessages();

private:
    QString nickname;
    QScopedPointer<QUdpSocket> socket;
    QMap<QString, QString> users;

    static constexpr quint16 PORT = 1546;
};