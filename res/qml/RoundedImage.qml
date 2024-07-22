import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import QtQuick.Effects

Item {
	id: control
	visible: true
	width: srcheight
	height: srcheight
	antialiasing: true

	property int srcwidth: 68
	property int srcheight: 68
	property string imgsource: ""

	Image {
		id: image
		visible: false
		anchors.fill: parent
		fillMode: Image.PreserveAspectCrop
		sourceSize.width: srcwidth
		sourceSize.height: srcheight
		source: imgsource
	}

	MultiEffect {
		source: image
		anchors.fill: image
		maskEnabled: true
		maskSource: mask
	}

	Item {
		id: mask
		width: image.width
		height: image.height
		layer.enabled: true
		visible: false

		Rectangle {
			anchors.fill: parent
			anchors.margins: 2
			width: image.width
			height: image.height
			radius: width / 2
			smooth: true
			antialiasing: true
		}
	}
}
