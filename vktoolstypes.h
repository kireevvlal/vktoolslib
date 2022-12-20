#ifndef EXTRATYPES_H
#define EXTRATYPES_H
#include <QtGlobal>

#define nobit0 0xfe
#define nobit1 0xfd
#define nobit2 0xfb
#define nobit3 0xf7
#define nobit4 0xef
#define nobit5 0xdf
#define nobit6 0xbf
#define nobit7 0x7f


enum ExchangeType { Master, Slave, Receive };
enum OrderType { Direct, Reverse };
enum ProtocolType { Staffing, Unknown };
enum DataType { Bits, Byte, UByte, Int16, Uint16, Int32, Uint32, Float, Double };
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
