// =============================================================================
// Kivo Music Qt - cover_image_provider.cpp
// 职责: QQuickImageProvider 实现 — 路径即 ID，按需解码，LRU 缓存
// =============================================================================

#include "cover_image_provider.hpp"

#include <QFileInfo>
#include <QImageReader>

namespace kivo::qt::data {

namespace {
CoverImageProvider* g_artworkProvider = nullptr;
} // namespace

CoverImageProvider* globalArtworkProvider() noexcept { return g_artworkProvider; }
void setGlobalArtworkProvider(CoverImageProvider* p) noexcept { g_artworkProvider = p; }

CoverImageProvider::CoverImageProvider()
    : QQuickImageProvider(QQuickImageProvider::Image) {}

QImage CoverImageProvider::requestImage(
    const QString& id,
    QSize* size,
    const QSize& requestedSize) {
    {
        QMutexLocker locker(&mutex_);
        if (cache_.contains(id)) {
            lruOrder_.removeAll(id);
            lruOrder_.append(id);
            const QImage& img = cache_[id];
            if (size) *size = img.size();
            return img;
        }
    }

    // Cache miss — decode from path (id IS the local file path).
    // requestedSize honors Qt's source-size hints (e.g. AlbumArtwork sets 180×180).
    const QSize decodeSize = requestedSize.isValid() ? requestedSize : QSize(512, 512);
    const QImage img = decodeScaled(id, decodeSize);
    if (img.isNull()) {
        if (size) *size = QSize(1, 1);
        return QImage(1, 1, QImage::Format_ARGB32);
    }

    {
        QMutexLocker locker(&mutex_);
        evictIfNeeded();
        cache_[id] = img;
        lruOrder_.removeAll(id);
        lruOrder_.append(id);
    }

    if (size) *size = img.size();
    return img;
}

void CoverImageProvider::preload(const QString& localPath) {
    if (localPath.isEmpty()) return;
    {
        QMutexLocker locker(&mutex_);
        if (cache_.contains(localPath)) return;
    }
    const QImage img = decodeScaled(localPath, QSize(512, 512));
    if (img.isNull()) return;
    QMutexLocker locker(&mutex_);
    if (!cache_.contains(localPath)) {
        evictIfNeeded();
        cache_[localPath] = img;
        lruOrder_.append(localPath);
    }
}

QImage CoverImageProvider::decodeScaled(const QString& path, const QSize& requestedSize) const {
    if (!QFileInfo::exists(path)) return {};
    QImageReader reader(path);
    reader.setAutoTransform(true);
    if (requestedSize.isValid()) {
        reader.setScaledSize(requestedSize);
    }
    return reader.read();
}

void CoverImageProvider::evictIfNeeded() {
    while (cache_.size() >= kMaxCacheEntries && !lruOrder_.isEmpty()) {
        cache_.remove(lruOrder_.takeFirst());
    }
}

void CoverImageProvider::clearCache() {
    QMutexLocker locker(&mutex_);
    cache_.clear();
    lruOrder_.clear();
}

int CoverImageProvider::cacheSize() const {
    QMutexLocker locker(&mutex_);
    return cache_.size();
}

} // namespace kivo::qt::data
