#include "priv/qsdiffrunneralgo_p.h"

#define MISSING_KEY_WARNING "QSDiffRunner.compare() - Duplicated or missing key."

using namespace QSAlgoTypes;

QSDiffRunnerAlgo::QSDiffRunnerAlgo()
{
    insertStart = -1;
    removeStart = -1;

    skipped = 0;
    indexT = -1;
    indexF = -1;

    removing = 0;
}

QString QSDiffRunnerAlgo::keyField() const
{
    return m_keyField;
}

void QSDiffRunnerAlgo::setKeyField(const QString &keyField)
{
    m_keyField = keyField;
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
    QVariantMap f;
    QVariantMap t;

    for (index = 0 ; index < min ;index++) {

        f = from[index].toMap();
        t = to[index].toMap();

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

void QSDiffRunnerAlgo::appendMovePatch(QSAlgoTypes::MoveOp &moveOp)
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

void QSDiffRunnerAlgo::appendRemovePatches()
{
    appendPatch(QSPatch::createRemove(indexT,
                                      indexT + removing - 1), false);

    removeStart = -1;
    removing = 0;
}

void QSDiffRunnerAlgo::updateTree()
{
    while (tree.root() != 0 && tree.min() <= indexF) {
        tree.remove(tree.min());
    }
}

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
    //@TODO - Discover duplicated key

    indexF = skipped;
    indexT = skipped;
    int fromSize = from.size();
    int toSize = to.size();

    State state;

    while (indexF < fromSize || indexT < toSize) {

        keyF.clear();

        while (indexF < fromSize) {
            // Process until it found an item that remain in origianl position (neither removd / moved).
            itemF = from.at(indexF).toMap();
            keyF = itemF[m_keyField].toString();
            state = hash[keyF]; // It mush obtain the key value


            if (state.posT < 0) {
                markItemAtFromList(Remove, state);
                indexF++;
            } else if (state.isMoved) {
                markItemAtFromList(Move, state);
                indexF++;
            } else {
                markItemAtFromList(NoMove, state);
                // The item remain in original position.
                break;
            }
        }

        if (indexF >= fromSize && indexT < toSize) {
            // The rest in "to" list is new items
            appendPatch(createInsertPatch(indexT, toSize - 1, to), false);
            return combine();
        }

        while (indexT < toSize ) {
            itemT = to.at(indexT).toMap();
            keyT = itemT[m_keyField].toString();
            state = hash[keyT];

            if (state.posF < 0) {
                // new item
                markItemAtToList(Insert, state);
                indexT++;
            } else {
                if (keyT != keyF) {
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

    return combine();
}

void QSDiffRunnerAlgo::markItemAtFromList(QSAlgoTypes::Type type, State &state)
{
    if (removeStart >= 0 && type != QSAlgoTypes::Remove) {
        appendRemovePatches();
    }

    if (type == QSAlgoTypes::Remove) {
        if (removeStart < 0) {
            removeStart = indexF;
        }
        removing++;

        if (indexF == from.size() - 1) {
            // It is the last item
            appendRemovePatches();
        }
    }

    if (type == Move) {
        updateTree();
    }

    state.posF = indexF;
    hash[keyF] = state;
}

void QSDiffRunnerAlgo::markItemAtToList(QSAlgoTypes::Type type, State& state)
{

    if (insertStart >= 0 && type != QSAlgoTypes::Insert) {
        /* Insert */
        appendPatch(createInsertPatch(insertStart,
                                     indexT - 1, to), false);
        insertStart = -1;
    }

    if (type == QSAlgoTypes::Insert) {
        if (insertStart < 0) {
            insertStart = indexT;
        }
    }

    if (type == QSAlgoTypes::Move) {
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
        hash[keyT] = state;
    }

    if (type != QSAlgoTypes::Move && !pendingMovePatch.isNull()) {
        appendMovePatch(pendingMovePatch);
        pendingMovePatch.clear();
    }

    if (indexT < to.size() && (type == QSAlgoTypes::Move || type == QSAlgoTypes::NoMove)) {
        QVariantMap tmpItemF = from[state.posF].toMap();
        QVariantMap diff = compareMap(tmpItemF, itemT);
        if (diff.size()) {
            updatePatches << QSPatch(QSPatch::Update, indexT, indexT, 1, diff);
        }
    }
}
