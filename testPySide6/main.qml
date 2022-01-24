import QtQuick 2.15
import QtQuick.Controls 2.15
ApplicationWindow{
    visible: true
    width: 400
    height: 600
    title: "Clock"
    objectname: "item
    flags: Qt.FramelessWindowHint | Qt.Window
    property string currTime:"00:00:00"
    
    property QtObject backend
    Connections{
        target: backend
        function onUpdated(msg) {
            currTime = msg
        }
    }
    Rectangle{
        anchors.fill: parent
        Rectangle{
            Text{
            // anchors{
            //     bottom:parent.bottom
            //     bottomMargin:12
            //     left:parent.left
            //     leftMargin:12
            // }
            text:currTime
            font.pixelSize: 48
            color:"red"
            }
        }

    }
}