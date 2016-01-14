#include "qsdiffrunner.h"

QSDiffRunner::QSDiffRunner()
{

}

QString QSDiffRunner::keyField() const
{
    return m_keyField;
}

void QSDiffRunner::setKeyField(const QString &keyField)
{
    m_keyField = keyField;
}

/*! \fn QList<QSChange> QSDiffRunner::run(const QVariantList &previous, const QVariantList &current)

    Call this function to compare previous and current list, then return a
    list of changes required to transform from previous to current with
    the minimum number of steps. It uses an algorithm with O(n) runtime.
 */

QList<QSChange> QSDiffRunner::run(const QVariantList &previous, const QVariantList &current)
{
    QList<QSChange> res;

    return res;
}

