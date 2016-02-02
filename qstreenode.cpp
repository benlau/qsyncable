#include <QtCore>
#include "qstreenode.h"

QSTreeNode::QSTreeNode()
{
    m_key = 0;
    m_height = 0;
    m_count = 0;
    m_sum = 0;
    m_left = 0;
    m_right = 0;
    m_parent = 0;

}

QSTreeNode::QSTreeNode(int key, int count)
{
    m_key = key;
    m_count = count;
    m_sum = count;
    m_height = 1;
    m_left = 0;
    m_right = 0;
    m_parent = 0;
}

QSTreeNode::~QSTreeNode()
{
    if (m_left) {
        delete m_left;
    }

    if (m_right) {
        delete m_right;
    }
}

int QSTreeNode::count() const
{
    return m_count;
}

void QSTreeNode::setCount(int count)
{
    m_count = count;
}

int QSTreeNode::key() const
{
    return m_key;
}

void QSTreeNode::setKey(int key)
{
    m_key = key;
}

int QSTreeNode::sum() const
{
    return m_sum;
}

void QSTreeNode::setSum(int sum)
{
    m_sum = sum;
}

int QSTreeNode::height() const
{
    return m_height;
}

void QSTreeNode::setHeight(int height)
{
    m_height = height;
}

QSTreeNode *QSTreeNode::left() const
{
    return m_left;
}

void QSTreeNode::setLeft(QSTreeNode *left)
{
    m_left = left;
    if (left) {
        left->setParent(this);
    }
}

QSTreeNode *QSTreeNode::right() const
{
    return m_right;
}

void QSTreeNode::setRight(QSTreeNode *right)
{
    m_right = right;
    if (right) {
        right->setParent(this);
    }
}

void QSTreeNode::update()
{
    int h = 1;
    m_sum = m_count;
    if (m_left) {
        h = qMax(h,m_left->height() + 1);
        m_sum += m_left->sum();
    }

    if (m_right) {
        h = qMax(h, m_right->height() + 1);
        m_sum += m_right->sum();
    }

    m_height = h;
}

bool QSTreeNode::hasLeft() const
{
    return m_left != 0;
}

bool QSTreeNode::hasRight() const
{
    return m_right != 0;
}

int QSTreeNode::leftHeight() const
{
    int ret = 0;
    if (m_left) {
        ret = m_left->height();
    }
    return ret;
}

int QSTreeNode::rightHeight() const
{
    int ret = 0;
    if (m_right) {
        ret = m_right->height();
    }
    return ret;
}

int QSTreeNode::balance() const
{
    int l = 0, r = 0;
    if (m_left) {
        l = m_left->height();
    }

    if (m_right) {
        r = m_right->height();
    }

    return l - r;
}

QSTreeNode *QSTreeNode::parent() const
{
    return m_parent;
}

void QSTreeNode::setParent(QSTreeNode *parent)
{
    m_parent = parent;
}

QSTreeNode *QSTreeNode::takeLeft()
{
    QSTreeNode* res = 0;

    if (m_left) {
        res = m_left;
        m_left->setParent(0);
        m_left = 0;
    }

    return res;
}

QSTreeNode *QSTreeNode::takeRight()
{
    QSTreeNode* res = 0;

    if (m_right) {
        res = m_right;
        m_right->setParent(0);
        m_right = 0;
    }

    return res;
}

void QSTreeNode::unparent()
{
    if (m_parent) {
        if (m_parent->left() == this) {
            m_parent->takeLeft();
        } else if (m_parent->right() == this) {
            m_parent->takeRight();
        } else {
            qWarning() << "QSTreeNode::unparent(): Invalid tree structure";
        }
    }
}

