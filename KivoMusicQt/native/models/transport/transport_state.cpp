#include "transport_state.h"

TransportState::TransportState(QObject* parent)
    : QObject(parent),
      m_title("Signal in the Dark"),
      m_subtitle("Luna Orbit - Nocturne Signals"),
      m_formatText("FLAC 24-bit / 96kHz"),
      m_elapsedText("1:42"),
      m_durationText("4:37"),
      m_progress(0.38),
      m_volume(0.62),
      m_artVariant(0),
      m_playing(true) {}

QString TransportState::title() const {
    return m_title;
}

QString TransportState::subtitle() const {
    return m_subtitle;
}

QString TransportState::formatText() const {
    return m_formatText;
}

QString TransportState::elapsedText() const {
    return m_elapsedText;
}

QString TransportState::durationText() const {
    return m_durationText;
}

double TransportState::progress() const {
    return m_progress;
}

double TransportState::volume() const {
    return m_volume;
}

int TransportState::artVariant() const {
    return m_artVariant;
}

bool TransportState::playing() const {
    return m_playing;
}
