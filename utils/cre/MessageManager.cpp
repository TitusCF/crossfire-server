#include "MessageManager.h"
#include "MessageFile.h"

extern "C" {
    #include "global.h"
}

#include <QDir>
#include <QDebug>

MessageManager::MessageManager()
{
}

MessageManager::~MessageManager()
{
    qDeleteAll(myMessages);
}

void MessageManager::loadMessages()
{
    loadDirectory("");

    /* get pre and post conditions */
    findPrePost("pre", myPreConditions);
    findPrePost("post", myPostConditions);
}

void MessageManager::saveMessages()
{
    foreach(MessageFile* file, myMessages)
    {
        file->save();
    }
}

QList<MessageFile*>& MessageManager::messages()
{
    return myMessages;
}

void MessageManager::loadDirectory(const QString& directory)
{
    //qDebug() << "load" << directory;
    QDir dir(QString("%1/%2/%3").arg(settings.datadir, settings.mapdir, directory));

    // first messages
    QStringList messages = dir.entryList(QStringList("*.msg"), QDir::Files);
    //qDebug() << "found" << messages;
    foreach(QString message, messages)
    {
        QString path = directory + QDir::separator() + message;
        MessageFile* file = new MessageFile(path);
        if (file->parseFile())
        {
            myMessages.append(file);
        }
        else
        {
            qDebug() << "dialog parse error" << path;
            delete file;
        }
    }

    // recurse
    QStringList subdirs = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    foreach(QString sub, subdirs)
        loadDirectory(directory + QDir::separator() + sub);
}

const QStringList& MessageManager::preConditions() const
{
    return myPreConditions;
}

const QStringList& MessageManager::postConditions() const
{
    return myPostConditions;
}

void MessageManager::findPrePost(const QString directory, QStringList& list)
{
    QDir dir(QString("%1/%2/python/dialog/%3").arg(settings.datadir, settings.mapdir, directory));
    QFileInfoList files = dir.entryInfoList(QStringList("*.py"));
    foreach(QFileInfo file, files)
    {
        list.append(file.baseName());
    }
}
