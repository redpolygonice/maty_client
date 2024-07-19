import QtQuick
import QtQuick.Controls.Basic
import "theme.js" as Theme

Rectangle {
	id: mainRect
	color: Theme.backColor1
	height: textView.contentHeight + 22
	visible: contactsView.currentIndex !== -1

	property bool modify: false
	property alias text: textView

	TextArea {
		id: textView
		focus: true
		anchors.fill: parent
		anchors.margins: 5
		anchors.verticalCenter: parent.verticalCenter
		color: "#000000"
		selectionColor: Theme.selectionColor
		selectByKeyboard: true
		selectByMouse: true
		renderType: Text.NativeRendering
		textFormat: TextEdit.PlainText
		wrapMode: TextEdit.Wrap
		font.pointSize: Theme.fontPointSize
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
					dispatcher.modifyHistory({ "hid": historyView.currentId, "cid": historyView.currentCid,
												 "rid": historyView.currentRid, "text": textView.text})
				else
				{
					dispatcher.addHistory({ "cid": settings.params["id"], "rid": contactsView.currentId,
											  "text": textView.text})
					if (contactsView.currentModel === searchModel)
						dispatcher.addSearchContact(contactsView.currentModel.card(contactsView.currentIndex))
				}

				modify = false
				event.accepted = true
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
