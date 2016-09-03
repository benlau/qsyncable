#ifndef QSDIFFRUNNERALGO_H
#define QSDIFFRUNNERALGO_H

#include <QString>
#include "qspatch.h"
#include "qstree.h"

class QSDiffRunnerAlgo {

public:
    QSDiffRunnerAlgo();

    QSPatchSet compare(const QVariantList& from, const QVariantList& to);

    QString keyField() const;

    void setKeyField(const QString &keyField);

private:

    enum Type {
        NoMove,
        Insert,
        Remove,
        Move
    };

    class State {
    public:
        State(int from = -1 , int to = -1);

        // The position in "from" list
        int posF;

        // The position in "to" list
        int posT;

        // Is it moved?
        bool isMoved;
    };

    // Move data storage
    class MoveOp {
    public:
        MoveOp(int posF = -1, int from = -1, int to = -1, int count = 1);

        bool isNull() const;

        bool canMerge(const MoveOp& other) const;

        void merge(const MoveOp& other);

        void clear();

        // The absolute position in "from"
        int posF;

        int from;

        int to;

        int count;
    };

    // Combine all the processing patches into a single list. It will clear the processing result too.
    QSPatchSet combine();

    static QList<QSPatch> compareWithoutKey(const QVariantList& from, const QVariantList& to);

    static QVariantMap compareMap(const QVariantMap& prev, const QVariantMap& current);

    // Preprocess the list, stop until the key is different. It will also handle common pattern (like append to end , remove from end)
    int preprocess(const QVariantList& from, const QVariantList& to);

    void buildHashTable();

    void appendPatch(const QSPatch& patch, bool merge = true);

    // Mark an item for insert, remove, move
    void markItemAtFromList(Type type, State &mapper);

    void markItemAtToList(Type type, State& state);

    static QSPatch createInsertPatch(int from, int to, const QVariantList& source );

    void appendMovePatch(MoveOp& patch);

    void updateTree();

    QVariantList from;

    QVariantList to;

    // Stored patches (without any update patches)
    QList<QSPatch> patches;

    // Update patches
    QList<QSPatch> updatePatches;

    // Hash table
    QHash<QString, State> hash;

    // The start position of remove block
    int removeStart;

    // The start position of insertion block
    int insertStart;

    // A no. of item could be skipped found preprocess().
    int skipped;

    QString keyF,keyT;

    int indexF,indexT;

    QVariantMap itemF,itemT;

    int removing;

    /* Move Patches */
    MoveOp pendingMovePatch;

    // Tree of move patch
    QSTree tree;

    QString m_keyField;


};


#endif // QSDIFFRUNNERALGO_H
