// =============================================================================
// Kivo Music Qt - eq_preset.cpp
// 职责: EQ预设数据初始化
// =============================================================================

#include "eq_preset.hpp"

namespace kivo::qt::audio_bridge::eq {

EqPresetManager::EqPresetManager(QObject* parent) : QObject(parent) {
    initPresets();
}

void EqPresetManager::initPresets() {
    // 10 bands: 32Hz, 64Hz, 125Hz, 250Hz, 500Hz, 1kHz, 2kHz, 4kHz, 8kHz, 16kHz
    presets_["Flat"]      = {"Flat",      { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0}};
    presets_["Rock"]      = {"Rock",      { 5,  4,  2,  0, -1,  2,  4,  3,  2,  1}};
    presets_["Pop"]       = {"Pop",       {-1,  2,  3,  2,  0, -1,  0,  2,  3,  1}};
    presets_["Jazz"]      = {"Jazz",      { 3,  2,  0,  0, -2,  0,  1,  2,  2,  3}};
    presets_["Classical"] = {"Classical", { 4,  3,  1,  0, -1, -2,  0,  2,  3,  4}};
    presets_["Vocal"]     = {"Vocal",     {-2, -1,  0,  3,  4,  2,  0, -1, -2, -3}};
    presets_["Bass Boost"]={"Bass Boost",{ 8,  6,  4,  2,  0,  0,  0,  0,  0,  0}};
    presets_["Treble"]    = {"Treble",    {-3, -2, -1,  0,  1,  2,  3,  5,  7,  9}};
}

QStringList EqPresetManager::presetNames() const {
    return presets_.keys();
}

QVector<double> EqPresetManager::gainsFor(const QString& name) const {
    if (presets_.contains(name))
        return presets_[name].gains;
    return presets_["Flat"].gains;
}

} // namespace kivo::qt::audio_bridge::eq
