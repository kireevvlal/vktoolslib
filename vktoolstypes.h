#ifndef EXTRATYPES_H
#define EXTRATYPES_H
#include <QtGlobal>

enum ExchangeType { Master, Slave, Receive };
enum OrderType { Direct, Reverse };
enum ProtocolType { Staffing, Unknown };
enum DataType { Bit, Byte, UByte, Int16, Uint16, Int32, Uint32, Float, Double };
enum RegistrationType { Record,  Bulk, Archive};

typedef  union {
    qint16 Value;
    char Array[2];
} UnionInt16;

typedef  union {
    quint16 Value;
    char Array[2];
} UnionUInt16;

typedef  union {
    qint32 Value;
    char Array[4];
} UnionInt32;

typedef  union {
    quint16 Value;
    char Array[4];
} UnionUInt32;

//typedef  union {
//    float Value;
//    char Array[4];
//} UnionFloat;

//typedef  union {
//    quint16 Value;
//    char Array[8];
//} UnionDouble;
#endif // EXTRATYPES_H
