#include "parameter.h"

Parameter::Parameter()
{
    Name = "";
    Bit = -1;
    Size = 0;
    Value = -1;
    Type = DataType::Byte;
    Index = 0;
}
//--------------------------------------------------------------------------------
void Parameter::Parse(NodeXML *node)
{
    int i;
    QString val;
    QString value;
    Name = node->Text;
    for (i = 0; i < node->Attributes.count(); i++) {
        AttributeXML *attr = node->Attributes[i];
        if (attr->Name == "type") {
            val = attr->Value.toLower();
            Type = (val == "bit") ? DataType::Bit : (val == "byte") ? DataType::Byte : (val == "ubyte") ? DataType::UByte : (val == "int") ? DataType::Int16 :
                   (val == "uint") ? DataType::Uint16 : (val == "int32") ? DataType::Int32 : (val == "uint32") ? DataType::Uint32 :
                   (val == "float") ? DataType::Float : (val == "double") ? DataType::Double : DataType::Byte;
        }
        else
        if (attr->Name == "size")
            Size = attr->Value.toInt();
        else if (attr->Name == "byte")
            Byte = attr->Value.toInt();
        else if (attr->Name == "bit")
            Bit = attr->Value.toInt();
        else if (attr->Name == "val")
            Value = attr->Value.toInt();
        else if (attr->Name == "index")
            Index = attr->Value.toInt();
        else if (attr->Name == "coeff")
            Coefficient = attr->Value.toFloat();
        else if (attr->Name == "var")
            Variable = attr->Value;
    }
//    if (node->Child != nullptr) {
//        node = node->Child;
//        while (node != nullptr) {
//            if (node->Name == "val")
//                ;//ParsePort(node);
//            node = node->Next;
//        }
//    }
}
