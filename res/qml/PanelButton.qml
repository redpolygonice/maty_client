import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "theme.js" as Theme

ToolButton {
	id: control
	Layout.preferredHeight: 30
	Layout.preferredWidth: 30

	property color backcolor: Theme.backColor1

	background: Rectangle {
		color: backcolor
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
