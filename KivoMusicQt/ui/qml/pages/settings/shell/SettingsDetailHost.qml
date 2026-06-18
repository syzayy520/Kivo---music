// =============================================================================
// Kivo Music - SettingsDetailHost.qml
// 职责: 设置右侧详情区——按 categoryId 从 settingsCatalog 解析页面并异步加载
// =============================================================================

import QtQuick
import KivoMusic

Item {
    id: host
    property string categoryId: ""

    function sourceFor(id) {
        var cats = (typeof settingsCatalog !== "undefined") ? settingsCatalog.categories : []
        for (var i = 0; i < cats.length; i++) {
            if (cats[i].id === id) return cats[i].qmlSource
        }
        return ""
    }

    Loader {
        id: loader
        anchors.fill: parent
        asynchronous: true
        // qmlSource is relative to ui/qml/pages/settings/; this file lives in
        // pages/settings/shell/, so prefix "../" to reach categories/.
        source: {
            var s = host.sourceFor(host.categoryId)
            return s.length > 0 ? "../" + s : ""
        }
    }

    // Lightweight loading veil while an async page instantiates.
    Rectangle {
        anchors.fill: parent
        color: Theme.page
        visible: loader.status === Loader.Loading
        Text {
            anchors.centerIn: parent
            text: "···"
            color: Theme.faint
            font.pixelSize: 20
        }
    }
}
