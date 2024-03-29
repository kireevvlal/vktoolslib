#include "extserialport.h"

ExtSerialPort::ExtSerialPort(QObject *parent)
{
    Alias = "";
    _settings.Name = "";
    _settings.BaudRate = 9600;
    _settings.DataBits = QSerialPort::Data8;
    _settings.Parity = QSerialPort::NoParity;
    _settings.StopBits = QSerialPort::OneStop;
    _settings.FlowControl = QSerialPort::NoFlowControl;
    _type_exchange = ExchangeType::Receive;
    _delay = 1000;
    _limit = 3000;
    _wait = _watchdog = _wd_bytes = 0;
    _is_exchange = _is_bytes = false;
    _counter = 0;
    _quality = 0;
    _errors_count = 0;
}
//--------------------------------------------------------------------------------
ExtSerialPort::~ExtSerialPort()
{
    if (isOpen())
        close();
    emit FinishedSignal();  //Сигнал о завершении работы
}
//--------------------------------------------------------------------------------
bool ExtSerialPort::Connect()
{
    setPortName(_settings.Name);
    if (setBaudRate(_settings.BaudRate)
            && setDataBits(_settings.DataBits)
            && setParity(_settings.Parity)
            && setStopBits(_settings.StopBits)
            && setFlowControl(_settings.FlowControl)) {
        if (open(QIODevice::ReadWrite)) {
            flush();
            LogSignal(_settings.Name + " открыт!\r");
            return true;
        } else {
//            close();
            LogSignal(errorString().toLocal8Bit());
        }
    } else {
        close();
        LogSignal(errorString().toLocal8Bit());
    }
    return false;
}
//--------------------------------------------------------------------------------
void ExtSerialPort::WriteSettings(QString name, int baudrate,int databits, int parity, int stopbits, int flowcontrol)
{
    _settings.Name = name;
    _settings.BaudRate = baudrate;
    _settings.DataBits = (QSerialPort::DataBits)databits;
    _settings.Parity = (QSerialPort::Parity)parity;
    _settings.StopBits = (QSerialPort::StopBits)stopbits;
    _settings.FlowControl = (QSerialPort::FlowControl)flowcontrol;
}
//--------------------------------------------------------------------------------
// Отключаем порт
void  ExtSerialPort::Disconnect()
{
    if (isOpen()){
        close();
        LogSignal(_settings.Name.toLocal8Bit() + " >> Закрыт!\r");
    }
}
//--------------------------------------------------------------------------------
void ExtSerialPort::Start()
{
    if (Connect()) {
        connect(this, SIGNAL(error(QSerialPort::SerialPortError)), this, SLOT(HandleError(QSerialPort::SerialPortError))); // подключаем проверку ошибок порта
        connect(this, SIGNAL(readyRead()), this, SLOT(Read()));//подключаем   чтение с порта по сигналу readyRead()
        connect(&InData, SIGNAL(ReceivePacketSignal()), this, SLOT(ReceivePacket()));//подключаем  обработку по сигналу приема пакета

        connect(&_timer, SIGNAL(timeout()), this, SLOT(TimerStep()));
        _timer.start(TIMEOUT);
    }
}
//--------------------------------------------------------------------------------
//проверка ошибок при работе
void ExtSerialPort::HandleError(QSerialPort::SerialPortError error)
{
    _errors_count++;
    if ((isOpen()) && (error == QSerialPort::ResourceError)) {
        LogSignal(errorString().toLocal8Bit());
        Disconnect();
    }
}
//--------------------------------------------------------------------------------
// Обработка сигнала таймера передачи пакетов
void ExtSerialPort::TimerStep()
{
    if (_wait + TIMEOUT >= _delay) {
        if (_type_exchange == ExchangeType::Master)
            Write();
        _quality += ((_quality < 20) ? 1 : 0);
        _wait = 0;
    } else
        _wait += TIMEOUT;
    _watchdog += TIMEOUT;
    _wd_bytes += TIMEOUT;
    if (_watchdog > _limit) {
        if (_is_exchange) {
            _is_exchange = false;
//            InData.Reset();
            LostExchangeSignal(Alias);
        }
        _watchdog = _limit; //
    }
    if (_wd_bytes > _limit) {
        if (_is_bytes) {
            _is_bytes = false;
        }
        _wd_bytes = _limit; //
    }
}
//--------------------------------------------------------------------------------
// Запись данных в порт
void ExtSerialPort::Write()
{
    QByteArray out = OutData.Build();
    if (isOpen()){        
        write(out);
        WriteSignal(Alias);
    }
}
//--------------------------------------------------------------------------------
// Чтение данных из порта
void ExtSerialPort::Read()
{
    QByteArray data;
    if (isOpen())
        data = readAll();
    if (data.size() > 0) {
        if (_type_protocol != ProtocolType::UndeterminedProtocol) {
            InData.Decode(data);
        } else {
            ReadSignal(data);
        }
    }
    if (!_is_bytes) {
        _is_bytes = true;
    }
    _wd_bytes = _timer.remainingTime();
}
//--------------------------------------------------------------------------------
// Пакет успешно принят
void ExtSerialPort::ReceivePacket() {
    if (!_is_exchange) {
        _is_exchange = true;
        RestoreExchangeSignal(Alias);
    }
    _watchdog = _timer.remainingTime();
    _counter++;
    DecodeSignal(Alias);
    if (_type_exchange == ExchangeType::Slave)
        Write();
    _quality = (_quality > 1) ? _quality - 2 : 0;
}
//--------------------------------------------------------------------------------
void ExtSerialPort::Parse(NodeXML* node) {
    int i;
    QString value;
    for (i = 0; i < node->Attributes.count(); i++) {
        AttributeXML *attr = node->Attributes[i];
        if (attr->Name == "name")
            Alias = attr->Value;
        else if (attr->Name == "exchange") {
            value = attr->Value.toLower();
            _type_exchange = (value == "master") ? ExchangeType::Master : (value == "slave") ? ExchangeType::Slave : ExchangeType::Receive;
        }
        else if (attr->Name == "delay")
            _delay = attr->Value.toInt();
        else if (attr->Name == "limit")
            _limit = attr->Value.toInt();
        else if (attr->Name == "protocol") {
            value = attr->Value.toLower();
            _type_protocol = (value == "staffing") ? ProtocolType::Staffing : ProtocolType::UndeterminedProtocol;
        }
    }
    if (node->Child != nullptr) {
        node = node->Child;
        while (node != nullptr) {
            if (node->Name == "port")
                ParsePort(node);
            else if (node->Name == "input")
                InData.Parse(node);
            else if (node->Name == "output") {
                OutData.Parse(node);
            }
            node = node->Next;
        }
    }
    InData.SetProtocol(_type_protocol);
    OutData.SetProtocol(_type_protocol);
}
//--------------------------------------------------------------------------------
void ExtSerialPort:: ParsePort(NodeXML* node)
{
    int i;
    QString value;
    _settings.Name = node->Text;
    for (i = 0; i < node->Attributes.count(); i++) {
        AttributeXML *attr = node->Attributes[i];
        if (attr->Name == "baudrate")
            _settings.BaudRate = attr->Value.toInt();
        else if (attr->Name == "databits") {
            value = attr->Value.toLower();
            _settings.DataBits = (value == "8") ? QSerialPort::Data8 : (value == "7") ? QSerialPort::Data7 : (value == "6") ? QSerialPort::Data6 : (value == "5") ? QSerialPort::Data5 : QSerialPort::Data8;
        }
        else if (attr->Name == "stopbits") {
            value = attr->Value.toLower();
            _settings.StopBits = (value == "1") ? QSerialPort::OneStop : (value == "2") ? QSerialPort::TwoStop : (value == "3") ? QSerialPort::OneAndHalfStop : QSerialPort::OneStop;
        }
        else if (attr->Name == "parity") {
            value = attr->Value.toLower();
            _settings.Parity = (value == "no") ? QSerialPort::NoParity : (value == "even") ? QSerialPort::EvenParity : (value == "odd") ? QSerialPort::OddParity : (value == "space") ? QSerialPort::SpaceParity : (value == "mark") ? QSerialPort::MarkParity : QSerialPort::NoParity;
        }
        else if (attr->Name == "flowcontrol") {
            value = attr->Value.toLower();
            _settings.FlowControl = (value == "no") ? QSerialPort::NoFlowControl : (value == "nardware") ? QSerialPort::HardwareControl : (value == "software") ? QSerialPort::SoftwareControl : QSerialPort::NoFlowControl;
        }
    }
}
