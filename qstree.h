#ifndef QSTREE_H
#define QSTREE_H

#include "qstreenode.h"

class QSTree
{
public:
    QSTree();
    ~QSTree();

    int min() const;

    int max() const;

    int sum() const;

    int height() const;

    QSTreeNode *root() const;

    QSTreeNode* insert(int key, int count);

    void remove(int key);

    QSTreeNode* search(int key) const;

    int countLessThan(int key) const;


private:
    int countLessThan(QSTreeNode* node) const;

    void insert(QSTreeNode* node);

    void searchNodeToInsert(QSTreeNode* current, QSTreeNode* node);

    QSTreeNode* search(QSTreeNode* node, int key) const;

    QSTreeNode* searchMin(QSTreeNode* node) const;

    QSTreeNode* searchMax(QSTreeNode* node) const;

    void setRoot(QSTreeNode *root);

    void setSum(int sum);

    void setHeight(int height);

    void setMax(int max);

    void setMin(int min);

    void updateFromRoot();

    // Remove a non-root node without two childs
    void simpleRemove(QSTreeNode* node);

    int m_min;
    int m_max;
    int m_sum;
    int m_height;

    QSTreeNode* m_root;

};

QDebug operator<<(QDebug dbg, const QSTree& change);


#endif // QSTREE_H
