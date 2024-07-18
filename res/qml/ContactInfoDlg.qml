import QtQuick
import QtQuick.Dialogs
import QtQuick.Layouts
import QtQuick.Controls.Basic
import QtQuick.Window
import "theme.js" as Theme

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
			color: Theme.backColor4
			width: contactDlg.width
			height: 80
			Layout.alignment: Qt.AlignHCenter | Qt.AlignTop

			RowLayout {
				anchors.fill: parent
				Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

				RoundedImage {
					id: contactImage
					Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
					srcwidth: 70
					srcheight: 70
					imgsource: ""

					MouseArea {
						id: imageArea
						anchors.fill: parent
						acceptedButtons: Qt.LeftButton
						hoverEnabled: true
						cursorShape: Qt.PointingHandCursor
						onClicked: {
							imageDlg.contactImageSrc = contactImage.imgsource
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
				color: Theme.textColor
				font.pointSize: Theme.fontPointSize
				font.bold: true
			}

			Text {
				id: textName
				text: ""
				color: Theme.textColor
				font.pointSize: Theme.fontPointSize
			}

			Text {
				text: "Login:"
				color: Theme.textColor
				font.pointSize: Theme.fontPointSize
				font.bold: true
			}

			Text {
				id: textLogin
				text: ""
				color: Theme.textColor
				font.pointSize: Theme.fontPointSize
			}

			Text {
				text: "Phone:"
				color: Theme.textColor
				font.pointSize: Theme.fontPointSize
				font.bold: true
			}

			Text {
				id: textPhone
				text: ""
				color: Theme.textColor
				font.pointSize: Theme.fontPointSize
			}
		}

		Text {
			text: "About:"
			color: Theme.textColor
			font.pointSize: Theme.fontPointSize
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
			font.pointSize: Theme.fontPointSize
			color: Theme.textColor
			background: Rectangle {
				color: Theme.backColor4
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
		contactImage.imgsource = ""
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
			contactImage.imgsource =  "qrc:///img/user.png"
		else
		{
			if (contactsView.currentModel === searchModel)
				contactImage.imgsource = "file:///" + settings.tempPath() + "/" + image
			else
				contactImage.imgsource = "file:///" + settings.imagePath() + "/" + image
		}

		textName.text = contactMap["name"]
		textLogin.text = contactMap["login"]
		textPhone.text = contactMap["phone"]
	}

	function save()
	{
	}
}
