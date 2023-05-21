#pragma once
#include "sqladapter.h"

class MySqlAdapter : public SqlAdapter
{
public:
    MySqlAdapter(std::shared_ptr<QSqlDatabase> db);
    bool createDatabase(const QString& name);
    bool createTable(const QString& tableName, const QHash<QString, QString>& info);
    bool createTableRelations(const QString& parent, Relation rel, const QString& child);
    void initDB(const QString& name);

};
