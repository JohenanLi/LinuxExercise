import QtQuick 2.12
import QtQuick.Controls 2.12

ApplicationWindow {
    id: applicationWindow
    width: 1024
    height: 768
    property alias swipeView: swipeView
    visible: true
    title: qsTr("Linux检测客户端")
    menuBar: MenuBar {
        // spacing: 50
        Menu{
            title: qsTr("命令")
            Action {
                    id: execAction
                    text: qsTr("&E执行命令")
                    onTriggered: {
                        execdialog.open()
                    }
                    
            }
        }
        // Menu{
        //     title: qsTr("帮助")
        //     Action { text: qsTr("&A关于")
        //             onTriggered: {
        //                 console.log("About")
        //             }
        //             }
        //     }
        
        Menu{
            title: qsTr("退出")
            Action { text: qsTr("&Q退出")
                    onTriggered: Qt.quit()
                    
            }
        }
        }
    Dialog {
    id: execdialog
    width: 800
    height: 600
    anchors.centerIn: parent
    title: "命令执行框"
    contentItem: Rectangle{
        Rectangle {
        id: rectangle
        x: 24
        y: 34
        width: 184
        height: 48
        color: "#dae1e5"

        Text {
            id: element
            text: qsTr("命令输入：")
            anchors.rightMargin: 90
            anchors.bottomMargin: 8
            anchors.fill: parent
            font.pixelSize: 37
        }
    }

    Rectangle {
        id: rectangle1
        x: 227
        y: 34
        width: 531
        height: 48
        color: "#ffffff"
        border.color: "#ef3a3a"

        TextInput {
            id: execInputText
            text: qsTr("在这里输入命令")
            selectionColor: "#2edbef"
            anchors.fill: parent
            font.pixelSize: 37
        }
    }

    Rectangle {
        id: rectangle2
        x: 24
        y: 100
        width: 734
        height: 399
        color: "#f8fafb"
        border.color: "#2edbef"

        TextField {
            id: execResultText
            text: "在这里显示结果"
            anchors.fill: parent
            placeholderText: qsTr("Text Field")
        }
    }

    Button {
        id: execbutton
        x: 483
        y: 523
        text: qsTr("执行命令")
        onClicked:{
            console.log(execInputText.text)
            console.log(execResultText.text)
        }
    }
    
    Button {
        id: quitbutton
        x: 640
        y: 523
        text: qsTr("退出")
        onClicked:{
            execdialog.close()
        }
    }
    }
        
    }
    SwipeView {
        id: swipeView
        visible: true
        orientation: Qt.Horizontal
        anchors.fill: parent
        currentIndex: tabBar.currentIndex
        CPU{

        }
        Memory{

        }
        Disk{

        }
        Network{

        }
        KernelModule{

        }
        SystemInfo{

        }
    }

    header: TabBar {
        id: tabBar
        currentIndex: swipeView.currentIndex

        TabButton {
            text: qsTr("CPU")
        }
        TabButton {
            text: qsTr("内存")
            hoverEnabled: true
            wheelEnabled: false
            spacing: 20
        }
        TabButton{
            text :qsTr("硬盘")
        }
        TabButton{
            text :qsTr("网络")
        }
        TabButton{
            text :qsTr("内核模块")
        }
        TabButton{
            text :qsTr("系统信息")
        }
    }
}
