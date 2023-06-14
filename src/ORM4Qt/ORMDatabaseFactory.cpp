#include "ORMDatabaseFactory.h"
std::shared_ptr<ORMDatabaseFactory> ORMDatabaseFactory::m_instance = {};

std::once_flag ORMDatabaseFactory::s_flag;

ORMDatabaseFactory::ORMDatabaseFactory() {
}

ORMDatabaseFactory::~ORMDatabaseFactory()
{
	//delete m_instance.get();
}


/// <summary>
/// driverName is "QMYSQL"
/// </summary>
/// <param name="DBname"></param>
/// <param name="hostName"></param>
/// <param name="userName"></param>
/// <param name="password"></param>
/// <returns></returns>
bool ORMDatabaseFactory::registerDatabase(QString DBname, QString hostName, QString userName, QString password)
{
	if (m_dbs.contains(DBname))
	{
		return true;
	}
	std::shared_ptr<ORMDatabase> db = std::make_shared<ORMDatabase>("QMYSQL");
	db->setHostName(hostName);
	db->setUserName(userName);
	db->setPassword(password);
	bool res = false;
	if (db->open())
	{
		res = db->setActiveDatabase(DBname);
		db->setLogDeep(OrmLogger::LogDeep::ldAll);

		m_dbs.insert(DBname, db);
	}
	return res;
}

bool ORMDatabaseFactory::registerDatabase(QString DBname, QString userName, QString password)
{
	std::shared_ptr<ORMDatabase> db = std::make_shared<ORMDatabase>("QSQLITE");
	db->setDatabaseName(DBname + ".db");
	db->setUserName(userName);
	db->setPassword(password);
	bool res = false;
	if (db->open())
	{
		res = db->setActiveDatabase(DBname);
		db->setLogDeep(OrmLogger::LogDeep::ldWarningsOnly);

		m_dbs.insert(DBname, db);
	}
	return res;
}

void ORMDatabaseFactory::registerDatabase(QString DBname, std::shared_ptr<ORMDatabase> ormDB)
{
	m_dbs.insert(DBname, ormDB);
}

std::shared_ptr<ORMDatabase> ORMDatabaseFactory::getDatabase(QString DBname)
{
	if (m_dbs.find(DBname) != m_dbs.end())
	{
		return m_dbs[DBname];
	}
	else
	{
		return nullptr;
	}
}

std::shared_ptr<ORMDatabase> ORMDatabaseFactory::getDefaultDatabase()
{
	if (m_dbs.count() > 0)
	{
		return m_dbs.first();
	}
	else
	{
		return nullptr;
	}
}

void ORMDatabaseFactory::closeAndClearAllDatabase()
{
	for each (auto var in m_dbs.values())
	{
		var->close();
	}
	m_dbs.clear();
}