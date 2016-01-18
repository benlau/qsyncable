#include <QtQml>
#include "qsdiffrunner.h"
#include "qsjsonmodel.h"

QSJsonModel::QSJsonModel(QObject *parent) : QSListModel(parent)
{
    componentCompleted = false;
}

QString QSJsonModel::keyField() const
{
    return m_keyField;
}

void QSJsonModel::setKeyField(const QString &keyField)
{
    m_keyField = keyField;
    emit keyFieldChanged();
}

QVariantList QSJsonModel::source() const
{
    return m_source;
}

void QSJsonModel::setSource(const QVariantList &source)
{
    m_source = source;

    if (componentCompleted) {
        sync();
    }

    emit sourceChanged();

}

QStringList QSJsonModel::fieldNames() const
{
    return m_fieldNames;
}

void QSJsonModel::setFieldNames(const QStringList &roleNames)
{
    m_fieldNames = roleNames;
    setRoleNames(roleNames);
    emit fieldNamesChanged();
}

void QSJsonModel::classBegin()
{

}

void QSJsonModel::componentComplete()
{
    componentCompleted = true;

    if (!m_source.isEmpty()) {
        sync();
    }
}

void QSJsonModel::sync()
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
        qmlRegisterType<QSJsonModel>("QSyncable", 1, 0, "JsonModel");
    }
};

static QSJsonModelRegistionHelper registerHelper;
