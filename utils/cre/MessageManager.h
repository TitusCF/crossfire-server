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

        QList<MessageFile*> messages();

    private:
        QList<MessageFile*> myMessages;

        void loadDirectory(const QString& directory);
};

#endif	/* _MESSAGEMANAGER_H */

