#ifndef QSTREENODE_H
#define QSTREENODE_H

class QSTreeNode
{
public:
    QSTreeNode();
    QSTreeNode(int key, int count);
    ~QSTreeNode();

    int count() const;
    void setCount(int count);

    int key() const;
    void setKey(int key);

    int sum() const;
    void setSum(int sum);

    int height() const;
    void setHeight(int height);

    QSTreeNode *left() const;
    void setLeft(QSTreeNode *left);

    QSTreeNode *right() const;
    void setRight(QSTreeNode *right);

    QSTreeNode *parent() const;
    void setParent(QSTreeNode *parent);

    // Remove the left node and return it.
    QSTreeNode* takeLeft();

    // Remove the right node and return it
    QSTreeNode* takeRight();


    // Update height and sum from child
    void update();

    bool hasLeft() const;

    bool hasRight() const;

private:

    int m_key;
    int m_count;
    int m_sum;
    int m_height;
    QSTreeNode* m_left;
    QSTreeNode* m_right;
    QSTreeNode* m_parent;

};

#endif // QSTREENODE_H
