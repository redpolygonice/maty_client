import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts

Button {
	id: control
	Layout.minimumWidth: beam
	Layout.maximumHeight: 35
	Layout.preferredHeight: 35
	Layout.preferredWidth: beam
	Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

	property color backcolor: "transparent"
	property color backcolorhover: "#eeeeee"
	property int beam: 70

	background: Rectangle {
		color: {
			if (mouseArea.containsMouse)
			{
				//opacity = 0.4
				return backcolorhover
			}
			else
				return backcolor
		}
		radius: 7
	}

	contentItem: Text {
		text: control.text
		font.pointSize: 12
		color: "black"
		horizontalAlignment: Text.AlignHCenter
		verticalAlignment: Text.AlignVCenter
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
