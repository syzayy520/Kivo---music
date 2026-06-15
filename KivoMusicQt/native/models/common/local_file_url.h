#pragma once

#include <QString>

class LocalFileUrl final {
public:
    static QString fromPath(const QString& path);
};
