#pragma once
#include <QVariantMap>
#include <QMetaMethod>

template <typename T>
class QSImmutableWrapper {

public:
    inline bool fastCompare(const T& v1, const T& v2) const {
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

    QVariant key(const T& value) {
        QVariant ret;
        const QMetaObject meta = T::staticMetaObject;
        int index = meta.indexOfMethod("key()");
        if (index < 0) {
            return ret;
        }

        QMetaMethod method = meta.method(index);
        method.invokeOnGadget((void*) &value, Q_RETURN_ARG(QVariant, ret));
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
};

template<>
class QSImmutableWrapper<QVariantMap> {
public:
    inline bool fastCompare(const QVariantMap& v1, const QVariantMap& v2) const {
        Q_UNUSED(v1);
        Q_UNUSED(v2);
        /// QVariantMap do not support fast compare
        return false;
    }

    inline QVariantMap convert(const QVariantMap& object) {
        return object;
    }
};
