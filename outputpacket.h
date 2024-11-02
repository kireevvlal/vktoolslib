#ifndef OUTPUTPACKET_H
#define OUTPUTPACKET_H
#include <QObject>
#include "treexml.h"
#include "vktoolstypes.h"
#include "parameter.h"

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
private:
    ParameterList _parameters;
     QByteArray _data;
    int _length;
    OrderType _order;
    QByteArray (OutputPacket::*BuildFunction)();
    QByteArray Staffing();
    void Pack();
};

#endif // OUTPUTPACKET_H
