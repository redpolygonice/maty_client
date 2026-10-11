import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Basic
import "theme.js" as Theme

Rectangle {
	id: imageView
	visible: false
	anchors.fill: parent

	property string contactImageSrc: ""
	property string contactName: ""

	ColumnLayout {
		anchors.fill: parent
		spacing: 0
		Layout.margins: 0

		Rectangle {
			Layout.fillWidth: true
			Layout.fillHeight: false
			Layout.minimumHeight: 40
			Layout.maximumHeight: 40
			height: 40

			RowLayout {
				anchors.fill: parent
				Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
				spacing: 10

				Label {
					Layout.fillWidth: true
				}

				PanelButton {
					text: "<b>....</b>"
					backcolor: "#ffffff"
					onClicked: {
					}
				}

				PanelButton {
					Layout.rightMargin: 10
					text: "<b>X</b>"
					backcolor: "#ffffff"
					onClicked: {
						imageView.visible = false
					}
				}
			}
		}

		Rectangle {
			color: Theme.backColor4
			Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
			Layout.margins: 0
			Layout.fillWidth: true
			Layout.fillHeight: true

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
	}
}
