// =============================================================================
// Kivo Music - SettingsCategoryRail.qml
// 职责: 设置左栏——搜索框 + 按分组展示的分类列表（数据全部来自 settingsCatalog）
// 分组的标题与排序来自 settingsCatalog.groups，新增分组无需改本文件。
// =============================================================================

import QtQuick
import QtQuick.Controls
import KivoMusic

Rectangle {
    id: rail

    property string currentCategoryId: ""
    property string searchText: ""
    signal categorySelected(string id)

    color: Theme.panelSoft

    // right divider
    Rectangle {
        anchors.right: parent.right
        width: 1
        height: parent.height
        color: Theme.line
    }

    // Group display title from the catalog registry; falls back to a title-cased
    // id so an unregistered group still degrades gracefully (no raw lowercase).
    function groupTitleFor(id) {
        var gs = (typeof settingsCatalog !== "undefined" && settingsCatalog.groups)
            ? settingsCatalog.groups : []
        for (var i = 0; i < gs.length; i++) {
            if (gs[i].id === id) return gs[i].title
        }
        return id.length > 0 ? id.charAt(0).toUpperCase() + id.slice(1) : id
    }

    // Flat item list with synthetic group headers. Categories are bucketed by
    // group (so a header appears EXACTLY once per group regardless of weights),
    // groups ordered by the registry rank, with any unregistered group appended
    // in first-seen order. This makes adding new groups (AI/Cloud/…) robust.
    function buildItems() {
        var src = (typeof settingsCatalog !== "undefined") ? settingsCatalog.categories : []
        var q = rail.searchText.trim().toLowerCase()

        var buckets = ({})
        var seenOrder = []
        for (var i = 0; i < src.length; i++) {
            var c = src[i]
            if (q.length > 0) {
                var hay = (c.title + " " + (c.keywords ? c.keywords.join(" ") : "")).toLowerCase()
                if (hay.indexOf(q) < 0) continue
            }
            var g = c.group
            if (buckets[g] === undefined) { buckets[g] = []; seenOrder.push(g) }
            buckets[g].push(c)
        }

        // Group order: registry rank first, then any unregistered group in
        // first-seen order.
        var gs = (typeof settingsCatalog !== "undefined" && settingsCatalog.groups)
            ? settingsCatalog.groups.slice() : []
        gs.sort(function(a, b) { return (a.rank || 0) - (b.rank || 0) })
        var orderedIds = []
        for (var r = 0; r < gs.length; r++) {
            if (buckets[gs[r].id] !== undefined && orderedIds.indexOf(gs[r].id) < 0)
                orderedIds.push(gs[r].id)
        }
        for (var s = 0; s < seenOrder.length; s++) {
            if (orderedIds.indexOf(seenOrder[s]) < 0) orderedIds.push(seenOrder[s])
        }

        var items = []
        for (var m = 0; m < orderedIds.length; m++) {
            var gid = orderedIds[m]
            var arr = buckets[gid]
            arr.sort(function(a, b) { return (a.weight || 0) - (b.weight || 0) })
            items.push({ kind: "header", group: gid })
            for (var n = 0; n < arr.length; n++) items.push({ kind: "cat", cat: arr[n] })
        }
        return items
    }

    property var items: buildItems()
    onSearchTextChanged: items = buildItems()

    Column {
        anchors.fill: parent
        anchors.margins: Theme.space3
        spacing: Theme.space3

        // ── Search ───────────────────────────────────────────
        Rectangle {
            width: parent.width
            height: 36
            radius: Theme.radiusLarge
            color: Theme.panel
            border.color: Theme.line

            Row {
                anchors.fill: parent
                anchors.leftMargin: 10
                anchors.rightMargin: 10
                spacing: 6
                Text {
                    text: "🔍"
                    font.pixelSize: 13
                    anchors.verticalCenter: parent.verticalCenter
                }
                TextField {
                    id: searchField
                    width: parent.width - 26
                    anchors.verticalCenter: parent.verticalCenter
                    placeholderText: qsTr("Search settings")
                    color: Theme.text
                    placeholderTextColor: Theme.faint
                    font.pixelSize: 13
                    font.family: Theme.fontFamily
                    background: Rectangle { color: "transparent" }
                    onTextChanged: rail.searchText = text
                }
            }
        }

        // ── Category list ────────────────────────────────────
        Flickable {
            width: parent.width
            height: parent.height - 36 - Theme.space3
            contentWidth: width
            contentHeight: listCol.implicitHeight
            clip: true

            Column {
                id: listCol
                width: parent.width
                spacing: 2

                Repeater {
                    model: rail.items

                    delegate: Item {
                        required property var modelData
                        width: listCol.width
                        height: modelData.kind === "header" ? 30 : 40

                        // Group header
                        Text {
                            visible: modelData.kind === "header"
                            text: modelData.kind === "header" ? rail.groupTitleFor(modelData.group) : ""
                            color: Theme.muted
                            font.pixelSize: 10
                            font.weight: Font.DemiBold
                            font.capitalization: Font.AllUppercase
                            font.family: Theme.fontFamily
                            anchors.left: parent.left
                            anchors.leftMargin: 8
                            anchors.bottom: parent.bottom
                            anchors.bottomMargin: 4
                        }

                        // Category row
                        Rectangle {
                            visible: modelData.kind === "cat"
                            anchors.fill: parent
                            radius: Theme.radiusLarge
                            readonly property bool selected:
                                modelData.kind === "cat" && modelData.cat.id === rail.currentCategoryId
                            color: selected
                                ? Theme.accent
                                : (catMouse.containsMouse ? Theme.panelHover : "transparent")
                            Behavior on color { ColorAnimation { duration: Theme.animFast } }

                            Text {
                                id: catIcon
                                visible: modelData.kind === "cat"
                                text: modelData.kind === "cat" ? modelData.cat.icon : ""
                                font.pixelSize: 15
                                anchors.left: parent.left
                                anchors.leftMargin: 10
                                anchors.verticalCenter: parent.verticalCenter
                            }
                            Text {
                                visible: modelData.kind === "cat"
                                text: modelData.kind === "cat" ? modelData.cat.title : ""
                                color: parent.selected ? Theme.accentInk : Theme.text
                                font.pixelSize: 14
                                font.family: Theme.fontFamily
                                elide: Text.ElideRight
                                anchors.left: catIcon.right
                                anchors.leftMargin: 10
                                anchors.right: parent.right
                                anchors.rightMargin: 10
                                anchors.verticalCenter: parent.verticalCenter
                            }
                            MouseArea {
                                id: catMouse
                                anchors.fill: parent
                                enabled: modelData.kind === "cat"
                                hoverEnabled: true
                                cursorShape: Qt.PointingHandCursor
                                onClicked: rail.categorySelected(modelData.cat.id)
                            }
                        }
                    }
                }
            }
        }
    }
}
