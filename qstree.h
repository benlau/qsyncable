#ifndef QSTREE_H
#define QSTREE_H

#include "qstreenode.h"

class QSTree
{
public:
    QSTree();

    int min() const;

    int max() const;

    int sum() const;

    int height() const;

    QSTreeNode *root() const;

    QSTreeNode* insert(int value, int count);

    void remove(int value);

    QSTreeNode* search(int value) const;

    int countLessThan(int valueOfNode) const;

private:
    void insert(QSTreeNode* node);

    void searchNodeToInsert(QSTreeNode* current, QSTreeNode* node);

    QSTreeNode* search(QSTreeNode* node, int value) const;

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

#endif // QSTREE_H
