#ifndef PARAMETER_H
#define PARAMETER_H
#include <QObject>
#include "vktoolstypes.h"
#include "treexml.h"

class Parameter
{
public:
    QString Name;
    QString Variable;
    int Size;
    DataType Type;
    int Byte;
    int Bit;
    Parameter();
    void Parse(NodeXML*);
    int Value;
    int Index;
    float Coefficient;
};

class ParameterList : public QVector<Parameter*> {
public:
};

#endif // PARAMETER_H
