#pragma once
#include <memory>
#include <QSqlDatabase>
#include "adapters/sqladapter.h"
#include "adapters/mysqladapter.h"
#include "adapters/sqliteadapter.h"
#include "adapters/postgresqladapter.h"

/*!
   \class ORMDatabase
   \brief The ORMDatabase is wrapper for QSqlDatabase

   The ORMDatabase provides interface for comfortable accessing a database with one connection.
   If you want use ORMObject models, you must create object of this class. Example:
   \code
   ORMDatabase myDatabase("QMYSQL");
   myDatabase.setUserName("root");
   myDatabase.open();
   myDatabase.createDatabase("MyDatabase");
   \endcode
   or
   \code
   ORMDatabase myDatabase;
   myDatabase = ORMDatabase::addORMDatabase("QMYSQL");
   myDatabase.setUserName("root");
   myDatabase.open();
   myDatabase.createDatabase("MyDatabase");
   \endcode
 */

#include "orm4qt_global.h"
class ORMQSqlDatabase : public QSqlDatabase
{
public:
	/// <summary>
	/// driverName
	/// </summary>
	/// <param name="driverName"></param>
	ORMQSqlDatabase(QString driverName) :QSqlDatabase(driverName)
	{}
};
class ORM4QT_EXPORT ORMDatabase
{
public:
	/*!
	   Constructs ORMDatabase with given driverName. \a driverName must be supported Qt.
	 */
	ORMDatabase(QString driverName);
	/*!
	   Creates database with given \a name.

	   Returns true if database created, otherwise return false.
	 */
	bool createDatabase(QString name);
	/*!
	   Deletes database with given \a name.

	   Returns true if database deleted, otherwise return false
	 */
	bool dropDatabase(QString name);
	/*!
	   Returns last query to database.
	 */
	QString lastQuery() const;
	void setLogDeep(OrmLogger::LogDeep deep);

	void setUserName(const QString& name)
	{
		m_db->setUserName(name);
	}
	void setPassword(const QString& password)
	{
		m_db->setPassword(password);

	}
	void setHostName(const QString& host)
	{
		m_db->setHostName(host);

	}
	std::shared_ptr<ORMAbstractAdapter> getAdapter()
	{
		return m_adapter;
	}

	bool open() 
	{
		return m_db->open();
	}

	void close() {
		m_db->close();
	}

private:
	std::shared_ptr<QSqlDatabase> m_db;
	std::shared_ptr<ORMAbstractAdapter> m_adapter;
};

