#include "qstablemodel.h"
#include "checkindex.h"
#include "QLoggingCategory"

QSTableModel::QSTableModel(QObject *parent) : QAbstractTableModel(parent)
{
}

int QSTableModel::rCount() const
{
    return rowCount();
}

int QSTableModel::cCount() const
{
    return columnCount();
}

void QSTableModel::setStorage(const QVariantList &value)
{
    const auto oldCount = m_storage.count();

    if (m_columns.isEmpty() && value.size() > 0) {
        setColumnMapping(value.at(0).toMap());
    }

    beginResetModel();
    m_storage = value;
    endResetModel();

    if (oldCount != m_storage.size()) {
        emit rCountChanged();
    }
}

QVariantList QSTableModel::storage() const
{
    return m_storage;
}

QVariant QSTableModel::data(const QModelIndex &index, int role) const
{
    CHECK_data(index);

    if (role == Qt::DisplayRole)
    {
        const auto key = m_columns[index.column()];
        return m_storage.at(index.row()).toMap().value(key);
    }
    if (role == Qt::UserRole + 1)
    {
        const auto key = m_columns[index.column()];
        const auto type = m_storage.at(index.row()).toMap().value(key).typeName();
        qDebug() << "c:" << index.column() << "r:" << index.row() << "t:" << type;
        return m_storage.at(index.row()).toMap().value(key).typeName();

    }


    return{};
}

bool QSTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    CHECK_setData(index);
    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        setColumn(index.row(), index.column(), value);
        return true;
    }

    return false;
}

QVariant QSTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    CHECK_headerData(section, orientation);

    if(role != Qt::DisplayRole)
        return "*";

    if (orientation == Qt::Horizontal && section < columnCount())
    {
        return m_columns.value(section);
    }

    return "#";
}


int QSTableModel::rowCount(const QModelIndex &parent) const
{
    CHECK_rowCount(parent);
    return m_storage.size();
}

int QSTableModel::columnCount(const QModelIndex &parent) const
{
    CHECK_columnCount(parent);
    return m_columns.size();
}

void QSTableModel::append(const QVariantMap &value)
{
    if (m_columns.isEmpty()) {
        setColumnMapping(value);
    }

    beginInsertRows(QModelIndex(),m_storage.size(),m_storage.size());
    m_storage.append(value);
    endInsertRows();
    emit rCountChanged();
}

void QSTableModel::insert(int index, const QVariantMap &value)
{
    if (m_columns.isEmpty()) {
        setColumnMapping(value);
    }

    beginInsertRows(QModelIndex(), index, index);
    m_storage.insert(index, value);
    endInsertRows();
    emit rCountChanged();
}

void QSTableModel::clear()
{
    const auto oldColumnCount = m_columns.size();
    const auto oldRowCount = m_storage.size();
    beginResetModel();
    m_storage.clear();
    m_columns.clear();
    m_columnsLookup.clear();
    endResetModel();
    if (oldColumnCount)
        emit cCountChanged();
    if(oldRowCount)
        emit rCountChanged();
}

void QSTableModel::setColumnMapping(QVariantMap value)
{
    const auto oldColumnCount = m_columns.size();

    m_columns.clear();
    m_columnsLookup.clear();

    QMapIterator<QString,QVariant> iter(value);

    int columnCounter = 0;

    while (iter.hasNext()) {
        iter.next();
        m_columns[columnCounter] = iter.key().toLocal8Bit();
        m_columnsLookup[iter.key()] = columnCounter++;
    }

    if (oldColumnCount != m_columns.size())
        emit cCountChanged();
}

QVariantMap QSTableModel::get(int row) const
{
    QVariantMap map;
    if (row >=0 && row < m_storage.size()) {
        map = m_storage.at(row).toMap();
    }
    return map;
}

/*! \fn  void QSTableModel::setProperty(int idx, QString property, QVariant value)

Apply the changes to a record at index. Only modified value will be set.
        */
void QSTableModel::setColumn(int row, int column, QVariant value)
{
    if (row < 0 || row >= rowCount() || column < 0 || column >= columnCount())
        return;

    QVariantMap item = get(row);

    item[m_columns[column]] = value;

    m_storage[row] = item;

    emit dataChanged(index(row,column),
                     index(row,column),
                     {});
}

/*! \fn void QSTableModel::insert(int index, const QVariantList &value)

Returns the index position of the first occurrence of the byte array ba in this byte array,
    */
void QSTableModel::insert(int index, const QVariantList &value)
{
    if (value.count() == 0) {
        return;
    } else if (value.count() == 1) {
        insert(index, value.at(0).toMap());
        return;
    }

    if (m_columns.isEmpty()) {
        setColumnMapping(value.at(0).toMap());
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
    emit rCountChanged();
}

/*! \fn void QSTableModel::move(int from, int to, int n)

Moves n items from one position to another.
        */
    void QSTableModel::move(int from, int to, int count)
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

void QSTableModel::remove(int i, int count)
{
    if (count < 1 || i + count > m_storage.size()) {
        return;
    }
    beginRemoveRows(QModelIndex(), i, i + count - 1);
    for (int j = 0; j < count; ++j) {
        m_storage.removeAt(i);
    }
    endRemoveRows();
    emit rCountChanged();
}

void QSTableModel::set(int index, QVariantMap data)
{
    if (index < 0 || index > m_storage.size()) {
        return;
    }

    if (index == m_storage.size()) {
        append(data);
        return;
    }

    auto affectedColumns = std::vector<int>();
    QVariantMap original = get(index);
    QMapIterator<QString, QVariant> iter(data);

    while (iter.hasNext()) {
        iter.next();
        if (!original.contains(iter.key()) ||
            original[iter.key()] != iter.value()) {

            const auto column = m_columnsLookup.value(iter.key(), -1);
            if (column != -1) {
                affectedColumns.push_back(column);
            }
            original[iter.key()] = iter.value();
        }
    }

    m_storage[index] = original;

    for (const auto column : qAsConst(affectedColumns))
    {
        emit dataChanged(this->index(index, column),
                         this->index(index, column),
                         {Qt::DisplayRole});
    }
}

QHash<int, QByteArray> QSTableModel::roleNames() const
{
    auto hash = QAbstractTableModel::roleNames();
    hash.insert(Qt::UserRole +1, "type");
    return hash;
}
