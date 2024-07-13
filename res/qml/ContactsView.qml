import QtQuick
import QtQuick.Controls.Basic
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
	property string contactImageSrc: ""
	property string contactNameText: ""

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
					id: imageId
					fillMode: Image.PreserveAspectFit
					sourceSize.width: 36
					sourceSize.height: 36
					smooth: true
					source: {
						if (image === 'empty' || image.length === 0 ||
								image === null || image === undefined)
							return "qrc:///img/user.png"
						else
							return "file:///" + settings.imagePath() + "/" + image
					}
				}

				Text {
					id: textName
					text: name
					font.pointSize: 13
					color: Common.textColor
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
		model: searchModel
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

		Component.onCompleted: {
			listView.currentIndex = -1
			dispatcher.onSearched.connect(function(result) {
				if (result === 0)
				{
					listView.model = searchModel;
					listView.update()
				}
				else
				{
					listView.model = contactsModel;
					listView.update()
				}
			})
		}

		header: ToolBar {
			id: toolbar
			width: parent.width
			height: 125

			background: Rectangle {
				color: Common.backColor1
			}

			Rectangle {
				id: contactRect
				anchors.fill: parent
				color: Common.backColor2
				anchors.leftMargin: 3
				anchors.topMargin: 7

				ColumnLayout {
					anchors.fill: parent
					spacing: 0

					RowLayout {
						spacing: 0
						Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter

						Rectangle {
							id: imageRect
							color: Common.backColor1
							border.width: 1
							border.color: "black"
							Layout.leftMargin: 10
							width: 65
							height: 65

							Image {
								id: contactImage
								fillMode: Image.PreserveAspectFit
								sourceSize.width: 64
								sourceSize.height: 64
								sourceClipRect: Qt.rect(0, 0, 64, 64)
								smooth: true
								source: contactImageSrc

								MouseArea {
									id: imageArea
									anchors.fill: parent
									acceptedButtons: Qt.LeftButton | Qt.RightButton
									hoverEnabled: true
									cursorShape: Qt.PointingHandCursor
									onClicked: (mouse) => {
											   }
								}
							}
						}

						TextArea {
							id: contactText
							text: contactNameText
							readOnly: true
							Layout.fillWidth: true
							width: 110
							wrapMode: Text.Wrap
							font.pointSize: 11
							color: Common.textColor
						}

						PanelButton {
							id: actionButton
							Layout.rightMargin: 10
							text: "<b>....</b>"
							onClicked: {
								actionsMenu.popup()
							}
						}
					}

					RowLayout {
						spacing: 0

						TextField {
							id: searchText
							text: "ale"
							placeholderText: "Search .."
							Layout.leftMargin: 11
							Layout.bottomMargin: 5
							Layout.fillWidth: true
							Layout.alignment: Qt.AlignLeft | Qt.AlignTop
							font.pointSize: Common.fontPointSize
							implicitHeight: 30
							color: Common.textColor
							background: Rectangle {
								color: Common.backColor1
							}

							Keys.onPressed: (event)=> {
								if (event.key === Qt.Key_Enter || event.key === Qt.Key_Return)
								{
									if (searchText.text !== "")
										dispatcher.searchContact(searchText.text)
								}
							}
						}

						PanelButton {
							id: searchCoseButton
							Layout.rightMargin: 10
							Layout.alignment: Qt.AlignLeft | Qt.AlignTop
							text: "<b>X</b>"
							onClicked: {
								searchText.text = ""
							}
						}
					}
				}
			}
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

	function setContactInfo()
	{
		if (settings.params["image"] === undefined || settings.params["image"] === "")
			contactImageSrc = ""
		else
			contactImageSrc = "file:///" + settings.imagePath() + "/" + settings.params["image"]

		if (settings.params["name"] === undefined || settings.params["name"] === "")
			contactNameText = ""
		else
			contactNameText = settings.params["name"]
	}
}
