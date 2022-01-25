import QtQuick 2.15
import QtQuick.Controls 2.15
Page{
    visible: true
    width: 400
    height: 600
    title: "memory"
    objectName: "memorypage"
    header: Label {
        text: qsTr("内存页面")
        font.pixelSize: Qt.application.font.pixelSize * 2
        padding: 10
    }

    property QtObject backend
    property string mytext:""
    property var memoryModel:ListModel {
    id: sysInfo
}
    Connections{
        target: backend
        function onMemoryInfo(msg) {
            memoryModel.clear()
            var a = JSON.parse(msg)
            var arr = Object.keys(a);
            for(var i=0;i<arr.length;i++){
                memoryModel.append(a["ListElement"+i])
            }

        }
        }


    ListView {
        id: memoryListView
        anchors.rightMargin: 0
        anchors.bottomMargin: 0
        anchors.leftMargin: 0
        anchors.topMargin: 0
        anchors.fill: parent
        model: memoryModel
        spacing: 20
        delegate: Item {
            x: 5
            width: 80
            height: 40
            Row {
                id: row1
                Rectangle {
                    width: 40
                    height: 40
                    color: colorCode
                }

                Text {
                    text: name+":"+value
                    anchors.verticalCenter: parent.verticalCenter
                    font.bold: true
                }
                spacing: 10
            }
        }
Slider {
    id: slider
    x: 765
    y: -39
    value: 0.5

    Label {
        id: label
        x: 34
        y: -15
        text: qsTr("超过x个不可用时报警")
    }
}
}
}