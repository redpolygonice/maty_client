import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import "theme.js" as Theme

Rectangle {
	id: mainRect
	color: Theme.backColor2

	property int selfId: 0
	property int currentId: -1
	property int maxItemWidth: mainRect.width * 0.7

	Component.onCompleted: {
		if (settings.params["id"] !== undefined)
			selfId = settings.params["id"]
	}

	ContactInfoDlg {
		id: contactInfoDlg
	}

	Component {
		id: delegate

		Item {
			id: itemRect
			width: mainRect.width - 22
			height: textArea.contentHeight + dateText.height + 5

			property alias text: textArea
			property int textWidth: 0

			TextMetrics {
				id: textMetrics
				font: textArea.font
				text: message
			}

			Component.onCompleted: {
				setWidth()
			}

			onWidthChanged: {
				setWidth()
			}

			function setWidth()
			{
				var rectWidth = mainRect.width - image.width
				textWidth = textMetrics.boundingRect.width
				if (textWidth > rectWidth)
					textRect.width = rectWidth - 35
				else
					textRect.width = textWidth + 30
			}

			Row {
				anchors.fill: parent
				spacing: 7

				RoundedImage {
					id: rowImage
					srcwidth: 36
					srcheight: 36
					imgsource: {
						var imageFile = ""
						if (cid === selfId)
							imageFile = settings.params["image"]
						else
							imageFile = image

						if (imageFile === null || imageFile === undefined || imageFile.length === 0)
							return "qrc:///img/user.png"
						else
							return "file:///" + settings.imagePath() + "/" + imageFile
					}
				}

				Column {
					height: parent.height + 3
					spacing: 3

					Text {
						id: dateText
						text: date
						color: "gray"
						font.pointSize: 10
						font.italic: true
						leftPadding: 3
					}

					Rectangle {
						id: textRect
						height: parent.height
						color: {
							if (cid === selfId)
								return Theme.msgBackColor2
							else
								return Theme.msgBackColor1
						}
						radius: 20

						TextArea {
							id: textArea
							readOnly: true
							anchors.fill: parent
							anchors.margins: 5
							anchors.verticalCenter: parent.verticalCenter
							verticalAlignment: TextEdit.AlignVCenter
							text: message
							font.pointSize: Theme.fontPointSize
							wrapMode: Text.Wrap
							color: Theme.textColor
							selectionColor: "#aaaaaa"
							selectByKeyboard: true
							selectByMouse: true
							background: Rectangle {
								color: "transparent"
							}
						}

						MouseArea {
							anchors.fill: parent
							cursorShape: Qt.IBeamCursor
							acceptedButtons: Qt.RightButton

							onClicked: (mouse) => {
								listView.currentIndex = index
								currentId = id

								if (mouse.button === Qt.RightButton)
								{
									contextMenu.popup()
								}
							}
						}
					}
				}
			}
		}
	}

	ColumnLayout {
		anchors.fill: parent

		Rectangle {
			id: topPanel
			z: 1
			Layout.fillWidth: true
			Layout.fillHeight: false
			Layout.minimumHeight: 50
			color: Theme.backColor1
			visible: contactsView.currentIndex !== -1

			RowLayout {
				anchors.fill: parent
				anchors.leftMargin: 15
				spacing: 10
				Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter

				// RoundedImage {
				// 	id: topImage
				// 	srcwidth: 36
				// 	srcheight: 36
				// 	imgsource: {
				// 		var imageFile = contactsView.currentModel.card(contactsView.currentIndex)["image"]
				// 		if (imageFile === null || imageFile === undefined || imageFile.length === 0)
				// 			return ""
				// 		else
				// 		{
				// 			if (contactsView.currentModel === searchModel)
				// 				return "file:///" + settings.tempPath() + "/" + imageFile
				// 			else
				// 				return "file:///" + settings.imagePath() + "/" + imageFile
				// 		}
				// 	}

				// 	MouseArea {
				// 		id: topImageArea
				// 		anchors.fill: parent
				// 		acceptedButtons: Qt.LeftButton | Qt.RightButton
				// 		hoverEnabled: true
				// 		cursorShape: Qt.PointingHandCursor
				// 		onClicked: {
				// 			contactInfoDlg.contactMap = contactsView.currentModel.card(contactsView.currentIndex)
				// 			contactInfoDlg.open()
				// 		}
				// 	}
				// }

				Text {
					id: text
					text: contactsView.currentModel.card(contactsView.currentIndex)["name"]
					font.pointSize: Theme.fontPointSize
					font.bold: true
					color: Theme.textColor
					elide: Text.ElideRight
					verticalAlignment: Text.AlignVCenter
					horizontalAlignment: Text.AlignLeft
					Layout.fillWidth: true

					MouseArea {
						id: topNameArea
						anchors.fill: parent
						acceptedButtons: Qt.LeftButton | Qt.RightButton
						hoverEnabled: true
						cursorShape: Qt.PointingHandCursor
						onClicked: {
							contactInfoDlg.contactMap = contactsView.currentModel.card(contactsView.currentIndex)
							contactInfoDlg.open()
						}
					}
				}
			}
		}

		ListView {
			id: listView
			z: 0
			Layout.fillWidth: true
			Layout.fillHeight: true
			Layout.leftMargin: 10
			Layout.topMargin: 10
			Layout.bottomMargin: 30
			spacing: 30
			model: historyModel
			delegate: delegate

			ScrollBar.vertical: ScrollBar {
				policy: ScrollBar.AsNeeded
			}

			onCountChanged: {
				positionViewAtIndex(count - 1, ListView.Beginning)
			}
		}
	}

	Menu {
		id: contextMenu
		topPadding: 3
		bottomPadding: 3

		Action {
			text: "Select"
			onTriggered: {
				listView.currentItem.text.selectAll()
			}
		}

		Action {
			text: "Copy"
			onTriggered: {
				listView.currentItem.text.selectAll()
				listView.currentItem.text.copy()
				listView.currentItem.text.deselect()
			}
		}

		Action {
			text: "Edit"
			onTriggered: {
				messageView.text.text = listView.currentItem.text.text
				messageView.modify = true
				messageView.id = currentId
				messageView.text.focus = true
			}
		}

		Action {
			text: "Remove"
			onTriggered: {
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
				color: menuItem.highlighted ? Theme.menuHighlightColor : "transparent"
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
		messageText: "Remove current history record ?"

		onYesClicked: {
			dispatcher.removeHistory(currentId)
			historyModel.update(contactsView.currentId)
		}
	}

	function positionAtEnd()
	{
		listView.positionViewAtIndex(listView.count - 1, ListView.Beginning)
	}
}
