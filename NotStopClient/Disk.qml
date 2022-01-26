import QtQuick 2.15
import QtQuick.Controls 2.15
Page{
    visible: true
    width: 400
    height: 600
    title: "disk"
    objectName: "diskpage"
    header: Label {
        text: qsTr("存储页面")
        font.pixelSize: Qt.application.font.pixelSize * 2
        padding: 10
    }

    property QtObject backend
    property string mytext:""
    property var diskModel:ListModel {
    id: sysInfo
}
    Connections{
        target: backend
        function onDiskInfo(msg) {
            diskModel.clear()
            var a = JSON.parse(msg)
            var arr = Object.keys(a);
            for(var i=0;i<arr.length;i++){
                diskModel.append(a["ListElement"+i])
            }
            var numerator = a["ListElement3"].value.match(/\d+/g)
            console.log(numerator)
        }
        }


    ListView {
        id: diskListView
        anchors.rightMargin: 0
        anchors.bottomMargin: 0
        anchors.leftMargin: 0
        anchors.topMargin: 0
        anchors.fill: parent
        model: diskModel
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
                    text: name+value
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
        text: qsTr("超过")+slider.value+"%"+"不可用时报警"
    }
}
}
}
