import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs

Menu {
	id: menu
	topPadding: 3
	bottomPadding: 3

	property variant contactInfoDlg

	Component {
		id: contactInfoComponent
		ContactInfoDlg {}
	}

	Action {
		text: "My contact"
		onTriggered: {
			contactInfoDlg = contactInfoComponent.createObject(mainWindow)
			contactInfoDlg.self = true
			contactInfoDlg.contactMap = dispatcher.selfContactInfo()
			contactInfoDlg.open()
		}
	}

	Action {
		text: "Add new contact"
		onTriggered: {

		}
	}

	Action {
		text: "Settings"
		onTriggered: {
			contactInfoDlg.self = true
			contactInfoDlg.contactMap = dispatcher.selfContactInfo()
			contactInfoDlg.open()
		}
	}

	Action {
		text: "Quit"
		onTriggered: {
			Qt.quit()
		}
	}

	delegate: MenuItem {
		id: menuItem
		implicitWidth: 200
		implicitHeight: 40

		contentItem: Text {
			leftPadding: 10
			rightPadding: 10
			text: menuItem.text
			font.pointSize: 12
			opacity: enabled ? 1.0 : 0.3
			color: "#4D4D4D"
			horizontalAlignment: Text.AlignLeft
			verticalAlignment: Text.AlignVCenter
			elide: Text.ElideRight
		}

		background: Rectangle {
			implicitWidth: 200
			implicitHeight: 40
			opacity: enabled ? 1 : 0.3
			color: menuItem.highlighted ? "#E5E5E5" : "transparent"
		}
	}

	background: Rectangle {
		implicitWidth: 200
		implicitHeight: 40
		color: "#ffffff"
		border.width: 0
		radius: 8
	}
}
