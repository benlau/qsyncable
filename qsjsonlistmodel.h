#pragma once

#include <QObject>
#include <QQmlParserStatus>
#include "qslistmodel.h"

/* QSJsonListModel is a data model that combine QSListModel and QSDiffRunner
 * into a single class. It may take a Javascript array object as source input,
 * it will compare with previous input then update the model according to
 * the diff.
 */

class QSJsonListModel : public QSListModel, public QQmlParserStatus
{
    Q_OBJECT
    Q_PROPERTY(QString keyField READ keyField WRITE setKeyField NOTIFY keyFieldChanged)
    Q_PROPERTY(QVariantList source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(QStringList fields READ fields WRITE setFields NOTIFY fieldsChanged)
    Q_PROPERTY(bool synchronizeModelChangesToSource READ synchronizeModelChangesToSource WRITE setSynchronizeModelChangesToSource NOTIFY synchronizeModelChangesToSourceChanged)

    Q_INTERFACES(QQmlParserStatus)

public:
    explicit QSJsonListModel(QObject *parent = 0);

    QString keyField() const;
    void setKeyField(const QString &keyField);

    QVariantList source() const;
    void setSource(const QVariantList &source);

    QStringList fields() const;
    void setFields(const QStringList &roleNames);

    bool synchronizeModelChangesToSource() const;
    void setSynchronizeModelChangesToSource(bool synchronizeModelChangesToSource);

signals:
    void keyFieldChanged();
    void sourceChanged();
    void fieldsChanged();
    void synchronizeModelChangesToSourceChanged();

protected:
    virtual void classBegin() override;
    virtual void componentComplete() override;

private slots:
    void handleDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles = QVector<int>());
    void handleRowsInserted(const QModelIndex &parent, int first, int last);
    void handleRowsMoved(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int row);
    void handleRowsRemoved(const QModelIndex &parent, int first, int last);

private:
    void sync();
    void syncStorageToSource();

    QString m_keyField;
    QVariantList m_source;
    QStringList m_fields;
    bool m_synchronizeModelChangesToSource = false;
    bool componentCompleted;
};
