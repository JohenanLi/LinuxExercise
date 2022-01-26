import QtQuick 2.12
import QtQuick.Controls 2.12
import examples.signals.qmltopy1 1.0
ApplicationWindow {
    id: applicationWindow
    objectName:"mainwindow"
    width: 1024
    height: 768
    property alias swipeView: swipeView
    visible: true
    title: qsTr("Linux检测客户端")
    property QtObject backend
    Connections{
        target:backend
        function onCmdResult(msg){
            execResultText.text = msg
            
        }
    }
    Console {
        id: pyConsole
    }

    menuBar: MenuBar {
        spacing: 50
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

        
        Menu{
            title: qsTr("退出")
            Action { text: qsTr("&Q退出")
                    onTriggered: {

                        Qt.quit()
                    }
                    
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

        Text {
            id: execResultText
            text: "在这里显示结果"
            objectName: "resultText"
            anchors.fill: parent
            // lineHeight: Text.ProportionalHeight //设置行间距
            wrapMode: Text.WordWrap //换行
            font.pixelSize: 15 
            fontSizeMode: Text.Fit //固定 Text 显示大小->字体自动变化的模式选中还有几种看文档
            minimumPixelSize: 10 

        }
        }
    

    Button {
        id: execbutton
        objectName: "execbutton"
        x: 483
        y: 523
        text: qsTr("执行命令")
        onClicked:{
                pyConsole.output(execInputText.text)
        }

        }
    }
    
    Button {
        id: quitbutton
        x: 640
        y: 523
        text: qsTr("退出")
        onClicked:{
            execInputText.text = "在这里输入命令"
            pyConsole.output("在这里输入命令")
            execdialog.close()
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
