#include "postgresqladapter.h"
PostgreSQLAdapter::PostgreSQLAdapter(std::shared_ptr<QSqlDatabase> db) :SqlAdapter(db)
{
	fillTableTypes();
}

bool PostgreSQLAdapter::createTable(const QString& tableName, const QHash<QString, QString>& info)
{
	QString name;
	auto m_lastQuery = QString("CREATE TABLE %1(" + QString(idColumnName) + " BIGSERIAL, ")
		.arg(tableName);
	foreach(name, info.keys())
		m_lastQuery += QString("%1 %2, ")
		.arg(name)
		.arg(m_tableTypes.value(info.value(name)));
	m_lastQuery += "PRIMARY KEY (" + QString(idColumnName) + "));";
	return exec(m_lastQuery);
}

bool PostgreSQLAdapter::createTableRelations(const QString& parent, ORMAbstractAdapter::Relation rel, const QString& child)
{
	QString sql;
	if (rel == HasMany)
		sql = QString("ALTER TABLE %1 ADD %2_" + QString(idColumnName) + " BIGINT, ADD FOREIGN KEY(%2_" + QString(idColumnName) + ") REFERENCES %2(" + QString(idColumnName) + ");")
		.arg(child)
		.arg(parent);
	else if (rel == HasOne)
		sql = QString("ALTER TABLE %1 ADD %2_" + QString(idColumnName) + " BIGINT, ADD FOREIGN KEY(%2_" + QString(idColumnName) + ") REFERENCES %2(" + QString(idColumnName) + "),"
					  "ADD UNIQUE(%2_" + QString(idColumnName) + ");")
		.arg(child)
		.arg(parent);
	return exec(sql);
}

int PostgreSQLAdapter::addRecord(const QString& tableName, const QHash<QString, QVariant>& info)
{
	QString key;
	QString sql;
	if (info.isEmpty())
		sql = QString("INSERT INTO %1 DEFAULT VALUES RETURNING " + QString(idColumnName) + ";")
		.arg(tableName);
	else
	{
		sql = QString("INSERT INTO %1(")
			.arg(tableName);
		foreach(key, info.keys())
			sql += key + ", ";
		if (!info.isEmpty())
			sql.resize(sql.size() - 2);
		sql += ") VALUES(";
		foreach(key, info.keys())
			sql += "'" + info.value(key).toString() + "', ";
		if (!info.isEmpty())
			sql.resize(sql.size() - 2);
		sql += ") RETURNING " + QString(idColumnName) + ";";
	}
	if (exec( sql))
	{
		m_query.next();
		return m_query.record().value(0).toInt();
	}
	else
		return -1;
}

void PostgreSQLAdapter::fillTableTypes()
{
	m_tableTypes.insert("bool", "BOOL");
	m_tableTypes.insert("int", "INT");
	m_tableTypes.insert("uint", "INT");
	m_tableTypes.insert("qlonglong", "BIGINT");
	m_tableTypes.insert("qulonglong", "BIGINT");
	m_tableTypes.insert("double", "DOUBLE PRECISION");
	m_tableTypes.insert("QByteArray", "BYTEA");
	m_tableTypes.insert("QChar", "CHAR(1)");
	m_tableTypes.insert("QDate", "DATE");
	m_tableTypes.insert("QTime", "TIME");
	m_tableTypes.insert("QDateTime", "TIMESTAMP");
	m_tableTypes.insert("QString", "TEXT");
}
