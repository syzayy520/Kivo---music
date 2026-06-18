// =============================================================================
// Kivo Music Qt - cover_color_extractor.cpp
// =============================================================================

#include "cover_color_extractor.hpp"

#include <QImage>

#include <cmath>

namespace kivo::qt::data {

QColor CoverColorExtractor::dominant(const QString& localFilePath) {
    if (localFilePath.isEmpty()) {
        return {};
    }
    QImage image(localFilePath);
    if (image.isNull()) {
        return {};
    }
    // Downscale for speed + noise reduction. Preserve alpha when present so fully
    // transparent pixels can be EXCLUDED — otherwise RGB32 folds them in as black
    // and darkens the result. Opaque art takes the RGB32 fast path.
    const bool hasAlpha = image.hasAlphaChannel();
    image = image
        .scaled(48, 48, Qt::IgnoreAspectRatio, Qt::SmoothTransformation)
        .convertToFormat(hasAlpha ? QImage::Format_ARGB32 : QImage::Format_RGB32);

    // Accumulate vibrancy into HUE BINS (not a single-pixel argmax). The heaviest
    // bin is the dominant vibrant family, so a small saturated logo on a large
    // calm field can't hijack the whole gradient the way one outlier pixel could.
    constexpr int kBins = 24;  // 15° per bin
    double binWeight[kBins] = {0};
    double binR[kBins] = {0}, binG[kBins] = {0}, binB[kBins] = {0};
    long long sumR = 0, sumG = 0, sumB = 0, count = 0;

    for (int y = 0; y < image.height(); ++y) {
        const auto* line = reinterpret_cast<const QRgb*>(image.constScanLine(y));
        for (int x = 0; x < image.width(); ++x) {
            const QRgb px = line[x];
            if (hasAlpha && qAlpha(px) < 16) {
                continue;  // ~transparent: not a real artwork color
            }
            const QColor c = QColor::fromRgb(px);
            sumR += c.red();
            sumG += c.green();
            sumB += c.blue();
            ++count;

            int h = 0, s = 0, v = 0;
            c.getHsv(&h, &s, &v);
            const double sat = s / 255.0;
            const double val = v / 255.0;
            // Favor saturated colors near mid-high brightness; penalize extremes.
            const double score = sat * (1.0 - std::abs(val - 0.62) * 1.25);
            if (h < 0 || score <= 0.0) {
                continue;  // achromatic / too dim or too bright to count as vibrant
            }
            const int bin = (h / 15) % kBins;
            binWeight[bin] += score;
            binR[bin] += score * c.red();
            binG[bin] += score * c.green();
            binB[bin] += score * c.blue();
        }
    }
    if (count == 0) {
        return {};  // wholly transparent / empty
    }
    const QColor average(
        static_cast<int>(sumR / count),
        static_cast<int>(sumG / count),
        static_cast<int>(sumB / count));

    int bestBin = -1;
    double bestWeight = 0.0;
    for (int i = 0; i < kBins; ++i) {
        if (binWeight[i] > bestWeight) {
            bestWeight = binWeight[i];
            bestBin = i;
        }
    }
    if (bestBin < 0 || bestWeight <= 0.0) {
        return average;  // muted art: no vibrant family
    }
    const QColor dominantHue(
        static_cast<int>(binR[bestBin] / bestWeight),
        static_cast<int>(binG[bestBin] / bestWeight),
        static_cast<int>(binB[bestBin] / bestWeight));

    int bh = 0, bs = 0, bv = 0;
    dominantHue.getHsv(&bh, &bs, &bv);
    // Use the dominant vibrant family when it's actually colorful; else the average.
    return bs > 60 ? dominantHue : average;
}

} // namespace kivo::qt::data
