import QtQuick
import QtQuick.Dialogs
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Window
import "theme.js" as Theme

Dialog {
	id: dialog
	visible: false
	x: mainWindow.width / 2 - width / 2
	y: mainWindow.height / 2 - height / 2
	width: 300
	height: 160
	padding: 10
	margins: 0
	modal: true

	enum DialogButtons {
		Ok,
		Cancel,
		Yes,
		No
	}

	property string messageText: ""
	property int buttons: ConfirmDlg.Ok | ConfirmDlg.Cancel

	signal okClicked()
	signal yesClicked()

	background: Rectangle {
		color: "#FFFFFF"
		radius: 15
	}

	header: Label {
		height: 0
	}

	footer: Item {}

	ColumnLayout {
		Layout.topMargin: 20
		Layout.leftMargin: 20
		Layout.rightMargin: 20
		anchors.fill: parent

		Rectangle {
			id: textRect
			height: 100
			width: 280
			Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

			TextArea {
				id: textMessage
				anchors.fill: parent
				font.pointSize: 13
				text: messageText
				readOnly: true
				wrapMode: Text.Wrap
				color: Theme.textColor
				Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
				horizontalAlignment: Text.AlignHCenter
				verticalAlignment: Text.AlignVCenter
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
					visible: {
						return buttons & ConfirmDlg.Ok
					}

					onClicked: {
						dialog.okClicked();
						accept()
					}
				}

				LabelButton {
					id: buttonYes
					backcolor: "#eeeeee"
					backcolorhover: "#dddddd"
					text: "Yes"
					visible: {
						return buttons & ConfirmDlg.Yes
					}

					onClicked: {
						dialog.yesClicked();
						accept()
					}
				}

				LabelButton {
					id: buttonCancel
					backcolor: "#eeeeee"
					backcolorhover: "#dddddd"
					text: "Cancel"

					visible: {
						return buttons & ConfirmDlg.Cancel
					}

					onClicked: {
						reject()
					}
				}

				LabelButton {
					id: buttonNo
					backcolor: "#eeeeee"
					backcolorhover: "#dddddd"
					text: "No"

					visible: {
						return buttons & ConfirmDlg.No
					}

					onClicked: {
						reject()
					}
				}
			}
		}
	}
}
