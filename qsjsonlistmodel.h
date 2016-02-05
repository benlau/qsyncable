#pragma once

#include <QObject>
#include <QQmlParserStatus>
#include "qslistmodel.h"

class QSJsonListModel : public QSListModel, public QQmlParserStatus
{
    Q_OBJECT
    Q_PROPERTY(QString keyField READ keyField WRITE setKeyField NOTIFY keyFieldChanged)
    Q_PROPERTY(QVariantList source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(QStringList fields READ fields WRITE setFields NOTIFY fieldsChanged)
    Q_INTERFACES(QQmlParserStatus)

public:
    explicit QSJsonListModel(QObject *parent = 0);

    QString keyField() const;

    void setKeyField(const QString &keyField);

    QVariantList source() const;

    void setSource(const QVariantList &source);

    QStringList fields() const;

    void setFields(const QStringList &roleNames);

signals:

    void keyFieldChanged();

    void sourceChanged();

    void fieldsChanged();

public slots:

protected:
    virtual void classBegin();
    virtual void componentComplete();

private:
    void sync();

    QString m_keyField;

    QVariantList m_source;

    QStringList m_fields;

    bool componentCompleted;
};
