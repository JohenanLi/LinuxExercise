import QtQuick 2.12
import QtQuick.Controls 2.5

Page {
    width: 1024

    height: 768


    header: Label {
        text: qsTr("CPU页面")+"\t\t总共：%x个"+"\t\t可用：%x个"
        font.pixelSize: Qt.application.font.pixelSize * 2
        padding: 10
    }
    ListModel {
        id:cpuListModel
        ListElement {
            name: "总计"
            colorCode: "white"
        }
        ListElement {
            name: "核0"
            colorCode: "grey"
        }

        ListElement {
            name: "核1"
            colorCode: "red"
        }

        ListElement {
            name: "核2"
            colorCode: "blue"
        }

        ListElement {
            name: "核3"
            colorCode: "purple"
        }
        ListElement {
            name: "核4"
            colorCode: "pink"
        }
        ListElement {
            name: "核6"
            colorCode: "yellow"
        }
        ListElement {
            name: "核5"
            colorCode: "brown"
        }
        ListElement {
            name: "核7"
            colorCode: "black"
        }


    }
    ListView {
        id: cpuListView
        anchors.fill: parent
        model: cpuListModel
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
                    text: name+"\t\t"+"cpuMHZ:2400.000"+"\t\t"+"当前状况：可用"
                    anchors.verticalCenter: parent.verticalCenter
                    font.bold: true
                }
                spacing: 10
            }
        }


    Slider {
        id: slider
        x: 767
        y: -35
        value: 0.5

        Label {
            id: label
            x: 34
            y: -6
            text: qsTr("超过x个不可用时报警")
        }
    }
}



/*##^##
Designer {
    D{i:2;anchors_height:160;anchors_width:110;anchors_x:10;anchors_y:5}
}
##^##*/
}
