import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Basic

Menu {
	id: menu
	topPadding: 3
	bottomPadding: 3

	Action {
		text: "My contact"
		onTriggered: {
			hideContactInfo()
			contactInfo.parent = contentRect
			contactInfo.self = true
			contactInfo.contactMap = dispatcher.selfContactInfo()
			showContactInfo();
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
