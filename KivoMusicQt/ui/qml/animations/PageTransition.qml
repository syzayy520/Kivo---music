// =============================================================================
// Kivo Music - PageTransition.qml
// 职责: 页面切换过渡动画（淡入淡出 + 方向位移）
// Apple Music 级别: 250ms 淡出 → 加载 → 300ms 淡入 + 微位移
// =============================================================================

pragma Singleton
import QtQuick

QtObject {
    // 统一的页面切换时长
    readonly property int fadeOutDuration: 200
    readonly property int fadeInDuration: 300
    readonly property int slideDistance: 12

    // 创建标准淡出动画
    function createFadeOut(target, onComplete) {
        var anim = Qt.createQmlObject(
            'import QtQuick; SequentialAnimation { ' +
            '  NumberAnimation { target: ' + target + '; property: "opacity"; from: 1; to: 0; duration: ' + fadeOutDuration + '; easing.type: Easing.InQuad } ' +
            '  NumberAnimation { target: ' + target + '; property: "y"; from: ' + target.y + '; to: ' + (target.y + slideDistance) + '; duration: ' + fadeOutDuration + '; easing.type: Easing.InQuad } ' +
            '}',
            target
        );
        if (onComplete) anim.onFinished.connect(onComplete);
        return anim;
    }

    // 创建标准淡入动画
    function createFadeIn(target, fromY) {
        target.y = fromY - slideDistance;
        target.opacity = 0;
        var anim = Qt.createQmlObject(
            'import QtQuick; ParallelAnimation { ' +
            '  NumberAnimation { target: ' + target + '; property: "opacity"; from: 0; to: 1; duration: ' + fadeInDuration + '; easing.type: Easing.OutCubic } ' +
            '  NumberAnimation { target: ' + target + '; property: "y"; from: ' + (fromY - slideDistance) + '; to: ' + fromY + '; duration: ' + fadeInDuration + '; easing.type: Easing.OutCubic } ' +
            '}',
            target
        );
        return anim;
    }
}
