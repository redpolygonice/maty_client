import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import "common.js" as Common

Rectangle {
	id: mainRect
	color: Common.backColor1

	enum ConfirmDlgSender {
		None,
		RemoveContact,
		ClearHistory
	}

	property int currentId: -1
	property alias currentIndex: listView.currentIndex

	Component {
		id: delegate

		Rectangle {
			width: mainRect.width
			height: id > 1 ? 50 : 0
			visible: id > 1
			color: mouseArea.containsMouse ? Common.contHighlightColor : "transparent"

			RowLayout {
				anchors.fill: parent
				spacing: 10
				Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter

				Image {
					id: image
					fillMode: Image.PreserveAspectFit
					sourceSize.width: 36
					sourceSize.height: 36
					smooth: true
					source: {
						if (imgname === 'empty' || imgname.length === 0 ||
								imgname === null || imgname === undefined)
							return "qrc:///img/user.png"
						else
							return "file:///" + settings.imagePath() + "/" + imgname
					}
				}

				Text {
					id: textName
					text: name
					font.pointSize: 13
					color: "#4D4D4D"
					elide: Text.ElideRight
					verticalAlignment: Text.AlignVCenter
					horizontalAlignment: Text.AlignLeft
					Layout.fillWidth: true
				}
			}

			MouseArea {
				id: mouseArea
				anchors.fill: parent
				propagateComposedEvents: true
				acceptedButtons: Qt.LeftButton | Qt.RightButton
				hoverEnabled: true
				cursorShape: Qt.PointingHandCursor

				onClicked: (mouse) => {
					currentId = id
					listView.currentIndex = index
					 mouse.accepted = false

					if (mouse.button === Qt.RightButton)
					{
					contextMenu.popup()
					}
				}
			}
		}
	}

	ListView {
		id: listView
		anchors.fill: parent
		model: contactsModel
		delegate: delegate
		focus: true

		highlight: Rectangle {
			color: Common.contHighlightColor
		}

		highlightFollowsCurrentItem: true
		highlightMoveDuration: 100
		highlightMoveVelocity: -1

		ScrollBar.vertical: ScrollBar {
			policy: ScrollBar.AsNeeded
		}

		header: ToolBar {
			id: toolbar
			width: parent.width
			height: 50

			background: Rectangle {
				color: Common.backColor1
			}

			RowLayout {
				spacing: 10
				anchors.fill: parent
				anchors.leftMargin: 5
				anchors.bottomMargin: 10
				Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter

				PanelButton {
					id: actionButton
					text: "<b>....</b>"
					ToolTip.text: "Actions .."
					onClicked: {
						actionsMenu.popup()
					}
				}
			}
		}

		Component.onCompleted: {
			listView.currentIndex = -1
		}

		onCountChanged: {
			positionViewAtIndex(count - 1, ListView.Beginning)
		}

		onCurrentIndexChanged: {
			historyModel.update(currentId)
		}
	}

	Menu {
		id: contextMenu
		topPadding: 3
		bottomPadding: 3

		Action {
			text: "Card"
			onTriggered: {
				cardDlg.contactId = currentId
				cardDlg.reason = CardDlg.Reason.Edit
				cardDlg.open()
			}
		}

		Action {
			text: "Remove"
			onTriggered: {
				removeDlg.sender = ContactsView.ConfirmDlgSender.RemoveContact
				removeDlg.messageText = "Remove current contact ?"
				removeDlg.open()
			}
		}

		Action {
			text: "Clear history"
			onTriggered: {
				removeDlg.sender = ContactsView.ConfirmDlgSender.ClearHistory
				removeDlg.messageText = "Clear contact history ?"
				removeDlg.open()
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

	ConfirmDlg {
		id: removeDlg

		property int sender: ContactsView.ConfirmDlgSender.None

		onOkClicked: {
			if (sender === ContactsView.ConfirmDlgSender.RemoveContact)
			{
				removeContact()
			}
			else if (sender === ContactsView.ConfirmDlgSender.ClearHistory)
			{
				clearHistory()
			}
		}
	}

	function removeContact()
	{
		database.removeContact(currentId)
		database.clearHistory(currentId)
		contactsModel.update()
		historyModel.update(0)
		listView.currentIndex = -1
	}

	function clearHistory()
	{
		database.clearHistory(currentId)
		historyModel.update(contactsView.currentId)
	}
}
