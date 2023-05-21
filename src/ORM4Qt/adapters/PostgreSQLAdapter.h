#pragma once

#include "sqladapter.h"

#include "../orm4qt_global.h"

class PostgreSQLAdapter : public SqlAdapter
{
public:
    PostgreSQLAdapter(std::shared_ptr<QSqlDatabase> db);
    bool createTable(const QString &tableName, const QHash<QString, QString> &info);
    bool createTableRelations(const QString &parent, Relation rel, const QString &child);
    int addRecord(const QString &tableName, const QHash<QString, QVariant> &info);

protected:
    void fillTableTypes();
};

