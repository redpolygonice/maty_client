import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import "common.js" as Common

ApplicationWindow {
	id: mainWindow
	width: 800
	height: 600
	visible: false
	title: "Maty"

	property variant regForm
	property variant loginForm

	signal newText(int id, string text)

	Component.onCompleted: {
		if (!settings.params["autologin"])
			login()
		else
			visible = true

		x = Screen.width / 2 - mainWindow.width / 2
		y = Screen.height / 2 - mainWindow.height / 2
		historyModel.update(-1)
		contactsModel.update()
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

	CardDlg {
		id: cardDlg
	}

	SplitView {
		id: splitView
		anchors.fill: parent
		orientation: Qt.Horizontal

		handle: Rectangle {
			implicitWidth: 3
			color: Common.backColor1
		}

		ContactsView {
			id: contactsView
			SplitView.fillWidth: false
			SplitView.fillHeight: true
			SplitView.minimumWidth: 200
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
}
