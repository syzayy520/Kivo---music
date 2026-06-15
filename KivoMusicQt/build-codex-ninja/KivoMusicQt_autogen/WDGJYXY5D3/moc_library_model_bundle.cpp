/****************************************************************************
** Meta object code from reading C++ file 'library_model_bundle.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../native/models/library/library_model_bundle.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'library_model_bundle.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN18LibraryModelBundleE_t {};
} // unnamed namespace


#ifdef QT_MOC_HAS_STRINGDATA
static constexpr auto qt_meta_stringdata_ZN18LibraryModelBundleE = QtMocHelpers::stringData(
    "LibraryModelBundle",
    "albums",
    "AlbumListModel*",
    "topPicks",
    "pins",
    "continueListening",
    "recent",
    "rediscover",
    "afterHours",
    "artists",
    "ArtistListModel*",
    "songs",
    "TrackListModel*",
    "favorites",
    "focus",
    "videos"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA

Q_CONSTINIT static const uint qt_meta_data_ZN18LibraryModelBundleE[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
      12,   14, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // properties: name, type, flags, notifyId, revision
       1, 0x80000000 | 2, 0x00015409, uint(-1), 0,
       3, 0x80000000 | 2, 0x00015409, uint(-1), 0,
       4, 0x80000000 | 2, 0x00015409, uint(-1), 0,
       5, 0x80000000 | 2, 0x00015409, uint(-1), 0,
       6, 0x80000000 | 2, 0x00015409, uint(-1), 0,
       7, 0x80000000 | 2, 0x00015409, uint(-1), 0,
       8, 0x80000000 | 2, 0x00015409, uint(-1), 0,
       9, 0x80000000 | 10, 0x00015409, uint(-1), 0,
      11, 0x80000000 | 12, 0x00015409, uint(-1), 0,
      13, 0x80000000 | 12, 0x00015409, uint(-1), 0,
      14, 0x80000000 | 12, 0x00015409, uint(-1), 0,
      15, 0x80000000 | 12, 0x00015409, uint(-1), 0,

       0        // eod
};

Q_CONSTINIT const QMetaObject LibraryModelBundle::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_ZN18LibraryModelBundleE.offsetsAndSizes,
    qt_meta_data_ZN18LibraryModelBundleE,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_tag_ZN18LibraryModelBundleE_t,
        // property 'albums'
        QtPrivate::TypeAndForceComplete<AlbumListModel*, std::true_type>,
        // property 'topPicks'
        QtPrivate::TypeAndForceComplete<AlbumListModel*, std::true_type>,
        // property 'pins'
        QtPrivate::TypeAndForceComplete<AlbumListModel*, std::true_type>,
        // property 'continueListening'
        QtPrivate::TypeAndForceComplete<AlbumListModel*, std::true_type>,
        // property 'recent'
        QtPrivate::TypeAndForceComplete<AlbumListModel*, std::true_type>,
        // property 'rediscover'
        QtPrivate::TypeAndForceComplete<AlbumListModel*, std::true_type>,
        // property 'afterHours'
        QtPrivate::TypeAndForceComplete<AlbumListModel*, std::true_type>,
        // property 'artists'
        QtPrivate::TypeAndForceComplete<ArtistListModel*, std::true_type>,
        // property 'songs'
        QtPrivate::TypeAndForceComplete<TrackListModel*, std::true_type>,
        // property 'favorites'
        QtPrivate::TypeAndForceComplete<TrackListModel*, std::true_type>,
        // property 'focus'
        QtPrivate::TypeAndForceComplete<TrackListModel*, std::true_type>,
        // property 'videos'
        QtPrivate::TypeAndForceComplete<TrackListModel*, std::true_type>,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<LibraryModelBundle, std::true_type>
    >,
    nullptr
} };

void LibraryModelBundle::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<LibraryModelBundle *>(_o);
    if (_c == QMetaObject::RegisterPropertyMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 6:
        case 5:
        case 4:
        case 3:
        case 2:
        case 1:
        case 0:
            *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< AlbumListModel* >(); break;
        case 7:
            *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< ArtistListModel* >(); break;
        case 11:
        case 10:
        case 9:
        case 8:
            *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< TrackListModel* >(); break;
        }
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< AlbumListModel**>(_v) = _t->albums(); break;
        case 1: *reinterpret_cast< AlbumListModel**>(_v) = _t->topPicks(); break;
        case 2: *reinterpret_cast< AlbumListModel**>(_v) = _t->pins(); break;
        case 3: *reinterpret_cast< AlbumListModel**>(_v) = _t->continueListening(); break;
        case 4: *reinterpret_cast< AlbumListModel**>(_v) = _t->recent(); break;
        case 5: *reinterpret_cast< AlbumListModel**>(_v) = _t->rediscover(); break;
        case 6: *reinterpret_cast< AlbumListModel**>(_v) = _t->afterHours(); break;
        case 7: *reinterpret_cast< ArtistListModel**>(_v) = _t->artists(); break;
        case 8: *reinterpret_cast< TrackListModel**>(_v) = _t->songs(); break;
        case 9: *reinterpret_cast< TrackListModel**>(_v) = _t->favorites(); break;
        case 10: *reinterpret_cast< TrackListModel**>(_v) = _t->focus(); break;
        case 11: *reinterpret_cast< TrackListModel**>(_v) = _t->videos(); break;
        default: break;
        }
    }
}

const QMetaObject *LibraryModelBundle::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *LibraryModelBundle::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ZN18LibraryModelBundleE.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int LibraryModelBundle::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    }
    return _id;
}
QT_WARNING_POP
