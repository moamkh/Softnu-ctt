/****************************************************************************
** Meta object code from reading C++ file 'AmiClient.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.17)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../src/services/CppAmi/core/AmiClient.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AmiClient.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.17. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_AmiClient_t {
    QByteArrayData data[15];
    char stringdata0[184];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_AmiClient_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_AmiClient_t qt_meta_stringdata_AmiClient = {
    {
QT_MOC_LITERAL(0, 0, 9), // "AmiClient"
QT_MOC_LITERAL(1, 10, 9), // "connected"
QT_MOC_LITERAL(2, 20, 0), // ""
QT_MOC_LITERAL(3, 21, 12), // "disconnected"
QT_MOC_LITERAL(4, 34, 5), // "error"
QT_MOC_LITERAL(5, 40, 12), // "errorMessage"
QT_MOC_LITERAL(6, 53, 13), // "eventReceived"
QT_MOC_LITERAL(7, 67, 9), // "eventData"
QT_MOC_LITERAL(8, 77, 11), // "onConnected"
QT_MOC_LITERAL(9, 89, 14), // "onDisconnected"
QT_MOC_LITERAL(10, 104, 7), // "onError"
QT_MOC_LITERAL(11, 112, 28), // "QAbstractSocket::SocketError"
QT_MOC_LITERAL(12, 141, 11), // "socketError"
QT_MOC_LITERAL(13, 153, 11), // "onReadyRead"
QT_MOC_LITERAL(14, 165, 18) // "onReconnectTimeout"

    },
    "AmiClient\0connected\0\0disconnected\0"
    "error\0errorMessage\0eventReceived\0"
    "eventData\0onConnected\0onDisconnected\0"
    "onError\0QAbstractSocket::SocketError\0"
    "socketError\0onReadyRead\0onReconnectTimeout"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_AmiClient[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   59,    2, 0x06 /* Public */,
       3,    0,   60,    2, 0x06 /* Public */,
       4,    1,   61,    2, 0x06 /* Public */,
       6,    1,   64,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       8,    0,   67,    2, 0x08 /* Private */,
       9,    0,   68,    2, 0x08 /* Private */,
      10,    1,   69,    2, 0x08 /* Private */,
      13,    0,   72,    2, 0x08 /* Private */,
      14,    0,   73,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void, QMetaType::QJsonObject,    7,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 11,   12,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void AmiClient::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<AmiClient *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->connected(); break;
        case 1: _t->disconnected(); break;
        case 2: _t->error((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->eventReceived((*reinterpret_cast< const QJsonObject(*)>(_a[1]))); break;
        case 4: _t->onConnected(); break;
        case 5: _t->onDisconnected(); break;
        case 6: _t->onError((*reinterpret_cast< QAbstractSocket::SocketError(*)>(_a[1]))); break;
        case 7: _t->onReadyRead(); break;
        case 8: _t->onReconnectTimeout(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 6:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QAbstractSocket::SocketError >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (AmiClient::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&AmiClient::connected)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (AmiClient::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&AmiClient::disconnected)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (AmiClient::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&AmiClient::error)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (AmiClient::*)(const QJsonObject & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&AmiClient::eventReceived)) {
                *result = 3;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject AmiClient::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_AmiClient.data,
    qt_meta_data_AmiClient,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *AmiClient::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AmiClient::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_AmiClient.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int AmiClient::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void AmiClient::connected()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void AmiClient::disconnected()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void AmiClient::error(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void AmiClient::eventReceived(const QJsonObject & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
