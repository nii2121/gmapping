/****************************************************************************
** QParticleViewer meta object code from reading C++ file 'qparticleviewer.h'
**
** Created: Wed Nov 18 19:51:13 2009
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.6   edited Mar 8 17:43 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "qparticleviewer.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *QParticleViewer::className() const
{
    return "QParticleViewer";
}

QMetaObject *QParticleViewer::metaObj = 0;
static QMetaObjectCleanUp cleanUp_QParticleViewer( "QParticleViewer", &QParticleViewer::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString QParticleViewer::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "QParticleViewer", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString QParticleViewer::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "QParticleViewer", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* QParticleViewer::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QWidget::staticMetaObject();
    static const QUParameter param_slot_0[] = {
	{ "mp", &static_QUType_ptr, "MatchingParameters", QUParameter::In }
    };
    static const QUMethod slot_0 = {"setMatchingParameters", 1, param_slot_0 };
    static const QUParameter param_slot_1[] = {
	{ "mp", &static_QUType_ptr, "StartParameters", QUParameter::In }
    };
    static const QUMethod slot_1 = {"setStartParameters", 1, param_slot_1 };
    static const QUMethod slot_2 = {"start", 0, 0 };
    static const QUMethod slot_3 = {"stop", 0, 0 };
    static const QUParameter param_slot_4[] = {
	{ 0, &static_QUType_charstar, 0, QUParameter::In }
    };
    static const QUMethod slot_4 = {"loadFile", 1, param_slot_4 };
    static const QUParameter param_slot_5[] = {
	{ 0, &static_QUType_int, 0, QUParameter::In },
	{ 0, &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_5 = {"received", 2, param_slot_5 };
    static const QUParameter param_slot_6[] = {
	{ 0, &static_QUType_int, 0, QUParameter::In },
	{ 0, &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_6 = {"getabsolute", 2, param_slot_6 };
    static const QMetaData slot_tbl[] = {
	{ "setMatchingParameters(const MatchingParameters&)", &slot_0, QMetaData::Public },
	{ "setStartParameters(const StartParameters&)", &slot_1, QMetaData::Public },
	{ "start()", &slot_2, QMetaData::Public },
	{ "stop()", &slot_3, QMetaData::Public },
	{ "loadFile(const char*)", &slot_4, QMetaData::Public },
	{ "received(int,int)", &slot_5, QMetaData::Public },
	{ "getabsolute(int,int)", &slot_6, QMetaData::Public }
    };
    static const QUParameter param_signal_0[] = {
	{ 0, &static_QUType_double, 0, QUParameter::In }
    };
    static const QUMethod signal_0 = {"neffChanged", 1, param_signal_0 };
    static const QUParameter param_signal_1[] = {
	{ 0, &static_QUType_double, 0, QUParameter::In },
	{ 0, &static_QUType_double, 0, QUParameter::In },
	{ 0, &static_QUType_double, 0, QUParameter::In }
    };
    static const QUMethod signal_1 = {"poseEntropyChanged", 3, param_signal_1 };
    static const QUParameter param_signal_2[] = {
	{ 0, &static_QUType_double, 0, QUParameter::In },
	{ 0, &static_QUType_double, 0, QUParameter::In },
	{ 0, &static_QUType_double, 0, QUParameter::In }
    };
    static const QUMethod signal_2 = {"trajectoryEntropyChanged", 3, param_signal_2 };
    static const QUParameter param_signal_3[] = {
	{ 0, &static_QUType_double, 0, QUParameter::In }
    };
    static const QUMethod signal_3 = {"mapsEntropyChanged", 1, param_signal_3 };
    static const QUParameter param_signal_4[] = {
	{ 0, &static_QUType_double, 0, QUParameter::In }
    };
    static const QUMethod signal_4 = {"mapsIGainChanged", 1, param_signal_4 };
    static const QUParameter param_signal_5[] = {
	{ 0, &static_QUType_int, 0, QUParameter::In },
	{ 0, &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod signal_5 = {"valueChanged", 2, param_signal_5 };
    static const QMetaData signal_tbl[] = {
	{ "neffChanged(double)", &signal_0, QMetaData::Public },
	{ "poseEntropyChanged(double,double,double)", &signal_1, QMetaData::Public },
	{ "trajectoryEntropyChanged(double,double,double)", &signal_2, QMetaData::Public },
	{ "mapsEntropyChanged(double)", &signal_3, QMetaData::Public },
	{ "mapsIGainChanged(double)", &signal_4, QMetaData::Public },
	{ "valueChanged(int,int)", &signal_5, QMetaData::Public }
    };
    metaObj = QMetaObject::new_metaobject(
	"QParticleViewer", parentObject,
	slot_tbl, 7,
	signal_tbl, 6,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_QParticleViewer.setMetaObject( metaObj );
    return metaObj;
}

void* QParticleViewer::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "QParticleViewer" ) )
	return this;
    return QWidget::qt_cast( clname );
}

// SIGNAL neffChanged
void QParticleViewer::neffChanged( double t0 )
{
    activate_signal( staticMetaObject()->signalOffset() + 0, t0 );
}

#include <qobjectdefs.h>
#include <qsignalslotimp.h>

// SIGNAL poseEntropyChanged
void QParticleViewer::poseEntropyChanged( double t0, double t1, double t2 )
{
    if ( signalsBlocked() )
	return;
    QConnectionList *clist = receivers( staticMetaObject()->signalOffset() + 1 );
    if ( !clist )
	return;
    QUObject o[4];
    static_QUType_double.set(o+1,t0);
    static_QUType_double.set(o+2,t1);
    static_QUType_double.set(o+3,t2);
    activate_signal( clist, o );
}

// SIGNAL trajectoryEntropyChanged
void QParticleViewer::trajectoryEntropyChanged( double t0, double t1, double t2 )
{
    if ( signalsBlocked() )
	return;
    QConnectionList *clist = receivers( staticMetaObject()->signalOffset() + 2 );
    if ( !clist )
	return;
    QUObject o[4];
    static_QUType_double.set(o+1,t0);
    static_QUType_double.set(o+2,t1);
    static_QUType_double.set(o+3,t2);
    activate_signal( clist, o );
}

// SIGNAL mapsEntropyChanged
void QParticleViewer::mapsEntropyChanged( double t0 )
{
    activate_signal( staticMetaObject()->signalOffset() + 3, t0 );
}

// SIGNAL mapsIGainChanged
void QParticleViewer::mapsIGainChanged( double t0 )
{
    activate_signal( staticMetaObject()->signalOffset() + 4, t0 );
}

// SIGNAL valueChanged
void QParticleViewer::valueChanged( int t0, int t1 )
{
    if ( signalsBlocked() )
	return;
    QConnectionList *clist = receivers( staticMetaObject()->signalOffset() + 5 );
    if ( !clist )
	return;
    QUObject o[3];
    static_QUType_int.set(o+1,t0);
    static_QUType_int.set(o+2,t1);
    activate_signal( clist, o );
}

bool QParticleViewer::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: setMatchingParameters((const MatchingParameters&)*((const MatchingParameters*)static_QUType_ptr.get(_o+1))); break;
    case 1: setStartParameters((const StartParameters&)*((const StartParameters*)static_QUType_ptr.get(_o+1))); break;
    case 2: start(); break;
    case 3: stop(); break;
    case 4: loadFile((const char*)static_QUType_charstar.get(_o+1)); break;
    case 5: received((int)static_QUType_int.get(_o+1),(int)static_QUType_int.get(_o+2)); break;
    case 6: getabsolute((int)static_QUType_int.get(_o+1),(int)static_QUType_int.get(_o+2)); break;
    default:
	return QWidget::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool QParticleViewer::qt_emit( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->signalOffset() ) {
    case 0: neffChanged((double)static_QUType_double.get(_o+1)); break;
    case 1: poseEntropyChanged((double)static_QUType_double.get(_o+1),(double)static_QUType_double.get(_o+2),(double)static_QUType_double.get(_o+3)); break;
    case 2: trajectoryEntropyChanged((double)static_QUType_double.get(_o+1),(double)static_QUType_double.get(_o+2),(double)static_QUType_double.get(_o+3)); break;
    case 3: mapsEntropyChanged((double)static_QUType_double.get(_o+1)); break;
    case 4: mapsIGainChanged((double)static_QUType_double.get(_o+1)); break;
    case 5: valueChanged((int)static_QUType_int.get(_o+1),(int)static_QUType_int.get(_o+2)); break;
    default:
	return QWidget::qt_emit(_id,_o);
    }
    return TRUE;
}
#ifndef QT_NO_PROPERTIES

bool QParticleViewer::qt_property( int id, int f, QVariant* v)
{
    return QWidget::qt_property( id, f, v);
}

bool QParticleViewer::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
