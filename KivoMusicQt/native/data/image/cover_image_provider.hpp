// =============================================================================
// Kivo Music Qt - cover_image_provider.hpp
// 职责: C++ QQuickImageProvider — LRU内存缓存 + 本地文件加载
// QML: Image { source: "image://covers/Rolling Stones_Let It Bleed" }
// 流程: 先查LRU缓存 → 再查本地MusicCoverFinder → 异步触发MusicCoverFetcher
// =============================================================================

#pragma once

#include <QQuickImageProvider>
#include <QImage>
#include <QHash>
#include <QMutex>
#include <QString>
#include <QStringList>

namespace kivo::qt::data {

class CoverImageProvider : public QQuickImageProvider {
public:
    static CoverImageProvider& instance();

    QImage requestImage(const QString& id, QSize* size, const QSize& requestedSize) override;

    // 外部调用：通知有新封面文件可用
    void notifyCoverAvailable(const QString& artist, const QString& album, const QString& localPath);

    void clearCache();
    int cacheSize() const;

private:
    explicit CoverImageProvider();
    ~CoverImageProvider() override = default;

    QImage loadAndCache(const QString& key, const QString& path, const QSize& requestedSize);
    void evictIfNeeded();
    QString makeKey(const QString& artist, const QString& album) const;

    struct CacheEntry {
        QImage image;
        QString filePath;
    };

    mutable QMutex mutex_;
    QHash<QString, CacheEntry> cache_;
    QStringList lruOrder_;
    int maxCacheSize_ = 64;
};

} // namespace kivo::qt::data
