import QtQuick 2.0
import CochlearNerveWindow 1.0

/*Splash screen. This QML file is displayed as soon as possible when the application is
  starting in order to guve feedback to the user. Then, the method Touchradio::Init() is
  called, and the main QML file is loaded and will be displayed when the init is done.
  Thanks to http://qt-project.org/forums/viewthread/36745/
  */

Item {
    id: root
    width:  400
    height: 300

    Rectangle {
        id: backgroundRectangle
        anchors.fill: parent
        color: "red"
    }

    Loader {
        id: main
        anchors.fill: parent
        asynchronous: true
        visible: status == Loader.Ready
    }

    PauseAnimation {
        id: fakeLoadingDelay
        duration: 50
        onRunningChanged: {
            if ( !running ) {
                MainWindow.init();
                main.source = "qrc:qml/main.qml"
            }
        }
    }

    Component.onCompleted: fakeLoadingDelay.start()
}
