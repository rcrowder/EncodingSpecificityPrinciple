import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0

Rectangle {
    id: rootRectangle
    width:  200
    height: 150

    // This item is the item that will be dislplayed as the splash screen
    Rectangle {
        id: backgroundRectangle
        anchors.fill: parent
        color: "blue"

        Text {
            anchors.centerIn: parent
            text: "Hello World!"
        }
    }

}
