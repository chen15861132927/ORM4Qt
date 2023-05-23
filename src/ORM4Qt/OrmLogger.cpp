#include "ormlogger.h"
#include <QDebug>
OrmLogger::OrmLogger() :
	m_logDeep(ldAll)
{
}

bool OrmLogger::exec(QSqlQuery query)
{
	bool result = query.exec();
	switch (m_logDeep)
	{
		case ldAll:
		{
			qDebug() << '[' + QTime::currentTime().toString() + "] " + query.lastQuery() << endl;
			if (!result)
				qDebug() << "Warning: " << query.lastError().text() << endl;
			break;
		}
		case ldWarningsOnly:
		{
			if (!result)
			{
				qDebug() << '[' + QTime::currentTime().toString() + "] " + query.lastQuery() << endl;
				qDebug() << "Warning: " << query.lastError().text() << endl;
			}
			break;
		}
		case ldNone: {}
	}
	return result;
}

bool OrmLogger::exec(QSqlQuery& query, const QString& queryString)
{
	bool result = query.exec(queryString);
	switch (m_logDeep)
	{
		case ldAll:
		{
			qDebug() << '[' + QTime::currentTime().toString() + "] " + query.lastQuery() << endl;
			if (!result)
				qDebug() << "Warning: " << query.lastError().text() << endl;
			break;
		}
		case ldWarningsOnly:
		{
			if (!result)
			{
				qDebug() << '[' + QTime::currentTime().toString() + "] " + query.lastQuery() << endl;
				qDebug() << "Warning: " << query.lastError().text() << endl;
			}
			break;
		}
		case ldNone: {}
	}
	return result;
}

void OrmLogger::setLogDeep(OrmLogger::LogDeep deep)
{
	m_logDeep = deep;
}
