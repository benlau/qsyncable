/* QSyncable Project
   Author: Ben Lau
   License: Apache-2.0
   Web: https://github.com/benlau/qsyncable
*/
#include <QHash>
#include <QDebug>
#include <QLinkedList>
#include "qsdiffrunner.h"

#define MISSING_KEY_WARNING "QSDiffRunner.compare() - Duplicated or missing key."

class QSDiffRunnerState {
public:
    QSDiffRunnerState(int from = -1 , int to = -1) {
        this->atFrom = from;
        this->atTo = to;
        atRemovedFrom = -1;
        shiftedPos = -1;
        isMoved = false;
    }

    // The position in "from" list
    int atFrom;

    // The position in "from" after removal.
    int atRemovedFrom;

    // The position in "from" after applied "removed" / "inserted" and "moved"
    int shiftedPos;

    // Is it moved?
    bool isMoved;

    // The position in "to" list
    int atTo;


};

static QSPatch createInsertPath(int from, int to, const QVariantList& source ) {
    int count = to - from + 1;

    return QSPatch(QSPatch::Insert, from, to, count, source.mid(from, count));
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

    return patches;
}


class QSDiffRunnerAlgo {
public:
    enum Type {
        NoMove,
        Insert,
        Remove,
        Move
    };

    QSDiffRunnerAlgo() {
        insertStart = -1;
        removeStart = -1;
        removed = 0;
        inserted = 0;
        moved = 0;

        skipped = 0;
    }

    // Combine all the processing patches into a single list. It will clear the processing result too.
    QList<QSPatch> combine();

    QList<QSPatch> compare(const QVariantList& from, const QVariantList& to);

    // Preprocess the list, stop until the key is different. It will also handle common pattern (like append to end , remove from end)
    int preprocess(const QVariantList& from, const QVariantList& to);

    void buildHashTable();

    void appendPatch(const QSPatch& patch, bool merge = true);

    // Mark an item for insert, remove, move
    void markItemAtFromList(Type type,int index, QSDiffRunnerState &mapper);

    void markItemAtToList(Type type,int index, QSDiffRunnerState& mapper);

    QVariantList from;
    QVariantList to;

    // Stored patches (without any update patches)
    QList<QSPatch> patches;

    // Update patches
    QList<QSPatch> updatePatches;
    QString m_keyField;

    // Hash table
    QHash<QString, QSDiffRunnerState> hash;

    // The start position of remove block
    int removeStart;

    // The start position of insertion block
    int insertStart;

    // No. of removed item
    int removed;

    // No. of inserted item
    int inserted;

    // No. of moved item
    int moved;

    // A no. of item could be skipped found preprocess().
    int skipped;

    QString fKey,tKey;
};


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
    QSDiffRunnerAlgo algo;
    algo.m_keyField = m_keyField;
    return algo.compare(from, to);
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

QList<QSPatch> QSDiffRunnerAlgo::combine()
{
    QList<QSPatch> movePatches;
    movePatches.reserve(patches.size());

    for (int i = patches.size() - 1 ; i >= 0 ; i--) {
        QSPatch patch = patches.at(i);
        if (patch.type() == QSPatch::Move) {
            QVariantMap item = to.at(patch.to()).toMap();
            QString key = item[m_keyField].toString();
            QSDiffRunnerState mapper = hash[key];

            for (int j = movePatches.size() - 1; j >= 0;j--) {
                qDebug() << "compare" << movePatches.at(j).to() << patch.to();
                if (movePatches.at(j).to() < patch.to()) {
                    qDebug() << "remove";
                    movePatches.removeAt(j);
                }
            }

            if (mapper.atFrom != mapper.shiftedPos + movePatches.size()) {
                qDebug() << "update move" << patch.from() << mapper.shiftedPos
                       << mapper.shiftedPos + movePatches.size();
            }
            patch.setFrom(mapper.shiftedPos + movePatches.size());
            patches[i] = patch;
            movePatches << patch;
        }
    }

    if (updatePatches.size() > 0) {
        patches.append(updatePatches);
    }

    return patches;
}

QList<QSPatch> QSDiffRunnerAlgo::compare(const QVariantList &from, const QVariantList &to)
{
    patches.clear();
    updatePatches.clear();

    this->from = from;
    this->to = to;

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

    buildHashTable();

#if 1
    int f = skipped, t = skipped;

    removed = 0;
    inserted = 0;
    QVariantMap fItem,tItem;

    while (f < from.size() || t < to.size()) {
        // Step 1. Check removal
        QSDiffRunnerState mapper;

        fKey.clear();

        while (f < from.size()) {
            fItem = from.at(f).toMap();
            fKey = fItem[m_keyField].toString();
            mapper = hash[fKey]; // It mush obtain the key value

            if (mapper.atTo < 0) {
                markItemAtFromList(Remove, f++, mapper);
            } else if (mapper.isMoved) {
                markItemAtFromList(Move, f++, mapper);
            } else {
                markItemAtFromList(NoMove, f, mapper);
                break;
            }
        }

        if (f >= from.size() && t < to.size()) {
            // The rest in "to" list is new items
            appendPatch(createInsertPath(t, to.size() - 1, to), false);
            return combine();
        }

        while (t < to.size() ) {
            tItem = to.at(t).toMap();
            tKey = tItem[m_keyField].toString();
            mapper = hash[tKey];

            if (mapper.atFrom < 0) {
                // new item
                markItemAtToList(Insert,t++, mapper);
            } else {
                if (tKey != fKey) {
                    markItemAtToList(Move, t++, mapper);
                } else {
                    markItemAtToList(NoMove, t++, mapper);
                    f++;
                    break;
                }
            }
        }
    }
    //@TODO - combine should update move patch
    return combine();

#else
    QVariantMap item;

    hash.reserve( (qMax(to.size(), from.size()) - start) * 2 + 10);

    /* Step 1 - Check Removal */

    for (int i = start ; i < to.size() ; i++) {
        // Build hash index table on "to" list
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
        // Find removed item and build index table on "from" list.

        item = from.at(i).toMap();
        QString key = item[m_keyField].toString();

        QSDiffRunnerMapper mapper = QSDiffRunnerMapper(-1,-1);

        if (hash.contains(key)) {
            mapper = hash[key];
        }

        if (mapper.atTo < 0) {
            markItemAtFromList(QSDiffRunnerAlgo::Remove, i);
            shift--;
        } else {

            markItemAtFromList(QSDiffRunnerAlgo::NoMove, i);

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

    insertStart = -1;
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

            markItemAtToList(QSDiffRunnerAlgo::Insert, i, mapper);
            inserted++;

        } else {
            int realPos = mapper.atFrom; // Real position in fromList
            int expectedPos = mapper.atRemovedFrom; // Expected position in from
            int shiftedPos = expectedPos + inserted; // Expected position + shift;

            qDebug() << "key" << key << i << moved << moveCounter << expectedPos;

            if (expectedPos < maxMoveFrom) {
                shiftedPos += moved;
            }

            if ( i >= maxMoveFrom ) {
                moved = 0;
            }

            if (moveCounter > 0) {
                moveCounter--;
            }

            if (shiftedPos != i) {
                // Must mark item before append.
                markItemAtToList(QSDiffRunnerAlgo::Move, i, mapper);

                qDebug() << "move from" << shiftedPos << i;
                moved++;
                if (shiftedPos > maxMoveFrom) {
                    maxMoveFrom = shiftedPos;
                }
//                maxMoveFrom = shiftedPos;
                moveCounter = shiftedPos - i;
                qDebug() << "moved" << moved << "moveCounter" << moveCounter;
                QSPatch change(QSPatch::Move, shiftedPos, i, 1);
                appendPatch(change);
            } else {
                markItemAtToList(QSDiffRunnerAlgo::NoMove, i, mapper);
            }

            QVariantMap before = from.at(realPos).toMap();
            QVariantMap after = item;
            QVariantMap diff = compareMap(before, after);
            if (diff.size() > 0) {
                updatePatches << QSPatch(QSPatch::Update, i, i, 1, diff);
            }
        }
    }

    return combine();
#endif
}

int QSDiffRunnerAlgo::preprocess(const QVariantList &from, const QVariantList &to)
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
            //@TODO reserve in block size
            updatePatches << QSPatch::createUpdate(index, diff);
        }
    }

    if (from.size() == index && to.size() - index > 0)  {
        // Special case: append to end
        appendPatch(createInsertPath(index,to.size() - 1,to));
        return to.size();
    }

    if (to.size() == index && from.size() - index> 0) {
        // Special case: removed from end
        appendPatch(QSPatch(QSPatch::Remove, index, from.size() - 1 , from.size() - index));
        return from.size();
    }

    skipped = index;

    return index;
}

void QSDiffRunnerAlgo::buildHashTable()
{
    hash.reserve( (qMax(to.size(), from.size()) - skipped) * 2 + 100);

    for (int i = skipped; i < from.size() ; i++) {
        QVariantMap item = from.at(i).toMap();
        QString key = item[m_keyField].toString();
        if (hash.contains(key)) {
            qWarning() << MISSING_KEY_WARNING;
            //@TODO fail back to burte force mode
        }
        QSDiffRunnerState mapper(i,-1);
        hash[key] = mapper;
    }

    for (int i = skipped; i < to.size() ; i++) {
        QVariantMap item = to.at(i).toMap();
        QString key = item[m_keyField].toString();
        QSDiffRunnerState mapper;

        if (hash.contains(key)) {
            mapper = hash[key];
        }
        mapper.atTo = i;
        hash[key] = mapper;
    }

}

void QSDiffRunnerAlgo::appendPatch(const QSPatch &value, bool merge)
{
    bool merged = false;

    if (patches.size() > 0 && merge) {
        if (patches.last().canMerge(value)) {
            patches.last().merge(value);
            merged = true;
        }
    }

    if (!merged) {
        patches << value;
    }
}

void QSDiffRunnerAlgo::markItemAtFromList(QSDiffRunnerAlgo::Type type, int index, QSDiffRunnerState &mapper)
{
//    qDebug() << "/*markItemAtFrom*/" << index << type;
    if (removeStart >= 0 && type != QSDiffRunnerAlgo::Remove) {
        int pos = index - 1 - removed + inserted;
        appendPatch(QSPatch::createRemove(removeStart, pos), false);

        // update removed at the end
        removed += (pos - removeStart + inserted + 1);
        removeStart = -1;
    }

    if (type == QSDiffRunnerAlgo::Remove) {
        if (removeStart < 0) {
            removeStart = index - removed + inserted;
        }
    }

    if (type == Move) {
        moved--;
        mapper.shiftedPos = mapper.atFrom - removed + inserted;
        qDebug() << "mark move" <<
                    mapper.atFrom <<
                    mapper.shiftedPos << moved;
        hash[fKey] = mapper;
    }

}

void QSDiffRunnerAlgo::markItemAtToList(QSDiffRunnerAlgo::Type type, int index,  QSDiffRunnerState& mapper)
{
    qDebug() << "markItemAtToList" << index << type;

    /* Insert */
    if (insertStart >= 0 && type != QSDiffRunnerAlgo::Insert) {
        appendPatch(createInsertPath(insertStart, index - 1, to), false);
        insertStart = -1;
    }

    if (type == QSDiffRunnerAlgo::Insert) {
        if (insertStart < 0) {
            insertStart = index;
        }
    }

    if (type == QSDiffRunnerAlgo::Move) {
        QSPatch change(QSPatch::Move, mapper.atFrom, index, 1);
        appendPatch(change);
        moved++;

        mapper.isMoved = true;
        hash[tKey] = mapper;
    }
}

