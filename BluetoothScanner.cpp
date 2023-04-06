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
            //qDebug() << "Bluetooth turned on";

            // Wait for Bluetooth to be enabled
            QEventLoop loop;
            QObject::connect(localDevice, SIGNAL(hostModeStateChanged(QBluetoothLocalDevice::HostMode)),
                             &loop, SLOT(quit()));
            loop.exec();
        }
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
    qDebug() << device.serviceUuids();
    availableDevices.append(device);
}

void BluetoothScanner::connectToDevice(QString deviceName)
{
    if(mSocket) {
        disconnecToDevice();
    }
    mSocket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol, this);
    QBluetoothUuid serviceUuid = QBluetoothUuid::ServiceClassUuid::GenericAudio;
//    connect(mSocket, &QBluetoothSocket::connected, this, &BluetoothScanner::socketConnected);
//    connect(mSocket, &QBluetoothSocket::disconnected, this, &BluetoothScanner::socketDisconnected);
    for(int i = 0; i < availableDevices.count(); i++) {
        if(deviceName == availableDevices.at(i).name()) {
            currentDevice = availableDevices.at(i);
            if(localDevice->pairingStatus(availableDevices.at(i).address()) != QBluetoothLocalDevice::Paired) {
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
    //connect(mSocket, &QBluetoothSocket::disconnected, this, &BluetoothScanner::socketDisconnected);
    mSocket->disconnectFromService();
    delete mSocket;
    mSocket = nullptr;
    localDevice->requestPairing(currentDevice.address(), QBluetoothLocalDevice::Unpaired);
}

QStringList BluetoothScanner::getProfile(QString deviceName)
{
    QStringList profiles;

    QJniEnvironment env;
    if (env.checkAndClearExceptions()) {
        // Handle JNI exception
        env.checkAndClearExceptions();
        return profiles;
    }

    // Get the BluetoothAdapter object
    QJniObject bluetoothAdapter = QJniObject::callStaticObjectMethod("android/bluetooth/BluetoothAdapter",
                                                                     "getDefaultAdapter",
                                                                     "()Landroid/bluetooth/BluetoothAdapter;");

    if (env.checkAndClearExceptions()) {
        // Handle JNI exception
        env.checkAndClearExceptions();
        return profiles;
    }

    // Get the Set of paired Bluetooth devices
    QJniObject pairedDevices = bluetoothAdapter.callObjectMethod("getBondedDevices",
                                                                  "()Ljava/util/Set;");

    // Convert the paired devices Set to a QList<QString>
    //QList<QString> deviceList;
    QJniObject iterator = pairedDevices.callObjectMethod("iterator", "()Ljava/util/Iterator;");
    while (iterator.callMethod<jboolean>("hasNext", "()Z")) {
        QJniObject device = iterator.callObjectMethod("next", "()Ljava/lang/Object;");
        QString name = device.callMethod<jstring>("getName", "()Ljava/lang/String;").toString();
        if (name == deviceName) {
            QJniObject uuids = device.callObjectMethod("getUuids", "()[Landroid/os/ParcelUuid;");
            jobjectArray uuidsArray = uuids.object<jobjectArray>();
            int uuidsArrayLength = env->GetArrayLength(uuidsArray);
            // Get the list of supported profiles for this device
            QList<QString> profileList;
            for (int i = 0; i < uuidsArrayLength; i++) {
                QJniObject uuid(env->GetObjectArrayElement(uuidsArray, i));
                //QString uuidStr = uuid.callObjectMethod<jstring>("toString", "()Ljava/lang/String;").toString();
                const char* signature = "()Ljava/lang/String;";
                QString uuidStr = uuid.callObjectMethod<jstring>(signature).toString();
                if (uuidStr.contains("0000111e")) {
                    profileList.append("Handsfree");
                } else if (uuidStr.contains("0000110b")) {
                    profileList.append("Audio Sink");
                } else if (uuidStr.contains("00001105")) {
                    profileList.append("SPP");
                } else if (uuidStr.contains("0000111f")) {
                    profileList.append("Advanced Audio Distribution Profile (A2DP)");
                } else if (uuidStr.contains("0000110e")) {
                    profileList.append("Personal Area Networking Profile (PAN)");
                } else if (uuidStr.contains("0000112f")) {
                    profileList.append("Phone Book Access Profile (PBAP)");
                }
            }
            profiles = profileList;
            return profiles;
        }
    }
    // If the device name was not found, return an empty list
    qDebug() << "Truong123";
    return profiles;
}



//void BluetoothScanner::socketConnected()
//{
//    qDebug() << "Bluetooth socket connected";
//}

//void BluetoothScanner::socketDisconnected()
//{
//    qDebug() << "Bluetooth socket disco nnected";
//}




