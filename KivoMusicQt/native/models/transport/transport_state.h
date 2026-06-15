#pragma once

#include <QObject>
#include <QString>

class TransportState final : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString title READ title NOTIFY changed)
    Q_PROPERTY(QString subtitle READ subtitle NOTIFY changed)
    Q_PROPERTY(QString formatText READ formatText NOTIFY changed)
    Q_PROPERTY(QString elapsedText READ elapsedText NOTIFY changed)
    Q_PROPERTY(QString durationText READ durationText NOTIFY changed)
    Q_PROPERTY(double progress READ progress NOTIFY changed)
    Q_PROPERTY(double volume READ volume NOTIFY changed)
    Q_PROPERTY(int artVariant READ artVariant NOTIFY changed)
    Q_PROPERTY(bool playing READ playing NOTIFY changed)

public:
    explicit TransportState(QObject* parent = nullptr);

    QString title() const;
    QString subtitle() const;
    QString formatText() const;
    QString elapsedText() const;
    QString durationText() const;
    double progress() const;
    double volume() const;
    int artVariant() const;
    bool playing() const;

signals:
    void changed();

private:
    QString m_title;
    QString m_subtitle;
    QString m_formatText;
    QString m_elapsedText;
    QString m_durationText;
    double m_progress = 0.0;
    double m_volume = 0.0;
    int m_artVariant = 0;
    bool m_playing = false;
};
