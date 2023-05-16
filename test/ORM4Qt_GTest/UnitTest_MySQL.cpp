#include "pch.h"
#include "../../src/ORM4Qt/ORMDatabase.h"
#include "../ORM_Model/ORM_Model.h"
#include "qdebug.h"
TEST(UnitTest_MySQL, createTable)
{
	//EXPECT_EQ(1, 1);
	//EXPECT_TRUE(true);

	auto db = ORMDatabase::addORMDatabase("QMYSQL");
	db.setUserName("root");
	db.setHostName("localhost");
	db.setPassword("123456");

	//Test open
	if (!db.open())
	{
		QString temp = db.lastError().text();
		qDebug() << "open fail" << "connect to mysql error" << temp;
		EXPECT_TRUE(false);
	}
	else
	{
		QString dbName = "Test_ORMDatabase";
		db.exec(QString("DROP DATABASE %1;").arg(dbName));

		auto res = db.createDatabase(dbName);
		ORM_Model model;

		EXPECT_TRUE(model.createTable());
	}
}