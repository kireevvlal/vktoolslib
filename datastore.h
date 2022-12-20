#ifndef DATASTORE_H
#define DATASTORE_H
#include <QMap>
#include <QBitArray>
#include "threadserialport.h"
#include "vktoolstypes.h"

class DataStore
{
private:
    // словари по разным типам данных
    QMap<QString, QBitArray*> _bits_map;
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
    bool Add(QString, Parameter*);
    void Normalize(Parameter*, QByteArray);
    void LoadSpData(ThreadSerialPort*);
    // get value
    bool Bit(QString key, int index) { return _bits_map.contains(key) ? _bits_map[key]->testBit(index) : false; }
    QBitArray* Bits(QString key) { return _bits_map.contains(key) ? _bits_map[key] : false; }
    qint8 Byte(QString key) { return _bytes_map.contains(key) ? _bytes_map[key] : 0; }
    quint8 UByte(QString key) { return _ubytes_map.contains(key) ? _ubytes_map[key] : 0; }
    qint16 Int16(QString key) { return _int16_map.contains(key) ? _int16_map[key] : 0; }
    quint16 UInt16(QString key) { return _uint16_map.contains(key) ? _uint16_map[key] : 0; }
    qint32 Int32(QString key) { return _int32_map.contains(key) ? _int32_map[key] : 0; }
    quint32 UInt32(QString key) { return _uint32_map.contains(key) ? _uint32_map[key] : 0; }
    float Float(QString key) { return _float_map.contains(key) ? _float_map[key] : 0; }
    // set value
    bool SetBit(QString key, int index, bool value);
    bool SetByte(QString key, qint8 value);
    bool SetUByte(QString key, quint8 value);
    bool SetInt16(QString key, qint16 value);
    bool SetUInt16(QString key, quint16 value);
    bool SetInt32(QString key, qint32 value);
    bool SetUInt32(QString key, quint32 value);
    bool SetFloat(QString key, float value);
    // get map
    QMap<QString, QBitArray*> BitsMap() { return _bits_map; }
    QMap<QString, qint8> BytesMap() { return _bytes_map; }
    QMap<QString, quint8> UBytesMap() { return _ubytes_map; }
    QMap<QString, qint16> Int16Map() { return _int16_map; }
    QMap<QString, quint16> UInt16Map() { return _uint16_map; }
    QMap<QString, qint32> Int32Map() { return _int32_map; }
    QMap<QString, quint32> UInt32Map() { return _uint32_map; }
    QMap<QString, float> FloatMap() { return _float_map; }
    QMap<QString, double> DoubleMap() { return _double_map; }
    // get map
    void Insert(QString key, QBitArray* value) { _bits_map.insert(key, value); }
    void Insert(QString key, qint8 value) { _bytes_map.insert(key, value); }
    void Insert(QString key, quint8 value) { _ubytes_map.insert(key, value); }
    void Insert(QString key, qint16 value) { _int16_map.insert(key, value); }
    void Insert(QString key, quint16 value) {_uint32_map.insert(key, value); }
    void Insert(QString key, qint32 value) { _int32_map.insert(key, value); }
    void Insert(QString key, quint32 value) {_uint32_map.insert(key, value); }
    void Insert(QString key, float value) { _float_map.insert(key, value); }
};

#endif // DATASTORE_H

