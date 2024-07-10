import QtQuick
import QtQuick.Controls

Menu {
    id: menu
    topPadding: 3
    bottomPadding: 3

    Action {
        text: "My card .."
        onTriggered: {
            cardDlg.contactId = 1
            cardDlg.reason = CardDlg.Reason.Edit
            cardDlg.open()
        }
    }

    Action {
        text: "Add new contact .."
        onTriggered: {
            cardDlg.contactId = 0
            cardDlg.reason = CardDlg.Reason.New
            cardDlg.open()
        }
    }

    Action {
        text: "Settings .."
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
