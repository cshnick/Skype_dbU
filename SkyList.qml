import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

Rectangle {
    id: container
    anchors.fill: parent
    signal textChanged(string text)

    function filter(text) {
        sky_model.stringChanged(text)
    }

    ScrollView {
        anchors.fill: parent
        ListView {
            id: sky_view
            anchors.fill: parent
            width: parent.width
            model: sky_model
            highlight: Rectangle { color: "lightsteelblue"; radius: 0 }
            highlightMoveDuration : 50
            highlightMoveVelocity : 200

            section.property: "Chatname"
            section.criteria: ViewSection.FullString
            section.delegate: Rectangle {
                width: container.width
                height: 30
                color: "transparent"

                Rectangle {
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: parent.width - 80
                    height: 1
                    color: "#aaa"
                }

                Text {
                    renderType: Text.NativeRendering
                    text: section
                    font.bold: true
                    font.pointSize: 11
                    anchors.centerIn: parent
                }
            }

            delegate: Item {
                x: 5
                width: sky_view.width
                height: Height

                Text {
                    id: author

                    anchors.left: parent.left
                    anchors.top: parent.top

                    color: "#333"
                    renderType: Text.NativeRendering
                    text: Author
                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignTop
                    font.pointSize: 8
                    font.bold: true
                }

                Row {
                    y: author.height
                    height: parent.height - author.height
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
                Text {
                    id: time_stamp

                    anchors.right: parent.right
                    anchors.rightMargin: 20
                    anchors.bottom: parent.bottom

                    color: "#777"
                    renderType: Text.NativeRendering
                    text: Timestamp
                    horizontalAlignment: Text.AlignRight
                    verticalAlignment: Text.AlignBottom
                    font.pointSize: 8
                    font.bold: false
                }


                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        sky_view.currentIndex = index
                    }
                }

            }

            Component.onCompleted: {
                sky_model.loadSkypeTest()
            }
        }

        style: ScrollViewStyle {
            transientScrollBars: true
            handle: Item {
                implicitWidth: 14
                implicitHeight: 26
                Rectangle {
                    color: "#424246"
                    anchors.fill: parent
                    anchors.topMargin: 6
                    anchors.leftMargin: 4
                    anchors.rightMargin: 4
                    anchors.bottomMargin: 6
                }
            }
            scrollBarBackground: Item {
                implicitWidth: 14
                implicitHeight: 26
            }
        }
    }
}

