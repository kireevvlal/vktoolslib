#include "outputpacket.h"

OutputPacket::OutputPacket()
{
    _order = OrderType::Direct;
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
    for (i = 0; i < _parameters.size(); i++)
        _data[_parameters[i]->Byte] = _parameters[i]->Value;
}
//--------------------------------------------------------------------------------
QByteArray OutputPacket::Build() //Build()
{
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
void OutputPacket::SetProtocol(ProtocolType protocol)
{
    if (protocol == ProtocolType::Staffing)
        this->BuildFunction = &OutputPacket::Staffing;
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
