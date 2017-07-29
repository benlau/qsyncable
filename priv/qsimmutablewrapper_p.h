#pragma once
#include <QVariantMap>
#include <QMetaMethod>

template <typename T>
class QSImmutableWrapper {

public:
    inline bool isShared(const T& v1, const T& v2) const {
        return v1.isSharedWith(v2);
    }

    inline QVariantMap convert(const T& object) {
        // Default convertor function
        QVariantMap data;
        const QMetaObject meta = object.staticMetaObject;

        for (int i = 0 ; i < meta.propertyCount(); i++) {
            const QMetaProperty property = meta.property(i);
            const char* name = property.name();
            QVariant value = property.readOnGadget(&object);
            data[name] = value;
        }

        return data;
    }

    inline bool hasKey() {
        const QMetaObject meta = T::staticMetaObject;
        return meta.indexOfMethod("key()") >= 0;
    }

    QString key(const T& value) {
        QString ret;
        const QMetaObject meta = T::staticMetaObject;
        int index = meta.indexOfMethod("key()");
        if (index < 0) {
            return ret;
        }

        QMetaMethod method = meta.method(index);

        if (method.returnType() == QVariant::Int) {
            int iRet;
            method.invokeOnGadget((void*) &value, Q_RETURN_ARG(int, iRet));
            ret = QString::number(iRet);

        } else if (method.returnType() == QVariant::String) {
            method.invokeOnGadget((void*) &value, Q_RETURN_ARG(QString, ret));
        } else {
            qWarning() << "QSyncable::Invalid key type";
        }

        return ret;
    }

    QVariantMap diff(const T& v1, const T& v2) {
        auto prev = convert(v1);
        auto current = convert(v2);

        QVariantMap res;
        QMap<QString, QVariant>::const_iterator iter = current.begin();

        while (iter != current.end()) {
            QString key = iter.key();
            if (!prev.contains(key) ||
                 prev[key] != iter.value()) {
                res[key] = iter.value();
            }
            iter++;
        }

        return res;
    }

    QVariantMap fastDiff(const T& v1, const T& v2) {
        if (isShared(v1,v2)) {
            return QVariantMap();
        }
        return diff(v1, v2);
    }
};

template<>
class QSImmutableWrapper<QVariantMap> {
public:
    inline bool isShared(const QVariantMap& v1, const QVariantMap& v2) const {
        return v1.isSharedWith(v2);
    }

    inline QVariantMap convert(const QVariantMap& object) {
        return object;
    }

    QVariantMap diff(const QVariantMap& v1, const QVariantMap& v2) {
        auto prev = v1;
        auto current = v2;

        QVariantMap res;
        QMap<QString, QVariant>::const_iterator iter = current.begin();

        while (iter != current.end()) {
            QString key = iter.key();
            if (!prev.contains(key) ||
                 prev[key] != iter.value()) {
                res[key] = iter.value();
            }
            iter++;
        }

        return res;
    }

    QVariantMap fastDiff(const QVariantMap& v1, const QVariantMap& v2) {
        if (isShared(v1,v2)) {
            return QVariantMap();
        }
        return diff(v1, v2);
    }

    bool hasKey() {
        return !keyField.isNull();
    }

    QString key(const QVariantMap& object) {
        if (keyField.isNull()) {
            return QString();
        }

        QString res;
        QVariant value = object[keyField];
        if (value.type() == QVariant::Int) {
            res = QString::number(value.toInt());
        } else if (value.type() == QVariant::String) {
            res = value.toString();
        }
        return res;
    }

    QString keyField;

};
