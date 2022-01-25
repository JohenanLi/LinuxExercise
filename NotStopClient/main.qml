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
                    onTriggered: console.log("aaa")
                    
            }
        }
        Menu{
            title: qsTr("帮助")
            Action { text: qsTr("&A关于")
                    onTriggered: dialog.visible=true
                    }
            }
        
        Menu{
            title: qsTr("退出")
            Action { text: qsTr("&Q退出")
                    onTriggered: Qt.quit()
                    
            }
        }
        }
    Dialog {
    id: dialog
    title: "Title"
    standardButtons: Dialog.Ok | Dialog.Cancel

    onAccepted: console.log("Ok clicked")
    onRejected: console.log("Cancel clicked")
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
