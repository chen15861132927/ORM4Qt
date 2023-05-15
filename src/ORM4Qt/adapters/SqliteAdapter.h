#pragma once

#include "sqladapter.h"

#include "../orm4qt_global.h"

class ORM4QT_EXPORT SqliteAdapter : public SqlAdapter
{
public:
    SqliteAdapter();
    bool createTable(const QString &tableName, const QHash<QString, QString> &info);
    int addRecord(const QString &tableName, const QHash<QString, QVariant> &info);
};


