#ifndef RFCOMMSERVER_H
#define RFCOMMSERVER_H
#include <QBluetoothServer>
#include <QDebug>
#include <QBluetoothServiceInfo>

class RfcommServer
{
public:
    RfcommServer();
    void createSever();
};

#endif // RFCOMMSERVER_H
