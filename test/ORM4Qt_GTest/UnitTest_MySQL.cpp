#include "pch.h"
#include "../../src/ORM4Qt/ORMDatabase.h"
#include "../ORM_Model/ORM_Model.h"
#include "qdebug.h"
QString dbName = "Test_ORMDatabase";

TEST(UnitTest_MySQL, test_createTable)
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
		db.exec(QString("DROP DATABASE %1;").arg(dbName));

		auto res = db.createDatabase(dbName);
		ORM_Model model;

		EXPECT_TRUE(model.createTable());

		CarDriver driver;
		DriverLicense license;
		Car car;
		EXPECT_EQ(driver.createTable(), true);
		EXPECT_EQ(license.createTable(), true);
		EXPECT_EQ(car.createTable(), true);
		EXPECT_EQ(driver.createTableRelation(ORMAbstractAdapter::HasOne, "DriverLicense"), true);
		EXPECT_EQ(driver.createTableRelation(ORMAbstractAdapter::HasMany, "Car"), true);
	}
}

TEST(UnitTest_MySQL, test_save)
{
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
		((MySqlAdapter*)ORMDatabase::adapter)->initDB(dbName);
		db.exec("DELETE FROM ORM_Model;");
		QTime time = QTime::currentTime();
		ORM_Model model;
		model.setnameBool(true);
		model.setnameBlob(QByteArray(10000, '1'));
		model.setnameChar('A');
		model.setnameDate(QDate(2013, 03, 26));
		model.setnameDatetime(QDateTime(QDate(2013, 03, 26), time));
		model.setnamedouble(0.1);
		model.setnameInt(10);
		model.setnameLonglong(1234567890);
		model.setnameString("Hello world!");
		model.setnameTime(time);
		model.setnameUint(60000);
		model.setnameUlonglong(123456789123456789);
		EXPECT_EQ(model.save(), true);
		QSqlQuery query = db.exec("SELECT * FROM ORM_Model;");
		query.next();
		for (int i = 0; i < query.size(); i++)
			if (query.record().fieldName(i) != "id")
				EXPECT_EQ(query.value(i), model.property(query.record().fieldName(i).toLocal8Bit().constData()));
			else
				EXPECT_EQ(query.value(i).toInt(), model.getId());
	}
}