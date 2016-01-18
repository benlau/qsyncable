/* QSyncable Project
   Author: Ben Lau
   License: Apache-2.0
   Web: https://github.com/benlau/qsyncable
*/

#pragma once

#include <QAbstractItemModel>
#include <QAbstractListModel>
#include <QPointer>
#include <QSharedPointer>
#include "qspatchable.h"

class QSVariantListModel : public QAbstractListModel, public QSPatchable
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    explicit QSVariantListModel(QObject *parent = 0);

    int rowCount(const QModelIndex &parent) const;

    QVariant data(const QModelIndex &index, int role) const;

    Q_INVOKABLE void append(const QVariantMap&value);

    Q_INVOKABLE void insert(int index,const QVariantMap& value);

    Q_INVOKABLE void move(int from, int to, int count = 1);

    Q_INVOKABLE void clear();

    Q_INVOKABLE void remove(int i , int count  = 1);

    int count() const;

    Q_INVOKABLE QVariantMap get(int i) const;

    Q_INVOKABLE void setProperty(int index,QString property ,QVariant value);

    Q_INVOKABLE void set(int index,QVariantMap data);

    /// Apply the changes to a record at index. Only modified value will be set.
    void setProperties(int index,QVariantMap changes);

    QHash<int, QByteArray> roleNames() const;

    void setRoleNames(const QVariantMap& value);

    void setRoleNames(const QStringList& value);

    void setList(const QList<QVariantMap>& value);
    void setList(const QVariantList& value);


    QVariantList list() const;

    /* Extra API */

    Q_INVOKABLE int indexOf(QString field,QVariant value) const;

signals:
    void countChanged();

public slots:

private:

    QHash<int, QByteArray> m_roles;

    QVariantList m_data;
};
