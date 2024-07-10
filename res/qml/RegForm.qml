import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Basic
import QtQuick.Window
import "common.js" as Common

Window {
	id: regWindow
	visible: false
	width: 300
	height: 300
	x: Screen.width / 2 - regWindow.width / 2
	y: Screen.height / 2 - regWindow.height / 2
	modality: Qt.ApplicationModal
	flags: Qt.Dialog | Qt.CustomizeWindowHint | Qt.WindowTitleHint | Qt.WindowCloseButtonHint

	onClosing: (close) => {
		Qt.quit()
	}

	ColumnLayout {
		anchors.fill: parent
		anchors.margins: 0
		spacing: 10

		Label {
			text: "Register to the Maty messaging!"
			font.pointSize: 13
			font.bold: true
			color: Common.textColor
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
				Layout.fillWidth: true
				font.pointSize: Common.fontPointSize
				implicitHeight: 30
				color: Common.textColor
				background: Rectangle {
					color: Common.backColor1
				}
			}

			Text {
				text: "Login"
				font.pointSize: Common.fontPointSize
			}

			TextField {
				id: loginText
				text: ""
				Layout.fillWidth: true
				font.pointSize: Common.fontPointSize
				implicitHeight: 30
				color: Common.textColor
				background: Rectangle {
					color: Common.backColor1
				}
			}

			Text {
				text: "Password"
				font.pointSize: Common.fontPointSize
			}

			TextField {
				id: passwordText
				text: ""
				Layout.fillWidth: true
				font.pointSize: Common.fontPointSize
				implicitHeight: 30
				color: Common.textColor
				echoMode: TextInput.Password
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
	}

	function register()
	{
		dispatcher.regContact({ "name": nameText.text, "login": loginText.text,
								  "password": passwordText.text });

		dispatcher.onRegistration.connect(function(code) {
			if (code === 0)
			{
				mainWindow.login()
				regWindow.destroy()
			}
		})
	}

	function save()
	{
		settings.saveAuthData({ "autologin": false, "name": nameText.text,
								   "login": loginText.text, "password": passwordText.text });
		settings.save()
	}
}
