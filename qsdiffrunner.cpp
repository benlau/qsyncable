/* QSyncable Project
   Author: Ben Lau
   License: Apache-2.0
   Web: https://github.com/benlau/qsyncable
*/
#include <QHash>
#include <QDebug>
#include "qsdiffrunner.h"

#define MISSING_KEY_WARNING "QSDiffRunner.compare() - Duplicated or missing key."

class QSDiffRunnerMapper {
public:
    QSDiffRunnerMapper(int from = -1 , int to = -1) {
        this->atFrom = from;
        this->atTo = to;
    }

    // The position in "from" list
    int atFrom;

    // The position in "from" after removal.
    int atRemovedFrom;

    // The position in "to" list
    int atTo;

};

static QSPatch createInsertPath(int from, int to, const QVariantList& source ) {
    int count = to - from + 1;

    return QSPatch(QSPatch::Insert, from, to, count, source.mid(from, count));
}

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
            prev = prev.merged(current);
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

    // Compare the list, until it found moved component.
    int start = preprocess(from, to);

    if (start >= from.size() &&
        start >= to.size()) {
        // Nothing moved
        return combine();
    }

    QHash<QString, QSDiffRunnerMapper> hash;

    QVariantMap item;
    int shift = 0;

    hash.reserve(qMax(to.size(), from.size()) - start + 10);

    /* Step 1 - Check Removal */

    for (int i = start ; i < to.size() ; i++) {
        // Build hash table of "to" list
        item = to.at(i).toMap();
        QString key = item[m_keyField].toString();

        if (hash.contains(key)) {
            qWarning() << MISSING_KEY_WARNING;
            return compareWithoutKey(from, to);
        }

        QSDiffRunnerMapper mapper(-1,i);
        hash[key] = mapper;
    }

    shift = 0;

    for (int i = start ; i < from.size() ; i++) {
        // Find removed item and build index table.

        item = from.at(i).toMap();
        QString key = item[m_keyField].toString();

        QSDiffRunnerMapper mapper = QSDiffRunnerMapper(-1,-1);

        if (hash.contains(key)) {
            mapper = hash[key];
        }

        if (mapper.atTo < 0) {
            QSPatch patch = QSPatch(QSPatch::Remove,
                                    i, i, 1);

            appendPatch(patch);
            shift--;
        } else {

            if (mapper.atFrom >= 0) {
                qWarning() << MISSING_KEY_WARNING;
                return compareWithoutKey(from, to);
            }

            mapper.atFrom = i;
            mapper.atRemovedFrom = i + shift;
            hash[key] = mapper;
        }
    }

    /* Step 2 - Compare to find move and update */

    int insertStart = -1;
    shift = 0;

    for (int i = start ; i < to.size() ; i++) {
        item = to.at(i).toMap();
        QString key = item[m_keyField].toString();

        QSDiffRunnerMapper mapper = QSDiffRunnerMapper(-1,-1);

        if (hash.contains(key)) {
            mapper = hash[key];
        }

        if (mapper.atFrom < 0 ) {
            // New item
            shift++;

            if (insertStart < 0) {
                insertStart = i;
            }

        } else {

            if (insertStart >=0 ) {
                // Add Insert Patch with a block of data. It is faster.
                appendPatch(createInsertPath(insertStart, i - 1, to));
                insertStart = -1;
            }

            int realPos = mapper.atFrom; // Real position in fromList
            int expectedPos = mapper.atRemovedFrom; // Expected position in from
            int shiftedPos = expectedPos + shift; // Expected position + shift;

            if (shiftedPos != i) {
                QSPatch change(QSPatch::Move, expectedPos, i, 1);
                appendPatch(change);
                shift++;
            }

            QVariantMap before = from.at(realPos).toMap();
            QVariantMap after = item;
            QVariantMap diff = compareMap(before, after);
            if (diff.size() > 0) {
                updatePatches << QSPatch(QSPatch::Update, i, i, 1, diff);
            }
        }
    }

    if (insertStart >=0) {
        appendPatch(createInsertPath(insertStart, to.size()  - 1, to));
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

void QSDiffRunner::appendPatch(const QSPatch &value)
{
    bool merged = false;

    if (patches.size() > 0) {
        if (patches.last().canMerge(value)) {
            patches.last().merge(value);
            merged = true;
        }
    }

    if (!merged) {
        patches << value;
    }
}

