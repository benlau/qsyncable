#include <QtQml>
#include "qsdiffrunner.h"
#include "qsjsonlistmodel.h"

QSJsonListModel::QSJsonListModel(QObject *parent) : QSListModel(parent)
{
    componentCompleted = false;
}

QString QSJsonListModel::keyField() const
{
    return m_keyField;
}

void QSJsonListModel::setKeyField(const QString &keyField)
{
    m_keyField = keyField;
    emit keyFieldChanged();
}

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

QStringList QSJsonListModel::fieldNames() const
{
    return m_fieldNames;
}

void QSJsonListModel::setFieldNames(const QStringList &roleNames)
{
    m_fieldNames = roleNames;
    setRoleNames(roleNames);
    emit fieldNamesChanged();
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

void QSJsonListModel::sync()
{
    QSDiffRunner runner;
    runner.setKeyField(m_keyField);

    QList<QSPatch> patches = runner.compare(storage(), m_source);

    if (patches.size() > 0) {
        runner.patch(this, patches);
    }
}

class QSJsonModelRegistionHelper {

public:
    QSJsonModelRegistionHelper() {
        qmlRegisterType<QSJsonListModel>("QSyncable", 1, 0, "JsonListModel");
    }
};

static QSJsonModelRegistionHelper registerHelper;
