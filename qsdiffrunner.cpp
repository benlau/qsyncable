/* QSyncable Project
   Author: Ben Lau
   License: Apache-2.0
   Web: https://github.com/benlau/qsyncable
*/
#include <QHash>
#include <QDebug>
#include "qsdiffrunner.h"

#define MISSING_KEY_WARNING "QSDiffRunner.compare() - Duplicated or missing key."

static QList<QSPatch> merge(const QList<QSPatch> list) {

    if (list.size() <= 1) {
        return list;
    }

    QList<QSPatch> res;

    QSPatch prev;

    for (int i = 0 ; i < list.size() ; i++) {
        QSPatch current = list.at(i);

        if (prev.isNull()) {
            prev = current;
            continue;
        }

        if (prev.canMerge(current)) {
            prev = prev.merge(current);
        } else {
            res << prev;
            prev = current;
        }
    }

    if (!prev.isNull()) {
        res << prev;
    }

    return res;
}

static QVariantMap compareMap(const QVariantMap& prev, const QVariantMap &current) {
    // To make this function faster, it won't track removed fields from prev.
    // Clear a field to null value should set it explicitly.

    QVariantMap diff;

    QMap<QString, QVariant>::const_iterator iter = current.begin();

    while (iter != current.end()) {
        QString key = iter.key();
        if (!prev.contains(key) ||
             prev[key] != iter.value()) {
            diff[key] = iter.value();
        }
        iter++;
    }

    return diff;
}

static QList<QSPatch> compareWithoutKey(const QVariantList& from, const QVariantList& to) {

    QList<QSPatch> patches;

    int max = qMax(from.size(), to.size());

    for (int i = 0 ; i < max ; i++) {
        if (i >= from.size()) {
            patches << QSPatch(QSPatch::Insert, i, i, 1, to[i].toMap());
        } else if (i >= to.size() ) {
            patches << QSPatch(QSPatch::Remove, i, i, 1);
        } else {
            QVariantMap diff = compareMap(from[i].toMap(), to[i].toMap());
            if (diff.size()) {
                patches << QSPatch(QSPatch::Update, i, i, 1, diff);
            }
        }
    }

    return merge(patches);
}

QSDiffRunner::QSDiffRunner()
{

}

QString QSDiffRunner::keyField() const
{
    return m_keyField;
}

void QSDiffRunner::setKeyField(const QString &keyField)
{
    m_keyField = keyField;
}

/*! \fn QList<QSChange> QSDiffRunner::compare(const QVariantList &from, const QVariantList &to)

    Call this function to compare two list, then return a
    list of patches required to transform from a list to other with
    the minimum number of steps. It uses an algorithm with O(n) runtime.
 */

QList<QSPatch> QSDiffRunner::compare(const QVariantList &from, const QVariantList &to)
{
    patches.clear();
    updatePatches.clear();

    if (m_keyField.isEmpty()) {
        return compareWithoutKey(from, to);
    }

    int start = preprocess(from, to);

    if (start >= from.size() &&
        start >= to.size()) {
        // Nothing moved
        return combine();
    }

    QVariantList fromList;

    QHash<QString, int> toHashTable;
    QHash<QString, int> fromHashTable;
    QVariantMap item;
    int offset = 0;

    toHashTable.reserve(to.size() - start + 10);
    fromHashTable.reserve(from.size() - start + 10);

    /* Step 1 - Check Removal */

    for (int i = start ; i < to.size() ; i++) {
        // Build toHashTable
        item = to.at(i).toMap();
        QString key = item[m_keyField].toString();

        if (toHashTable.contains(key)) {
            qWarning() << MISSING_KEY_WARNING;
            return compareWithoutKey(from, to);
        }

        toHashTable[key] = i;
    }

    if (start != 0) {
        //@TODO - remove
        fromList = from.mid(0, start);
    }

    fromList.reserve(from.size());

    // Find removed item and build index table.
    for (int i = start ; i < from.size() ; i++) {
        item = from.at(i).toMap();
        QString key = item[m_keyField].toString();

        if (!toHashTable.contains(key)) {
            patches << QSPatch(QSPatch::Remove,
                           i, i, 1);
        } else {
            fromList << item;

            if (fromHashTable.contains(key)) {
                qWarning() << MISSING_KEY_WARNING;
                return compareWithoutKey(from, to);
            }

            fromHashTable[key] = fromList.count() - 1;
        }
    }

    /* Step 2 - Compare to find move and update */

    if (fromList.size() == 0 && to.size() > 0) {
        // Original items are all removed.
        // A special case. Insert all
        patches <<  QSPatch(QSPatch::Insert, 0, to.size() - 1,to.size(), to);
        return combine();
    }

    for (int i = start ; i < to.size() ; i++) {
        item = to.at(i).toMap();
        QString key = item[m_keyField].toString();

        if (!fromHashTable.contains(key)) {
            offset++;
            patches << QSPatch(QSPatch::Insert, i, i, 1, item);
        } else {
            int prevPos = fromHashTable[key];
            int expectedPos = prevPos + offset;

            if (expectedPos != i) {
                QSPatch change(QSPatch::Move, prevPos, i, 1);
                patches << change;

                offset++;
            }

            QVariantMap before = fromList.at(prevPos).toMap();
            QVariantMap after = item;
            QVariantMap diff = compareMap(before, after);
            if (diff.size() > 0) {
                updatePatches << QSPatch(QSPatch::Update, i, i, 1, diff);
            }
        }
    }


    return combine();
}

bool QSDiffRunner::patch(QSPatchable *patchable, const QList<QSPatch>& patches) const
{
    QVariantMap diff;
    foreach (QSPatch patch, patches) {
        switch (patch.type()) {
        case QSPatch::Remove:
            patchable->remove(patch.from(), patch.count());
            break;
        case QSPatch::Insert:
            patchable->insert(patch.from(), patch.data());
            break;
        case QSPatch::Move:
            patchable->move(patch.from(), patch.to(), patch.count());
            break;
        case QSPatch::Update:
            if (patch.data().size() > 0) {
                diff = patch.data().at(0).toMap();
            }
            patchable->setProperties(patch.from(), diff);
            break;
        default:
            break;
        }

    }

    return true;
}

QList<QSPatch> QSDiffRunner::combine()
{
    QList<QSPatch> tmp = merge(patches);

    if (updatePatches.size() > 0) {
        tmp.append(updatePatches);
    }

    patches.clear();
    updatePatches.clear();

    return tmp;
}

int QSDiffRunner::preprocess(const QVariantList &from, const QVariantList &to)
{
    int index = 0;
    int min = qMin(from.size(), to.size());

    for (index = 0 ; index < min ;index++) {

        QVariantMap f = from[index].toMap();
        QVariantMap t = to[index].toMap();

        if (f[m_keyField] != t[m_keyField]) {
            break;
        }

        QVariantMap diff = compareMap(f,t);
        if (diff.size()) {
            updatePatches << QSPatch::createUpdate(index, diff);
        }
    }

    return index;
}

