#pragma once
#include <QObject>
#include <QString>
#include <QSqlRecord>
#include <QSqlField>
#include <QList>
#include <QFile>
#include <QSqlDriver>
#include "ORMDatabase.h"
#include <memory>
#include <mutex>
#include "orm4qt_global.h"
#include "ORMLogger.h"
#include <QDebug>
class ORM4QT_EXPORT ORMDatabaseFactory
{
public:
	~ORMDatabaseFactory();

	static std::shared_ptr<ORMDatabaseFactory> getInstance()
	{
		std::call_once(s_flag, [=]() {
			m_instance = std::shared_ptr<ORMDatabaseFactory>(new ORMDatabaseFactory());
					   });
		return m_instance;
		//return	nullptr;
	}

	bool registerDatabase(QString DBname, QString hostName, QString userName, QString password);

	bool registerDatabase(QString DBname, QString userName, QString password);

	void registerDatabase(QString DBname, std::shared_ptr<ORMDatabase> ormDB);

	std::shared_ptr<ORMDatabase> getDatabase(QString DBname);

	std::shared_ptr<ORMDatabase> getDefaultDatabase();

	void closeAndClearAllDatabase();

	bool dropDatabase(QString DBname);
	void RemoveFromFactory(QString DBname);

private:
	ORMDatabaseFactory();

	QMap<QString, std::shared_ptr<ORMDatabase>> m_dbs;

	static std::once_flag s_flag;

	static std::shared_ptr<ORMDatabaseFactory> m_instance;

};
