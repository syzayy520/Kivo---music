#include "audio_file_type.h"

#include <QSet>

bool AudioFileType::isSupported(const QString& suffix) {
    static const QSet<QString> supported = {
        "flac", "wav", "aiff", "aif", "mp3", "m4a", "aac",
        "ogg", "opus", "dsf", "dff", "ape", "wv"
    };
    return supported.contains(suffix.toLower());
}

QString AudioFileType::labelFor(const QString& suffix) {
    const auto normalized = suffix.toLower();
    if (normalized == "dsf" || normalized == "dff") {
        return "DSD";
    }
    if (normalized == "flac") {
        return "FLAC";
    }
    if (normalized == "wav" || normalized == "aiff" || normalized == "aif") {
        return "PCM";
    }
    return normalized.toUpper();
}
