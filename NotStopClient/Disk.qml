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
            var numarr = a["ListElement3"].value.match(/\d+/g)
            var numerator = numarr[2]
            var denominator = numarr[1]
            if((100 -(numerator / denominator)*100) <  diskslider.value)
            {
                root.x = (parent.width - dialog.width) * 0.5
                root.y = (parent.height - dialog.height) * 0.5
                dialog.open();
            }
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
    id: diskslider
    x: 765
    y: -39
    from: 0
    to: 100
    stepSize: 1
    value: 50

    Label {
        id: label
        x: 34
        y: -15
        text: qsTr("可用空间少于")+diskslider.value+"%"+"报警"
    }
}

Item {
    id: root
    anchors.centerIn: parent
    //提示框内容
    property alias tipText: msg.text
    //提示框颜色
    property string backGroundColor: "white"
    property Item parentItem : Rectangle {}

    //Dialog header、contentItem、footer之间的间隔默认是12
    // 提示框的最小宽度是 100
    width: 300
    height: msg.implicitHeight + 24 + 100

    Dialog {
        id: dialog
        width: root.width
        height: root.height
        modal: true
        property string backGroundColor: "white"
        background: Rectangle {
            color: "white"
            anchors.fill: parent
            radius: 5
        }
        header: Rectangle {
            width: dialog.width
            height: 50
            border.color: "white"
            radius: 5
            Image {
                width: 40
                height: 40
                anchors.centerIn: parent
                source: "images/xfce4_xicon.png"
            }

        }
        contentItem: Rectangle {
            border.color: "white"
            color: "white"
            Text {
                anchors.fill: parent
                anchors.centerIn: parent
                font.family: "Microsoft Yahei"
                color: "gray"
                text: "警告,磁盘空间使用已超过阈值"
                wrapMode: Text.WordWrap
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter

            }
        }
        footer: Rectangle {
            width: msg.width
            height: 50
            border.color: "white"
            color: "white"
            radius: 5
            Button {
                anchors.centerIn: parent
                width: 80
                height: 30
                background: Rectangle {
                    anchors.centerIn: parent
                    width: 80
                    height: 30
                    radius: 5
                    border.color: "#0f748b"
                    border.width: 2
                    color: "white"
                    Text {
                        anchors.centerIn: parent
                        font.family: "Microsoft Yahei"
                        font.bold: true
                        color: "#0f748b"
                        text: "OK"
                    }
                }
                onClicked: {
                    dialog.close();
                }
            }
        }
    }

    //利用Text 的implicitWidth属性来调节提示框的大小
    //该Text的字体格式需要与contentItem中的字体一模一样
    Text {
        id: msg
        visible: false
        width: 300
        wrapMode: Text.WordWrap
        font.family: "Microsoft Yahei"
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
    }

    function openMsg() {
        root.x = (parent.width - dialog.width) * 0.5
        root.y = (parent.height - dialog.height) * 0.5
        dialog.open();
    }
}




}
}
