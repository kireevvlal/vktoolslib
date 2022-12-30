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
private:
    OrderType _order;
    StaffingStruct _staffing;
    int _delta; // смеещение, вызванное передачей разных пакетов (в ТИ)
    ParameterList _parameters;
    QByteArray _buffer;
    QByteArray _data;
    int _length;
    void (InputPacket::*DecodeFunction)(QByteArray);
    void DecodeStaffing(QByteArray);
signals:
    void ReceivePacketSignal();
};

#endif // INPUTPACKET_H
