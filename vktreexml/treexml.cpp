#include "treexml.h"

#include <QFile>
//#include <QDebug>

AttributeXML::AttributeXML(QString name, QString value)
{
    Name = name;
    Value = value;
}
//--------------------------------------------------------------------------------
AttributeXML::~AttributeXML()
{
    Name = Value = "";
}
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
NodeXML::NodeXML(QString name)
{
    Name = name;
    Next = Parent = Child = nullptr;
    Readed = false;
    Text = "";
}
//--------------------------------------------------------------------------------
NodeXML::~NodeXML()
{
    Next = Parent = Child = nullptr;
}
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
TreeXML::TreeXML()
{
    Current = Root = nullptr;
}
//--------------------------------------------------------------------------------
TreeXML::~TreeXML()
{
}
//--------------------------------------------------------------------------------
void TreeXML::Add(QString str)
{
    NodeXML *newNode = new NodeXML(str);
    if ((Current == nullptr) || (Current->Readed == true)) {
        if (Current != nullptr) {
            Current->Next = newNode;
            newNode->Parent = Current->Parent;
        }
        else
            Root = newNode;
        Current = newNode;
    }
    else {
        Current->Child = newNode;
        newNode->Parent = Current;
        Current = newNode;
    }
}
//--------------------------------------------------------------------------------
void TreeXML::EndElement(QString str)
{
    if (Current->Readed == false) {
        if (Current->Name == str)
            Current->Readed = true;
        else
           ;  //error
    }
    else  {
        if (Current->Parent != nullptr)  {
            Current = Current->Parent;
            EndElement(str);
        }
        else
            ; //error
    }
}
//--------------------------------------------------------------------------------
void TreeXML::SetAttributes(QXmlStreamAttributes attr)
{
     for (int i = 0; i <  attr.length(); i++) {
        AttributeXML *newAttr = new AttributeXML(attr[i].name().toString().toLower(),
                attr[i].value().toString()/*.toLower()*/);
        Current->Attributes.append(newAttr);
    }
}
//--------------------------------------------------------------------------------
void TreeXML::RefreshData(NodeXML *startNode)
{
    NodeXML *tmpNode = startNode;
    while ((tmpNode->Next != nullptr) || (tmpNode->Readed == true)) {
        tmpNode->Readed = false;
        if (tmpNode->Child != nullptr)
            RefreshData(tmpNode->Child);
        if (tmpNode->Next != nullptr)
            tmpNode = tmpNode->Next;
    }
}
//--------------------------------------------------------------------------------
void TreeXML::SetText(QString str)
{
    if (Current->Text == "")
    Current->Text = str; //DeleteSpecChars(str);
}
//--------------------------------------------------------------------------------
QString TreeXML::DeleteSpecChars(QString str)
{
    str = str.replace("\n", "");
    str = str.replace("\r", "");
    str = str.replace("\t", "");
    return str;
}
//--------------------------------------------------------------------------------
void TreeXML::Clear()
{
    Delete(Root);
    Root = Current = nullptr;
}
//--------------------------------------------------------------------------------
void TreeXML::Delete(NodeXML *node)
{
    // clear child
    if (node->Child != nullptr)
        Delete(node->Child);
    // clear next
    if (node->Next != nullptr)
        Delete(node->Next);
    //
    delete node;
    node = nullptr;
}
//--------------------------------------------------------------------------------
bool TreeXML::ReadFile(QString path)
{
    QFile* file = new QFile(path);
    QString name, text;
    if (!file->open(QIODevice::ReadOnly | QIODevice::Text)) {
        //emit Log(tr("Невозможно открыть XML-конфиг"), LOG_LEVEL_ERROR);
        return false;
    }
    else {
        QXmlStreamReader xml(file);
        while (!xml.atEnd() && !xml.hasError())
        {
            QXmlStreamReader::TokenType token = xml.readNext();
            //qDebug() << xml.text() << token;
            switch (token) {
            case QXmlStreamReader::StartDocument:
             //   qDebug() << "Start document";
                break;
            case QXmlStreamReader::EndDocument:
             //   qDebug() << "End document";
                break;
            case QXmlStreamReader::StartElement:
                name = xml.name().toString();
                Add(name);
                SetAttributes(xml.attributes());
              //  qDebug() << "Start element: " << name;
                break;
            case QXmlStreamReader::EndElement:
                name = xml.name().toString().toLower();
                EndElement(name);
              //  qDebug() << "End element" << name;
                break;
            case QXmlStreamReader::Characters:
                if ((text = DeleteSpecChars(xml.text().toString())) != "")
                    SetText(text);
              //  qDebug() << "Characters " <<   xml.text().toString() << token;
                break;
            default: break;
            }
        }
        if (xml.hasError())
            return false;
       // xml.clear();
    }
    file->close();
    delete file;
    return true;
}
