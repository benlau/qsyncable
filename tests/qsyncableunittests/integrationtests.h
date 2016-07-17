#ifndef INTEGRATIONTESTS_H
#define INTEGRATIONTESTS_H

#include <QObject>

class IntegrationTests : public QObject
{
    Q_OBJECT
public:
    explicit IntegrationTests(QObject *parent = 0);

signals:

public slots:

private slots:
    void sortFilterProxyModel();
};

#endif // INTEGRATIONTESTS_H
