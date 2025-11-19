import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ApplicationWindow {
    id: root
    width: 480
    height: 320
    visible: true
    title: qsTr("SpaceBuddy - Görsel Demo")
    color: "#f5f7fa"

    Column {
        anchors.centerIn: parent
        spacing: 16
        width: parent.width * 0.9

        Text {
            id: statusText
            text: "Bağlantı durumu: " + Backend.connectionStatus
            font.pointSize: 14
            horizontalAlignment: Text.AlignHCenter
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Row {
            spacing: 8
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width * 0.95

            TextField {
                id: inputField
                placeholderText: "Sifre"
                width: parent.width * 0.4
            }

            Button {
                text: "Bağlan"
                focusPolicy: Qt.NoFocus
                onClicked: Backend.connectToServer(inputField.text)
                enabled: Backend.connectionStatus !== "Bağlandı"
            }

            Button {
                text: "Bağlantıyı Kes"
                focusPolicy: Qt.NoFocus
                onClicked: Backend.disconnectFromServer()
                enabled: Backend.connectionStatus === "Bağlandı"
            }
        }

        Row {
            spacing: 12
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width * 0.95

            Button {
                text: Backend.syncEnabled ? "Senkronizasyonu durdur" : "Senkronizasyonu başlat"
                focusPolicy: Qt.NoFocus
                onClicked: Backend.syncEnabled = !Backend.syncEnabled
            }
        }

        Row {
            spacing: 12
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width * 0.95

            Button {
                text: "Test"
                focusPolicy: Qt.NoFocus
                onClicked: Backend.sendTestSignal()
                enabled: Backend.connectionStatus === "Bağlandı"
            }
        }
    }

}
