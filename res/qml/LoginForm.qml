import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Basic
import QtQuick.Window
import "theme.js" as Theme

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

		dispatcher.onConnectState.connect(function(status) {
			if (status === 2)
			{
				if (messageBox === null || messageBox === undefined)
					return;

				messageBox.messageText = "Not connected to server. Try again later!!"
				messageBox.open()
			}
		})
	}

	ColumnLayout {
		anchors.fill: parent
		anchors.margins: 0
		spacing: 7

		Label {
			text: "Login form"
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

				Keys.onPressed: (event) => {
					onKeyPressed(event.key)
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

				Keys.onPressed: (event) => {
					onKeyPressed(event.key)
				}
			}

			CheckBox {
				id: autologinCheck
				text: "Autologin"
				checked: false
				contentItem: Text {
					text: autologinCheck.text
					color: enabled ? Theme.textColor : "grey"
					font.pointSize: Theme.fontPointSize
					verticalAlignment: Text.AlignVCenter
					leftPadding: 40
				}
			}

			Label {
				Layout.topMargin: 10
				text: "Not registered?"
				font.pointSize: Theme.fontPointSize
				color: "steelblue"

				MouseArea {
					anchors.fill: parent
					hoverEnabled: true
					cursorShape: Qt.PointingHandCursor
					onClicked: {
						loginWindow.destroy()
						mainWindow.register()
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
							auth()
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
	}

	function save()
	{
		if (autologinCheck.checked)
		{
			settings.saveAuthData({ "autologin": autologinCheck.checked, "login": loginText.text,
									   "password": passwordText.text });
		}
		else
		{
			settings.saveAuthData({ "autologin": autologinCheck.checked, "login": loginText.text});
		}

		settings.save()
	}

	function auth()
	{
		if (loginText.text === "" || passwordText.text === "")
		{
			messageBox.messageText = "You must fill login and password!"
			messageBox.open()
			return
		}

		dispatcher.authContact({ "login": loginText.text, "password": passwordText.text, "autologin": false });

		dispatcher.onAuth.connect(function(code) {
			if (code === 0)
			{
				save();
				mainWindow.visible = true
				contactsModel.update()
				loginWindow.destroy()
			}
			else
			{
				messageBox.messageText = "Login or password is incorrect!"
				messageBox.open()
			}
		})
	}

	function onKeyPressed(key)
	{
		if (key === Qt.Key_Enter || key === Qt.Key_Return)
			auth()
		else if (key === Qt.Key_Escape)
			Qt.quit()
	}

	ConfirmDlg {
		id: messageBox
		buttons: ConfirmDlg.Ok
	}
}
