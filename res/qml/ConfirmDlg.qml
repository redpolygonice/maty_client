import QtQuick
import QtQuick.Dialogs
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Window
import "common.js" as Common

Dialog {
	id: dialog
	visible: false
	x: mainWindow.width / 2 - width / 2
	y: mainWindow.height / 2 - height / 2
	width: 300
	height: 160
	padding: 10
	margins: 0

	property string titleText: ""
	property string messageText: ""

	signal okClicked()

	Component.onCompleted: {
		if (titleText === "")
		{
			textTitle.visible = false
			//textRect.height = 50
			//dialog.height = 100
		}
	}

	background: Rectangle {
		color: "#FFFFFF"
		radius: 15
	}

	header: Label {
		height: 0
	}

	footer: Item {}

	ColumnLayout {
		Layout.leftMargin: 20
		Layout.rightMargin: 20
		anchors.fill: parent

		Rectangle {
			id: textRect
			height: 100

			ColumnLayout {
				Layout.leftMargin: 20
				Layout.rightMargin: 20
				anchors.fill: parent

				Text {
					id: textTitle
					wrapMode: Text.WordWrap
					font.pointSize: 13
					Layout.alignment: Qt.AlignHCenter
					text: titleText
				}

				Text {
					id: textMessage
					wrapMode: Text.WordWrap
					font.pointSize: 13
					text: messageText
				}
			}
		}

		Rectangle {
			Layout.fillHeight: true
			Layout.fillWidth: true

			RowLayout {
				anchors.fill: parent
				spacing: 5

				Label {
					Layout.fillWidth: true
				}

				LabelButton {
					id: buttonOk
					backcolor: "#eeeeee"
					backcolorhover: "#dddddd"
					text: "OK"

					onClicked: {
						dialog.okClicked();
						accept()
					}
				}

				LabelButton {
					id: buttonCancel
					backcolor: "#eeeeee"
					backcolorhover: "#dddddd"
					text: "Cancel"

					onClicked: {
						reject()
					}
				}
			}
		}
	}
}
