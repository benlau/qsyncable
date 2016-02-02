#include <QtCore>
#include "qstree.h"

QSTree::QSTree()
{
    m_min = m_max = m_sum = m_height = 0;
    m_root = 0;
}

QSTree::~QSTree()
{
    if (m_root) {
        delete m_root;
    }
}

bool QSTree::isNull() const
{
    return m_root == 0;
}

int QSTree::min() const
{
    return m_min;
}

void QSTree::setMin(int min)
{
    m_min = min;
}

void QSTree::updateFromRoot()
{
    if (m_root) {
        m_sum = m_root->sum();
        m_height = m_root->height();
    } else {
        m_sum = 0;
        m_height = 0;
    }
}

void QSTree::simpleRemove(QSTreeNode *node)
{
    QSTreeNode* child = 0;

    if (node->hasLeft()) {
        child = node->takeLeft();
    } else if (node->hasRight()) {
        child = node->takeRight();
    }

    QSTreeNode * parent = node->parent();

    if (parent) {
        if (parent->left() == node) {
            parent->setLeft(child);
        } else {
            parent->setRight(child);
        }
    } else {
        // It is root node
        m_root = child;
        updateFromRoot();
    }

    delete node;
}

QSTreeNode* QSTree::rotateLeft(QSTreeNode *n)
{
    /* n            x
     *  \         /
     *   x       n
     *  /         \
     * y            y
     */

    bool isLeft = false;
    QSTreeNode* parent = n->parent();

    if (parent) {
        isLeft = (parent->left() == n);
    }

    QSTreeNode* x = n->takeRight();
    QSTreeNode* y = x->takeLeft();

    x->setLeft(n);
    n->setRight(y);

    n->update();
    x->update();

    if (parent) {
        if (isLeft) {
            parent->setLeft(x);
        } else {
            parent->setRight(x);
        }
    } else {
        // It is root
        m_root = x;
        updateFromRoot();
    }

    return x;
}

QSTreeNode *QSTree::rotateRight(QSTreeNode *n)
{
    /*   n      x
     *  /         \
     * x           n
     *  \         /
     *   y       y
     */

    bool isLeft = false;
    QSTreeNode* parent = n->parent();

    if (parent) {
        isLeft = (parent->left() == n);
    }

    QSTreeNode* x = n->takeLeft();
    QSTreeNode* y = x->takeRight();

    x->setRight(n);
    n->setLeft(y);

    n->update();
    x->update();

    if (parent) {
        if (isLeft) {
            parent->setLeft(x);
        } else {
            parent->setRight(x);
        }
    } else {
        // It is root
        m_root = x;
        updateFromRoot();
    }

    return x;
}

int QSTree::max() const
{
    return m_max;
}

void QSTree::setMax(int max)
{
    m_max = max;
}

int QSTree::sum() const
{
    return m_sum;
}

void QSTree::setSum(int sum)
{
    m_sum = sum;
}

int QSTree::height() const
{
    return m_height;
}

void QSTree::setHeight(int depth)
{
    m_height = depth;
}

QSTreeNode *QSTree::root() const
{
    return m_root;
}

void QSTree::setRoot(QSTreeNode *root)
{
    m_root = root;
    if (root) {
        m_min = root->key();
        m_max = root->key();
        m_sum = root->count();
    } else {
        m_min = 0;
        m_max = 0;
        m_sum = 0;
    }
}

QSTreeNode *QSTree::insert(int key, int count)
{
    QSTreeNode* node = new QSTreeNode(key, count);
    insert(node);
    return node;
}

void QSTree::remove(int key)
{
    if (m_root == 0) {
        return;
    }

    remove(m_root, key);
    updateFromRoot();

    if (key == m_min && m_root) {
        QSTreeNode* node = searchMin(m_root);
        m_min = node->key();
    }

    // Remarks: It don't update max. Becoz it is not needed in Diff algorithm.

}

QSTreeNode *QSTree::search(int key) const
{
    if (m_root == 0) {
        return 0;
    }

    return search(m_root, key);
}

int QSTree::countLessThan(int key) const
{
    QSTreeNode* node = search(key);

    if (node == 0) {
        return 0;
    }

    return countLessThan(node);
}

int QSTree::countLessThan(QSTreeNode *node) const
{
    int sum = 0;
    bool fromRightChild = false;
    int key = node->key();

    while (node != 0) {

        if (!fromRightChild && node->left()) {
            sum += node->left()->sum();
        }

        if (node->key() < key) {
            sum += node->count();
        }

        if (node->parent()) {
            fromRightChild = node->parent()->right() != node;
        }

        node = node->parent();
    }

    return sum;
}

void QSTree::insert(QSTreeNode *node)
{
    if (m_root == 0) {
        setRoot(node);
    } else {
        insert(m_root, node);

        if (m_min > node->key()) {
            m_min = node->key();
        }
        if (m_max < node->key()) {
            m_max = node->key();
        }
    }

    updateFromRoot();
}

void QSTree::insert(QSTreeNode *current, QSTreeNode *node)
{
    if (node->key() < current->key()) {
        if (current->hasLeft()) {
            insert(current->left(), node);
        } else {
            current->setLeft(node);
        }
    } else {
        if (current->hasRight()) {
            insert(current->right(), node);
        } else {
            current->setRight(node);
        }
    }

    int balance = current->leftHeight() - current->rightHeight();

    if (balance > 1 && node->key() < current->left()->key()) {
        // Left Left Case
        rotateRight(current);
    } else if (balance < -1 && node->key() > current->right()->key()) {
        // Right Right Case
        rotateLeft(current);
    } else if (balance > 1 && node->key() > current->left()->key()) {
        // Left Right Case
        rotateLeft(current->left());
        rotateRight(current);
    } else if (balance < -1 && node->key() < current->right()->key()) {
        rotateRight(current->right());
        rotateLeft(current);
    }

    current->update();
}

void QSTree::remove(QSTreeNode *node, int key)
{
    if (node->key() != key) {

        if (key < node->key()) {
            if (node->hasLeft()) {
                remove(node->left(), key);
            }
        } else {
            if (node->hasRight()) {
                remove(node->right(), key);
            }
        }

    } else {

        if (node->hasLeft() && node->hasRight()) {
            QSTreeNode* minNode = searchMin(node->right());
            node->setCount(minNode->count());
            node->setKey(minNode->key());
            remove(node->right(), minNode->key());

        } else {

            QSTreeNode* parent = node->parent();
            QSTreeNode* child = 0;
            if (node->hasLeft()) {
                child = node->takeLeft();
            } else if (node->hasRight()) {
                child = node->takeRight();
            }

            if (parent) {
                if (parent->left() == node) {
                    parent->setLeft(child);
                } else {
                    parent->setRight(child);
                }
            } else {
                m_root = child;
            }

            delete node;
            node = child; // Replace current node
        }
    }

    if (node) {
        // The node is not deleted or replaced by its child
        // TODO: Balance checking
        node->update();
    }
}

QSTreeNode* QSTree::search(QSTreeNode *node, int key) const
{
    QSTreeNode* res = 0;

    if (node->key() == key) {
        return node;
    } else if (key < node->key() ) {
        if (node->left() != 0) {
            res = search(node->left(), key);
        }
    } else {
        if (node->right() != 0) {
            res = search(node->right(), key);
        }
    }
    return res;
}

QSTreeNode *QSTree::searchMin(QSTreeNode *node) const
{
    if (!node->hasLeft()) {
        return node;
    } else {
        return searchMin(node->left());
    }
}

QSTreeNode *QSTree::searchMax(QSTreeNode *node) const
{
    if (!node->hasRight()) {
        return node;
    } else {
        return searchMax(node->right());
    }
}

QDebug operator<<(QDebug dbg, const QSTree& tree) {

    QQueue<QSTreeNode*> queue;
    QStringList links;

    if (tree.root() != 0) {
        queue.enqueue(tree.root());
        links.append(QString("[%1]").arg(tree.root()->key()));
    }

    while (queue.size() > 0) {
        QSTreeNode* node = queue.dequeue();

        if (node->parent() != 0) {
            links << QString("%1 -> %2").arg(node->parent()->key()).arg(node->key());
        }

        if (node->hasLeft()) {
            queue.enqueue(node->left());
        }

        if (node->hasRight()) {
            queue.enqueue(node->right());
        }

    }

    dbg << links.join(",");
    return dbg;
}

