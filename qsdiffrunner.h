/* QSyncable Project
   Author: Ben Lau
   License: Apache-2.0
   Web: https://github.com/benlau/qsyncable
*/

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
