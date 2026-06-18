// =============================================================================
// Kivo Music - SettingsShell.qml
// 职责: 设置外壳（导航栏 + 详情区）。HomeWindow 的 "settings" 路由目标。
// 内部导航靠 currentCategoryId；不新增顶层 currentPage 路由。
// =============================================================================

import QtQuick
import KivoMusic

Item {
    id: shell

    property string currentCategoryId: ""

    // Default to the first registered category once the catalog is available.
    Component.onCompleted: {
        if (shell.currentCategoryId === ""
                && typeof settingsCatalog !== "undefined"
                && settingsCatalog.categories.length > 0) {
            shell.currentCategoryId = settingsCatalog.categories[0].id
        }
    }

    Row {
        anchors.fill: parent

        SettingsCategoryRail {
            id: rail
            width: 240
            height: parent.height
            currentCategoryId: shell.currentCategoryId
            onCategorySelected: (id) => { shell.currentCategoryId = id }
        }

        SettingsDetailHost {
            width: parent.width - rail.width
            height: parent.height
            categoryId: shell.currentCategoryId
        }
    }
}
