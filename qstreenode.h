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

    // Remove the left node and return it. It won't be destroyed
    QSTreeNode* takeLeft();

    // Remove the right node and return it. It won't be destroyed
    QSTreeNode* takeRight();

    // If the parent is not null, remove current this node from parent.
    void unparent();

    // Update height and sum from children
    void update();

    bool hasLeft() const;

    bool hasRight() const;

    int leftHeight() const;

    int rightHeight() const;

    // Calculate the differnet of heigth from left to right child
    int balance() const;

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
