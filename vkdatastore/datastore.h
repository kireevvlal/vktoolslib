#ifndef DATASTORE_H
#define DATASTORE_H
#include <QMap>
#include "threadserialport.h"
#include "vktoolstypes.h"

class DataStore
{
private:
    // словари по разным типам данных
    QMap<QString, bool> _bits_map;
    QMap<QString, qint8> _bytes_map;
    QMap<QString, quint8> _ubytes_map;
    QMap<QString, qint16> _int16_map;
    QMap<QString, quint16> _uint16_map;
    QMap<QString, qint32> _int32_map;
    QMap<QString, quint32> _uint32_map;
    QMap<QString, float> _float_map;
    QMap<QString, double> _double_map;
public:
    DataStore();
    // maps methods
    void FillMaps(ThreadSerialPort*);
    QStringList OutMaps();
    bool Exist(QString, DataType);
    bool Add(QString, DataType);
    int LoadSpData(ThreadSerialPort*);
    // get value
    bool Bit(QString key) { return _bits_map.contains(key) ? _bits_map[key] : false; }
    qint8 Byte(QString key) { return _bytes_map.contains(key) ? _bytes_map[key] : 0; }
    quint8 UByte(QString key) { return _ubytes_map.contains(key) ? _ubytes_map[key] : 0; }
    qint16 Int16(QString key) { return _int16_map.contains(key) ? _int16_map[key] : 0; }
    quint16 UInt16(QString key) { return _uint16_map.contains(key) ? _uint16_map[key] : 0; }
    qint32 Int32(QString key) { return _int32_map.contains(key) ? _int32_map[key] : 0; }
    quint32 UInt32(QString key) { return _uint32_map.contains(key) ? _uint32_map[key] : 0; }
    float Float(QString key) { return _float_map.contains(key) ? _float_map[key] : 0; }
    // set value
    bool Bit(QString key, bool value);
    bool Byte(QString key, qint8 value);
    bool UByte(QString key, quint8 value);
    bool Int16(QString key, qint16 value);
    bool UInt16(QString key, quint16 value);
    bool Int32(QString key, qint32 value);
    bool UInt32(QString key, quint32 value);
    bool Float(QString key, float value);
    // get map
    inline QMap<QString, bool> BitsMap() { return _bits_map; }
    inline QMap<QString, qint8> BytesMap() { return _bytes_map; }
    inline QMap<QString, quint8> UBytesMap() { return _ubytes_map; }
    inline QMap<QString, qint16> Int16Map() { return _int16_map; }
    inline QMap<QString, quint16> UInt16Map() { return _uint16_map; }
    inline QMap<QString, qint32> Int32Map() { return _int32_map; }
    inline QMap<QString, quint32> UInt32Map() { return _uint32_map; }
    inline QMap<QString, float> FloatMap() { return _float_map; }
    inline QMap<QString, double> DoubleMap() { return _double_map; }
};

#endif // DATASTORE_H

