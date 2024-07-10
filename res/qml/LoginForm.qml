import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Basic
import QtQuick.Window
import "common.js" as Common

Window {
	id: loginWindow
	visible: false
	width: 300
	height: 310
	x: Screen.width / 2 - width / 2
	y: Screen.height / 2 - height / 2
	modality: Qt.ApplicationModal
	flags: Qt.Dialog | Qt.CustomizeWindowHint | Qt.WindowTitleHint | Qt.WindowCloseButtonHint

	onClosing: (close) => {
		Qt.quit()
	}

	Component.onCompleted: {
		load()
	}

	ColumnLayout {
		anchors.fill: parent
		anchors.margins: 0
		spacing: 7

		Label {
			text: "Login form"
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

			CheckBox {
				id: autologinCheck
				text: "Autologin"
				checked: false
				contentItem: Text {
					text: autologinCheck.text
					color: enabled ? Common.textColor : "grey"
					font.pointSize: Common.fontPointSize
					verticalAlignment: Text.AlignVCenter
					leftPadding: 40
				}
			}

			Label {
				Layout.topMargin: 10
				text: "Not registered?"
				font.pointSize: Common.fontPointSize
				color: "blue"

				MouseArea {
					anchors.fill: parent
					hoverEnabled: true
					cursorShape: Qt.PointingHandCursor
					onClicked: {
						mainWindow.register()
						loginWindow.destroy()
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
							save()
							mainWindow.visible = true
							loginWindow.destroy()
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
		loginText.text = ""
		passwordText.text = ""
	}

	function load()
	{
		if (settings.params["login"] !== undefined)
			loginText.text = settings.params["login"];
		if (settings.params["password"] !== undefined)
			passwordText.text = settings.params["password"];
	}

	function save()
	{
		settings.saveLoginData({ "autologin": autologinCheck.checked, "login": loginText.text,
								   "password": passwordText.text });
		settings.save()
	}
}
