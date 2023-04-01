#ifndef BluetoothScanner_H
#define BluetoothScanner_H

#include <QObject>
#include <QDebug>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothDeviceInfo>
#include <QBluetoothServer>
#include <QBluetoothServiceInfo>
#include <QBluetoothLocalDevice>
#include <QEventLoop>
#include <QList>
#include <QByteArray>
#include <QVector>

class BluetoothScanner : public QObject
{
    Q_OBJECT
public:
    explicit BluetoothScanner(QObject *parent = nullptr);
signals:
public slots:
    void enableLocalDeviceBluetooth();
    void disableLocalDeviceBluetooth();
    void startDeviceDiscovery();
    QString getBluetoothDeviceName(int i);
    int getListLength();
    void clearAllDevice();
    void connectToDevice(QString deviceName);
    void disconnecToDevice();
private slots:
    void deviceDiscovered(const QBluetoothDeviceInfo &device);
//    void socketConnected();
//    void socketDisconnected();
    //void readData();
private:
    QList<QBluetoothDeviceInfo> availableDevices;
    QBluetoothSocket *mSocket;
    QBluetoothLocalDevice* localDevice;
    QBluetoothDeviceInfo currentDevice;
};

#endif // BluetoothScanner_H
