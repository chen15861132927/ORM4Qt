#pragma once

#include "sqladapter.h"

#include "../orm4qt_global.h"

class SqliteAdapter : public SqlAdapter
{
public:
    SqliteAdapter(std::shared_ptr<QSqlDatabase> db);
    bool createTable(const QString &tableName, const QHash<QString, QString> &info);
    int addRecord(const QString &tableName, const QHash<QString, QVariant> &info);
};


