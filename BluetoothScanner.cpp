#include "BluetoothScanner.h"

BluetoothScanner::BluetoothScanner(QObject *parent)
    : QObject{parent}
{
    mSocket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol, this);
    localDevice = new QBluetoothLocalDevice();
}

void BluetoothScanner::enableLocalDeviceBluetooth()
{
    //QBluetoothLocalDevice localDevice;
    if (localDevice->isValid()) {
        if (localDevice->hostMode() == QBluetoothLocalDevice::HostPoweredOff) {
            // Enable Bluetooth
            localDevice->powerOn();
            qDebug() << "Bluetooth turned on";

            // Wait for Bluetooth to be enabled
            QEventLoop loop;
            QObject::connect(localDevice, SIGNAL(hostModeStateChanged(QBluetoothLocalDevice::HostMode)),
                             &loop, SLOT(quit()));
            loop.exec();
        } else {
            qDebug() << "Bluetooth already enabled";
        }
    } else {
        qDebug() << "Bluetooth not available on this device";
    }
}

void BluetoothScanner::disableLocalDeviceBluetooth()
{
    if(mSocket != nullptr) {
        disconnecToDevice();
    }
    localDevice->setHostMode(QBluetoothLocalDevice::HostPoweredOff);
    qDebug() << "Call disable";
}

void BluetoothScanner::startDeviceDiscovery()
{
    QBluetoothLocalDevice localDevice;
    if (localDevice.hostMode() == QBluetoothLocalDevice::HostPoweredOff) {
        return;
    }
    QBluetoothDeviceDiscoveryAgent *discoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);
    //discoveryAgent->setInquiryType(QBluetoothDeviceDiscoveryAgent::GeneralUnlimitedInquiry);
    connect(discoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered,
            this, &BluetoothScanner::deviceDiscovered);
    // Start a discovery
    discoveryAgent->start();
}

QString BluetoothScanner::getBluetoothDeviceName(int i)
{
    return availableDevices[i].name();
}

int BluetoothScanner::getListLength() {
    return availableDevices.length();
}

void BluetoothScanner::clearAllDevice()
{
    availableDevices.clear();
}

void BluetoothScanner::deviceDiscovered(const QBluetoothDeviceInfo &device)
{
    if (device.name().contains("Bluetooth ") || device.name() == "") {
        return;
    }
    for(int i = 0; i < availableDevices.count(); i++) {
        if(device.name() == availableDevices.at(i).name()) {
            return;
        }
    }

    availableDevices.append(device);
}

void BluetoothScanner::connectToDevice(QString deviceName)
{
    if(mSocket) {
        disconnecToDevice();
    }
    mSocket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol, this);
    QBluetoothUuid serviceUuid = QBluetoothUuid::ServiceClassUuid::AdvancedAudioDistribution;
    connect(mSocket, &QBluetoothSocket::connected, this, &BluetoothScanner::socketConnected);
    connect(mSocket, &QBluetoothSocket::disconnected, this, &BluetoothScanner::socketDisconnected);
    for(int i = 0; i < availableDevices.count(); i++) {
        if(deviceName == availableDevices.at(i).name()) {
            currentDevice = availableDevices.at(i);
            if(!localDevice->pairingStatus(availableDevices.at(i).address())) {
                localDevice->requestPairing(availableDevices.at(i).address(), QBluetoothLocalDevice::Paired);
            }
            mSocket->connectToService(availableDevices.at(i).address(), serviceUuid,
                                      QIODevice::WriteOnly);
            break;
        }
    }
}

void BluetoothScanner::disconnecToDevice()
{
    connect(mSocket, &QBluetoothSocket::disconnected, this, &BluetoothScanner::socketDisconnected);
    mSocket->disconnectFromService();
    delete mSocket;
    mSocket = nullptr;
    localDevice->requestPairing(currentDevice.address(), QBluetoothLocalDevice::Unpaired);
}

void BluetoothScanner::socketConnected()
{
    qDebug() << "Bluetooth socket connected";
}

void BluetoothScanner::socketDisconnected()
{
    qDebug() << "Bluetooth socket disconnected";
}




