#ifndef _MESSAGEFILE_H
#define	_MESSAGEFILE_H

#include <QObject>
#include <QStringList>

class MessageRule : public QObject
{
    Q_OBJECT

    public:
        MessageRule();
        virtual ~MessageRule();

        const QStringList& match() const;
        QStringList& match();
        void setMatch(const QStringList& match);
        const QList<QStringList>& preconditions() const;
        void setPreconditions(const QList<QStringList>& preconditions);
        const QList<QStringList>& postconditions() const;
        void setPostconditions(const QList<QStringList>& postconditions);
        const QStringList& messages() const;
        void setMessages(const QStringList& messages);
        const QString& include() const;
        void setInclude(const QString& include);
        const QList<QStringList>& replies() const;
        void setReplies(const QList<QStringList>& replies);

        bool isModified() const;
        void setModified(bool modified = true);

    private:
        bool myIsModified;
        QStringList myMatch;
        QList<QStringList> myPreconditions;
        QList<QStringList> myPostconditions;
        QStringList myMessages;
        QString myInclude;
        QList<QStringList> myReplies;
};

class MessageFile : public QObject
{
    Q_OBJECT

    public:
        MessageFile(const QString& path);
        virtual ~MessageFile();

        bool parseFile();

        const QString& path() const;
        void setPath(const QString& path);

        const QString& location() const;
        void setLocation(const QString& location);

        QList<MessageRule*>& rules();

        void save();

        bool isModified() const;
        void setModified(bool modified = true);

    private:
        bool myIsModified;
        QString myPath;
        QString myLocation;
        QList<MessageRule*> myRules;
};

#endif	/* _MESSAGEFILE_H */

