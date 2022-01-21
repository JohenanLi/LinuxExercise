import QtQuick 2.12
import QtQuick.Controls 2.5

Page {
    width: 1024
    height: 768
    property string currTime:"testestestset"
    property QtObject diskmodel

    

    header: Label {
        text: qsTr("磁盘页面")
        font.pixelSize: Qt.application.font.pixelSize * 2
        padding: 10
    }
    Rectangle{
        Text {
            text: currTime
            anchors.verticalCenter: parent.verticalCenter
            font.bold: true
    }
    }

    Connections{
        target: diskmodel
        function onUpdated(msg){
            currTime = msg;
        }
    }
}
    

    // ListView {
    //     id: diskListView
    //     anchors.fill: parent

    //     model: diskmodel
    //     spacing: 20
    //     delegate: Item {
    //         x: 5
    //         width: 80
    //         height: 40
    //         Row {
    //             id: row1
    //             Rectangle {
    //                 width: 40
    //                 height: 40
    //                 color: colorCode
    //             }

    //             Text {
    //                 text: defaultDiskInfo
    //                 anchors.verticalCenter: parent.verticalCenter
    //                 font.bold: true
    //             }
    //             spacing: 10
    //         }
    //     }






/*##^##
Designer {
    D{i:2;anchors_height:160;anchors_width:110;anchors_x:10;anchors_y:5}
}
##^##*/

