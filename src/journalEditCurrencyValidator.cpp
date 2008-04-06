/* General Ledger, Copyright (C) 2004  Joshua Eckroth <josh@eckroth.net>
 * http://www.eckroth.net/software/genledg
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  US
*/

#include <qvalidator.h>
#include <qregexp.h>

#include "journalEditCurrencyValidator.h"

JournalEditCurrencyValidator::JournalEditCurrencyValidator(QObject *parent, const char *name)
    : QValidator(parent,name)
{
    regexp = new QRegExp("^\\d*\\.?\\d?\\d?$");
}

JournalEditCurrencyValidator::~JournalEditCurrencyValidator()
{
    delete regexp;
}

QValidator::State JournalEditCurrencyValidator::validate(QString &input, int&) const
{
    if(input == "")
        return QValidator::Acceptable;
    if(regexp->search(input) == -1)
        return QValidator::Invalid;
    else
        return QValidator::Intermediate;
}

void JournalEditCurrencyValidator::fixup(QString &input) const
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

