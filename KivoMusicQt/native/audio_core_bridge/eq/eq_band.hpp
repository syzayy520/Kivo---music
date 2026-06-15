// =============================================================================
// Kivo Music Qt - eq_band.hpp
// 职责: 单个EQ频段的参数定义
// =============================================================================

#pragma once

#include <QObject>
#include <QString>

namespace kivo::qt::audio_bridge::eq {

class EqBand : public QObject {
    Q_OBJECT

    Q_PROPERTY(QString label READ label CONSTANT)
    Q_PROPERTY(double frequency READ frequency CONSTANT)
    Q_PROPERTY(double gain READ gain WRITE setGain NOTIFY gainChanged)
    Q_PROPERTY(double q READ q CONSTANT)
    Q_PROPERTY(double minGain READ minGain CONSTANT)
    Q_PROPERTY(double maxGain READ maxGain CONSTANT)

public:
    explicit EqBand(const QString& label, double freq, double qVal,
                    QObject* parent = nullptr)
        : QObject(parent), label_(label), frequency_(freq), q_(qVal) {}

    QString label() const { return label_; }
    double frequency() const { return frequency_; }
    double gain() const { return gain_; }
    double q() const { return q_; }
    double minGain() const { return -12.0; }
    double maxGain() const { return 12.0; }

    void setGain(double g) {
        if (qFuzzyCompare(gain_, g)) return;
        gain_ = qBound(-12.0, g, 12.0);
        emit gainChanged();
    }

signals:
    void gainChanged();

private:
    QString label_;
    double frequency_;
    double gain_ = 0.0;
    double q_;
};

} // namespace kivo::qt::audio_bridge::eq
