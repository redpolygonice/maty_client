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

		Rectangle {
			Layout.fillWidth: true
			Layout.fillHeight: false
			Layout.minimumHeight: 40
			Layout.maximumHeight: 40
			Layout.margins: 0
			height: 40

			RowLayout {
				anchors.fill: parent
				Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
				Layout.margins: 0
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
						contactInfo.visible = false
					}
				}
			}
		}

		Rectangle {
			color: Theme.backColor4
			height: 80
			Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
			Layout.margins: 0
			Layout.fillWidth: true
			Layout.fillHeight: false

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

		GridLayout {
			columns: 2
			rows: 3
			Layout.topMargin: 10
			Layout.leftMargin: 15
			Layout.rightMargin: 15
			Layout.alignment: Qt.AlignLeft | Qt.AlignTop
			Layout.fillWidth: true

			Text {
				text: "Name:"
				color: Theme.textColor
				font.pointSize: Theme.fontPointSize
				font.bold: true
				Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
				Layout.margins: 0
			}

			TextField {
				id: textName
				readOnly: self === false
				text: ""
				color: Theme.textColor
				font.pointSize: Theme.fontPointSize
				implicitHeight: 30
				implicitWidth: 250
				Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
				Layout.margins: 0
				background: Rectangle {
					color: Theme.backColor4
				}
			}

			Text {
				text: "Login:"
				color: Theme.textColor
				font.pointSize: Theme.fontPointSize
				font.bold: true
				Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
				Layout.margins: 0
			}

			TextField {
				id: textLogin
				readOnly: true
				text: ""
				color: Theme.textColor
				font.pointSize: Theme.fontPointSize
				implicitHeight: 30
				implicitWidth: 250
				Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
				Layout.margins: 0
				background: Rectangle {
					color: Theme.backColor4
				}
			}

			Text {
				text: "Phone:"
				color: Theme.textColor
				font.pointSize: Theme.fontPointSize
				font.bold: true
				Layout.margins: 0
			}

			TextField {
				id: textPhone
				readOnly: self === false
				text: ""
				color: Theme.textColor
				font.pointSize: Theme.fontPointSize
				implicitHeight: 30
				implicitWidth: 250
				Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
				Layout.margins: 0
				background: Rectangle {
					color: Theme.backColor4
				}
			}
		}

		Text {
			text: "About:"
			color: Theme.textColor
			font.pointSize: Theme.fontPointSize
			font.bold: true
			Layout.topMargin: 10
			Layout.leftMargin: 15
			Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
		}

		TextArea {
			id: textAbout
			readOnly: self === false
			text: ""
			placeholderText: "About"
			Layout.fillWidth: true
			Layout.minimumHeight: 150
			Layout.topMargin: 10
			Layout.leftMargin: 15
			Layout.rightMargin: 15
			Layout.bottomMargin: 15
			Layout.alignment: Qt.AlignLeft | Qt.AlignTop
			wrapMode: Text.Wrap
			font.pointSize: Theme.fontPointSize
			color: Theme.textColor
			background: Rectangle {
				color: Theme.backColor4
			}
		}

		Rectangle {
			Layout.fillHeight: true
		}

		Rectangle {
			id: bottomRect
			Layout.fillWidth: true
			Layout.minimumHeight: 50
			Layout.alignment: Qt.AlignRight | Qt.AlignBottom

			RowLayout {
				anchors.fill: parent
				Layout.margins: 0
				spacing: 10

				LabelButton {
					id: buttonSave
					visible: self === true
					backcolor: "#eeeeee"
					backcolorhover: "#dddddd"
					beam: 100
					text: "Save"
					Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
					Layout.rightMargin: 10
					Layout.bottomMargin: 10

					onClicked: {
						save()
					}
				}
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

		var imageName = contactMap["image"]
		if (imageName === null || imageName === 'empty' ||
				imageName === undefined || imageName.length === 0)
			contactMiniImage.imgsource =  "qrc:///img/user.png"
		else
		{
			if (self)
				contactMiniImage.imgsource = "file:///" + settings.imagePath() + "/" + imageName
			else if (contactsView.currentModel === searchModel)
				contactMiniImage.imgsource = "file:///" + settings.tempPath() + "/" + imageName
			else
				contactMiniImage.imgsource = "file:///" + settings.imagePath() + "/" + imageName
		}

		textName.text = contactMap["name"]
		textLogin.text = contactMap["login"]
		textPhone.text = contactMap["phone"]
		textAbout.text = contactMap["about"]
	}

	function save()
	{
		contactMap["name"] = textName.text
		contactMap["login"] = textLogin.text
		contactMap["phone"] = textPhone.text
		contactMap["about"] = textAbout.text
		dispatcher.modifySelf(contactMap)
	}
}
