import QtQuick 2.4
import QtQuick.Controls 2.15
Page{
    visible: true
    width: 400
    height: 600
    title: "network"
    objectName: "networkpage"
    header: Label {
        text: qsTr("网络页面")
        font.pixelSize: Qt.application.font.pixelSize * 2
        padding: 10
    }

    property QtObject backend
    property string mytext:""
    property var networkModel:ListModel {
    id: sysInfo
}
    Connections{
        target: backend
        function onNetworkInfo(msg) {
            networkModel.clear()
            var a = JSON.parse(msg)
            var arr = Object.keys(a);
            for(var i=0;i<arr.length;i++){
                networkModel.append(a["ListElement"+i])
            }
        }
        }


    ListView {
        id: networkListView
        anchors.rightMargin: 0
        anchors.bottomMargin: 0
        anchors.leftMargin: 0
        anchors.topMargin: 0
        anchors.fill: parent
        model: networkModel
        spacing: 200
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
                    text: name+"\n"+value
                    anchors.verticalCenter: parent.verticalCenter
                    font.bold: true
                }
                spacing: 10
            }
        }

}
}
