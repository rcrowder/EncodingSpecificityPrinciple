import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0

Rectangle {
    id: rootRectangle
    width: 1280
    height: 800

    Item
    {
        id: toolbar
        height: 50
        anchors.top: rootRectangle.top
        anchors.left: rootRectangle.left
        anchors.right: rootRectangle.right

        Rectangle {
            anchors.fill: parent
            color: "transparent"
            opacity: 0.2
        }

        Timer {
            interval: 1000
            running: true
            repeat: true
            onTriggered: {
                clock.update()
            }
        }

        Text{
            id: clock
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: powerButton.left
            anchors.rightMargin: 20
            text : (Qt.formatDateTime(new Date(), "h:mm - dd/MM"))
            font.pixelSize: 40
            color: "white"

            function getCurrentTime()
            {
                return (Qt.formatDateTime(new Date(), "h:mm - dd/MM"));
            }

            function update()
            {
                clock.text = clock.getCurrentTime();
            }
        }

    }
    Rectangle {
        id: volumeRectangle
        color: "transparent"
        opacity: 0

        anchors.right: volumeButton.right
        anchors.bottom: volumeButton.bottom
        width: volumeButton.width
        height: volumeButton.height + volumeBar.height

        Behavior on opacity {
            NumberAnimation { target: volumeRectangle; property: "opacity"; duration: 500; easing.type: Easing.InOutQuad; }
        }

        Rectangle {
            id: volumeBgRectangle
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            height: volumeButton.height
            color: "white"
            opacity: 0.8
        }

        Rectangle {
            id: volumeFgRectangle
            anchors.right: volumeBgRectangle.right
            anchors.bottom: volumeBgRectangle.top
            anchors.left: volumeBgRectangle.left
            width: 170
            height: 250
            color: "white"
            opacity: 0.8
        }

        VolumeBar {
            id: volumeBar
            anchors.top: volumeFgRectangle.top
            anchors.bottom: volumeFgRectangle.bottom
            anchors.horizontalCenter: volumeFgRectangle.horizontalCenter
            currentVolume: statusManager.mixer_volume
            onVolumeChanged: playerController.setVolume(volume)
        }
    }

}
