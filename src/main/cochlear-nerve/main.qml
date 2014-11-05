import QtQuick 2.3
import QtQuick.Controls 1.2

ApplicationWindow {
    visible: true
    width: 400
    height: 300
    title: qsTr("Cochlear Neural Encoding using AIMc")

    MouseArea {
        anchors.fill: parent
        onClicked: { _fileOpenHandler.buttonClicked() }
    }

    menuBar: MenuBar {
        Menu {
            title: qsTr("File")
            MenuItem {
                text: qsTr("&Open settings")
                onTriggered: console.log("Open action triggered");
            }
            MenuItem {
                text: qsTr("&Save settings")
                onTriggered: console.log("Save action triggered");
            }
            MenuItem {
                text: qsTr("E&xit")
                onTriggered: Qt.quit();
            }
        }
    }
}
