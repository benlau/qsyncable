#include <QtQml>
#include "qsdiffrunner.h"
#include "qsjsonlistmodel.h"

/*! \qmltype JsonListModel
    \inqmlmodule QSyncable

JsonListModel is a syncable list model.
It could be used as a wrapper of other Javascript array object.
Whatever the source is updated, it will trigger synchronization automatically.
It will emit signals of insertion, removal and moving automatically.
 */

/*! \qmlproperty int JsonListModel::count

    No. of items in this list model.

 */

/*! \qmlmethod QVariantMap JsonListModel::get(int i) const

    Returns the item at index in the list model.
 */

/*! \qmlmethod  void JsonListModel::setProperty(int index, QString property, QVariant value)

    Apply the changes to a record at index. Only modified value will be set.
 */

/*! \qmlmethod  void JsonListModel::set(int index, QVariantMap changes)

    Changes the item at index in the list model with the values in changes. Properties not appearing in changes are left unchanged.

    If index is equal to count() then a new item is appended to the list. Otherwise, index must be an element in the list.
 */

/*! \qmlmethod  void JsonListModel::insert(int index,const QVariantMap& value);

  Inserts an item at index position.

 */

/*! \qmlmethod  void JsonListModel::remove(int i , int count = 1);

    Deletes the content at index from the model. You may specific the no. of items to be removed by count argument.
 */

/*! \qmlmethod void JsonListModel::move(int from, int to, int n)

    Moves n items from one position to another.
 */

/*! \qmlmethod void JsonListModel::append(const QVariantMap &value)

  Append an items at the end of list

 */

/*! \qmlmethod int JsonListModel::indexOf(QString field, QVariant value)

  Get the index of the given record which is equal to input value on given field.

  If it is not found, it will return -1.
 */

QSJsonListModel::QSJsonListModel(QObject *parent) : QSListModel(parent)
{
    componentCompleted = false;
    connect(this, &QAbstractItemModel::rowsInserted, this, &QSJsonListModel::handleRowsInserted);
    connect(this, &QAbstractItemModel::rowsMoved, this, &QSJsonListModel::handleRowsMoved);
    connect(this, &QAbstractItemModel::rowsRemoved, this, &QSJsonListModel::handleRowsRemoved);
    connect(this, &QAbstractItemModel::dataChanged, this, &QSJsonListModel::handleDataChanged);
}

/*! \qmlproperty string JsonListModel::keyField

  Set the key field of data source.
  The value in key field should be unique.

  If it is not set, JsonListModel won't be able to identify insertion, removal
  and moving changes.

 */
QString QSJsonListModel::keyField() const
{
    return m_keyField;
}

void QSJsonListModel::setKeyField(const QString &keyField)
{
    m_keyField = keyField;
    emit keyFieldChanged();
}

/*! \qmlproperty array JsonListModel::source

  JsonListModel is a wrapper of another Javascript array.
  Update this property will trigger the synchronization and emit changes according to the difference.

  Example:

\code
    JsonListModel {
        keyField: "id"

        source: [
            { "id": "a", "value": 1},
            { "id": "b", "value": 2}
        ]

        fields: [
            "id",
            "value"
        ]
    }

\endcode


 */

QVariantList QSJsonListModel::source() const
{
    return m_source;
}

void QSJsonListModel::setSource(const QVariantList &source)
{
    m_source = source;

    if (componentCompleted) {
        sync();
    }

    emit sourceChanged();
}

/*! \qmlproperty array JsonListModel::fields

  Define the available fields of this model.
  If it is not assigned, JsonListModel will use the first appended record.
  Then you are not able to change afterward.

  Example:

\code

    JsonListModel {
        keyField: "id"
        fields: [
            "id",
            "value"
        ]
    }

\endcode
 */

QStringList QSJsonListModel::fields() const
{
    return m_fields;
}

void QSJsonListModel::setFields(const QStringList &roleNames)
{
    m_fields = roleNames;
    setRoleNames(roleNames);
    emit fieldsChanged();
}

/*! \qmlproperty array JsonListModel::synchronizeModelChangesToSource

Propagate the changes in the Model back to the source property
    Default is false.
        */

bool QSJsonListModel::synchronizeModelChangesToSource() const
{
    return m_synchronizeModelChangesToSource;
}

void QSJsonListModel::setSynchronizeModelChangesToSource(bool synchronizeModelChangesToSource)
{
    if (m_synchronizeModelChangesToSource == synchronizeModelChangesToSource)
        return;
    m_synchronizeModelChangesToSource = synchronizeModelChangesToSource;
    emit synchronizeModelChangesToSourceChanged();
}

void QSJsonListModel::classBegin()
{

}

void QSJsonListModel::componentComplete()
{
    componentCompleted = true;

    if (!m_source.isEmpty()) {
        sync();
    }
}

void QSJsonListModel::handleDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
    if (synchronizeModelChangesToSource())
        syncStorageToSource();
}

void QSJsonListModel::handleRowsInserted(const QModelIndex &parent, int first, int last)
{
    if (synchronizeModelChangesToSource())
        syncStorageToSource();
}

void QSJsonListModel::handleRowsMoved(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int row)
{
    if (synchronizeModelChangesToSource())
        syncStorageToSource();
}

void QSJsonListModel::handleRowsRemoved(const QModelIndex &parent, int first, int last)
{
    if (synchronizeModelChangesToSource())
        syncStorageToSource();
}

void QSJsonListModel::sync()
{
    QSDiffRunner runner;
    runner.setKeyField(m_keyField);

    QList<QSPatch> patches = runner.compare(storage(), m_source);

    if (patches.size() > 0) {
        runner.patch(this, patches);
    }
}

void QSJsonListModel::syncStorageToSource()
{
    // don't call setSource as this invokes a unneccessary sync
    m_source = storage();
    emit sourceChanged();
}
