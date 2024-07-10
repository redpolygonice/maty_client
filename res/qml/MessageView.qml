import QtQuick
import QtQuick.Controls.Basic
import "common.js" as Common

Rectangle {
	id: mainRect
	color: Common.backColor1
	height: textView.contentHeight + 22
	visible: contactsView.currentIndex !== -1

	property int id: 0
	property bool modify: false
	property alias text: textView

	TextArea {
		id: textView
		focus: true
		anchors.fill: parent
		anchors.margins: 5
		anchors.verticalCenter: parent.verticalCenter
		color: "#000000"
		selectionColor: "#aaaaaa"
		selectByKeyboard: true
		selectByMouse: true
		renderType: Text.NativeRendering
		textFormat: TextEdit.PlainText
		wrapMode: TextEdit.Wrap
		font.pointSize: 13
		placeholderText: "Enter text .."
		background: Rectangle {
			color: "transparent"
		}

		Keys.onPressed: (event) => {
			if (event.key === Qt.Key_Enter || event.key === Qt.Key_Return)
			{
				if (textView.text.trim().length === 0)
				{
					event.accepted = true
					return;
				}

				if (modify)
					database.modifyHistory(id, textView.text)
				else
					database.appendHistory([contactsView.currentId, false, textView.text])

				modify = false
				event.accepted = true
				mainWindow.newText(contactsView.currentId, textView.text)
				textView.clear()
				historyModel.update(contactsView.currentId)
			}
			else if (event.key === Qt.Key_Escape)
			{
				if (modify)
				{
					modify = false
					event.accepted = true
					textView.clear()
				}
			}
		}
	}
}
