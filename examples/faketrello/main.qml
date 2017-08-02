import QtQuick 2.3
import QtQuick.Window 2.2
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.1
import "./views"

ApplicationWindow {
    id: window
    width: 640
    height: 480
    visible: true

    ColumnLayout {
        anchors.fill: parent

        ToolBar {
            Layout.fillWidth: true
        }

        Board {
            Layout.fillHeight: true
            Layout.fillWidth: true
        }

    }

    // NPM: Persist the model to file on application close.
    // see https://github.com/benlau/qsyncable/issues/2
    // BUG: when running on Android, putting application in background and closing
    // (via swipe or close window button) doesn't call this -- you just get
    // '"org.qtproject.example.faketrello" died.' message before anything can be done.
    // However,  closing android app with back button works and persists the board.
    onClosing: {
        //console.log("Model=" + App.stringifyModel());
        console.log("Exiting application... Persisting model to file " + PersistFilePath);
        App.persistModel(PersistFilePath);
        close.accepted = true;
    }
}
