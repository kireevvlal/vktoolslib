#include "outputpacket.h"

OutputPacket::OutputPacket()
{
    _order = OrderType::Direct;
    _MO_05_version = true;
}
//--------------------------------------------------------------------------------
void OutputPacket::Parse(NodeXML* node)
{
    int i;
    QString value;
    for (i = 0; i < node->Attributes.count(); i++) {
        AttributeXML *attr = node->Attributes[i];
        if (attr->Name == "length") {
            _length = attr->Value.toInt();
//            Data.resize(Length + 1);
//            Data[0] = 0xff;
//            Data[1] = Length;
        } else if (attr->Name == "order")
            _order = (attr->Value.toLower() == "reverse") ? OrderType::Reverse : OrderType::Direct;
        else if (attr->Name == "device")
            _modbus.Device =  attr->Value.toInt();
        else if (attr->Name == "function")
            _modbus.Function =  attr->Value.toInt();
        else if (attr->Name == "address")
            _modbus.Address =  attr->Value.toInt();
        else if (attr->Name == "parameter")
            _modbus.Parameter =  attr->Value.toInt();
    }
    if (node->Child != nullptr) {
        node = node->Child;
        while (node != nullptr) {
            if (node->Name == "par") {
                Parameter *newPar = new Parameter;
                newPar->Parse(node);
                _parameters.append(newPar);
            }
            node = node->Next;
        }     
    }
    // Init Data
    _data.resize(_length - 1);
    _data.fill(0);
//    for (i = 0; i < _parameters.size(); i++)
//        _data[_parameters[i]->Byte] = _parameters[i]->Value;
}
//--------------------------------------------------------------------------------
void OutputPacket::SetByteParameter(QString var, qint8 val)
{
    for (int i = 0; i < _parameters.size(); i++)
        if (_parameters[i]->Variable == var)
                _parameters[i]->Value = val;
}
//--------------------------------------------------------------------------------
QByteArray OutputPacket::Build() //Build()
{
    for (int i = 0; i < _parameters.size(); i++)
        _data[_parameters[i]->Byte] = _parameters[i]->Value;
    return (this->*BuildFunction)();
}

//--------------------------------------------------------------------------------
QByteArray OutputPacket::Staffing() // Staffing byte
{
    QByteArray data;
    char ks = 0;
    int i;
    data.append(0xff);
    ks = _length;
    data.append(_length);
    for (i = 0; i < _data.size(); i++) {
        data.append(_data[i]);
        ks += _data[i];
        if ((quint8)_data[i] == 0xff)
            data.append(0xfe);
    }
    data.append(~ks + 1);
    return data;
}
//--------------------------------------------------------------------------------
QByteArray OutputPacket::Modbus() // Send modbus packet
{
    int length = 0;
    QByteArray query;
    quint16 CRC = 0xffff;
    query.append(_modbus.Device);
    query.append(_modbus.Function);
    query.append((uchar)((_modbus.Address & 0xff00) >> 8));
    query.append((uchar)(_modbus.Address & 0xff));
    query.append((uchar)((_modbus.Parameter & 0xff00) >> 8));
    query.append((uchar)(_modbus.Parameter & 0xff));
    if ((_modbus.Function == 0x10) || (_modbus.Function == 0x71))
    {
        if (_modbus.Function == 0x71)
            length = 2;
        else
            length = _modbus.Parameter;
        for (int i = 0; i < length; i++)
        {
            query.append((uchar)((_modbus.Values[i] & 0xff00) >> 8));
            query.append((uchar)(_modbus.Values[i] & 0xff));
        }
    }

    // CRC
    for (int i = 0; i < 6 + length * 2; i++)
        CRC = CRC16(query[i], CRC);
    query.append((uchar)(CRC & 0xff));
    query.append((uchar)((CRC & 0xff00) >> 8));
    return query;
}
//--------------------------------------------------------------------------------
QByteArray OutputPacket::MO_05() // Send MO-05 query
{
    return QByteArray(_MO_05_version ? "Pdata\r" : "MO-05\r");
}
//--------------------------------------------------------------------------------
void OutputPacket::SetModbusQuery(uchar function, quint16 address, quint16 parameter, quint16* values) {
    _modbus.Function = function;
    _modbus.Address = address;
    _modbus.Parameter = parameter;
    for (int i = 0; i < 16; i++)
        if (values[i] != NULL)
            _modbus.Values[i] = values[i];
}
//--------------------------------------------------------------------------------
void OutputPacket::SetProtocol(ProtocolType protocol) {
    switch (protocol) {
    case ProtocolType::Staffing: this->BuildFunction = &OutputPacket::Staffing; break;
    case ProtocolType::Modbus: this->BuildFunction = &OutputPacket::Modbus; break;
    case ProtocolType::MO_05: this->BuildFunction = &OutputPacket::MO_05; break;
    }
}
//--------------------------------------------------------------------------------
void OutputPacket::SetData(int pos, int len, QByteArray newdata)
{
    if (_data.size() < pos + len)
        _data.resize(pos + len);
    for (int i = 0; i < len; i++)
        _data[i + pos] = newdata[i];
}
//--------------------------------------------------------------------------------
ParameterList OutputPacket::Parameters()
{
    return _parameters;
}
//--------------------------------------------------------------------------------
quint16 OutputPacket::CRC16(uchar ch, quint16 crc) {
    crc ^= ch;
    for (int i = 0; i < 8; i++)
        crc = (quint16)(((crc& 0x0001) == 1) ? ((crc >> 1) ^ 0xA001) : (crc >> 1));
    return crc;
}
