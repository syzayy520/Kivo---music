// =============================================================================
// Kivo Music Qt - eq_preset.hpp
// 职责: EQ预设定义（Flat/Rock/Pop/Jazz/Classical/Vocal/BassBoost/自定义）
// =============================================================================

#pragma once

#include <QObject>
#include <QString>
#include <QVector>
#include <QMap>

namespace kivo::qt::audio_bridge::eq {

struct EqPresetData {
    QString name;
    QVector<double> gains; // 10 bands: 32,64,125,250,500,1k,2k,4k,8k,16k
};

class EqPresetManager : public QObject {
    Q_OBJECT

public:
    explicit EqPresetManager(QObject* parent = nullptr);

    Q_INVOKABLE QStringList presetNames() const;
    Q_INVOKABLE QVector<double> gainsFor(const QString& name) const;
    Q_INVOKABLE int presetCount() const { return presets_.size(); }

signals:
    void presetChanged(const QString& name);

private:
    void initPresets();
    QMap<QString, EqPresetData> presets_;
};

} // namespace kivo::qt::audio_bridge::eq
