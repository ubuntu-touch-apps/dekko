import QtQuick 2.3
import DekkoCore 0.2

Image {
    id: root
    property alias color: helper.color
    property alias name: helper.icon
    // set's the images horizontal & vertical ratio in terms of width/height of the component
    // Default = 1.0
    property alias horizontalRatio: helper.horizontalRatio
    property alias verticalRatio: helper.verticalRatio

    cache: true;
    smooth: false;
    fillMode: Image.Pad;
    asynchronous: true;
    antialiasing: false;
    ImageHelper on source {
        id: helper
        size: root.height
    }
}
