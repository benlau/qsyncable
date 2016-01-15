#ifndef QSDIFFRUNNER_H
#define QSDIFFRUNNER_H

#include "qschange.h"

class QSDiffRunner
{
public:
    QSDiffRunner();

    QString keyField() const;

    void setKeyField(const QString &keyField);

    QList<QSChange> compare(const QVariantList& previous,
                            const QVariantList& current);

signals:

public slots:

private:
    QString m_keyField;
};

#endif // QSDIFFRUNNER_H
