// =============================================================================
// Kivo Music Qt - smb_discovery_controller.hpp
// 职责: QML 侧的局域网 SMB 发现门面（context property "discovery"）
// 线程: UI 线程对象；持有一个工作线程执行扫描，结果以队列信号回到 UI 线程。
// =============================================================================

#pragma once

#include <QObject>
#include <QString>
#include <QVariantList>

class QThread;

namespace kivo::qt::sources {

class SmbDiscoveryWorker;
class SourceManagerController;

// Drives LAN SMB auto-discovery for the Settings UI. Owns a dedicated worker
// thread; discovered shares stream in via results. Adding a discovered share
// delegates to the SourceManagerController (so it persists exactly like a
// manually-typed network folder).
class SmbDiscoveryController final : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool scanning READ scanning NOTIFY scanningChanged)
    Q_PROPERTY(QString statusText READ statusText NOTIFY statusChanged)
    // Each entry: { host, share, path, added }.
    Q_PROPERTY(QVariantList results READ results NOTIFY resultsChanged)

public:
    explicit SmbDiscoveryController(
        SourceManagerController* sources,
        QObject* parent = nullptr);
    ~SmbDiscoveryController() override;

    [[nodiscard]] bool scanning() const { return scanning_; }
    [[nodiscard]] QString statusText() const { return statusText_; }
    [[nodiscard]] QVariantList results() const { return results_; }

    // Start a fresh LAN scan (no-op if one is already running).
    Q_INVOKABLE void startScan();
    // Abort the in-flight scan.
    Q_INVOKABLE void cancelScan();
    // Persist a discovered share (by index in `results`) as a music source.
    Q_INVOKABLE void addDiscovered(int index);

signals:
    void scanningChanged();
    void statusChanged();
    void resultsChanged();

private:
    void onShareFound(const QString& host, const QString& share, const QString& uncPath);
    void onFinished(int hostsReachable, int sharesFound);
    void setScanning(bool value);
    void setStatus(const QString& text);
    [[nodiscard]] bool alreadyConfigured(const QString& uncPath) const;

    SourceManagerController* sources_ = nullptr;
    QThread* workerThread_ = nullptr;
    SmbDiscoveryWorker* worker_ = nullptr;
    QVariantList results_;
    bool scanning_ = false;
    QString statusText_;
};

} // namespace kivo::qt::sources
