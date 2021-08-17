#ifndef _CREMULTILINEITEMDELEGATE_H
#define _CREMULTILINEITEMDELEGATE_H

/**
 * Allow the edition of a multiline item.
 */
class CREMultilineItemDelegate : public QStyledItemDelegate
{
    public:
        /**
         * Standard constructor.
         * @param parent ancestor of this item.
         * @param asStringList if true then data is a QStringList, else a QString.
         * @param trimEmpty if true then all empty items will be removed, onyl significant if asStringList is true.
         */
        CREMultilineItemDelegate(QObject* parent, bool asStringList = true, bool trimEmpty = false);
        virtual ~CREMultilineItemDelegate();

        virtual QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const;
        virtual void setEditorData(QWidget* editor, const QModelIndex& index) const;
        virtual void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const;

private:
    bool myAsStringList;
    bool myTrimEmpty;
};

#endif /* _CREMULTILINEITEMDELEGATE_H */
