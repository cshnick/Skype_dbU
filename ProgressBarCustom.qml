import QtQuick 2.0

Rectangle {
    property int value: 0
    property int min: 0
    property int max: 100

    function calc_width() {
        var val = value;
        if (val < min) {
            return 0
        }
        if (val > max) {
            return parent.width
        }

        var range1 = max - min
        var range2 = parent.width

        var coeff = range2 / range1
        val = val * coeff - min * coeff

        /*console.log("Range1:" + range1)
        console.log("Range2:" + range2)
        console.log("New value:" + val)*/
        return val
    }

    color: "transparent"
    width: 100
    height: 4

    Rectangle {
        height: parent.height
        width: calc_width()
        color: "#579AE0"

        Behavior on width {SmoothedAnimation {velocity: 200 }}
    }
}

