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
#include "qsyncablefunctions.h"

class QSListModel : public QAbstractListModel, public QSPatchable
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    explicit QSListModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent) const override;

    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;

    int count() const;

    QHash<int, QByteArray> roleNames() const override;

    void setRoleNames(const QVariantMap& value);

    void setRoleNames(const QStringList& value);

    void setStorage(const QVariantList& value);

    QVariantList storage() const;

    virtual void insert(int index, const QVariantList &value) override;


public slots:

    void append(const QVariantMap&value);

    void insert(int index,const QVariantMap& value);

    void clear();

    virtual void move(int from, int to, int count = 1) override;

    virtual void remove(int i , int count  = 1) override;

    int indexOf(QString field,QVariant value) const;

    QVariantMap get(int i) const;

    void setProperty(int index,QString property ,QVariant value);

    void set(int index,QVariantMap data) override;



signals:
    void countChanged();

private:

    QHash<int, QByteArray> m_roles;
    QHash<QString, int> m_rolesLookup;

    QVariantList m_storage;
};
