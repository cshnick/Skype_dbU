import QtQuick 2.0

Rectangle {
    width: 100
    height: 62

    ListView {
        id: listView1
        anchors.fill: parent
        model: sky_model
        delegate: Item {
            x: 5
            width: 80
            height: 40
            Row {
                id: row1
                Rectangle {
                    width: 40
                    height: 40
                    color: "#ddd"
                }

                Text {
                    text: Name
                    anchors.verticalCenter: parent.verticalCenter
                    font.bold: true
                }
                spacing: 10
            }
        }

        Component.onCompleted: {
            sky_model.loadTest()
        }
    }
}

