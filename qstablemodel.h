#pragma once

#include <QAbstractTableModel>
#include "qspatchable.h"
#include "qsyncablefunctions.h"

class QSTableModel : public QAbstractTableModel, public QSPatchable
{
    Q_OBJECT
    Q_PROPERTY(int rowCount READ rCount NOTIFY rCountChanged)
    Q_PROPERTY(int columnCount READ cCount NOTIFY cCountChanged)

public:
    explicit QSTableModel(QObject *parent = nullptr);

    int rCount() const;
    int cCount() const;

    void setStorage(const QVariantList& value);
    QVariantList storage() const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    virtual int rowCount(const QModelIndex &parent = {}) const override;
    virtual int columnCount(const QModelIndex &parent = {}) const override;

    void append(const QVariantMap &value);
    void insert(int index,const QVariantMap &value);
    void clear();

    void setColumnMapping(QVariantMap value);

    int indexOf(QString field,QVariant value) const;
    QVariantMap get(int i) const;
    void setColumn(int row, int column ,QVariant value);

    // QSPatchableInterface
    virtual void insert(int index, const QVariantList &value) override;
    virtual void move(int from, int to, int count = 1) override;
    virtual void remove(int i , int count  = 1) override;
    void set(int index,QVariantMap data) override;

signals:
    void rCountChanged();
    void cCountChanged();

private:
    QHash<int, QString> m_columns;
    QHash<QString, int> m_columnsLookup;

    QVariantList m_storage;

    // QAbstractItemModel interface
public:
    virtual QHash<int, QByteArray> roleNames() const override;
};
