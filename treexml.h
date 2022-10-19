#ifndef TREEXML_H
#define TREEXML_H

#include <QXmlStreamReader>
//#include "ExtraTools_global.h"

class AttributeXML
{
public:
    QString Name;
    QString Value;
    AttributeXML(QString, QString);
    ~AttributeXML();
};
//--------------------------------------------------------------------------------
class NodeXML
{
public:
    QString Name;
    QString Text;
    NodeXML *Next;
    NodeXML *Parent;
    NodeXML *Child;
    bool Readed;
    QList<AttributeXML*> Attributes;
    NodeXML(QString);
    ~NodeXML();
};
//--------------------------------------------------------------------------------
class TreeXML
{
private:
    void Add(QString);                          // Add node to tree
    void EndElement(QString);                   // EndElement token routine
    void SetAttributes(QXmlStreamAttributes);   // Read attributes
    void RefreshData(NodeXML*);
    void SetText(QString);                      // Read text
    QString DeleteSpecChars(QString);           // Remove spec chars from text
public:
    NodeXML *Current;                           // pointer on active node
    NodeXML *Root;                              // root node
    TreeXML();
    ~TreeXML();
    void Delete(NodeXML*);
    void Clear();
    bool ReadFile(QString);                     // read tree from file
};

#endif // TREEXML_H
