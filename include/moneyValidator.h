#ifndef MONEY_VALIDATOR_H
#define MONEY_VALIDATOR_H

#include <qvalidator.h>

class MoneyValidator : public QValidator
{
    public:
        MoneyValidator(QObject *parent, const char *name = 0);
        ~MoneyValidator();

        QValidator::State validate(QString &input, int&) const;
        void fixup(QString &input) const;

    private:
        QRegExp *regexp;
};

#endif
