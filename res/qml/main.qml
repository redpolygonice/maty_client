import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import "theme.js" as Theme
import "common.js" as Common

ApplicationWindow {
	id: mainWindow
	width: 800
	height: 600
	visible: false
	title: "Maty"

	property variant regForm
	property variant loginForm
	property string connectImageSrc: ""

	Timer {
		id: timer
		interval: 100; running: false; repeat: false
		onTriggered: auth()
	}

	Component.onCompleted: {
		if (settings.params["autologin"])
		{
			visible = true
			timer.start()
		}
		else
			login()

		x = Screen.width / 2 - mainWindow.width / 2
		y = Screen.height / 2 - mainWindow.height / 2

		dispatcher.onSetSelfId.connect(function(id) {
			historyView.selfId = id
		})

		Common.connectState = Common.ConnectState.Connecting
		connectImageSrc = "qrc:///img/bullet_yellow.png"

		dispatcher.onConnectState.connect(function(state) {
			Common.connectState = state
			if (Common.connectState === Common.ConnectState.Connecting)
			{
				connectSate.text = "Connecting .."
				connectImageSrc = "qrc:///img/bullet_yellow.png"
				messageView.enabled = false
			}
			else if (Common.connectState === Common.ConnectState.Connected)
			{
				connectSate.text = "Connected"
				connectImageSrc = "qrc:///img/bullet_green.png"
				messageView.enabled = true
			}
			else if (Common.connectState === Common.ConnectState.NotConneted)
			{
				connectSate.text = "Not connected"
				connectImageSrc = "qrc:///img/bullet_red.png"
				messageView.enabled = false
			}
		})

		historyModel.update(-1)
		contactsModel.update()
	}

	onVisibleChanged: {
		if (visible)
			contactsView.setContactInfo()
	}

	Component {
		id: regComponent
		RegForm { }
	}

	Component {
		id: loginComponent
		LoginForm { }
	}

	ActionsMenu {
		id: actionsMenu
	}

	ConfirmDlg {
		id: messageDlg
		buttons: ConfirmDlg.Ok
	}

	SplitView {
		id: splitView
		anchors.fill: parent
		orientation: Qt.Horizontal

		handle: Rectangle {
			implicitWidth: 3
			color: Theme.backColor1
		}

		ContactsView {
			id: contactsView
			SplitView.fillWidth: false
			SplitView.fillHeight: true
			SplitView.minimumWidth: 250
		}

		Rectangle {
			id: contentRect
			SplitView.fillWidth: true
			SplitView.fillHeight: true
			SplitView.minimumWidth: 200

			Column {
				anchors.fill: parent

				HistoryView {
					id: historyView
					width: mainWindow.contentItem.width - contactsView.width
					height: contactsView.currentIndex === -1 ? mainWindow.contentItem.height :
															   mainWindow.contentItem.height - messageView.height
				}

				MessageView {
					id: messageView
					width: mainWindow.contentItem.width - contactsView.width
				}
			}
		}
	}

	footer: Rectangle {
			id: footer
			width: parent.width
			height: 30
			color: Theme.selectionColor

			Row {
				anchors.fill: parent
				spacing: 5

				Image {
					id: connectImage
					fillMode: Image.PreserveAspectFit
					sourceSize.width: 30
					sourceSize.height: 30
					smooth: true
					source: connectImageSrc
				}

				Text {
					id: connectSate
					height: footer.height
					text: "Connecting .."
					color: "#000000"
					font.pointSize: Theme.fontPointSize
					horizontalAlignment: Text.Left
					verticalAlignment: Text.AlignVCenter
				}
			}
		}

	function register()
	{
		regForm = regComponent.createObject(mainWindow)
		regForm.show()
	}

	function login()
	{
		loginForm = loginComponent.createObject(mainWindow)
		loginForm.show()
	}

	function auth()
	{
		if (settings.params["login"] === "" || settings.params["password"] === "")
		{
			messageDlg.messageText = "Login or password is empty!"
			messageDlg.open()
		}

		dispatcher.authContact({ "login": settings.params["login"], "password": settings.params["password"],
								   "autologin": true })

		dispatcher.onAuth.connect(function(code) {
			if (code === 0)
			{
				mainWindow.visible = true
			}
			else
			{
				messageDlg.messageText = "Not connected. Login or password is incorrect!"
				messageDlg.open()
			}
		})
	}
}
