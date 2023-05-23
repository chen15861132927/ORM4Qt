#include "mysqladapter.h"

MySqlAdapter::MySqlAdapter(std::shared_ptr<QSqlDatabase> db) :SqlAdapter(db)
{
	fillTableTypes();
}

bool MySqlAdapter::createDatabase(const QString& name)
{
	m_lastQuery = QString("CREATE DATABASE %1;")
		.arg(name);

	bool result = m_logger.exec(m_query, m_lastQuery);
	if (result)
		initDB(name);
	return result;
}

bool MySqlAdapter::createTable(const QString& tableName, const QHash<QString, QString>& info)
{
	QString name;
	m_lastQuery = QString("CREATE TABLE %1( " + QString(idColumnName) + " BIGINT AUTO_INCREMENT, ")
		.arg(tableName);
	foreach(name, info.keys())
		m_lastQuery += QString("%1 %2, ")
		.arg(name)
		.arg(m_tableTypes.value(info.value(name)));
	m_lastQuery += "PRIMARY KEY (" + QString(idColumnName) + "));";
	m_query.clear();
	return m_logger.exec(m_query, m_lastQuery);
}

bool MySqlAdapter::createTableRelations(const QString& parent, ORMAbstractAdapter::Relation rel, const QString& child)
{
	if (rel == HasMany)
		m_lastQuery = QString("ALTER TABLE %1 ADD %2_" + QString(idColumnName) + " BIGINT AFTER " + QString(idColumnName) + ", ADD FOREIGN KEY(%2_" + QString(idColumnName) + ") REFERENCES %2(" + QString(idColumnName) + ");")
		.arg(child)
		.arg(parent);
	else if (rel == HasOne)
		m_lastQuery = QString("ALTER TABLE %1 ADD %2_" + QString(idColumnName) + " BIGINT AFTER " + QString(idColumnName) + ", ADD FOREIGN KEY(%2_" + QString(idColumnName) + ") REFERENCES %2(" + QString(idColumnName) + "),"
							  "ADD UNIQUE(%2_" + QString(idColumnName) + ");")
		.arg(child)
		.arg(parent);
	return m_logger.exec(m_query, m_lastQuery);
}

bool MySqlAdapter::initDB(const QString& name)
{
	bool result = false;
	QString sql = QString("SHOW DATABASES LIKE '%1';").arg(name);
	bool checkresult = exec(sql);

	bool existDB = false;
	while (m_query.next())
	{
		QString name = m_query.value(0).toString();
		if (name.toLower() == name.toLower())
		{
			existDB = true;
			result = true;
		}
	}
	if (!existDB)
	{
		sql = QString("CREATE DATABASE %1").arg(name);
		result = exec(sql);
	}

	if (result)
	{
		m_lastQuery = QString("USE %1;").arg(name);
		result = m_logger.exec(m_query, m_lastQuery);
	}
	return result;
}
