#include "ormabstractadapter.h"

ORMAbstractAdapter::ORMAbstractAdapter(std::shared_ptr<QSqlDatabase> db)
{
	m_query = db->exec();
}

QSqlError ORMAbstractAdapter::lastError() const
{
	return m_query.lastError();
}

QString ORMAbstractAdapter::lastQuery() const
{
	return m_lastQuery;
}

QSqlQuery ORMAbstractAdapter::lastQSqlQuery() const
{
	return m_query;
}

bool ORMAbstractAdapter::exec(const QString& query)
{
	return m_logger.exec(m_query, query);
}