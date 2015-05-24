import QtQuick 2.0

Rectangle {
    id: container
    anchors.fill: parent
    signal textChanged(string text)

    function filter(text) {
        sky_model.stringChanged(text)
    }

    ListView {
        id: sky_view
        anchors.fill: parent
        model: sky_model
        delegate: Item {
            x: 5
            width: sky_view.width
            height: Height
            Row {
                height: parent.height
                id: row1
                Rectangle {
                    anchors.verticalCenter: parent.verticalCenter
                    width: 4
                    height: parent.height - 4
                    color: index % 2 == 0 ? "#E61875" : "#03A9F4"
                }

                Text {
                    renderType: Text.NativeRendering
                    text: Name
                    anchors.verticalCenter: parent.verticalCenter
                    font.bold: false
                    font.pointSize: 10
                }
                spacing: 10
            }
        }

        Component.onCompleted: {
            sky_model.loadSkypeTest()
        }
    }
}

