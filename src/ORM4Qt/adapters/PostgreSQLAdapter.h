#pragma once

#include "sqladapter.h"

#include "../orm4qt_global.h"

class ORM4QT_EXPORT PostgreSQLAdapter : public SqlAdapter
{
public:
    PostgreSQLAdapter();
    bool createTable(const QString &tableName, const QHash<QString, QString> &info);
    bool createTableRelations(const QString &parent, Relation rel, const QString &child);
    int addRecord(const QString &tableName, const QHash<QString, QVariant> &info);

protected:
    void fillTableTypes();
};

