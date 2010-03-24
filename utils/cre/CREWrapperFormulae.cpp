#include "CREWrapperFormulae.h"

CREWrapperFormulae::CREWrapperFormulae()
{
    myFormulae = NULL;
}

void CREWrapperFormulae::setFormulae(const recipe* rec)
{
    myFormulae = rec;
}

QString CREWrapperFormulae::title() const
{
    return myFormulae->title;
}

int CREWrapperFormulae::chance() const
{
    return myFormulae->chance;
}

int CREWrapperFormulae::difficulty() const
{
    return myFormulae->diff;
}

QStringList CREWrapperFormulae::archs() const
{
    QStringList archs;
    for (int i = 0; i < myFormulae->arch_names; i++)
    {
        archs.append(myFormulae->arch_name[i]);
    }
    return archs;
}
