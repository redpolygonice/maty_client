import QtQuick
import QtQuick.Dialogs
import QtQuick.Layouts
import QtQuick.Controls.Basic
import QtQuick.Window
import "common.js" as Common

Window {
	id: imageDlg
	visible: false
	width: 600
	height: 500
	x: mainWindow.width / 2 - width / 2
	y: mainWindow.height / 2 - height / 2
	flags: Qt.Tool

	property string contactImageSrc: ""

	Component.onCompleted: {
		width = image.width
		height = image.height
	}

	Image {
		id: image
		anchors.fill: parent
		fillMode: Image.PreserveAspectFit
		Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
		smooth: true
		source: contactImageSrc

		MouseArea {
			id: imageArea
			anchors.fill: parent
			acceptedButtons: Qt.LeftButton | Qt.RightButton
			hoverEnabled: true
			cursorShape: Qt.PointingHandCursor
			onClicked: {
			}
		}
	}
}
