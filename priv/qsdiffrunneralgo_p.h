#ifndef QSDIFFRUNNERALGO_H
#define QSDIFFRUNNERALGO_H

#include <QString>
#include "qspatch.h"

class QSDiffRunnerState {
public:
    QSDiffRunnerState(int from = -1 , int to = -1);

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

class QSDiffRunnerStackItem {
public:
    QSDiffRunnerStackItem(int from = 0, int count = 0, int acc = 0);

    int from;
    int count;
    int acc;
};

class QSDiffRunnerAlgo {

public:
    enum Type {
        NoMove,
        Insert,
        Remove,
        Move
    };

    QSDiffRunnerAlgo();

    // Combine all the processing patches into a single list. It will clear the processing result too.
    QList<QSPatch> combine();

    void fixMovePatches();

    static QList<QSPatch> compareWithoutKey(const QVariantList& from, const QVariantList& to);

    static QVariantMap compareMap(const QVariantMap& prev, const QVariantMap& current);

    QList<QSPatch> compare(const QVariantList& from, const QVariantList& to);

    // Preprocess the list, stop until the key is different. It will also handle common pattern (like append to end , remove from end)
    int preprocess(const QVariantList& from, const QVariantList& to);

    void buildHashTable();

    void appendPatch(const QSPatch& patch, bool merge = true);

    // Mark an item for insert, remove, move
    void markItemAtFromList(Type type,int index, QSDiffRunnerState &mapper);

    void markItemAtToList(Type type,int index, QSDiffRunnerState& mapper);

    static QSPatch createInsertPatch(int from, int to, const QVariantList& source );

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

    // No. of removing item
    int removing;

    // No. of inserted item
    int inserted;

    // No. of inserting item
    int inserting;

    // No. of moved item
    int moved;

    // A no. of item could be skipped found preprocess().
    int skipped;

    QString fKey,tKey;

    int indexF,indexT;
};


#endif // QSDIFFRUNNERALGO_H
