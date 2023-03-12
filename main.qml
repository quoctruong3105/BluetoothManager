import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.0
import com.company.bluetoothscanner 1.0
import Qt.labs.settings 1.0

Window {
    id: rootId
    width: 640
    height: 350
    visible: true
    title: qsTr("Hello World")

    property string colorOn: "#4361ee"
    property string colorOff: "grey"
    property bool isOn: false
    property int pairedDeviceCount: 0

    function showAll() {
        var length = bluetoothScannerId.getListLength()
        for(var i = 0; i < length; i ++) {
            bluetoothAvailableList.append({"name" : bluetoothScannerId.getBluetoothDeviceName(i)})
        }
    }

    function removeDevice() {
        for (var i = 0; i < bluetoothAvailableList.count; i++) {
            if (bluetoothAvailableList.get(i).name === pairedDeviceNameId.text) {
                bluetoothAvailableList.remove(i);
                break;
            }
        }
    }

    function startScanning() {
        loadingId.running = true
        timer.start()
    }

    BluetoothScanner {
        id: bluetoothScannerId
    }

    BusyIndicator {
        id: loadingId
        running: false
        anchors.top: availableDeviceTextId.bottom
        anchors.topMargin: 10
        anchors.horizontalCenter: parent.horizontalCenter
    }

    Timer {
        id: timer
        interval: 2000
        repeat: false
        onTriggered: {
            loadingId.running = false
            showAll()
        }
    }

    Rectangle {
        id: refreshBtnId
        height: availableDeviceTextId.height
        width: height
        anchors.bottom: availableDeviceTextId.bottom
        anchors.left: availableDeviceTextId.right
        anchors.leftMargin: 10
        visible: availableDeviceTextId.visible
        Image {
            anchors.fill: parent
            source: "img/image/refresh.png"
        }
        MouseArea {
            hoverEnabled: true
            onEntered: refreshBtnId.scale = 1.2
            onExited: refreshBtnId.scale = 1
            anchors.fill: parent
            onClicked: {
                bluetoothAvailableList.clear()
                var length = bluetoothScannerId.getListLength()
                for(var i = 0; i < length; i ++) {
                    if(bluetoothScannerId.getBluetoothDeviceName(i) !== pairedDeviceNameId.text) {
                        bluetoothAvailableList.append({"name" : bluetoothScannerId.getBluetoothDeviceName(i)})
                    }
                }
            }
        }
    }

    Rectangle {
        id: btnId
        width: 50
        height: width
        radius: 10
        color: isOn ? colorOn : colorOff
        y: 10
        anchors.horizontalCenter: parent.horizontalCenter
        Image {
            width: parent.width
            height: width
            anchors.centerIn: parent
            source: "img/image/bluetooth.png"
        }
        MouseArea {
            anchors.fill: parent
            onClicked: {
                isOn = !isOn
                if(isOn) {
                    bluetoothScannerId.enableLocalDeviceBluetooth()
                    bluetoothScannerId.startDeviceDiscovery()
                    startScanning()
                } else {
                    pairedDeviceCount = 0
                    bluetoothAvailableList.clear()
                    bluetoothScannerId.disableLocalDeviceBluetooth()
                }
            }
        }
    }

    Text {
        id: pairedDeviceTextId
        text: "PAIRED DEVICE"
        font.pointSize: 10
        x: 20
        anchors.top: btnId.bottom
        anchors.topMargin: 5
        visible: pairedDeviceCount === 0 ? false : true
    }

    Rectangle {
        id: pairedDeviceId
        width: rootId.width
        height: pairedDeviceNameId.implicitHeight + 15
        anchors.topMargin: 5
        radius: 30
        color: "lightgrey"
        border.color: "white"
        visible: pairedDeviceTextId.visible
        anchors.top: pairedDeviceTextId.bottom
        Text {
            id: pairedDeviceNameId
            x: 20
            anchors.verticalCenter: parent.verticalCenter
            font.pointSize: 15
        }
        MouseArea {
            anchors.fill: parent
            onClicked: {
                bluetoothAvailableList.append({"name":pairedDeviceNameId.text})
                pairedDeviceNameId.text = ""
                pairedDeviceCount--
                bluetoothScannerId.disconnecToDevice()
            }
        }
    }

    Text {
        id: availableDeviceTextId
        x: 20
        visible: isOn ? true : false
        text: "AVAILABLE DEVICES"
        anchors.top: pairedDeviceTextId.visible ? pairedDeviceId.bottom : btnId.bottom
        anchors.topMargin: pairedDeviceTextId.visible ? 5 : 10
        font.pointSize: 10
    }

    ListView {
        id: bluetoothAvailableView
        width: parent.width
        height: parent.height
        anchors.top: availableDeviceTextId.bottom
        anchors.topMargin: 5
        clip: true
        visible: availableDeviceTextId.visible
        model: bluetoothAvailableList
        delegate: RowLayout {
            spacing: 20
            Rectangle {
                width: rootId.width
                height: deviceNameId.implicitHeight + 15
                radius: 30
                color: "lightgrey"
                border.color: "white"
                Text {
                    id: deviceNameId
                    x: 20
                    anchors.verticalCenter: parent.verticalCenter
                    text: name
                    font.pointSize: 15
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        if(pairedDeviceCount === 0) {
                            pairedDeviceCount++
                            pairedDeviceNameId.text = name
                            removeDevice()
                            bluetoothScannerId.connectToDevice(pairedDeviceNameId.text)
                        } else {
                            bluetoothAvailableList.append({"name":pairedDeviceNameId.text})
                            pairedDeviceNameId.text = name
                            removeDevice()
                            bluetoothScannerId.connectToDevice(pairedDeviceNameId.text)
                        }
                    }
                }
            }
        }
    }

    ListModel {
        id: bluetoothAvailableList
    }

    Component.onCompleted: {
        bluetoothScannerId.startDeviceDiscovery()
    }
}
