import QtQuick
import QtQuick.Dialogs
import QtQuick.Layouts
import QtQuick.Controls.Basic
import QtQuick.Window
import "theme.js" as Theme

Rectangle {
	id: imageDlg
	visible: false
	anchors.fill: parent

	property string contactImageSrc: ""
	property string contactName: ""
	property int imgwidth: 700
	property int imgheight: 600

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
