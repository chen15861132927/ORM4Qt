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

TEST(UnitTest_MySQL, test_ORM_HAS_ONE)
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
		db.exec("DELETE FROM Car;");
		std::cout << "Warning: " << db.lastError().text().toStdString() << endl;
		db.exec("DELETE FROM DriverLicense;");

		db.exec("DELETE FROM CarDriver;");
		std::cout << "Warning: " << db.lastError().text().toStdString() << endl;

		CarDriver driver1, driver2;
		DriverLicense license;
		std::shared_ptr<DriverLicense> pointer;
		driver1.removeAll();
		license.removeAll();
		driver1.setName("Alex");
		driver2.setName("Paul");
		driver1.save();
		driver2.save();
		EXPECT_TRUE(driver1.getDriverLicense() == 0);
		QHash<QString, QVariant> info;
		info.insert("Number", 123);
		EXPECT_EQ((pointer = driver1.createDriverLicense(info))->getNumber(), 123);
		EXPECT_EQ((pointer = driver1.getDriverLicense())->getNumber(), 123);
		EXPECT_TRUE(driver2.getDriverLicense() == 0);
		license.setNumber(456);
		license.save();
		driver2.setDriverLicense(license.getId());
		EXPECT_EQ((pointer = driver2.getDriverLicense())->getNumber(), 456);
		int idDr2Lic = (pointer = driver2.getDriverLicense())->getId();
		EXPECT_EQ(license.exists(idDr2Lic), true);
		EXPECT_EQ((pointer = driver2.getDriverLicense())->remove(), true);
		EXPECT_EQ(license.exists(idDr2Lic), false);
		EXPECT_TRUE(driver2.getDriverLicense() == 0);
		EXPECT_TRUE((pointer = driver1.getDriverLicense())->getId() >= 0);
	}
}

TEST(UnitTest_MySQL, test_ORM_HAS_MANY)
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
		db.exec("DELETE FROM Car;");
		std::cout << "Warning: " << db.lastError().text().toStdString() << endl;
		db.exec("DELETE FROM DriverLicense;");

		db.exec("DELETE FROM CarDriver;");
		std::cout << "Warning: " << db.lastError().text().toStdString() << endl;

		CarDriver driver1, driver2;
		Car car1, car2, car3;
		std::shared_ptr<Car> pointer;
		driver1.removeAll();
		car1.removeAll();
		driver1.setName("Alex");
		driver2.setName("Paul");
		EXPECT_TRUE(driver1.save());
		EXPECT_TRUE(driver2.save());
		EXPECT_TRUE(driver1.getAllCar().size() == 0);
		QHash<QString, QVariant> info;
		info.insert("Number", "123");
		EXPECT_EQ((pointer = driver1.createCar(info))->getNumber(), QString("123"));
		info.clear();
		info.insert("Number", "456");
		EXPECT_EQ((pointer = driver1.createCar(info))->getNumber(), QString("456"));
		auto list = driver1.getAllCar();
		EXPECT_EQ(list.size(), 2);
		EXPECT_EQ(list.first()->getNumber(), QString("123"));
		EXPECT_EQ(list.value(1)->getNumber(), QString("456"));
		EXPECT_TRUE(driver2.getAllCar().size() == 0);
		car1.setNumber("111");
		car2.setNumber("111");
		car3.setNumber("222");
		car1.save();
		car2.save();
		car3.save();
		driver2.addCar(car1.getId());
		driver2.addCar(car2.getId());
		driver2.addCar(car3.getId());
		driver2.removeAllCar();
		EXPECT_EQ(driver2.getAllCar().size(), 0);
		EXPECT_EQ(car1.exists(), true);
		EXPECT_EQ(car2.exists(), true);
		EXPECT_EQ(car3.exists(), true);
		driver2.addCar(car1.getId());
		driver2.addCar(car2.getId());
		driver2.addCar(car3.getId());

		auto list2 = driver2.getAllCar();
		EXPECT_EQ(list2.size(), 3);
		EXPECT_EQ(list2.first()->getNumber(), QString("111"));
		EXPECT_EQ(list2.value(1)->getNumber(), QString("111"));
		EXPECT_EQ(list2.value(2)->getNumber(), QString("222"));

		/*
		*	need setsql_mode=only_full_group_by
			auto list3 = driver2.findCarWhere2(ORMWhere("Number", ORMWhere::Equals, "111") || ORMWhere("Number", ORMWhere::Equals, "222"),
											  ORMGroupBy("Number"), ORMOrderBy("Number", ORMOrderBy::Descending));
			EXPECT_EQ(list3.size(), 2);

			EXPECT_EQ(list3.first()->getNumber(), QString("222"));
			EXPECT_EQ(list3.value(1)->getNumber(), QString("111"));
			auto list4 = driver2.getAllCar(ORMGroupBy("Number"), ORMOrderBy("Number", ORMOrderBy::Descending));
			EXPECT_EQ(list4.size(), 2);
			EXPECT_EQ(list4.first()->getNumber(), QString("222"));
			EXPECT_EQ(list4.value(1)->getNumber(), QString("111"));
		*/

		EXPECT_EQ(car1.remove(), true);
		auto list5 = driver2.getAllCar();
		EXPECT_EQ(list5.size(), 2);

		EXPECT_EQ(car1.exists(car1.getId()), false);
	}
}