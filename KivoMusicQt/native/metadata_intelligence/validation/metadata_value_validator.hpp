#pragma once

#include <QString>

namespace kivo::metadata {

// =============================================================================
// MetadataValueValidator
// 职责：验证元数据字段值的有效性（是否为空、占位符、未知值）
// 单一职责：只判断值是否有效，不做业务决策（不决定是否交换字段）
// =============================================================================

class MetadataValueValidator {
public:
    // Check if artist field contains a valid, non-placeholder value
    static bool isValidArtist(const QString& artist);

    // Check if title field contains a valid, non-placeholder value
    static bool isValidTitle(const QString& title);

    // Check if album field contains a valid, non-placeholder value
    static bool isValidAlbum(const QString& album);

    // Get the type of placeholder (for diagnostics)
    enum class PlaceholderType {
        NotPlaceholder,
        Empty,
        UnknownEnglish,      // "Unknown Artist", "Unknown Album", etc.
        UnknownChinese,      // "未知艺人", "未知专辑", etc.
        GenericUnknown       // Just "Unknown" or "未知"
    };
    static PlaceholderType getPlaceholderType(const QString& value);

private:
    // Internal: check if value is a known placeholder
    static bool isPlaceholder(const QString& value);
};

}  // namespace kivo::metadata
