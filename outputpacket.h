#ifndef OUTPUTPACKET_H
#define OUTPUTPACKET_H
#include <QObject>
#include "treexml.h"
#include "vktoolstypes.h"
#include "parameter.h"

typedef struct ModbusOutStruct {
    uchar Device;
    uchar Function;
    quint16 Address;
    quint16 Parameter;
    quint16 Values[16]; // !!!!!!!!!!!!!!!
    ModbusOutStruct() {
        Device = Function = 1;
        Address = 0;
        Parameter = 2;
        for (int i = 0; i < 16; i++)
            Values[i] = 0;
    }    
} ModbusOutStruct;

class OutputPacket
{
public:
    OutputPacket();
    void Parse(NodeXML*);
    QByteArray Build();
    void SetProtocol(ProtocolType);
    ParameterList Parameters();
    void SetData(int, int, QByteArray);
    void SetByteParameter(QString, qint8);
    void SetModbusDevice(uchar device) { _modbus.Device = device; }
    void SetModbusQuery(uchar, quint16, quint16, quint16*);
    void SetMO_05_Version(bool value) {_MO_05_version = value; }
private:
    ModbusOutStruct _modbus;
    bool _MO_05_version;
    ParameterList _parameters;
     QByteArray _data;
    int _length;
    OrderType _order;
    QByteArray (OutputPacket::*BuildFunction)();
    QByteArray Staffing();
    QByteArray Modbus();
    QByteArray MO_05();
    void Pack();
    quint16 CRC16(uchar, quint16);
};

#endif // OUTPUTPACKET_H
