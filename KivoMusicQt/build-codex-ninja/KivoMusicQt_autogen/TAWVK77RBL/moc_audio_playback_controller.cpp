/****************************************************************************
** Meta object code from reading C++ file 'audio_playback_controller.hpp'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../native/audio_core_bridge/audio_playback_controller.hpp"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'audio_playback_controller.hpp' doesn't include <QObject>."
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
struct qt_meta_tag_ZN4kivo2qt12audio_bridge23AudioPlaybackControllerE_t {};
} // unnamed namespace


#ifdef QT_MOC_HAS_STRINGDATA
static constexpr auto qt_meta_stringdata_ZN4kivo2qt12audio_bridge23AudioPlaybackControllerE = QtMocHelpers::stringData(
    "kivo::qt::audio_bridge::AudioPlaybackController",
    "titleChanged",
    "",
    "artistChanged",
    "albumChanged",
    "formatTextChanged",
    "elapsedTextChanged",
    "durationTextChanged",
    "progressChanged",
    "volumeChanged",
    "playingChanged",
    "loadingChanged",
    "bitPerfectStatusChanged",
    "errorOccurred",
    "message",
    "endOfStream",
    "fileOpened",
    "openFile",
    "filePath",
    "play",
    "pause",
    "togglePlayPause",
    "stop",
    "seekTo",
    "progress",
    "skipForward",
    "seconds",
    "skipBackward",
    "title",
    "artist",
    "album",
    "formatText",
    "elapsedText",
    "durationText",
    "volume",
    "playing",
    "loading",
    "bitPerfectStatus"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA

Q_CONSTINIT static const uint qt_meta_data_ZN4kivo2qt12audio_bridge23AudioPlaybackControllerE[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      24,   14, // methods
      11,  192, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      14,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,  158,    2, 0x06,   12 /* Public */,
       3,    0,  159,    2, 0x06,   13 /* Public */,
       4,    0,  160,    2, 0x06,   14 /* Public */,
       5,    0,  161,    2, 0x06,   15 /* Public */,
       6,    0,  162,    2, 0x06,   16 /* Public */,
       7,    0,  163,    2, 0x06,   17 /* Public */,
       8,    0,  164,    2, 0x06,   18 /* Public */,
       9,    0,  165,    2, 0x06,   19 /* Public */,
      10,    0,  166,    2, 0x06,   20 /* Public */,
      11,    0,  167,    2, 0x06,   21 /* Public */,
      12,    0,  168,    2, 0x06,   22 /* Public */,
      13,    1,  169,    2, 0x06,   23 /* Public */,
      15,    0,  172,    2, 0x06,   25 /* Public */,
      16,    0,  173,    2, 0x06,   26 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      17,    1,  174,    2, 0x0a,   27 /* Public */,
      19,    0,  177,    2, 0x0a,   29 /* Public */,
      20,    0,  178,    2, 0x0a,   30 /* Public */,
      21,    0,  179,    2, 0x0a,   31 /* Public */,
      22,    0,  180,    2, 0x0a,   32 /* Public */,
      23,    1,  181,    2, 0x0a,   33 /* Public */,
      25,    1,  184,    2, 0x0a,   35 /* Public */,
      25,    0,  187,    2, 0x2a,   37 /* Public | MethodCloned */,
      27,    1,  188,    2, 0x0a,   38 /* Public */,
      27,    0,  191,    2, 0x2a,   40 /* Public | MethodCloned */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   14,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,   18,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Double,   24,
    QMetaType::Void, QMetaType::Int,   26,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   26,
    QMetaType::Void,

 // properties: name, type, flags, notifyId, revision
      28, QMetaType::QString, 0x00015001, uint(0), 0,
      29, QMetaType::QString, 0x00015001, uint(1), 0,
      30, QMetaType::QString, 0x00015001, uint(2), 0,
      31, QMetaType::QString, 0x00015001, uint(3), 0,
      32, QMetaType::QString, 0x00015001, uint(4), 0,
      33, QMetaType::QString, 0x00015001, uint(5), 0,
      24, QMetaType::Double, 0x00015001, uint(6), 0,
      34, QMetaType::Double, 0x00015103, uint(7), 0,
      35, QMetaType::Bool, 0x00015001, uint(8), 0,
      36, QMetaType::Bool, 0x00015001, uint(9), 0,
      37, QMetaType::QString, 0x00015001, uint(10), 0,

       0        // eod
};

Q_CONSTINIT const QMetaObject kivo::qt::audio_bridge::AudioPlaybackController::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_ZN4kivo2qt12audio_bridge23AudioPlaybackControllerE.offsetsAndSizes,
    qt_meta_data_ZN4kivo2qt12audio_bridge23AudioPlaybackControllerE,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_tag_ZN4kivo2qt12audio_bridge23AudioPlaybackControllerE_t,
        // property 'title'
        QtPrivate::TypeAndForceComplete<QString, std::true_type>,
        // property 'artist'
        QtPrivate::TypeAndForceComplete<QString, std::true_type>,
        // property 'album'
        QtPrivate::TypeAndForceComplete<QString, std::true_type>,
        // property 'formatText'
        QtPrivate::TypeAndForceComplete<QString, std::true_type>,
        // property 'elapsedText'
        QtPrivate::TypeAndForceComplete<QString, std::true_type>,
        // property 'durationText'
        QtPrivate::TypeAndForceComplete<QString, std::true_type>,
        // property 'progress'
        QtPrivate::TypeAndForceComplete<double, std::true_type>,
        // property 'volume'
        QtPrivate::TypeAndForceComplete<double, std::true_type>,
        // property 'playing'
        QtPrivate::TypeAndForceComplete<bool, std::true_type>,
        // property 'loading'
        QtPrivate::TypeAndForceComplete<bool, std::true_type>,
        // property 'bitPerfectStatus'
        QtPrivate::TypeAndForceComplete<QString, std::true_type>,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<AudioPlaybackController, std::true_type>,
        // method 'titleChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'artistChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'albumChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'formatTextChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'elapsedTextChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'durationTextChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'progressChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'volumeChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'playingChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'loadingChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'bitPerfectStatusChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'errorOccurred'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'endOfStream'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'fileOpened'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'openFile'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'play'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'pause'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'togglePlayPause'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'stop'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'seekTo'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<double, std::false_type>,
        // method 'skipForward'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'skipForward'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'skipBackward'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'skipBackward'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void kivo::qt::audio_bridge::AudioPlaybackController::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<AudioPlaybackController *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->titleChanged(); break;
        case 1: _t->artistChanged(); break;
        case 2: _t->albumChanged(); break;
        case 3: _t->formatTextChanged(); break;
        case 4: _t->elapsedTextChanged(); break;
        case 5: _t->durationTextChanged(); break;
        case 6: _t->progressChanged(); break;
        case 7: _t->volumeChanged(); break;
        case 8: _t->playingChanged(); break;
        case 9: _t->loadingChanged(); break;
        case 10: _t->bitPerfectStatusChanged(); break;
        case 11: _t->errorOccurred((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 12: _t->endOfStream(); break;
        case 13: _t->fileOpened(); break;
        case 14: _t->openFile((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 15: _t->play(); break;
        case 16: _t->pause(); break;
        case 17: _t->togglePlayPause(); break;
        case 18: _t->stop(); break;
        case 19: _t->seekTo((*reinterpret_cast< std::add_pointer_t<double>>(_a[1]))); break;
        case 20: _t->skipForward((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 21: _t->skipForward(); break;
        case 22: _t->skipBackward((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 23: _t->skipBackward(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _q_method_type = void (AudioPlaybackController::*)();
            if (_q_method_type _q_method = &AudioPlaybackController::titleChanged; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _q_method_type = void (AudioPlaybackController::*)();
            if (_q_method_type _q_method = &AudioPlaybackController::artistChanged; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _q_method_type = void (AudioPlaybackController::*)();
            if (_q_method_type _q_method = &AudioPlaybackController::albumChanged; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _q_method_type = void (AudioPlaybackController::*)();
            if (_q_method_type _q_method = &AudioPlaybackController::formatTextChanged; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
        {
            using _q_method_type = void (AudioPlaybackController::*)();
            if (_q_method_type _q_method = &AudioPlaybackController::elapsedTextChanged; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
        {
            using _q_method_type = void (AudioPlaybackController::*)();
            if (_q_method_type _q_method = &AudioPlaybackController::durationTextChanged; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 5;
                return;
            }
        }
        {
            using _q_method_type = void (AudioPlaybackController::*)();
            if (_q_method_type _q_method = &AudioPlaybackController::progressChanged; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 6;
                return;
            }
        }
        {
            using _q_method_type = void (AudioPlaybackController::*)();
            if (_q_method_type _q_method = &AudioPlaybackController::volumeChanged; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 7;
                return;
            }
        }
        {
            using _q_method_type = void (AudioPlaybackController::*)();
            if (_q_method_type _q_method = &AudioPlaybackController::playingChanged; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 8;
                return;
            }
        }
        {
            using _q_method_type = void (AudioPlaybackController::*)();
            if (_q_method_type _q_method = &AudioPlaybackController::loadingChanged; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 9;
                return;
            }
        }
        {
            using _q_method_type = void (AudioPlaybackController::*)();
            if (_q_method_type _q_method = &AudioPlaybackController::bitPerfectStatusChanged; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 10;
                return;
            }
        }
        {
            using _q_method_type = void (AudioPlaybackController::*)(const QString & );
            if (_q_method_type _q_method = &AudioPlaybackController::errorOccurred; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 11;
                return;
            }
        }
        {
            using _q_method_type = void (AudioPlaybackController::*)();
            if (_q_method_type _q_method = &AudioPlaybackController::endOfStream; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 12;
                return;
            }
        }
        {
            using _q_method_type = void (AudioPlaybackController::*)();
            if (_q_method_type _q_method = &AudioPlaybackController::fileOpened; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 13;
                return;
            }
        }
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = _t->title(); break;
        case 1: *reinterpret_cast< QString*>(_v) = _t->artist(); break;
        case 2: *reinterpret_cast< QString*>(_v) = _t->album(); break;
        case 3: *reinterpret_cast< QString*>(_v) = _t->formatText(); break;
        case 4: *reinterpret_cast< QString*>(_v) = _t->elapsedText(); break;
        case 5: *reinterpret_cast< QString*>(_v) = _t->durationText(); break;
        case 6: *reinterpret_cast< double*>(_v) = _t->progress(); break;
        case 7: *reinterpret_cast< double*>(_v) = _t->volume(); break;
        case 8: *reinterpret_cast< bool*>(_v) = _t->isPlaying(); break;
        case 9: *reinterpret_cast< bool*>(_v) = _t->isLoading(); break;
        case 10: *reinterpret_cast< QString*>(_v) = _t->bitPerfectStatus(); break;
        default: break;
        }
    }
    if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 7: _t->setVolume(*reinterpret_cast< double*>(_v)); break;
        default: break;
        }
    }
}

const QMetaObject *kivo::qt::audio_bridge::AudioPlaybackController::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *kivo::qt::audio_bridge::AudioPlaybackController::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ZN4kivo2qt12audio_bridge23AudioPlaybackControllerE.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int kivo::qt::audio_bridge::AudioPlaybackController::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 24)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 24;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 24)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 24;
    }
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    }
    return _id;
}

// SIGNAL 0
void kivo::qt::audio_bridge::AudioPlaybackController::titleChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void kivo::qt::audio_bridge::AudioPlaybackController::artistChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void kivo::qt::audio_bridge::AudioPlaybackController::albumChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void kivo::qt::audio_bridge::AudioPlaybackController::formatTextChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void kivo::qt::audio_bridge::AudioPlaybackController::elapsedTextChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void kivo::qt::audio_bridge::AudioPlaybackController::durationTextChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void kivo::qt::audio_bridge::AudioPlaybackController::progressChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}

// SIGNAL 7
void kivo::qt::audio_bridge::AudioPlaybackController::volumeChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 7, nullptr);
}

// SIGNAL 8
void kivo::qt::audio_bridge::AudioPlaybackController::playingChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 8, nullptr);
}

// SIGNAL 9
void kivo::qt::audio_bridge::AudioPlaybackController::loadingChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 9, nullptr);
}

// SIGNAL 10
void kivo::qt::audio_bridge::AudioPlaybackController::bitPerfectStatusChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 10, nullptr);
}

// SIGNAL 11
void kivo::qt::audio_bridge::AudioPlaybackController::errorOccurred(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 11, _a);
}

// SIGNAL 12
void kivo::qt::audio_bridge::AudioPlaybackController::endOfStream()
{
    QMetaObject::activate(this, &staticMetaObject, 12, nullptr);
}

// SIGNAL 13
void kivo::qt::audio_bridge::AudioPlaybackController::fileOpened()
{
    QMetaObject::activate(this, &staticMetaObject, 13, nullptr);
}
QT_WARNING_POP
