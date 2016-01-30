#include <QtCore>
#include "qstreenode.h"

QSTreeNode::QSTreeNode()
{
    m_value = 0;
    m_height = 0;
    m_count = 0;
    m_sum = 0;
    m_left = 0;
    m_right = 0;
    m_parent = 0;

}

QSTreeNode::QSTreeNode(int value, int count)
{
    m_value = value;
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

int QSTreeNode::value() const
{
    return m_value;
}

void QSTreeNode::setValue(int value)
{
    m_value = value;
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
}

QSTreeNode *QSTreeNode::right() const
{
    return m_right;
}

void QSTreeNode::setRight(QSTreeNode *right)
{
    m_right = right;
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

QSTreeNode *QSTreeNode::parent() const
{
    return m_parent;
}

void QSTreeNode::setParent(QSTreeNode *parent)
{
    m_parent = parent;
}

