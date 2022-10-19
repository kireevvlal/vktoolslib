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
    if (protocol == ProtocolType::Staffing)
        this->DecodeFunction = &InputPacket::DecodeStaffing;
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
ParameterList InputPacket::Parameters() {
    return _parameters;
}
//--------------------------------------------------------------------------------
QByteArray InputPacket::Data() {
    return _data;
}
//--------------------------------------------------------------------------------
void InputPacket::Swap() {
    int i, tmp;
    if (_order == OrderType::Reverse)
        for (i = 0; i < _parameters.count(); i++) {
            if (_parameters[i]->Type == DataType::Float || _parameters[i]->Type == DataType::Int16 ||
                    _parameters[i]->Type == DataType::Uint16) {
                tmp = _data[_parameters[i]->Byte];
                _data[_parameters[i]->Byte] = _data[_parameters[i]->Byte + 1];
                _data[_parameters[i]->Byte + 1] = tmp;
            }
        }
}
