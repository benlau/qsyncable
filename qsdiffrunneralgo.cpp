#include "priv/qsdiffrunneralgo_p.h"

#define MISSING_KEY_WARNING "QSDiffRunner.compare() - Duplicated or missing key."
#define USE_4WAY_PASS_ALGO

QSDiffRunnerAlgo::State::State(int from, int to) {
    this->posF = from;
    this->posT = to;
    isMoved = false;
}

QSDiffRunnerAlgo::QSDiffRunnerAlgo()
{
    insertStart = -1;
    removeStart = -1;

    skipped = 0;
    indexT = -1;
    indexF = -1;

    removing = 0;
}

QSPatchSet QSDiffRunnerAlgo::combine()
{
    if (updatePatches.size() > 0) {
        patches.append(updatePatches);
    }

    return patches;
}

QList<QSPatch> QSDiffRunnerAlgo::compareWithoutKey(const QVariantList &from, const QVariantList &to)
{
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

QVariantMap QSDiffRunnerAlgo::compareMap(const QVariantMap &prev, const QVariantMap &current)
{
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
        skipped = to.size();
        appendPatch(createInsertPatch(index,to.size() - 1,to));
        return to.size();
    }

    if (to.size() == index && from.size() - index> 0) {
        // Special case: removed from end
        appendPatch(QSPatch::createRemove(index, from.size() - 1));
        skipped = from.size();
        return from.size();
    }

    skipped = index;

    return index;
}

void QSDiffRunnerAlgo::buildHashTable()
{
    hash.reserve( (qMax(to.size(), from.size()) - skipped) * 2 + 100);

    State state;
    QVariantMap item;
    QString key;
    int fromSize = from.size();
    int toSize = to.size();

    for (int i = skipped; i < fromSize ; i++) {
        item = from.at(i).toMap();
        key = item[m_keyField].toString();
        if (hash.contains(key)) {
            qWarning() << MISSING_KEY_WARNING;
            //@TODO fail back to burte force mode
        }
        state.posF = i;
        state.posT = -1;
        hash.insert(key, state);
    }

    for (int i = skipped; i < toSize ; i++) {
        item = to.at(i).toMap();
        key = item[m_keyField].toString();

        if (hash.contains(key)) {
            hash[key].posT = i;
        } else {
            state.posF = -1;
            state.posT = i;
            hash.insert(key, state);
        }
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

QSPatch QSDiffRunnerAlgo::createInsertPatch(int from, int to, const QVariantList &source)
{
    int count = to - from + 1;

    return QSPatch(QSPatch::Insert, from, to, count, source.mid(from, count));
}

void QSDiffRunnerAlgo::appendMovePatch(MoveOp &moveOp)
{

    QSPatch patch(QSPatch::Move, moveOp.from, moveOp.to, moveOp.count);

    int offset = 0;

    QSTreeNode* node = tree.insert(moveOp.posF,moveOp.count);
    offset = tree.countLessThan(node);

    if (offset > 0) {
        patch.setFrom(patch.from() - offset);
    }

    appendPatch(patch);
}

void QSDiffRunnerAlgo::updateTree()
{
    while (tree.root() != 0 && tree.min() <= indexF) {
        tree.remove(tree.min());
    }
}


#ifdef USE_4WAY_PASS_ALGO
QSPatchSet QSDiffRunnerAlgo::compare(const QVariantList &from, const QVariantList &to) {
    patches.clear();
    updatePatches.clear();

    this->from = from;
    this->to = to;

    if (m_keyField.isEmpty()) {
        return compareWithoutKey(from, to);
    }

    // Compare the list, until it found moved component.
    preprocess(from, to);

    if (skipped >= from.size() &&
        skipped >= to.size()) {
        // Nothing moved
        return combine();
    }

    buildHashTable();

    indexF = skipped;
    indexT = skipped;

    QVariantMap fItem,tItem;

    while (indexF < from.size() || indexT < to.size()) {
        State state;

        fKey.clear();

        while (indexF < from.size()) {
            // Find non-removed item / moved item
            fItem = from.at(indexF).toMap();
            fKey = fItem[m_keyField].toString();
            state = hash[fKey]; // It mush obtain the key value

            if (state.posT < 0) {
                markItemAtFromList(Remove, state);
                indexF++;
            } else if (state.isMoved) {
                markItemAtFromList(Move, state);
                indexF++;
            } else {
                markItemAtFromList(NoMove, state);
                break;
            }
        }

        if (indexF >= from.size() && indexT < to.size()) {
            // The rest in "to" list is new items
            appendPatch(createInsertPatch(indexT, to.size() - 1, to), false);
            return combine();
        }

        while (indexT < to.size() ) {
            tItem = to.at(indexT).toMap();
            tKey = tItem[m_keyField].toString();
            state = hash[tKey];

            if (state.posF < 0) {
                // new item
                markItemAtToList(Insert, state);
                indexT++;
            } else {
                if (tKey != fKey) {
                    markItemAtToList(Move, state);
                    indexT++;
                } else {
                    markItemAtToList(NoMove, state);
                    indexT++;
                    indexF++;
                    break;
                }
            }
        }
    }
    State dummy;

    markItemAtToList(NoMove, dummy);
    markItemAtFromList(NoMove, dummy);

    //@TODO - combine should update move patch
    return combine();
}

void QSDiffRunnerAlgo::markItemAtFromList(QSDiffRunnerAlgo::Type type, State &state)
{
    if (removeStart >= 0 && type != QSDiffRunnerAlgo::Remove) {

        appendPatch(QSPatch::createRemove(indexT,
                                          indexT + removing - 1), false);

        removeStart = -1;
        removing = 0;
    }

    if (type == QSDiffRunnerAlgo::Remove) {
        if (removeStart < 0) {
            removeStart = indexF;
        }
        removing++;
    }

    if (type == Move) {
        updateTree();
    }

    state.posF = indexF;
    hash[fKey] = state;
}

void QSDiffRunnerAlgo::markItemAtToList(QSDiffRunnerAlgo::Type type, State& state)
{

    if (insertStart >= 0 && type != QSDiffRunnerAlgo::Insert) {
        /* Insert */
        appendPatch(createInsertPatch(insertStart,
                                     indexT - 1, to), false);
        insertStart = -1;
    }

    if (type == QSDiffRunnerAlgo::Insert) {
        if (insertStart < 0) {
            insertStart = indexT;
        }
    }

    if (type == QSDiffRunnerAlgo::Move) {
        MoveOp change(state.posF,
                      indexT + state.posF - indexF,
                      indexT);

        if (pendingMovePatch.isNull()) {
            pendingMovePatch = change;
        } else if (pendingMovePatch.canMerge(change)){
            pendingMovePatch.merge(change);
        } else {
            appendMovePatch(pendingMovePatch);
            pendingMovePatch = change;
        }

        state.isMoved = true;
        hash[tKey] = state;
    }

    if (type != QSDiffRunnerAlgo::Move && !pendingMovePatch.isNull()) {
        appendMovePatch(pendingMovePatch);
        pendingMovePatch.clear();
    }

    if (indexT < to.size() && (type == QSDiffRunnerAlgo::Move || type == QSDiffRunnerAlgo::NoMove)) {
        QVariantMap itemF = from[state.posF].toMap();
        QVariantMap itemT = to[state.posT].toMap();
        QVariantMap diff = compareMap(itemF, itemT);
        if (diff.size()) {
            updatePatches << QSPatch(QSPatch::Update, indexT, indexT, 1, diff);
        }
    }
}


#else

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
    preprocess(from, to);

    if (skipped >= from.size() &&
        skipped >= to.size()) {
        // Nothing moved
        return combine();
    }

    QVariantMap item;

    hash.reserve(to.size() + from.size() - skipped * 2 + 100);

    /* Step 1 - Check Removal */

    for (int i = skipped ; i < to.size() ; i++) {
        // Build hash index table on "to" list
        item = to.at(i).toMap();
        QString key = item[m_keyField].toString();

        if (hash.contains(key)) {
            qWarning() << MISSING_KEY_WARNING;
            return compareWithoutKey(from, to);
        }

        QSDiffRunnerState mapper(-1,i);
        hash[key] = mapper;
    }

    removed = 0;

    for (int i = skipped ; i < from.size() ; i++) {
        // Find removed item and build index table on "from" list.

        item = from.at(i).toMap();
        fKey = item[m_keyField].toString();

        QSDiffRunnerState state = QSDiffRunnerState(-1,-1);

        if (hash.contains(fKey)) {
            state = hash[fKey];
        }

        if (state.atTo < 0) {
            // It is removed.
            markItemAtFromList(QSDiffRunnerAlgo::Remove, i, state);
        } else {

            if (state.atFrom >= 0) {
                qWarning() << MISSING_KEY_WARNING;
                return compareWithoutKey(from, to);
            }

            markItemAtFromList(QSDiffRunnerAlgo::NoMove, i, state);
        }
    }

    /* Step 2 - Compare to find move and update */

    insertStart = -1;
    inserted = 0;

    for (int i = skipped ; i < to.size() ; i++) {
        item = to.at(i).toMap();
        tKey = item[m_keyField].toString();

        QSDiffRunnerState state = QSDiffRunnerState(-1,-1);

        state = hash[tKey]; // It must exist

        if (state.atFrom < 0 ) {
            // New item
            markItemAtToList(QSDiffRunnerAlgo::Insert, i, state);
        } else {
            int realPos = state.atFrom; // Real position in fromList
            int expectedPos = state.atRemovedFrom; // Expected position in from
            int shiftedPos = expectedPos + inserted + inserting + moved; // Expected position + shift;

            qDebug() << i << shiftedPos << expectedPos + i << inserted;
            if (shiftedPos != i) {
                // Must mark item before append.
                markItemAtToList(QSDiffRunnerAlgo::Move, i, state);
            } else {
                markItemAtToList(QSDiffRunnerAlgo::NoMove, i, state);
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
}

void QSDiffRunnerAlgo::markItemAtFromList(QSDiffRunnerAlgo::Type type, int index, QSDiffRunnerState &state)
{
    qDebug() << "markItemAtFrom" << index << type;
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
        state.shiftedPos = state.atFrom - removed + inserted;
        qDebug() << "mark move" <<
                    state.atFrom <<
                    state.shiftedPos << moved;
        hash[fKey] = state;
    }

    if (type == NoMove) {
        // It don't update removed node.
        state.atRemovedFrom = index - removed;
    }

    state.atFrom = index;
    state.atRemovedFrom = index - removed;
    hash[fKey] = state;
}

void QSDiffRunnerAlgo::markItemAtToList(QSDiffRunnerAlgo::Type type, int index,  QSDiffRunnerState& mapper)
{
    qDebug() << "markItemAtToList" << index << type;

    /* Insert */
    if (insertStart >= 0 && type != QSDiffRunnerAlgo::Insert) {
        appendPatch(createInsertPatch(insertStart,
                                     index - 1, to), false);
        insertStart = -1;
        inserted += inserting;
        inserting = 0;
    }

    if (type == QSDiffRunnerAlgo::Insert) {
        if (insertStart < 0) {
            insertStart = index;
        }
        inserting++;
    }

    if (type == QSDiffRunnerAlgo::Move) {
        QSPatch change(QSPatch::Move,
                       mapper.atRemovedFrom + inserted + inserting,
                       index, 1);
        appendPatch(change);

        mapper.isMoved = true;
        hash[tKey] = mapper;
        moved++;
    }
}

#endif



QSDiffRunnerAlgo::MoveOp::MoveOp(int indexF, int from, int to, int count) : posF(indexF) , from(from), to(to), count(count)
{
}

bool QSDiffRunnerAlgo::MoveOp::canMerge(const QSDiffRunnerAlgo::MoveOp &other) const
{
    return posF + count == other.posF && to + count == other.to ;
}

void QSDiffRunnerAlgo::MoveOp::merge(const QSDiffRunnerAlgo::MoveOp &other)
{
    count += other.count;
}

bool QSDiffRunnerAlgo::MoveOp::isNull() const
{
    return posF < 0;
}

void QSDiffRunnerAlgo::MoveOp::clear()
{
    posF = from = to = -1;
}

