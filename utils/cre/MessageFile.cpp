#include <qiodevice.h>
#include <qfile.h>
#include <qscriptengine.h>
#include <QDebug>

#include "MessageFile.h"

extern "C" {
    #include "global.h"
}

MessageRule::MessageRule()
{

}

MessageRule::~MessageRule()
{
}

const QStringList& MessageRule::match() const
{
    return myMatch;
}

void MessageRule::setMatch(const QStringList& match)
{
    myMatch = match;
}

const QList<QStringList>& MessageRule::preconditions() const
{
    return myPreconditions;
}

void MessageRule::setPreconditions(const QList<QStringList>& preconditions)
{
    myPreconditions = preconditions;
}

const QList<QStringList>& MessageRule::postconditions() const
{
    return myPostconditions;
}

void MessageRule::setPostconditions(const QList<QStringList>& postconditions)
{
    myPostconditions = postconditions;
}

const QStringList& MessageRule::messages() const
{
    return myMessages;
}

void MessageRule::setMessages(const QStringList& messages)
{
    myMessages = messages;
}

const QString& MessageRule::include() const
{
    return myInclude;
}

void MessageRule::setInclude(const QString& include)
{
    myInclude = include;
}

const QList<QStringList>& MessageRule::replies() const
{
    return myReplies;
}

void MessageRule::setReplies(const QList<QStringList>& replies)
{
    myReplies = replies;
}

MessageFile::MessageFile(const QString& path)
{
    myPath = path;
}

MessageFile::~MessageFile()
{
    qDeleteAll(myRules);
}

const QString& MessageFile::location() const
{
    return myLocation;
}

const QString& MessageFile::path() const
{
    return myPath;
}

void MessageFile::setLocation(const QString& location)
{
    myLocation = location;
}

void convert(QScriptValue& value, QList<QStringList>& list)
{
    list.clear();
    int length = value.property("length").toInt32();
    for (int l = 0; l < length; l++)
    {
        QStringList items;
        QScriptValue sub = value.property(l);

        int subl = sub.property("length").toInt32();
        for (int s = 0; s < subl; s++)
            items.append(sub.property(s).toString());

        list.append(items);
    }
}

bool MessageFile::parseFile()
{
    QString full = QString("%1/%2/%3").arg(settings.datadir, settings.mapdir, myPath);
    QFile file(full);
    file.open(QIODevice::ReadOnly);

    QByteArray data = file.readAll();
    if (data.size() == 0)
        return false;
    QString script = data;
    if (!script.startsWith('['))
        script = "[" + script + "]";

    QScriptValue value;
    QScriptEngine engine;
    value = engine.evaluate(script);
    if (engine.hasUncaughtException())
    {
        qDebug() << "message file evaluate error" << myPath << engine.uncaughtException().toString();
        return false;
    }

    QScriptValue first = value.property(0);
    myLocation = first.property("location").toString();

    QScriptValue rules = first.property("rules");
    int length = rules.property("length").toInt32();
    for (int r = 0; r < length; r++)
    {
        MessageRule* rule = new MessageRule();
        myRules.append(rule);
        QScriptValue v = rules.property(r);

        QStringList items;
        qScriptValueToSequence(v.property("match"), items);
        rule->setMatch(items);

        QList<QStringList> lists;
        QScriptValue p = v.property("pre");
        convert(p, lists);
        rule->setPreconditions(lists);

        p = v.property("post");
        convert(p, lists);
        rule->setPostconditions(lists);

        items.clear();
        qScriptValueToSequence(v.property("msg"), items);

        rule->setInclude(v.property("include").toString());
        rule->setMessages(items);

        p = v.property("replies");
        convert(p, lists);
        rule->setReplies(lists);
    }
    return true;
}

QList<MessageRule*> MessageFile::rules()
{
    return myRules;
}