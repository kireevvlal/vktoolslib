#include "datastore.h"
//#include <QDebug>

DataStore::DataStore()
{

}
//--------------------------------------------------------------------------------
void DataStore::FillMaps(ThreadSerialPort* port) {
    int j;
    ParameterList parameters;
    Parameter *current;
    parameters = port->InData.Parameters();
    for (j = 0; j < parameters.size(); j++) {
        current = parameters[j];
        switch(current->Type) {
        case DataType::Bit: _bits_map.insert(current->Variable, 0); break;
        case DataType::Byte: _bytes_map.insert(current->Variable, 0); break;
        case DataType::UByte: _ubytes_map.insert(current->Variable, 0); break;
        case DataType::Int16: _int16_map.insert(current->Variable, 0); break;
        case DataType::Uint16: _uint16_map.insert(current->Variable, 0); break;
        case DataType::Int32: _int32_map.insert(current->Variable, 0); break;
        case DataType::Uint32: _uint32_map.insert(current->Variable, 0); break;
        case DataType::Float:  _float_map.insert(current->Variable, 0); break;
        case DataType::Double: _double_map.insert(current->Variable, 0); break;
        }
    }

    parameters = port->OutData.Parameters();
    for (j = 0; j < parameters.size(); j++) {
        current = parameters[j];
        switch(current->Type) {
        case DataType::Bit: _bits_map.insert(current->Variable, 0); break;
        case DataType::Byte: _bytes_map.insert(current->Variable, 0); break;
        case DataType::UByte: _ubytes_map.insert(current->Variable, 0); break;
        case DataType::Int16: _int16_map.insert(current->Variable, 0); break;
        case DataType::Uint16: _uint16_map.insert(current->Variable, 0); break;
        case DataType::Int32: _int32_map.insert(current->Variable, 0); break;
        case DataType::Uint32: _uint32_map.insert(current->Variable, 0); break;
        case DataType::Float:  _float_map.insert(current->Variable, 0); break;
        case DataType::Double: _double_map.insert(current->Variable, 0); break;
        }
    }
}
//--------------------------------------------------------------------------------
bool DataStore::Exist(QString key, DataType type) {
    bool result = false;
    switch(type) {
    case DataType::Bit: if (_bits_map.contains(key)) result = true; break;
    case DataType::Byte: if (_bytes_map.contains(key)) result = true; break;
    case DataType::UByte: if (_ubytes_map.contains(key)) result = true; break;
    case DataType::Int16: if (_int16_map.contains(key)) result = true; break;
    case DataType::Uint16: if (_uint16_map.contains(key)) result = true; break;
    case DataType::Int32: if (_int32_map.contains(key)) result = true; break;
    case DataType::Uint32: if (_uint32_map.contains(key)) result = true; break;
    case DataType::Float:  if (_float_map.contains(key)) result = true; break;
    case DataType::Double: if (_double_map.contains(key)) result = true; break;
    }
    return result;
}
//--------------------------------------------------------------------------------
bool DataStore::Add(QString key, DataType type) {
    if (_bits_map.contains(key) || _bytes_map.contains(key) || _ubytes_map.contains(key) || _int16_map.contains(key)
            || _uint16_map.contains(key) || _int32_map.contains(key) || _uint32_map.contains(key)
            || _float_map.contains(key) || _double_map.contains(key))
        return false;
    else {
        switch(type) {
        case DataType::Bit: _bits_map.insert(key, 0); break;
        case DataType::Byte: _bytes_map.insert(key, 0); break;
        case DataType::UByte: _ubytes_map.insert(key, 0); break;
        case DataType::Int16: _int16_map.insert(key, 0); break;
        case DataType::Uint16: _uint16_map.insert(key, 0); break;
        case DataType::Int32: _int32_map.insert(key, 0); break;
        case DataType::Uint32: _uint32_map.insert(key, 0); break;
        case DataType::Float:  _float_map.insert(key, 0); break;
        case DataType::Double: _double_map.insert(key, 0); break;
        }
        return true;
    }
}
//--------------------------------------------------------------------------------
bool DataStore::Bit(QString key, bool value) {
    if (_bits_map.contains(key)) {
        _bits_map[key] = value;
        return true;
    }
    else
        return false;
}
//--------------------------------------------------------------------------------
bool DataStore::Byte(QString key, qint8 value) {
    if (_bytes_map.contains(key)) {
        _bytes_map[key] = value;
        return true;
    }
    else
        return false;
}
//--------------------------------------------------------------------------------
bool DataStore::UByte(QString key, quint8 value) {
    if (_ubytes_map.contains(key)) {
        _ubytes_map[key] = value;
        return true;
    }
    else
        return false;
}
//--------------------------------------------------------------------------------
bool DataStore::Int16(QString key, qint16 value) {
    if (_int16_map.contains(key)) {
        _int16_map[key] = value;
        return true;
    }
    else
        return false;
}
//--------------------------------------------------------------------------------
bool DataStore::UInt16(QString key, quint16 value) {
    if (_uint16_map.contains(key)) {
        _uint16_map[key] = value;
        return true;
    }
    else
        return false;
}
//--------------------------------------------------------------------------------
bool DataStore::Int32(QString key, qint32 value) {
    if (_int32_map.contains(key)) {
        _int32_map[key] = value;
        return true;
    }
    else
        return false;
}
//--------------------------------------------------------------------------------
bool DataStore::UInt32(QString key, quint32 value) {
    if (_uint32_map.contains(key)) {
        _uint32_map[key] = value;
        return true;
    }
    else
        return false;
}
//--------------------------------------------------------------------------------
bool DataStore::Float(QString key, float value) {
    if (_float_map.contains(key)) {
        _float_map[key] = value;
        return true;
    }
    else
        return false;
}
//--------------------------------------------------------------------------------
QStringList DataStore::OutMaps()
{
    QStringList strings;
    QMapIterator<QString, bool> b(_bits_map);
    while (b.hasNext()) {
        b.next();
        strings.append("Bit: " + b.key());
    }
    QMapIterator<QString, qint8> i8(_bytes_map);
    while (i8.hasNext()) {
        i8.next();
        strings.append("Byte: " + i8.key());
    }
    QMapIterator<QString, quint8> ui8(_ubytes_map);
    while (ui8.hasNext()) {
        ui8.next();
        strings.append("Unsigned byte: " + ui8.key());
    }
    QMapIterator<QString, qint16> i16(_int16_map);
    while (i16.hasNext()) {
        i16.next();
        strings.append("Int16: " + i16.key());
    }
    QMapIterator<QString, quint16> ui16(_uint16_map);
    while (ui16.hasNext()) {
        ui16.next();
        strings.append("Unsigned int16: " + ui16.key());
    }
    QMapIterator<QString, qint32> i32(_int32_map);
    while (i32.hasNext()) {
        i32.next();
        strings.append("Int32: " + i32.key());
    }
    QMapIterator<QString, quint32> ui32(_uint32_map);
    while (ui32.hasNext()) {
        ui32.next();
        strings.append("Unsigned int32: " + ui32.key());
    }
    QMapIterator<QString, float> f(_float_map);
    while (f.hasNext()) {
        f.next();
        strings.append("Float: " + f.key());
    }
    return strings;
}
//--------------------------------------------------------------------------------
int DataStore::LoadSpData(ThreadSerialPort *port) {
    ParameterList parameters = port->InData.Parameters();
    Parameter *current;
    QByteArray data = port->InData.Data();
    UnionInt16 i16;
    int result = 0;
    int byte;

    for (int i = 0; i < parameters.size(); i++) {
        current = parameters[i];
        byte = current->Byte;
        if ((port->InData.Index == 0xffff) || ((port->InData.Index != 0xffff) && ((int)data[port->InData.Index] == current->Index)))
            switch(current->Type) {
            case DataType::Bit:
                if (_bits_map.contains(current->Variable))
                    _bits_map[current->Variable] = data[byte] & (1 << current->Bit);
                else
                    result++;
                break;
            case DataType::Byte:
                if (_bytes_map.contains(current->Variable))
                    _bytes_map[current->Variable] = data[byte];
                else
                    result++;
                break;
            case DataType::UByte:
                if (_ubytes_map.contains(current->Variable))
                    _ubytes_map[current->Variable] = data[byte];
                else
                    result++;
                break;
            case DataType::Int16:
                    i16.Array[0] = data[byte];
                    i16.Array[1] = data[byte + 1];
                if (_int16_map.contains(current->Variable))
                    _int16_map[current->Variable] = i16.Value;
                else
                    result++;
                break;
            case DataType::Uint16:
                UnionUInt16 ui16;
                    ui16.Array[0] = data[byte];
                    ui16.Array[1] = data[byte + 1];
                if (_uint16_map.contains(current->Variable))
                    _uint16_map[current->Variable] = ui16.Value;
                else
                    result++;
                break;
            case DataType::Int32:
                UnionInt32 i32;
                    i32.Array[0] = data[byte];
                    i32.Array[1] = data[byte + 1];
                    i32.Array[2] = data[byte + 2];
                    i32.Array[3] = data[byte + 3];
                if (_int32_map.contains(current->Variable))
                    _int32_map[current->Variable] = i32.Value;
                else
                    result++;
                break;
            case DataType::Uint32:
                UnionUInt32 ui32;
                    ui32.Array[0] = data[byte];
                    ui32.Array[1] = data[byte + 1];
                    ui32.Array[2] = data[byte + 2];
                    ui32.Array[3] = data[byte + 3];
                if (_uint32_map.contains(current->Variable))
                    _uint32_map[current->Variable] = ui32.Value;
                else
                    result++;
                break;
            case DataType::Float:
                    i16.Array[0] = data[byte];
                    i16.Array[1] = data[byte + 1];
                if (_float_map.contains(current->Variable))
                    _float_map[current->Variable] = i16.Value * current->Coefficient;
                else
                    result++;
                break;
            case DataType::Double:
                break;
            }
    }
    return result;
}
