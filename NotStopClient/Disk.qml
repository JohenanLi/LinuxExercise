import QtQuick 2.10
import QtQuick.Controls 2.1
import QtQuick.Window 2.2

Page{
    title: qsTr("Test")
    width: 640
    height: 480
    visible: true
    ListView{
        model: myModel
        anchors.fill: parent
        delegate:
        Column{
            Text{
                text: model.display
            }

        }
    }
}
