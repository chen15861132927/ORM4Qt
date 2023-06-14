#pragma once
#include "sqladapter.h"

class MySqlAdapter : public SqlAdapter
{
public:
    MySqlAdapter(std::shared_ptr<QSqlDatabase> db);
    bool createDatabase(const QString& name);
    bool createTable(const QString& tableName, const QHash<QString, QString>& info);
    bool createTableRelations(const QString& parent, Relation rel, const QString& child);
    bool initDB(const QString& name);
    bool alterTable(const QString& tableName, const QHash<QString, QString>& info);
    int addRecord(const QString& tableName, const QHash<QString, QVariant>& info);

private:
    QHash<QString, QString> getTableColumns(const QString& tableName);

};
