/* QSyncable Project
   Author: Ben Lau
   License: Apache-2.0
   Web: https://github.com/benlau/qsyncable
*/

#ifndef QSDIFFRUNNER_H
#define QSDIFFRUNNER_H

#include "qspatch.h"
#include "qspatchable.h"

class QSDiffRunner
{
public:
    QSDiffRunner();

    QString keyField() const;

    void setKeyField(const QString &value);

    QList<QSPatch> compare(const QVariantList& from,
                           const QVariantList& to);

    bool patch(QSPatchable* patchable, const QList<QSPatch>& patches) const;

signals:

public slots:

private:

    // Combine all the processing patches into a single list. It will clear the processing result too.
    QList<QSPatch> combine();

    // Preprocess the list, stop until the key is different
    int preprocess(const QVariantList& from, const QVariantList& to);

    void appendPatch(const QSPatch& patch);

    QList<QSPatch> patches;
    QList<QSPatch> updatePatches;

    QString m_keyField;
};

#endif // QSDIFFRUNNER_H
