import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import "theme.js" as Theme

Rectangle {
	id: mainRect
	color: Theme.backColor1

	enum Reason {
		None,
		RemoveContact,
		ClearHistory
	}

	property int currentId: -1
	property alias currentIndex: listView.currentIndex
	property string contactImageSrc: ""
	property string contactNameText: ""
	property variant currentModel: contactsModel

	ContactInfoDlg {
		id: contactInfoDlg
	}

	Component {
		id: delegate

		Rectangle {
			width: mainRect.width
			height: 50
			visible: true
			color: mouseArea.containsMouse ? Theme.contactsHighlightColor : "transparent"

			RowLayout {
				anchors.fill: parent
				spacing: 10
				Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter

				RoundedImage {
					id: imageName
					Layout.leftMargin: 10
					srcwidth: 44
					srcheight: 44
					imgsource: {
						if (image === 'empty' || image.length === 0 ||
								image === null || image === undefined)
							return "qrc:///img/user.png"
						else
						{
							if (currentModel === searchModel)
								return "file:///" + settings.tempPath() + "/" + image
							else
								return "file:///" + settings.imagePath() + "/" + image
						}
					}
				}

				Text {
					id: textName
					text: name
					font.pointSize: 13
					color: Theme.textColor
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
							   historyView.positionAtEnd()
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
		model: currentModel
		delegate: delegate
		focus: true

		highlight: Rectangle {
			color: Theme.contactsHighlightColor
		}

		highlightFollowsCurrentItem: true
		highlightMoveDuration: 100
		highlightMoveVelocity: -1

		ScrollBar.vertical: ScrollBar {
			policy: ScrollBar.AsNeeded
		}

		Component.onCompleted: {
			listView.currentIndex = -1
			dispatcher.onSearchResult.connect(function(result) {
				if (result === 0)
					currentModel = searchModel;
				else
					currentModel = contactsModel;
			})
		}

		header: ToolBar {
			id: toolbar
			width: parent.width
			height: 125

			background: Rectangle {
				color: Theme.backColor1
			}

			Rectangle {
				id: contactRect
				anchors.fill: parent
				color: Theme.backColor2
				anchors.leftMargin: 3
				anchors.topMargin: 7

				ColumnLayout {
					anchors.fill: parent
					spacing: 0

					RowLayout {
						spacing: 0
						Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter

						RoundedImage {
							id: contactImage
							imgsource: contactImageSrc
							Layout.leftMargin: 10
							Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter

							MouseArea {
								id: imageArea
								anchors.fill: parent
								acceptedButtons: Qt.LeftButton
								hoverEnabled: true
								cursorShape: Qt.PointingHandCursor
								onClicked: {
									contactInfoDlg.self = true
									contactInfoDlg.contactMap = dispatcher.selfContactInfo()
									contactInfoDlg.open()
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
							color: Theme.textColor
						}

						PanelButton {
							id: actionButton
							Layout.rightMargin: 10
							backcolor: Theme.backColor2
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
							text: ""
							placeholderText: "Search .."
							Layout.leftMargin: 11
							Layout.bottomMargin: 5
							Layout.fillWidth: true
							Layout.alignment: Qt.AlignLeft | Qt.AlignTop
							font.pointSize: Theme.fontPointSize
							implicitHeight: 30
							color: Theme.textColor
							background: Rectangle {
								color: Theme.backColor1
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
								contactsModel.update()
								currentModel = contactsModel
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
			text: "Contact info"
			onTriggered: {
				contactInfoDlg.contactMap = currentModel.card(currentIndex)
				contactInfoDlg.open()
			}
		}

		Action {
			text: "Add contact"
			enabled: currentModel === searchModel
			onTriggered: {
				dispatcher.addSearchContact(currentModel.card(currentIndex))
			}
		}

		Action {
			text: "Remove contact"
			onTriggered: {
				removeDlg.sender = ContactsView.RemoveContact
				removeDlg.messageText = "Remove current contact ?"
				removeDlg.open()
			}
		}

		Action {
			text: "Clear history"
			onTriggered: {
				removeDlg.sender = ContactsView.ClearHistory
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
				color: Theme.textColor
				horizontalAlignment: Text.AlignLeft
				verticalAlignment: Text.AlignVCenter
				elide: Text.ElideRight


			}

			background: Rectangle {
				implicitWidth: 200
				implicitHeight: 40
				opacity: enabled ? 1 : 0.3
				color: menuItem.highlighted ? Theme.menuHighlightColor : "white"
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
		buttons: ConfirmDlg.Yes | ConfirmDlg.No

		property int sender: ContactsView.None

		onYesClicked: {
			if (sender === ContactsView.RemoveContact)
			{
				removeContact()
			}
			else if (sender === ContactsView.ClearHistory)
			{
				clearHistory()
			}
		}
	}

	function removeContact()
	{
		dispatcher.removeContact(currentModel.card(currentIndex))
		dispatcher.clearHistory(currentId)
		contactsModel.update()
		historyModel.update(0)
		listView.currentIndex = -1
	}

	function clearHistory()
	{
		dispatcher.clearHistory(currentId)
		historyModel.update(currentId)
	}

	function setContactInfo()
	{
		if (settings.params["image"] === undefined || settings.params["image"] === "")
			contactImageSrc = ""
		else
		{
			if (currentModel === searchModel)
				contactImageSrc = "file:///" + settings.tempPath() + "/" + settings.params["image"]
			else
				contactImageSrc = "file:///" + settings.imagePath() + "/" + settings.params["image"]
		}

		if (settings.params["name"] === undefined || settings.params["name"] === "")
			contactNameText = ""
		else
			contactNameText = settings.params["name"]
	}
}
