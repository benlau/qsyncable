/* QSyncable Project
   Author: Ben Lau
   License: Apache-2.0
   Web: https://github.com/benlau/qsyncable
*/
#include <QtCore>
#include "qslistmodel.h"

QSListModel::QSListModel(QObject *parent) :
    QAbstractListModel(parent)
{
}

int QSListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return count();
}

QVariant QSListModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= m_storage.size())
        return QVariant();

    QVariant v;

    if (m_roles.contains(role)) {
        QVariantMap item = m_storage.at(index.row()).toMap();
        v = item[m_roles[role]];
    }

    return v;
}

void QSListModel::append(const QVariantMap &value)
{
    beginInsertRows(QModelIndex(),m_storage.size(),m_storage.size());
    m_storage.append(value);
    endInsertRows();
    emit countChanged();
}

void QSListModel::insert(int index, const QVariantMap &value)
{
    beginInsertRows(QModelIndex(), index, index);
    m_storage.insert(index, value);
    endInsertRows();
    emit countChanged();
}

void QSListModel::insert(int index, const QVariantList &value)
{
    if (value.count() == 0) {
        return;
    } else if (value.count() == 1) {
        insert(index, value.at(0).toMap());
        return;
    }

    beginInsertRows(QModelIndex(), index, index + value.count() - 1);
    m_storage.reserve(m_storage.count() + value.count());

    QVariantList mid = m_storage.mid(index);

    for (int i = 0 ; i < value.size() ; i++) {
        if (index + i < m_storage.size()) {
            m_storage[index + i] = value.at(i);
        } else {
            m_storage.append(value.at(i));
        }
    }

    for (int i = 0; i < mid.size(); i++) {
        int idx = index + value.size() + i;
        if (idx < m_storage.count()) {
            m_storage[idx] = mid.at(i);
        } else {
            m_storage.append(mid.at(i));
        }
    }

    endInsertRows();
    emit countChanged();
}

void QSListModel::move(int from, int to, int count)
{

    if (from > to) {
        int f = from;
        int t = to;
        int c = count;
        count = f - t;
        from = t;
        to = t + c;
    }

    if (count <= 0 ||
        from == to ||
        from + count > m_storage.count() ||
        to + count > m_storage.count() ||
        from < 0 ||
        to < 0) {
        return;
    }

    beginMoveRows(QModelIndex(), from, from + count - 1,
                  QModelIndex(), to > from ? to + count : to);

    QVariantList tmp;
    tmp.reserve(to - from + count);

    // Move "to" block
    for (int i=0 ; i < (to - from) ; i++) {
        tmp.append(m_storage[from + count + i]);
    }

    // Move "from" block
    for (int i = 0 ; i < count ; i++) {
        tmp.append(m_storage[from + i]);
    }

    // Replace
    for (int i=0 ; i < tmp.count() ; ++i) {
        m_storage[from + i] = tmp[i];
    }

    endMoveRows();
}

void QSListModel::clear()
{
    if (m_storage.isEmpty())
        return;

    beginRemoveRows(QModelIndex(), 0, m_storage.count() - 1);
    m_storage.clear();
    endRemoveRows();
    emit countChanged();

}

void QSListModel::remove(int i, int count)
{
    beginRemoveRows(QModelIndex(), i, i + count - 1);
    for (int j = 0; j < count; ++j)
        m_storage.removeAt(i);
    endRemoveRows();
    emit countChanged();
}

int QSListModel::count() const
{
    return m_storage.size();
}

QVariantMap QSListModel::get(int i) const
{
    QVariantMap map;
    if (i >=0 && i < m_storage.size()) {
        map = m_storage.at(i).toMap();
    }
    return map;

}

void QSListModel::setProperty(int idx, QString property, QVariant value)
{
    if (idx < 0 || idx >= m_storage.size())
        return;
    QVector<int> roles;
    QVariantMap item = get(idx);

    QHashIterator<int, QByteArray> i(m_roles);
    while (i.hasNext()) {
        i.next();
        if (i.value() == property) {
            roles << i.key();
            break;
        }
    }

    item[property] = value;

    m_storage[idx] = item;

    emit dataChanged(index(idx,0),
                     index(idx,0),
                     roles);
}

void QSListModel::set(int idx, QVariantMap data)
{
    if (idx < 0 || idx >= m_storage.size())
        return;

    QVector<int> roles;
    QHashIterator<int, QByteArray> i(m_roles);
    while (i.hasNext()) {
        i.next();
        roles << i.key();
    }
    m_storage[idx] = data;

    emit dataChanged(index(idx,0),
                     index(idx,0),
                     roles);
}

void QSListModel::setProperties(int index, QVariantMap changes)
{
    QVariantMap original = get(index);

    QMapIterator<QString, QVariant> iter(changes);

    while (iter.hasNext()) {
        iter.next();
        if (!original.contains(iter.key()) ||
             original[iter.key()] != iter.value()) {
            setProperty(index,iter.key(),iter.value());
        }
    }
}

QHash<int, QByteArray> QSListModel::roleNames() const
{
    return m_roles;
}

void QSListModel::setRoleNames(const QVariantMap &value)
{
    m_roles.clear();
    QMapIterator<QString,QVariant> iter(value);

    int role = Qt::UserRole;

    while (iter.hasNext()) {
        iter.next();
        m_roles[role++] = iter.key().toLocal8Bit();
    }

}

void QSListModel::setRoleNames(const QStringList& list)
{
    m_roles.clear();
    int role = Qt::UserRole;

    for (int i = 0 ; i < list.size();i++) {
        m_roles[role++] = list.at(i).toLocal8Bit();
    }
}

void QSListModel::setStorage(const QVariantList &value)
{
    int oldCount = m_storage.count();
    beginResetModel();
    m_storage = value;
    endResetModel();
    if (oldCount != m_storage.size()) {
        emit countChanged();
    }
}

QVariantList QSListModel::storage() const
{
    return m_storage;
}

int QSListModel::indexOf(QString field, QVariant value) const
{
    int res = -1;
    for (int i = 0 ; i < m_storage.count();i++) {
        QVariantMap item = m_storage.at(i).toMap();
        if (item.contains(field) && item[field] == value) {
            res = i;
            break;
        }
    }
    return res;
}

