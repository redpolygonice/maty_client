import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import QtQuick.Dialogs
import "common.js" as Common

Rectangle {
	id: mainRect
	color: Common.backColor2

	property int currentId: -1
	property int maxItemWidth: mainRect.width * 0.7

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
				spacing: 10

				Image {
					id: image
					fillMode: Image.PreserveAspectFit
					sourceSize.width: 24
					sourceSize.height: 24
					smooth: true
					source: {
						var imageFile = ""
						if (sender)
							imageFile = imgname
						else
							imageFile = contactsModel.card(0)[1]

						if (imageFile === 'empty' || imageFile.length === 0 ||
								imageFile === null || imageFile === undefined)
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
							if (sender)
								return Common.msgBackColor1
							else
								return Common.msgBackColor2
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
							font.pointSize: 13
							wrapMode: Text.Wrap
							color: Common.textColor
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
			color: Common.backColor1
			visible: contactsView.currentIndex !== -1

			RowLayout {
				anchors.fill: parent
				anchors.leftMargin: 10
				spacing: 10
				Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter

				Image {
					id: image
					fillMode: Image.PreserveAspectFit
					sourceSize.width: 36
					sourceSize.height: 36
					smooth: true
					source: {
						if (contactsModel === null)
							return ""

						var imgname = contactsModel.card(contactsView.currentIndex)[1]
						if (imgname === 'empty' || imgname.length === 0 ||
								imgname === null || imgname === undefined)
							return ""
						else
							return "file:///" + settings.imagePath() + "/" + imgname
					}
				}

				Text {
					id: text
					text: contactsModel === null ? "" : contactsModel.card(contactsView.currentIndex)[0]
					font.pointSize: 14
					font.bold: true
					color: "#4D4D4D"
					elide: Text.ElideRight
					verticalAlignment: Text.AlignVCenter
					horizontalAlignment: Text.AlignLeft
					Layout.fillWidth: true
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
			Layout.bottomMargin: 20
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
				color: Common.textColor
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
		titleText: "Removing record"
		messageText: "Remove current history record ?"

		onOkClicked: {
			database.removeHistory(currentId)
			historyModel.update(contactsView.currentId)
		}

	}
}
