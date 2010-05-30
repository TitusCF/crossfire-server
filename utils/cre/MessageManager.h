#ifndef _MESSAGEMANAGER_H
#define	_MESSAGEMANAGER_H

#include <QList>
class MessageFile;

class MessageManager
{
    public:
        MessageManager();
        virtual ~MessageManager();

        void loadMessages();
        void saveMessages();

        QList<MessageFile*>& messages();

        const QStringList& preConditions() const;
        const QStringList& postConditions() const;

    private:
        QList<MessageFile*> myMessages;
        QStringList myPreConditions;
        QStringList myPostConditions;

        void loadDirectory(const QString& directory);
        void findPrePost(const QString directory, QStringList& list);
};

#endif	/* _MESSAGEMANAGER_H */

