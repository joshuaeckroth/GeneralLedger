#include <qvalidator.h>
#include <qregexp.h>

#include "moneyValidator.h"

MoneyValidator::MoneyValidator(QObject *parent, const char *name)
    : QValidator(parent,name)
{
    regexp = new QRegExp("^\\d*\\.?\\d?\\d?$");
}

MoneyValidator::~MoneyValidator()
{
    delete regexp;
}

QValidator::State MoneyValidator::validate(QString &input, int&) const
{
    if(input == "")
        return QValidator::Acceptable;
    if(regexp->search(input) == -1)
        return QValidator::Invalid;
    else
        return QValidator::Intermediate;
}

void MoneyValidator::fixup(QString &input) const
{
    if(input == "")
        return;

    if(input.right(1) == ".")
        input.append("00");
    if(input.mid(input.length() - 2, 1) == ".")
        input.append("0");
    if(input.find('.') == -1)
    {
        if(input.length() == 1)
            input.insert(input.length() - 1, ".0");
        else
            input.insert(input.length() - 2, ".");
    }

    if(input.left(1) == ".")
        input.insert(0, "0");
}
