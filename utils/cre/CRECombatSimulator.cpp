#include "CRECombatSimulator.h"
#include "CREPixmap.h"

extern "C" {
#include <global.h>
#include <sproto.h>
}

CRECombatSimulator::CRECombatSimulator()
{
    QGridLayout* layout = new QGridLayout(this);
    int line = 0;

    layout->addWidget(new QLabel(tr("First fighter:"), this), line, 0);
    myFirst = new QComboBox(this);
    layout->addWidget(myFirst, line++, 1);

    layout->addWidget(new QLabel(tr("Second fighter:"), this), line, 0);
    mySecond = new QComboBox(this);
    layout->addWidget(mySecond, line++, 1);

    const archetype* arch = first_archetype;
    while (arch)
    {
        if (arch->head == NULL && QUERY_FLAG(&arch->clone, FLAG_MONSTER))
        {
            myFirst->addItem(CREPixmap::getIcon(arch->clone.face->number), arch->name, qVariantFromValue((void*)arch));
            mySecond->addItem(CREPixmap::getIcon(arch->clone.face->number), arch->name, qVariantFromValue((void*)arch));
        }
        arch = arch->next;
    }

    myResultLabel = new QLabel(tr("Combat result:"), this);
    myResultLabel->setVisible(false);
    layout->addWidget(myResultLabel, line++, 0, 1, 2);
    myResult = new QLabel(this);
    myResult->setVisible(false);
    layout->addWidget(myResult, line++, 0, 1, 2);

    QDialogButtonBox* box = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Close, Qt::Horizontal, this);
    layout->addWidget(box, line++, 0, 1, 2);
    connect(box, SIGNAL(rejected()), this, SLOT(reject()));
    connect(box, SIGNAL(accepted()), this, SLOT(fight()));

    setWindowTitle(tr("Combat simulator"));
}

CRECombatSimulator::~CRECombatSimulator()
{
}

void CRECombatSimulator::fight()
{
    const archetype* first = (const archetype*)myFirst->itemData(myFirst->currentIndex()).value<void*>();
    const archetype* second = (const archetype*)mySecond->itemData(mySecond->currentIndex()).value<void*>();

    int limit = 5000;
    object* obfirst = object_create_arch((archetype*)first);
    object* obsecond = object_create_arch((archetype*)second);
    tag_t tagfirst = obfirst->count;
    tag_t tagsecond = obsecond->count;

    // make a big map so large monsters are ok in map
    mapstruct* test_map = get_empty_map(50, 50);

    // insert shifted for monsters like titans who have parts with negative values
    obfirst = object_insert_in_map_at(obfirst, test_map, NULL, 0, 12, 12);
    obsecond = object_insert_in_map_at(obsecond, test_map, NULL, 0, 37, 37);

    while (limit-- > 0 && obfirst->stats.hp > 0 && obsecond->stats.hp > 0)
    {
        if (obfirst->speed_left > 0) {
            --obfirst->speed_left;
            monster_do_living(obfirst);
            attack_ob(obfirst, obsecond);
            if (object_was_destroyed(obsecond, tagsecond))
                break;
        }

        if (obsecond->speed_left > 0) {
            --obsecond->speed_left;
            monster_do_living(obsecond);
            attack_ob(obsecond, obfirst);
            if (object_was_destroyed(obfirst, tagfirst))
                break;
        }

        if (obfirst->speed_left <= 0)
            obfirst->speed_left += FABS(obfirst->speed);
        if (obsecond->speed_left <= 0)
            obsecond->speed_left += FABS(obsecond->speed);
    }

    if (limit < 0)
        myResult->setText(tr("No clear winner"));
    else
        myResult->setText(tr("%1 hp: %2 - %3 hp: %4").arg(first->name).arg(obfirst->stats.hp).arg(second->name).arg(obsecond->stats.hp));

    if (!object_was_destroyed(obfirst, tagfirst))
    {
        object_remove(obfirst);
        object_free2(obfirst, 0);
    }
    if (!object_was_destroyed(obsecond, tagsecond))
    {
        object_remove(obsecond);
        object_free2(obsecond, 0);
    }
    free_map(test_map);

    myResultLabel->setVisible(true);
    myResult->setVisible(true);
}
