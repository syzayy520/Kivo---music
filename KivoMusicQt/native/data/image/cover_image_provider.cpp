// =============================================================================
// Kivo Music Qt - cover_image_provider.cpp
// 职责: C++ QQuickImageProvider 实现
// =============================================================================

#include "cover_image_provider.hpp"

#include <QFileInfo>
#include <QImageReader>

namespace kivo::qt::data {

CoverImageProvider& CoverImageProvider::instance() {
    static CoverImageProvider inst;
    return inst;
}

CoverImageProvider::CoverImageProvider()
    : QQuickImageProvider(QQuickImageProvider::Image) {}

QImage CoverImageProvider::requestImage(const QString& id, QSize* size, const QSize& requestedSize) {
    Q_UNUSED(requestedSize);
    QMutexLocker locker(&mutex_);

    // 命中缓存
    if (cache_.contains(id)) {
        lruOrder_.removeAll(id);
        lruOrder_.append(id);
        if (size) *size = cache_[id].image.size();
        return cache_[id].image;
    }

    // 缓存未命中 → 返回透明1x1占位（调用方应该先 preload）
    if (size) *size = QSize(1, 1);
    return QImage(1, 1, QImage::Format_ARGB32);
}

void CoverImageProvider::notifyCoverAvailable(const QString& artist, const QString& album, const QString& localPath) {
    const QString key = makeKey(artist, album);

    QMutexLocker locker(&mutex_);
    // 如果已缓存且路径相同，跳过
    if (cache_.contains(key) && cache_[key].filePath == localPath) return;

    QImage img = loadAndCache(key, localPath, QSize(256, 256));
    Q_UNUSED(img);
}

QImage CoverImageProvider::loadAndCache(const QString& key, const QString& path, const QSize& requestedSize) {
    QFileInfo fi(path);
    if (!fi.exists() || !fi.isReadable()) return QImage();

    QImageReader reader(path);
    reader.setAutoTransform(true);
    if (requestedSize.isValid()) {
        reader.setScaledSize(requestedSize);
    }

    QImage img = reader.read();
    if (img.isNull()) return QImage();

    evictIfNeeded();

    CacheEntry entry;
    entry.image = img;
    entry.filePath = path;
    cache_[key] = entry;
    lruOrder_.append(key);

    return img;
}

void CoverImageProvider::evictIfNeeded() {
    while (cache_.size() >= maxCacheSize_ && !lruOrder_.isEmpty()) {
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

QString CoverImageProvider::makeKey(const QString& artist, const QString& album) const {
    return artist + "_" + album;
}

} // namespace kivo::qt::data
