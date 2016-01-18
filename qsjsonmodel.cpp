#include <QtQml>
#include "qsdiffrunner.h"
#include "qsjsonmodel.h"

QSJsonModel::QSJsonModel(QObject *parent) : QSListModel(parent)
{

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

    QSDiffRunner runner;
    runner.setKeyField(m_keyField);

    QList<QSPatch> patches = runner.compare(storage(), source);

    if (patches.size() > 0) {
        runner.patch(this, patches);
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

class QSJsonModelRegistionHelper {

public:
    QSJsonModelRegistionHelper() {
        qmlRegisterType<QSJsonModel>("QSyncable", 1, 0, "JsonModel");
    }
};

static QSJsonModelRegistionHelper registerHelper;
