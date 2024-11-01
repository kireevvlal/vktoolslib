#include "inputpacket.h"

InputPacket::InputPacket(QObject *parent) : QObject(parent) {
    //_data.resize(1024);

    _delta = -1;
    Index = 0xffff; // нет индекса
    _order = OrderType::Direct;
}
//--------------------------------------------------------------------------------
void InputPacket::Parse(NodeXML* node) {
    int i;
    QString value;
    for (i = 0; i < node->Attributes.count(); i++) {
        AttributeXML *attr = node->Attributes[i];
        if (attr->Name == "inclen")
            _staffing.Offset = attr->Value.toInt();
        else if (attr->Name == "order")
            _order = (attr->Value.toLower() == "reverse") ? OrderType::Reverse : OrderType::Direct;
        else if (attr->Name == "byteslen")
            _staffing.BytesLen = attr->Value.toInt();
        else if (attr->Name == "index")
            Index = attr->Value.toInt();
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
}
//--------------------------------------------------------------------------------
void InputPacket::SetProtocol(ProtocolType protocol)
{
    switch (protocol) {
    case ProtocolType::Staffing :  this->DecodeFunction = &InputPacket::DecodeStaffing; break;
    case ProtocolType::Modbus : this->DecodeFunction = &InputPacket::DecodeModbus; break;
    case ProtocolType::MO_05 : this->DecodeFunction = &InputPacket::DecodeMO_05; break;
    }
}
//--------------------------------------------------------------------------------
void InputPacket::Decode(QByteArray data) //Build()
{
    (this->*DecodeFunction)(data);
}
//--------------------------------------------------------------------------------
void InputPacket::DecodeStaffing(QByteArray data) {
    uchar ch;
    int len = data.length();
    for (int i = 0; i < len; i++)
    {
        ch = data[i]; // Прием байта
        if (ch == 0xff)          // Прием синхробайта?
            _staffing.LastByte = 0xff;
        else
            if (ch == 0xfe)      // Прием байт-стаффинга (FE)
            {
                if ((quint8)_staffing.LastByte == 0xff)
                {                   // Был байт ff информации
                    ch = 0xff;      // Восстановление ff
                    _staffing.LastByte = 0;   // Сброс
                }
                if (_staffing.FlagBegin) {
                    _staffing.Checksum += ch;     // накапливаем сумму
                    _staffing.Counter++;       // принят очередной байт информации
                    _buffer.append(ch); // Data[Counter - BytesLen - 1] = ch;         // в пакет
                    if (_staffing.BytesLen == 2 && _staffing.Counter == 2)
                        _length += (ch << 8);
                    if (_staffing.Counter == _length + _staffing.Offset + _staffing.BytesLen)        // заказанная длина пакета
                    {
                        if (_staffing.Checksum == 0) {    // конец пакета
                            _data = _buffer;
                            _staffing.Reset();
                            _delta = -1;
                            ReceivePacketSignal();
                        }    // Happy end !!!!
                    }
                }
            }
            else                    // Прием иного символа
                if ((quint8)_staffing.LastByte == 0xff)
                {                   // Был действительно!
                    _length = ch;  // Байт длины посылки
                    _staffing.Checksum = ch;      // инициализация констрольной суммы
                    _staffing.Counter = 1;       // Счетчик байт
                    _staffing.LastByte = 0;
                    _staffing.FlagBegin = true; // Начало информации в пакете
                    _buffer.clear();
                }
                else
                {
                    if (_staffing.FlagBegin) {
                        _staffing.Checksum += ch;     // накапливаем сумму
                        _staffing.Counter++;       // принят очередной байт информации
                        _buffer.append(ch); // Data[Counter - BytesLen - 1] = ch;         // в пакет
                        if (_staffing.BytesLen == 2 && _staffing.Counter == 2)
                            _length += (ch << 8);
                        if (_staffing.Counter == _length + _staffing.Offset + _staffing.BytesLen)        // заказанная длина пакета ????? : +1
                        {
                            if (_staffing.Checksum == 0)  { // конец пакета
                                _data = _buffer;
                                _staffing.Reset();
                                _delta = -1;
                                ReceivePacketSignal();
                            }
                        }
                    }
                }
    }
}
//--------------------------------------------------------------------------------
void InputPacket::DecodeModbus(QByteArray data) {
    uchar ch;
    int len = data.length();
    for (int i = 0; i < len; i++)
    {
        ch = data[i];
        if (ch == _modbus.Device)
        {
            if ((_modbus.Index == 0) || (_modbus.Index >= _length))
            {
                _buffer.clear();
                _buffer.append(_modbus.Device);
                _modbus.Index = 1;
                _modbus.CRC = CRC16(ch, 0xffff);
                continue;
            }
        }
        // Not _device
        if (_modbus.Index == 1)
        {
            if ((ch == 0x01) || (ch == 0x03) || (ch == 0x04) || (ch == 0x05) || (ch == 0x06) || (ch == 0x10) || (ch == 0x70) || (ch == 0x71))
            {  // command
                _modbus.Function = ch;
                _buffer.append(ch);
                _modbus.Index = 2;
                _modbus.CRC = CRC16(ch, _modbus.CRC);
            }
            else // not command
                _modbus.Index = 0;
            continue;
        }
        if (_modbus.Index == 2) // length
        {
            //if ((_function == 0x03) || (_function == 0x04))
            //    _length = ch + 5;
            if (ch == 0xF0) // long packet
                _length = _modbus.Portion * 2 + 5;
            else
                if ((_modbus.Function == 0x01) || (_modbus.Function == 0x03) || (_modbus.Function == 0x04) || (_modbus.Function == 0x70))
                    _length = ch + 5;
                else
                    _length = 8;
        }
        if (_modbus.Index == _length - 2)
        {
            _buffer.append(ch); //_modbus.Buffer[_modbus.Index] = ch;
            _modbus.Index++;
            continue;
        }
        if (_modbus.Index == _length - 1)
        {
            if (_modbus.CRC == (uchar)(_buffer[_modbus.Index - 1]) + (ch << 8))
            {
                _buffer.append(ch);
                _modbus.Command = _buffer[1];
                _data = _buffer;
                ReceivePacketSignal();
            }
            _buffer.clear();
            _modbus.Index = 0;
            _modbus.CRC = 0;
            continue;
        }
        _buffer.append(ch);
        _modbus.CRC = CRC16(ch, _modbus.CRC);
        _modbus.Index++;
        //break;
    }
}
//--------------------------------------------------------------------------------
void InputPacket::DecodeMO_05(QByteArray data) {
    int len = data.length();
    _mo_05.Index += len;
    _buffer.append(data);
    if (!_mo_05.Version)
    {
        if (_mo_05.Index == 18)
        {
            for (int i = 0; i < 4; i++)
                _buffer.append(data[i]);
            _data = _buffer;
            _buffer.clear();
            ReceivePacketSignal();
            _mo_05.Index = 0;
        }
        else
            if (_mo_05.Index > 18)
                _mo_05.Index = 0;
    }
    else
    {
        if (_buffer[_mo_05.Index - 1] == '\r')
        {
            _data.clear();
            for (int i = 0; i < _mo_05.Index - 1; i++)
                _data.append(_buffer[i]);
            //_data = _buffer;
            _buffer.clear();
            ReceivePacketSignal();
            _mo_05.Index = 0;
        }
        else
            if (_mo_05.Index > 8)
                _mo_05.Index = 0;
    }
}
//--------------------------------------------------------------------------------
ParameterList InputPacket::Parameters() {
    return _parameters;
}
//--------------------------------------------------------------------------------
QByteArray InputPacket::Data() {
    return _data;
}
//--------------------------------------------------------------------------------
void InputPacket::Swap() {
    int i, tmp, byte;
    if (_order == OrderType::Reverse)
        for (i = 0; i < _parameters.count(); i++) {
            if (_parameters[i]->Type == DataType::Float || _parameters[i]->Type == DataType::Int16 ||
                    _parameters[i]->Type == DataType::Uint16)  {
                byte = _parameters[i]->Byte;
                if (byte + _parameters[i]->Size <= _data.size() )
                {
                    tmp = _data[byte];
                    _data[byte] = _data[byte + 1];
                    _data[byte + 1] = tmp;
                }
            }
        }
}
//--------------------------------------------------------------------------------
quint16 InputPacket::CRC16(uchar ch, quint16 crc) {
    crc ^= ch;
    for (int i = 0; i < 8; i++)
        crc = (quint16)(((crc& 0x0001) == 1) ? ((crc >> 1) ^ 0xA001) : (crc >> 1));
    return crc;
}
//--------------------------------------------------------------------------------
//void InputPacket::Reset() {
//    _data.fill('\0');
//}
