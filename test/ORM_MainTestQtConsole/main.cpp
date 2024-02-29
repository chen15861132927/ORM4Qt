#include <QtCore/QCoreApplication>
#include "../../src/ORM4Qt/ORMDatabase.h"
#include <Qdebug>
#if defined(_MSC_VER) && (_MSC_VER >= 1600)    
# pragma execution_character_set("utf-8")    
#endif
#include "../ORM_Model/ORM_Model.h"
#include "../../src/ORM4Qt/ORMDatabaseFactory.h"
int main(int argc, char* argv[])
{
	QString dbName = "Test_ORMDatabase";
	QString driverName = "QMYSQL";
	QString userName = "root";
	QString hostName = "localhost";
	QString password = "root";
	QCoreApplication a(argc, argv);
	/*
	auto db = std::make_shared<ORMDatabase>(driverName);
	db->setUserName(userName);
	db->setHostName(hostName);
	db->setPassword(password);

	//Test open
	if (!db->open())
	{
		QString temp = db->getAdapter()->lastError().text();
		qDebug() << "open fail" << "connect to mysql error" << temp;

	}
	else
	{
		db->getAdapter()->initDB(dbName);
		db->getAdapter()->exec(QString("DROP DATABASE %1;").arg(dbName));

		auto res = db->createDatabase(dbName);
		ORM_Model model;
		model.setAdapter(db->getAdapter());
		res = model.createTableWithRelation();
		qDebug() << "createTable:" << res;

	}
	*/

	bool res = ORMDatabaseFactory::getInstance()->registerDatabase(dbName, hostName, userName, password);
	ORM_Model model;
	res = model.createTableWithRelation();
	return a.exec();
}
