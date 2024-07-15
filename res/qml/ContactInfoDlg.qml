import QtQuick
import QtQuick.Dialogs
import QtQuick.Layouts
import QtQuick.Controls.Basic
import QtQuick.Window
import "common.js" as Common

Dialog {
	id: contactDlg
	visible: false
	width: 350
	height: 400
	x: mainWindow.width / 2 - width / 2
	y: mainWindow.height / 2 - height / 2
	modal: true
	padding: 0
	margins: 0

	property variant contactMap: ({})
	property bool self: false

	background: Rectangle {
		color: "#FFFFFF"
		radius: 15
	}

	onVisibleChanged: {
		if (visible)
			load()
		else
			self = false
	}

	ImageDlg {
		id: imageDlg
	}

	ColumnLayout {
		anchors.fill: parent
		spacing: 0
		Layout.margins: 0

		RowLayout {
			Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
			Layout.maximumHeight: 40
			Layout.minimumHeight: 40
			spacing: 10

			PanelButton {
				Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
				text: "<b>....</b>"
				backcolor: "#ffffff"
				onClicked: {
				}
			}

			PanelButton {
				Layout.rightMargin: 10
				Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
				text: "<b>X</b>"
				backcolor: "#ffffff"
				onClicked: {
					reject();
				}
			}
		}

		Rectangle {
			id: topRect
			color: Common.backColor4
			width: contactDlg.width
			height: 80
			Layout.alignment: Qt.AlignHCenter | Qt.AlignTop

			RowLayout {
				anchors.fill: parent
				Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

				Image {
					id: contactImage
					fillMode: Image.PreserveAspectFit
					Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
					sourceSize.width: 64
					sourceSize.height: 64
					sourceClipRect: Qt.rect(0, 0, 64, 64)
					width: 64
					height: 64
					smooth: true
					source: ""

					MouseArea {
						id: imageArea
						anchors.fill: parent
						acceptedButtons: Qt.LeftButton | Qt.RightButton
						hoverEnabled: true
						cursorShape: Qt.PointingHandCursor
						onClicked: {
							imageDlg.contactImageSrc = contactImage.source
							imageDlg.showNormal()
						}
					}
				}
			}
		}

		Grid {
			columns: 2
			rows: 3
			spacing: 10
			Layout.maximumHeight: 80
			Layout.minimumHeight: 80
			Layout.topMargin: 10
			Layout.leftMargin: 15
			Layout.alignment: Qt.AlignLeft | Qt.AlignTop

			Text {
				text: "Name:"
				color: Common.textColor
				font.pointSize: Common.fontPointSize
				font.bold: true
			}

			Text {
				id: textName
				text: ""
				color: Common.textColor
				font.pointSize: Common.fontPointSize
			}

			Text {
				text: "Login:"
				color: Common.textColor
				font.pointSize: Common.fontPointSize
				font.bold: true
			}

			Text {
				id: textLogin
				text: ""
				color: Common.textColor
				font.pointSize: Common.fontPointSize
			}

			Text {
				text: "Phone:"
				color: Common.textColor
				font.pointSize: Common.fontPointSize
				font.bold: true
			}

			Text {
				id: textPhone
				text: ""
				color: Common.textColor
				font.pointSize: Common.fontPointSize
			}
		}

		Text {
			text: "About:"
			color: Common.textColor
			font.pointSize: Common.fontPointSize
			font.bold: true
			Layout.leftMargin: 15
			Layout.alignment: Qt.AlignLeft | Qt.AlignTop
		}

		TextArea {
			id: textAbout
			text: ""
			placeholderText: "About"
			readOnly: self == false
			Layout.preferredHeight: 80
			Layout.fillWidth: true
			Layout.leftMargin: 15
			Layout.rightMargin: 15
			Layout.alignment: Qt.AlignLeft | Qt.AlignTop
			wrapMode: Text.Wrap
			font.pointSize: Common.fontPointSize
			color: Common.textColor
			background: Rectangle {
				color: Common.backColor4
			}
		}

		RowLayout {
			Layout.fillHeight: true
			Layout.alignment: Qt.AlignTop
			Layout.bottomMargin: 5
			spacing: 10

			Label {
				Layout.fillWidth: true
			}

			LabelButton {
				id: buttonSave
				visible: self == true
				backcolor: "#eeeeee"
				backcolorhover: "#dddddd"
				text: "Save"

				onClicked: {
					save()
					accept()
				}
			}

			LabelButton {
				id: buttonClose
				backcolor: "#eeeeee"
				backcolorhover: "#dddddd"
				Layout.rightMargin: 10
				text: "Close"

				onClicked: {
					accept()
				}
			}
		}
	}

	function clear()
	{
		contactImage.source = ""
		textName.text = ""
		textLogin.text = ""
		textPhone.text = ""
		textAbout.text = ""
	}

	function load()
	{
		clear()
		if (contactMap === null)
			return

		var image = contactMap["image"]
		if (image === null || image === 'empty' ||
				image.length === 0 || image === undefined)
			contactImage.source =  "qrc:///img/user.png"
		else
		{
			if (contactsView.currentModel === searchModel)
				contactImage.source = "file:///" + settings.tempPath() + "/" + image
			else
				contactImage.source = "file:///" + settings.imagePath() + "/" + image
		}

		textName.text = contactMap["name"]
		textLogin.text = contactMap["login"]
		textPhone.text = contactMap["phone"]
	}

	function save()
	{
	}
}