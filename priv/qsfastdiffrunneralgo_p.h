#pragma once
#include "priv/qsimmutablewrapper_p.h"
#include "priv/qsalgotypes_p.h"
#include "priv/qstree.h"
#include "qspatch.h"

template <typename T>
class QSFastDiffRunnerAlgo {

public:

    QSFastDiffRunnerAlgo() {
        insertStart = -1;
        removeStart = -1;

        skipped = 0;
        indexT = -1;
        indexF = -1;

        removing = 0;
    }

    QSPatchSet compare(const QList<T>& from, const QList<T>& to) {
        if (from.isSharedWith(to)) {
            return QSPatchSet();
        }
        patches.clear();
        updatePatches.clear();

        this->from = from;
        this->to = to;

        if (!wrapper.hasKey()) {
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
        //@TODO - Discover duplicated key. It should fallback to use compareWithoutKey

        indexF = skipped;
        indexT = skipped;
        int fromSize = from.size();
        int toSize = to.size();

        QSAlgoTypes::State state;

        while (indexF < fromSize || indexT < toSize) {

            keyF.clear();

            while (indexF < fromSize) {
                // Process until it found an item that remain in origianl position (neither removd / moved).
                itemF = from.at(indexF);
                keyF = wrapper.key(itemF);

                state = hash[keyF]; // It mush obtain the key value

                if (state.posT < 0) {
                    markItemAtFromList(QSAlgoTypes::Remove, state);
                    indexF++;
                } else if (state.isMoved) {
                    markItemAtFromList(QSAlgoTypes::Move, state);
                    indexF++;
                } else {
                    markItemAtFromList(QSAlgoTypes::NoMove, state);
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
                itemT = to.at(indexT);
                keyT = wrapper.key(itemT);
                state = hash[keyT];

                if (state.posF < 0) {
                    // new item
                    markItemAtToList(QSAlgoTypes::Insert, state);
                    indexT++;
                } else {
                    if (keyT != keyF) {
                        markItemAtToList(QSAlgoTypes::Move, state);
                        indexT++;
                    } else {
                        markItemAtToList(QSAlgoTypes::NoMove, state);
                        indexT++;
                        indexF++;
                        break;
                    }
                }
            }
        }
        QSAlgoTypes::State dummy;

        markItemAtToList(QSAlgoTypes::NoMove, dummy);
        markItemAtFromList(QSAlgoTypes::NoMove, dummy);

        return combine();
    }

private:

    // Combine all the processing patches into a single list. It will clear the processing result too.
    QSPatchSet combine() {
        if (updatePatches.size() > 0) {
            patches.append(updatePatches);
        }

        return patches;
    }

    QList<QSPatch> compareWithoutKey(const QList<T>& from, const QList<T>& to) {
        QList<QSPatch> patches;

        int max = qMax(from.size(), to.size());

        for (int i = 0 ; i < max ; i++) {
            if (i >= from.size()) {
                patches << QSPatch(QSPatch::Insert, i, i, 1, wrapper.convert(to[i]));
            } else if (i >= to.size() ) {
                patches << QSPatch(QSPatch::Remove, i, i, 1);
            } else {
                QVariantMap diff = wrapper.fastDiff(from[i], to[i]);
                if (diff.size()) {
                    patches << QSPatch(QSPatch::Update, i, i, 1, diff);
                }
            }
        }

        return patches;
    }

    // Preprocess the list, stop until the key is different. It will also handle common pattern (like append to end , remove from end)
    int preprocess(const QList<T>& from, const QList<T>& to) {
        int index = 0;
        int min = qMin(from.size(), to.size());
        T f;
        T t;

        for (index = 0 ; index < min ;index++) {

            f = from[index];
            t = to[index];

            if (wrapper.isShared(f, t)) {
                continue;
            }

            if (wrapper.key(f) != wrapper.key(t)) {
                break;
            }

            QVariantMap diff = wrapper.fastDiff(f,t);
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

    void buildHashTable() {
        hash.reserve( (qMax(to.size(), from.size()) - skipped) * 2 + 100);

        QSAlgoTypes::State state;
        T item;
        QString key;
        int fromSize = from.size();
        int toSize = to.size();

        for (int i = skipped; i < fromSize ; i++) {
            item = from.at(i);
            key = wrapper.key(item);
            if (hash.contains(key)) {
                qWarning() << "QSFastDiffRunner.compare() - Duplicated or missing key.";
                //@TODO fail back to burte force mode
            }
            state.posF = i;
            state.posT = -1;
            hash.insert(key, state);
        }

        for (int i = skipped; i < toSize ; i++) {
            item = to.at(i);
            key = wrapper.key(item);

            if (hash.contains(key)) {
                hash[key].posT = i;
            } else {
                state.posF = -1;
                state.posT = i;
                hash.insert(key, state);
            }
        }
    }

    // Mark an item for insert, remove, move
    void markItemAtFromList(QSAlgoTypes::Type type, QSAlgoTypes::State &state) {
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

        if (type == QSAlgoTypes::Move) {
            updateTree();
        }

        state.posF = indexF;
        hash[keyF] = state;
    }

    void markItemAtToList(QSAlgoTypes::Type type, QSAlgoTypes::State& state) {
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
            QSAlgoTypes::MoveOp change(state.posF,
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
            T tmpItemF = from[state.posF];
            QVariantMap diff = wrapper.fastDiff(tmpItemF, itemT);
            if (diff.size()) {
                updatePatches << QSPatch(QSPatch::Update, indexT, indexT, 1, diff);
            }
        }
    }

    QSPatch createInsertPatch(int from, int to, const QList<T>& source ) {
        int count = to - from + 1;
        QVariantList list;
        list.reserve(count);
        for (int i = from ; i < from + count;i++) {
            list << wrapper.convert(source[i]);
        }

        return QSPatch(QSPatch::Insert, from, to, count, list);
    }

    void appendPatch(const QSPatch& value, bool merge = true) {
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

    void appendMovePatch(QSAlgoTypes::MoveOp& moveOp) {
        QSPatch patch(QSPatch::Move, moveOp.from, moveOp.to, moveOp.count);

        int offset = 0;

        QSTreeNode* node = tree.insert(moveOp.posF,moveOp.count);
        offset = tree.countLessThan(node);

        if (offset > 0) {
            patch.setFrom(patch.from() - offset);
        }

        appendPatch(patch);
    }

    void appendRemovePatches() {
        appendPatch(QSPatch::createRemove(indexT,
                                          indexT + removing - 1), false);

        removeStart = -1;
        removing = 0;
    }

    void updateTree() {
        while (tree.root() != 0 && tree.min() <= indexF) {
            tree.remove(tree.min());
        }
    }

    QSImmutableWrapper<T> wrapper;

    QList<T> from;

    QList<T> to;

    // Stored patches (without any update patches)
    QList<QSPatch> patches;

    // Update patches
    QList<QSPatch> updatePatches;

    // Hash table
    QHash<QString, QSAlgoTypes::State> hash;

    // The start position of remove block
    int removeStart;

    // No. of item removing
    int removing;

    // The start position of insertion block
    int insertStart;

    // A no. of item could be skipped found preprocess().
    int skipped;

    QString keyF,keyT;

    int indexF,indexT;

    T itemF,itemT;

    /* Move Patches */
    QSAlgoTypes::MoveOp pendingMovePatch;

    // Tree of move patch
    QSTree tree;
};
