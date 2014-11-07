import QtQuick 2.0
import CochlearNerveWindow 1.0

// Splash screen. http://qt-project.org/forums/viewthread/36745/

Item {
    id: root
    width:  400
    height: 300

    Rectangle {
        id: backgroundRectangle
        anchors.fill: parent
        color: "red"

        Text {
            text: mainLoader.status != Loader.Ready ? 'Loading' : 'Loaded'
            anchors.centerIn: parent
        }
    }

    MouseArea {
        width:  200
        height: 150
        onClicked: {
            mainLoader.source = ""
            backgroundRectangle.visible = false
        }
    }

    Loader {
        id: mainLoader
        //anchors.fill: parent
        asynchronous: true
        visible: status == Loader.Ready
    }

    PauseAnimation {
        id: fakeLoadingDelay
        duration: 50
        onRunningChanged: {
            if ( !running ) {
                MainWindow.init();
                mainLoader.source = "qrc:qml/main.qml"
            }
        }
    }

    Component.onCompleted: fakeLoadingDelay.start()
}
