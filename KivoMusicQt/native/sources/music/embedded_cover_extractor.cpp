#include "embedded_cover_extractor.h"

#include <QCryptographicHash>
#include <QDir>
#include <QFile>

namespace {

QString extForMime(const QByteArray& mime) {
    if (mime.contains("png")) return QStringLiteral("png");
    if (mime.contains("webp")) return QStringLiteral("webp");
    return QStringLiteral("jpg");  // image/jpeg and unknowns
}

bool readU32(QFile& f, quint32& out) {
    uchar b[4];
    if (f.read(reinterpret_cast<char*>(b), 4) != 4) return false;
    out = (quint32(b[0]) << 24) | (quint32(b[1]) << 16)
        | (quint32(b[2]) << 8) | quint32(b[3]);
    return true;
}

} // namespace

QString EmbeddedCoverExtractor::extractTo(
    const QFileInfo& audioFile, const QString& cacheDir) {
    // Only FLAC for now (direct PICTURE-block parse).
    if (audioFile.suffix().compare(QStringLiteral("flac"), Qt::CaseInsensitive) != 0) {
        return {};
    }

    // Cache key = path + size, so an edited file produces a fresh cover.
    const QByteArray keyInput =
        (audioFile.absoluteFilePath() + QLatin1Char(':')
         + QString::number(audioFile.size())).toUtf8();
    const QString base = QString::fromLatin1(
        QCryptographicHash::hash(keyInput, QCryptographicHash::Sha1).toHex());

    // Fast path: already extracted.
    for (const QString& ext : {QStringLiteral("jpg"), QStringLiteral("png"),
                               QStringLiteral("webp")}) {
        const QString hit = cacheDir + QLatin1Char('/') + base + QLatin1Char('.') + ext;
        if (QFileInfo::exists(hit)) return hit;
    }

    QFile f(audioFile.absoluteFilePath());
    if (!f.open(QIODevice::ReadOnly)) return {};

    char marker[4];
    if (f.read(marker, 4) != 4 || qstrncmp(marker, "fLaC", 4) != 0) return {};

    // Walk metadata blocks until a PICTURE (type 6) or the last block.
    while (!f.atEnd()) {
        uchar hdr;
        if (f.read(reinterpret_cast<char*>(&hdr), 1) != 1) break;
        const bool last = (hdr & 0x80u) != 0;
        const int type = hdr & 0x7fu;

        uchar lenb[3];
        if (f.read(reinterpret_cast<char*>(lenb), 3) != 3) break;
        const quint32 blockLen =
            (quint32(lenb[0]) << 16) | (quint32(lenb[1]) << 8) | quint32(lenb[2]);

        if (type == 6) {  // METADATA_BLOCK_PICTURE
            quint32 picType = 0, mimeLen = 0;
            if (!readU32(f, picType) || !readU32(f, mimeLen) || mimeLen > 256) break;
            const QByteArray mime = f.read(mimeLen);
            quint32 descLen = 0;
            if (!readU32(f, descLen)) break;
            if (!f.seek(f.pos() + descLen)) break;  // skip UTF-8 description
            quint32 w = 0, h = 0, depth = 0, colors = 0, dataLen = 0;
            if (!readU32(f, w) || !readU32(f, h) || !readU32(f, depth)
                || !readU32(f, colors) || !readU32(f, dataLen)) break;
            if (dataLen == 0 || dataLen > 30u * 1024 * 1024) break;  // sanity cap

            const QByteArray data = f.read(dataLen);
            if (quint32(data.size()) != dataLen) break;

            if (!QDir().mkpath(cacheDir)) return {};
            const QString out =
                cacheDir + QLatin1Char('/') + base + QLatin1Char('.') + extForMime(mime);
            QFile of(out);
            if (of.open(QIODevice::WriteOnly)) {
                of.write(data);
                of.close();
                return out;
            }
            return {};
        }

        if (last) break;
        if (!f.seek(f.pos() + blockLen)) break;  // skip non-picture block
    }
    return {};
}
