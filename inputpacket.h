#ifndef INPUTPACKET_H
#define INPUTPACKET_H
#include <QObject>
#include "treexml.h"
#include "vktoolstypes.h"
#include "parameter.h"
typedef struct StaffingStruct {
    bool FlagBegin;
    char Checksum;
    char LastByte;
    int Counter;
    int BytesLen;
    int Offset;
    StaffingStruct() {
        FlagBegin = false;
        Checksum = 0;
        LastByte = 0;
        Counter = 0;
        BytesLen = 1;
        Offset = 0;
    }
    void Reset() {
        Counter = 1;           // все инициализируем исходными данными
        FlagBegin = false;
        Checksum = 0;
        LastByte = 0;
    }
} StaffingStruct;

typedef struct ModbusInStruct{
    int Index;
    uchar Device;
    quint16 CRC;
    uchar Function;
    uchar Command;
    quint16 Portion;
    //QByteArray Buffer;
    ModbusInStruct() {
        Device = Function = 1;
        Index = 0;
        CRC = 0;
      //  Buffer.resize(2048);
    }
} ModbusInStruct;

typedef struct MO_05_Struct{
    int Index;
    uchar Version;
    MO_05_Struct() {
        Index = 0;
        Version = true;
    }
} MO_05_Struct;

//pedef struct StaffingStruct Staffing;

class InputPacket : public QObject
{
    Q_OBJECT
public:
    int Index;
    OrderType Order() { return _order; }
    void SetOrder(OrderType value) { _order = value; }
    InputPacket(QObject *parent = nullptr);
    void Parse(NodeXML*);
    ParameterList Parameters();
    QByteArray Data();
//    void Reset(); // clear data bytes
    void Decode(QByteArray);
    void Swap();
    void SetProtocol(ProtocolType);
    void SetMO_05_Version(bool value) {_mo_05.Version = value; }
private:
    OrderType _order;
    StaffingStruct _staffing;
    ModbusInStruct _modbus;
    MO_05_Struct _mo_05;
    int _delta; // смеещение, вызванное передачей разных пакетов (в ТИ)
    ParameterList _parameters;
    QByteArray _buffer;
    QByteArray _data;
    int _length;
    void (InputPacket::*DecodeFunction)(QByteArray);
    void DecodeStaffing(QByteArray);
    void DecodeModbus(QByteArray);
    void DecodeMO_05(QByteArray);
    quint16 CRC16(uchar, quint16);
signals:
    void ReceivePacketSignal();
};

#endif // INPUTPACKET_H
