// =============================================================================
// Kivo Music Qt - cover_color_extractor.hpp
// 职责: 从专辑封面提取「主色/活力色」，用于沉浸式 Now-Playing 的取色渐变背景
// =============================================================================

#pragma once

#include <QColor>
#include <QString>

namespace kivo::qt::data {

// Extracts a vibrant representative color from a local image file (Apple Music-
// style artwork-driven backgrounds). Pure + off-UI-thread safe — call it on the
// cover-resolution worker. Returns an invalid QColor if the image can't load.
class CoverColorExtractor {
public:
    [[nodiscard]] static QColor dominant(const QString& localFilePath);
};

} // namespace kivo::qt::data
