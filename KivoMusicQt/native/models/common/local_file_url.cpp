#include "local_file_url.h"

#include <QUrl>

QString LocalFileUrl::fromPath(const QString& path) {
    if (path.isEmpty()) {
        return {};
    }
    return QUrl::fromLocalFile(path).toString();
}
