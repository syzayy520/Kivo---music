// =============================================================================
// Kivo Music - EntranceAnimation.qml
// 职责: 组件入场动画（延迟错开，staggered entrance）
// Apple Music 级别: 每个子元素依次淡入+上浮，间距 60ms
// =============================================================================

pragma Singleton
import QtQuick

QtObject {
    readonly property int staggerDelay: 60   // 每个元素的延迟间隔
    readonly property int duration: 350       // 单个动画时长
    readonly property int floatDistance: 16   // 上浮距离

    // 对单个 Item 应用入场动画
    function animateEntrance(item, delayMs) {
        item.opacity = 0;
        item.y = item.y + floatDistance;

        var anim = Qt.createQmlObject(
            'import QtQuick; SequentialAnimation { ' +
            '  PauseAnimation { duration: ' + delayMs + ' } ' +
            '  ParallelAnimation { ' +
            '    NumberAnimation { target: ' + item + '; property: "opacity"; from: 0; to: 1; duration: ' + duration + '; easing.type: Easing.OutCubic } ' +
            '    NumberAnimation { target: ' + item + '; property: "y"; from: ' + item.y + '; to: ' + (item.y - floatDistance) + '; duration: ' + duration + '; easing.type: Easing.OutCubic } ' +
            '  } ' +
            '}',
            item
        );
        anim.start();
    }

    // 对列表中的元素依次入场
    function animateList(items, startDelayMs) {
        for (var i = 0; i < items.length; i++) {
            animateEntrance(items[i], startDelayMs + i * staggerDelay);
        }
    }
}
