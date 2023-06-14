#include "ormdatabase.h"

ORMDatabase::ORMDatabase(QString driverName)
{
	m_db = std::make_shared<ORMQSqlDatabase>(driverName);
	if (driverName == "QMYSQL")
		m_adapter = std::make_shared<MySqlAdapter>(m_db);
	else if (driverName == "QSQLITE")
		m_adapter = std::make_shared<SqliteAdapter>(m_db);
	else if (driverName == "QPSQL")
		m_adapter = std::make_shared<PostgreSQLAdapter>(m_db);
	else
		m_adapter = std::make_shared<SqlAdapter>(m_db);


}

bool ORMDatabase::createDatabase(QString name)
{
	return m_adapter->createDatabase(name);
}

bool ORMDatabase::dropDatabase(QString name)
{
	return m_adapter->dropDatabase(name);
}

QString ORMDatabase::lastQuery() const
{
	return m_adapter->lastQuery();
}

void ORMDatabase::setLogDeep(OrmLogger::LogDeep deep)
{
	m_adapter->setLogDeep(deep);
}
