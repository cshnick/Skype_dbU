import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.3
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

ApplicationWindow {
    id: container_window
    title: qsTr("Hello World")
    width: 640
    height: 480
    visible: true

    menuBar: MenuBar {
        Menu {
            title: qsTr("&File")
            MenuItem {
                text: qsTr("&Open")
                onTriggered: messageDialog.show(qsTr("Open action triggered"));
            }
            MenuItem {
                text: qsTr("E&xit")
                onTriggered: Qt.quit();
            }
        }
    }

    TextField {
        id: search_field
        height: 30
        width: parent.width
        style: TextFieldStyle {
            background: Rectangle {
                implicitWidth: 100
                implicitHeight: 24
                border.color: "#333"
                border.width: 1
            }
        }

        onEditingFinished: {
            console.log("search_field editing finished")
            sky_list.filter(search_field.text)
        }
    }

    ProgressBarCustom {
        id: progress_bar

        y: search_field.height
        width: parent.width
        height: 2
        value: sky_model.loadProgress

        Connections {
            target: sky_model
            onLoadFinished: {
                progress_bar.visible = false
            }
        }
    }

    SkyList {
        id: sky_list
        function text_changed(text) {
            if (text.length > 2) {
                filter(text)
            }
        }

        anchors.fill: undefined
        x: 0
        y: search_field.height + 2
        clip: true
        height: parent.height - search_field.height -2
        width: parent.width

        Connections {
            target: search_field
            onTextChanged: {
                sky_list.text_changed(search_field.text)
            }
        }
    }
}
