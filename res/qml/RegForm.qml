import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Basic
import QtQuick.Window
import Qt.labs.platform
import "theme.js" as Theme

Window {
	id: regWindow
	visible: false
	width: 300
	height: 425
	x: Screen.width / 2 - regWindow.width / 2
	y: Screen.height / 2 - regWindow.height / 2
	modality: Qt.ApplicationModal
	flags: Qt.Dialog | Qt.CustomizeWindowHint | Qt.WindowTitleHint | Qt.WindowCloseButtonHint

	property string imageFile: ""

	onClosing: {
		Qt.quit()
	}

	Component.onCompleted: {
		dispatcher.onConnectState.connect(function(status) {
			if (status === 2)
			{
				messageBox.messageText = "Not connected to server. Try again later!!"
				messageBox.open()
			}
		})
	}

	ColumnLayout {
		anchors.fill: parent
		anchors.margins: 0
		spacing: 10

		Label {
			text: "Register to the Maty messaging!"
			font.pointSize: 13
			font.bold: true
			color: Theme.textColor
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
				font.pointSize: Theme.fontPointSize
			}

			TextField {
				id: nameText
				text: ""
				Layout.fillWidth: true
				font.pointSize: Theme.fontPointSize
				implicitHeight: 30
				color: Theme.textColor
				background: Rectangle {
					color: Theme.backColor1
				}
			}

			Text {
				text: "Login"
				font.pointSize: Theme.fontPointSize
			}

			TextField {
				id: loginText
				text: ""
				Layout.fillWidth: true
				font.pointSize: Theme.fontPointSize
				implicitHeight: 30
				color: Theme.textColor
				background: Rectangle {
					color: Theme.backColor1
				}
			}

			Text {
				text: "Password"
				font.pointSize: Theme.fontPointSize
			}

			TextField {
				id: passwordText
				text: ""
				Layout.fillWidth: true
				font.pointSize: Theme.fontPointSize
				implicitHeight: 30
				color: Theme.textColor
				echoMode: TextInput.Password
				background: Rectangle {
					color: Theme.backColor1
				}
			}

			Text {
				text: "Phone"
				font.pointSize: Theme.fontPointSize
			}

			TextField {
				id: phoneText
				text: ""
				Layout.fillWidth: true
				font.pointSize: Theme.fontPointSize
				implicitHeight: 30
				color: Theme.textColor
				echoMode: TextInput.Password
				background: Rectangle {
					color: Theme.backColor1
				}
			}

			RowLayout {

				Rectangle {
					id: imageRect
					Layout.topMargin: 10
					color: Theme.backColor1
					border.width: 2
					border.color: "black"
					width: 64
					height: 64

					Image {
						id: image
						fillMode: Image.PreserveAspectFit
						sourceSize.width: 64
						sourceSize.height: 64
						smooth: true
						source: ""
					}
				}

				Label {
					Layout.leftMargin: 15
					text: "Load image from file"
					font.pointSize: Theme.fontPointSize
					color: "steelblue"

					MouseArea {
						anchors.fill: parent
						hoverEnabled: true
						cursorShape: Qt.PointingHandCursor
						onClicked: {
							loadImage()
						}
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
						id: buttonRegister
						backcolor: "#eeeeee"
						backcolorhover: "#dddddd"
						beam: 100
						text: "Register"

						onClicked: {
							register()
						}
					}

					LabelButton {
						id: buttonCancel
						backcolor: "#eeeeee"
						backcolorhover: "#dddddd"
						text: "Cancel"

						onClicked: {
							Qt.quit()
						}
					}
				}
			}
		}
	}

	function clear()
	{
		nameText.text = ""
		loginText.text = ""
		passwordText.text = ""
		image.source = ""
	}

	function register()
	{
		if (nameText.text === "" || loginText.text === "" || passwordText.text === "")
		{
			messageBox.messageText = "You must fill all fields!"
			messageBox.open()
			return
		}

		dispatcher.regContact({ "name": nameText.text, "login": loginText.text,
								  "password": passwordText.text, "image": imageFile, "phone": phoneText.text });

		dispatcher.onReg.connect(function(code) {
			if (code === 0)
			{
				save()
				mainWindow.login()
				regWindow.destroy()
			}
		})
	}

	function save()
	{
		settings.saveAuthData({ "autologin": false, "name": nameText.text, "login": loginText.text ,
								  "phone": phoneText.text});
		settings.save()
	}

	function loadImage()
	{
		fileLoadDialog.open()
	}

	FileDialog {
		id: fileLoadDialog
		title: "Open image file"
		fileMode: FileDialog.OpenFile
		nameFilters: [ "Image files (*.png *.jpg *.jpeg", "All files (*.*)" ]
		selectedNameFilter.index: 0
		folder: {
			return StandardPaths.writableLocation(StandardPaths.HomeLocation)
		}

		onAccepted: {
			image.source = file;
			imageFile = file.toString().replace("file://", "")
		}
	}

	ConfirmDlg {
		id: messageBox
		buttons: ConfirmDlg.Ok
	}
}
