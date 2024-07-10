import QtQuick
import QtQuick.Dialogs
import QtQuick.Layouts
import QtQuick.Controls.Basic
import QtQuick.Window
import "common.js" as Common

Dialog {
	id: dialog
	visible: false
	title: "Contact card"
	x: mainWindow.width / 2 - width / 2
	y: mainWindow.height / 2 - height / 2
	width: 300
	height: 280
	padding: 0
	margins: 0

	enum Reason {
		Show,
		New,
		Edit
	}

	property int contactId: -1
	property int reason: CardDlg.Reason.Show

	background: Rectangle {
		color: "#FFFFFF"
		radius: 15
	}

	header: Label {
		height: 0
	}

	footer: Item {}

	onVisibleChanged: {
		if (visible)
			load()
	}

	ColumnLayout {
		anchors.fill: parent
		anchors.margins: 0
		spacing: 10

		Label {
			text: dialog.title
			font.pointSize: 13
			font.bold: true
			color: "black"
			horizontalAlignment: Text.AlignHCenter
			verticalAlignment: Text.AlignVCenter
			Layout.topMargin: 10
			Layout.fillWidth: true
		}

		ColumnLayout {
			Layout.leftMargin: 20
			Layout.rightMargin: 20
			width: parent.width

			Text {
				text: "Name"
				font.pointSize: Common.fontPointSize
			}

			TextField {
				id: nameText
				text: ""
				readOnly: reason === CardDlg.Reason.Show ? true : false
				Layout.fillWidth: true
				font.pointSize: Common.fontPointSize
				implicitHeight: 30
				color: "black"
				background: Rectangle {
					color: Common.backColor1
				}
			}

			Text {
				text: "Path to photo"
				font.pointSize: Common.fontPointSize
			}

			TextField {
				id: photoText
				text: ""
				readOnly: reason === CardDlg.Reason.Show ? true : false
				Layout.fillWidth: true
				font.pointSize: Common.fontPointSize
				implicitHeight: 30
				color: "black"
				background: Rectangle {
					color: Common.backColor1
				}
			}

			Text {
				text: "Phone number"
				font.pointSize: Common.fontPointSize
			}

			TextField {
				id: phoneText
				text: ""
				readOnly: reason === CardDlg.Reason.Show ? true : false
				Layout.fillWidth: true
				font.pointSize: Common.fontPointSize
				implicitHeight: 30
				color: "black"
				background: Rectangle {
					color: Common.backColor1
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
						text: "OK"

						onClicked: {
							save()
						}
					}

					LabelButton {
						id: buttonCancel
						text: "Cancel"

						onClicked: {
							reject()
						}
					}
				}
			}
		}
	}

	function save()
	{
		if (reason === CardDlg.Reason.Show)
		{
			reject()
			return
		}

		var args = [ nameText.text, photoText.text, phoneText.text ]

		if (reason === CardDlg.Reason.New)
			database.appendContact(args)
		else if (reason === CardDlg.Reason.Edit)
			database.modifyContact(contactId, args)

		contactsModel.update()
		accept()
	}

	function load()
	{
		clear()
		if (contactId <= 0)
			return

		var args = database.contactCard(contactId)
		nameText.text = args[0]
		photoText.text = args[1]
		phoneText.text = args[2]
	}

	function clear()
	{
		nameText.text = ""
		photoText.text = ""
		phoneText.text = ""
	}
}
