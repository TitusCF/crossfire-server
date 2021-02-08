#include "CREWrapperTreasure.h"

CREWrapperTreasureList::CREWrapperTreasureList(const treasurelist* list) : myList(list)
{
}

int CREWrapperTreasureList::itemCount() const
{
    int count = 0;
    auto item = myList->items;
    while (item) {
        count++;
        item = item->next;
    }
    return count;
}
