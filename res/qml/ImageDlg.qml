import QtQuick
import QtQuick.Dialogs
import QtQuick.Layouts
import QtQuick.Controls.Basic
import QtQuick.Window
import "theme.js" as Theme

Window {
	id: imageDlg
	visible: false
	width: imgwidth
	height: imgheight
	x: mainWindow.width / 2 - width / 2
	y: mainWindow.height / 2 - height / 2
	modality: Qt.ApplicationModal
	flags: Qt.Tool

	property string contactImageSrc: ""
	property int imgwidth: 600
	property int imgheight: 500

	Image {
		id: image
		anchors.fill: parent
		fillMode: Image.PreserveAspectFit
		Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
		width: imgwidth
		height: imgheight
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
