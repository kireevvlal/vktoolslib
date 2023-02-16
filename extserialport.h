#ifndef EXTSERIALPORT_H
#define EXTSERIALPORT_H
#include <QThread>
#include <QtSerialPort/QSerialPort>
#include <QTimer>
#include "treexml.h"
#include "outputpacket.h"
#include "inputpacket.h"
#include "vktoolstypes.h"
#define TIMEOUT 100

struct SerialPortSettings { //Структура с настройками порта
    QString Name;
    qint32 BaudRate;
    QSerialPort::DataBits DataBits;
    QSerialPort::Parity Parity;
    QSerialPort::StopBits StopBits;
    QSerialPort::FlowControl FlowControl;
 };

class ExtSerialPort : public QSerialPort
{
    Q_OBJECT
public:
    OutputPacket OutData;
    InputPacket InData;
    explicit ExtSerialPort(QObject *parent = nullptr);
    ~ExtSerialPort();
    QString Alias;
    quint32 ErrorsCount() { return _errors_count; }
    bool IsExchange() { return _is_exchange; }
    bool IsBytes() { return _is_bytes; }
    int Quality() { return _quality; }
    void Parse(NodeXML*);           // разбор ветви дерава XML с параметрами объекта
    void WriteSettings(QString, int, int, int, int, int);
private:
    bool _is_exchange;          // наличие приема декодированных пакетов за заданный интервал
    bool _is_bytes;             // наличие приема данных за заданный интервал
    int _quality;               // качество связи
    quint32 _errors_count;          // количество ошибок порта
    QTimer _timer;
    SerialPortSettings _settings;
    ExchangeType _type_exchange; // тип обмена: master - ведущий, slave - ведомый, async - асинхронный
    ProtocolType _type_protocol; // тип протокола: Staffing - со стафинг-байтом
    int _delay;              // Интервал в мс между посылаемыми пакетами (для Master и Async)
    int _limit;              // Интервал в мс, после которого связь считается утерянной
    int _wait;               // Накапливаемое время ожидания между передачей пакетов
    int _watchdog;           // Накапливаемое время наличия прихода пакетов
    int _wd_bytes;            // Накапливаемое время наличия прихода посылок (байтов, без анализа пакетов)
    int _counter;
    bool Connect();         // Сроединение по последовательному порту
    void ParsePort(NodeXML*);
    void Write();       //
signals:
    void FinishedSignal();      // Сигнал закрытия класса
    void LogSignal(QString);    // Сигнал лога порта
    void ReadSignal(QString);   // QByteArray data); // Сигнал получения данных
    void DecodeSignal(QString); // Success decode packet
    void WriteSignal(QString);  // QByteArray);
    void LostExchangeSignal(QString);
    void RestoreExchangeSignal(QString);
public slots:
    void Disconnect();  // Слот отключения порта
    void Start();       // Run threader? open port and start timer
private slots:
    void HandleError(QSerialPort::SerialPortError error);//Слот обработки ощибок
    void Read(); //Слот чтения из порта по ReadyRead
    void TimerStep();
    void ReceivePacket();
};

#endif // EXTSERIALPORT_H
