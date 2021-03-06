import QtQuick 2.4
import QtQuick.Controls 2.4
Page{
    visible: true
    width: 400
    height: 600
    title: "kernelmodule"
    objectName: "kernelmodulepage"
    header: Label {
        text: qsTr("内核模块页面")
        font.pixelSize: Qt.application.font.pixelSize * 2
        padding: 10
    }

    property QtObject backend
    property string mytext:""
    property var kernelmoduleModel:ListModel {
    id: sysInfo
}
    Connections{
        target: backend
        function onKernelModuleInfo(msg) {
            kernelmoduleModel.clear()
            var a = JSON.parse(msg)
            var arr = Object.keys(a);
            for(var i=0;i<arr.length;i++){
                kernelmoduleModel.append(a["ListElement"+i])
            }

        }
        }


    ListView {
        id: kernelmoduleListView
        anchors.rightMargin: 0
        anchors.bottomMargin: 0
        anchors.leftMargin: 0
        anchors.topMargin: 0
        anchors.fill: parent
        model: kernelmoduleModel
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
}
}
