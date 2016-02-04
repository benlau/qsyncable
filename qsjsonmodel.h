#ifndef QSJSONMODEL_H
#define QSJSONMODEL_H

#include <QObject>
#include <QQmlParserStatus>
#include "qslistmodel.h"

class QSJsonModel : public QSListModel, public QQmlParserStatus
{
    Q_OBJECT
    Q_PROPERTY(QString keyField READ keyField WRITE setKeyField NOTIFY keyFieldChanged)
    Q_PROPERTY(QVariantList source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(QStringList fieldNames READ fieldNames WRITE setFieldNames NOTIFY fieldNamesChanged)
    Q_INTERFACES(QQmlParserStatus)

public:
    explicit QSJsonModel(QObject *parent = 0);

    QString keyField() const;

    void setKeyField(const QString &keyField);

    QVariantList source() const;

    void setSource(const QVariantList &source);

    QStringList fieldNames() const;

    void setFieldNames(const QStringList &roleNames);

signals:

    void keyFieldChanged();

    void sourceChanged();

    void fieldNamesChanged();

public slots:

protected:
    virtual void classBegin();
    virtual void componentComplete();

private:
    void sync();

    QString m_keyField;

    QVariantList m_source;

    QStringList m_fieldNames;

    bool componentCompleted;
};

#endif // QSJSONMODEL_H
