import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "common.js" as Common

ToolButton {
    id: control
    Layout.preferredHeight: 30
    Layout.preferredWidth: 30
    ToolTip.visible: hovered
    ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval

    background: Rectangle {
        color: Common.backColor1
    }

    contentItem: Text {
        text: control.text
        font: control.font
        color: {
            return mouseArea.containsMouse ? "gray" : "black"
        }
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignTop
        elide: Text.ElideRight
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
        onClicked: control.clicked()
    }
}
