import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Basic
import "theme.js" as Theme

Rectangle {
	id: contactInfo
	visible: false
	anchors.fill: parent

	property variant contactMap: ({})
	property bool self: false

	onVisibleChanged: {
		if (visible)
			load()
		else
			self = false
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
					contactInfo.visible = false
				}
			}
		}

		Rectangle {
			id: topRect
			color: Theme.backColor4
			width: contactInfo.width
			height: 80
			Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
			Layout.margins: 0
			Layout.fillWidth: true

			RowLayout {
				anchors.fill: parent
				Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
				Layout.margins: 0

				RoundedImage {
					id: contactMiniImage
					Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
					Layout.margins: 0
					srcwidth: 72
					srcheight: 72
					imgsource: ""

					MouseArea {
						id: imageArea
						anchors.fill: parent
						acceptedButtons: Qt.LeftButton
						hoverEnabled: true
						cursorShape: Qt.PointingHandCursor
						onClicked: {
							hideContactImage()
							contactImage.parent = contentRect
							contactImage.contactImageSrc = contactMiniImage.imgsource
							contactImage.contactName = textName.text
							showContactImage()
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
	}

	function clear()
	{
		contactMiniImage.imgsource = ""
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
			contactMiniImage.imgsource =  "qrc:///img/user.png"
		else
		{
			if (contactsView.currentModel === searchModel)
				contactMiniImage.imgsource = "file:///" + settings.tempPath() + "/" + image
			else
				contactMiniImage.imgsource = "file:///" + settings.imagePath() + "/" + image
		}

		textName.text = contactMap["name"]
		textLogin.text = contactMap["login"]
		textPhone.text = contactMap["phone"]
	}

	function save()
	{
	}
}
