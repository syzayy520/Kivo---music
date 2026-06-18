#pragma once

#include <QFileInfo>
#include <QString>

// Extracts an embedded cover image to a cache file and returns its path (empty if
// none / unsupported). Currently parses the FLAC METADATA_BLOCK_PICTURE directly —
// no FFmpeg dependency — because FLAC is the hi-fi priority format and its picture
// block is trivially parseable. MP3 (ID3 APIC) / M4A (covr) are future work.
class EmbeddedCoverExtractor final {
public:
    // cacheDir is created if needed; results are keyed by path+size so an edited
    // file re-extracts. Safe to call off the UI thread (used by the scan worker).
    static QString extractTo(const QFileInfo& audioFile, const QString& cacheDir);
};
