#include "threadserialport.h"

ThreadSerialPort::ThreadSerialPort(QObject *parent)
{
    _thread = new QThread;
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
    _wait = _watchdog = 0;
    _is_exchange = false;
    _counter = 0;
    _quality = 0;
}
//--------------------------------------------------------------------------------
ThreadSerialPort::~ThreadSerialPort()
{
    if (isOpen())
        close();
//    delete _port;
    if (_thread->isRunning())
        _thread->quit();
    delete _thread;
    emit FinishedSignal();  //Сигнал о завершении работы
}
//--------------------------------------------------------------------------------
bool ThreadSerialPort::Connect()
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
            close();
            LogSignal(errorString().toLocal8Bit());
        }
    } else {
        close();
        LogSignal(errorString().toLocal8Bit());
    }
    return false;
}
//--------------------------------------------------------------------------------
void ThreadSerialPort::WriteSettings(QString name, int baudrate,int databits, int parity, int stopbits, int flowcontrol)
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
void  ThreadSerialPort::Disconnect()
{
    if (isOpen()){
        close();
        LogSignal(_settings.Name.toLocal8Bit() + " >> Закрыт!\r");
    }
}
//--------------------------------------------------------------------------------
void ThreadSerialPort::Start()
{
    moveToThread(_thread);
    connect(_thread, SIGNAL(started()), this, SLOT(Process()));  //Переназначения метода run
    connect(this, SIGNAL(FinishedSignal()), _thread, SLOT(quit()));//Переназначение метода выход
    connect(_thread, SIGNAL(finished()), this, SLOT(deleteLater()));//Удалить к чертям поток
    connect(this, SIGNAL(FinishedSignal()), _thread, SLOT(deleteLater()));//Удалить к чертям поток

    if (Connect()) {
        connect(&_timer, SIGNAL(timeout()), this, SLOT(TimerStep()));
        _timer.start(TIMEOUT);
        _thread->start();
    }
}
//--------------------------------------------------------------------------------
// Выполняется при старте класса
void ThreadSerialPort::Process()
{
    connect(this, SIGNAL(error(QSerialPort::SerialPortError)), this, SLOT(HandleError(QSerialPort::SerialPortError))); // подключаем проверку ошибок порта
    connect(this, SIGNAL(readyRead()), this, SLOT(Read()));//подключаем   чтение с порта по сигналу readyRead()
    connect(&InData, SIGNAL(ReceivePacketSignal()), this, SLOT(ReceivePacket()));//подключаем  обработку по сигналу приема пакета
}
//--------------------------------------------------------------------------------
//проверка ошибок при работе
void ThreadSerialPort::HandleError(QSerialPort::SerialPortError error)
{
    if ((isOpen()) && (error == QSerialPort::ResourceError)) {
        LogSignal(errorString().toLocal8Bit());
        Disconnect();
    }
}
//--------------------------------------------------------------------------------
// Обработка сигнала таймера передачи пакетов
void ThreadSerialPort::TimerStep()
{
    if (_wait + TIMEOUT >= _delay) {
        if (_type_exchange == ExchangeType::Master)
            Write();
        _quality += ((_quality < 20) ? 1 : 0);
        _wait = 0;
    } else
        _wait += TIMEOUT;
    _watchdog += TIMEOUT;
    if (_watchdog > _limit) {
        if (_is_exchange) {
            _is_exchange = false;
            LostExchangeSignal(Alias);
        }
        _watchdog = _limit; //
    }

}
//--------------------------------------------------------------------------------
// Запись данных в порт
void ThreadSerialPort::Write()
{
    if (isOpen()){
        QByteArray out = OutData.Build();
        write(out);
        WriteSignal(Alias);
    }
}
//--------------------------------------------------------------------------------
// Чтение данных из порта
void ThreadSerialPort::Read()
{
    if (_type_protocol != ProtocolType::Unknown) {
        InData.Decode(readAll());
    } else {
        QByteArray data;
        data.append(readAll());
        ReadSignal(data);
    }
}
//--------------------------------------------------------------------------------
// Чтение данных из порта
void ThreadSerialPort::ReceivePacket() {
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
void ThreadSerialPort::Parse(NodeXML* node) {
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
        else if (attr->Name == "protocol") {
            value = attr->Value.toLower();
            _type_protocol = (value == "staffing") ? ProtocolType::Staffing : ProtocolType::Unknown;
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
void ThreadSerialPort:: ParsePort(NodeXML* node)
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
