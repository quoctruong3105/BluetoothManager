#ifndef CHATSEVER_H
#define CHATSEVER_H

#include <QObject>
#include <QBluetoothServer>
#include <QBluetoothLocalDevice>

class ChatSever : public QObject
{
    Q_OBJECT
public:
    explicit ChatSever(QObject *parent = nullptr);

signals:
private slots:
    void clientConnected();
public slots:
    void resetServer();
private:
    static QBluetoothServer *m_rfcommServer;
    QList<QBluetoothSocket*> m_clientSockets;
};

#endif // CHATSEVER_H
