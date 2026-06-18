// =============================================================================
// Kivo Music Qt - cover_image_provider.hpp
// 职责: QQuickImageProvider — 按需解码 + LRU 内存缓存
// QML:  Image { source: "image://artwork/" + localFilePath }
// 注:   engine.addImageProvider("artwork", new CoverImageProvider)
//       Qt 接管所有权; 用 globalArtworkProvider()/setGlobalArtworkProvider()
//       在控制器侧调 preload()（可选预热，不调也能工作）。
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
    CoverImageProvider();
    ~CoverImageProvider() override = default;

    // QQuickImageProvider interface.
    // id = local file path as-is (no percent-encoding — Qt passes the raw QString).
    QImage requestImage(const QString& id, QSize* size, const QSize& requestedSize) override;

    // Optional: pre-populate cache on a background thread before QML requests.
    void preload(const QString& localPath);

    void clearCache();
    int cacheSize() const;

private:
    QImage decodeScaled(const QString& path, const QSize& requestedSize) const;
    void evictIfNeeded();

    mutable QMutex mutex_;
    QHash<QString, QImage> cache_;
    QStringList lruOrder_;
    static constexpr int kMaxCacheEntries = 64;
};

// Non-owning global — set once in main.cpp after addImageProvider().
// Lets AudioPlaybackController call preload() without a ctor-parameter refactor.
CoverImageProvider* globalArtworkProvider() noexcept;
void setGlobalArtworkProvider(CoverImageProvider* p) noexcept;

} // namespace kivo::qt::data
