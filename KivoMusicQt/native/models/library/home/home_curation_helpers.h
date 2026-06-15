// =============================================================================
// Kivo Music Qt - home_curation_helpers.h
// Common helpers for home page curation projections
// =============================================================================

#pragma once

#include "../album_list_model.h"
#include "../../../sources/music/music_file_record.h"

#include <QList>
#include <QSet>

AlbumListItem makeAlbumItem(const MusicFileRecord& record, const QString& note);
AlbumListItem makeTrackItem(const MusicFileRecord& record, const QString& note);
void appendUniqueItem(QList<AlbumListItem>* items, QSet<QString>* keys, const AlbumListItem& item, int limit);
QSet<QString> firstAlbumKeys(const QList<MusicFileRecord>& records, int limit);
bool looksNightRelated(const MusicFileRecord& record);
QString sourceMoodNote(const MusicFileRecord& record, int slot);
QList<AlbumListItem> coveredFirst(QList<AlbumListItem> items);
QList<MusicFileRecord> coveredRecordsFirst(const QList<MusicFileRecord>& records);
QString itemKey(const AlbumListItem& item);
