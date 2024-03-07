#include "pch.h"
#include "../../src/ORM4Qt/ORMDatabase.h"
#include "../ORM_Model/ORM_Model.h"
#include "qdebug.h"
QString dbName = "Test_ORMDatabase";
QString driverName = "QMYSQL";
QString userName = "root";
QString hostName = "localhost";
QString password = "root";


TEST(UnitTest_MySQL, t1_CreateDatabase)
{
	EXPECT_TRUE(ORMDatabaseFactory::getInstance()->registerDatabase(dbName, hostName, userName, password));
}
TEST(UnitTest_MySQL, t2_SampleTable_1_CreateTable)
{
	EXPECT_TRUE(ORMDatabaseFactory::getInstance()->registerDatabase(dbName, hostName, userName, password));
	ORMDatabaseFactory::getInstance()->dropDatabase(dbName);
	EXPECT_TRUE(ORMDatabaseFactory::getInstance()->registerDatabase(dbName, hostName, userName, password));
	auto db = ORMDatabaseFactory::getInstance()->getDefaultDatabase();


	QDateTime dateTime = QDateTime::currentDateTime();
	QString timemodelStr = "ORM_Model" + dateTime.toString("yyyyMMdd_hhmmss");

	ORM_Model model, timemodel(timemodelStr);

	EXPECT_TRUE(model.createTable());
	EXPECT_TRUE(timemodel.createTable());

	QString sqlmodel("SHOW TABLES LIKE '" + model.getMapDBTableName() + "';");
	db->getAdapter()->exec(sqlmodel);
	QSqlQuery query1 = db->getAdapter()->lastQSqlQuery();
	query1.next();
	EXPECT_EQ(query1.size(), 1);

	for (int i = 0; i < query1.size(); i++)
	{
		auto val = query1.value(i).toString();
		EXPECT_STRCASEEQ(model.getMapDBTableName().toStdString().c_str(), val.toStdString().c_str());
	}

	QString sqltimemodel("SHOW TABLES LIKE '" + timemodel.getMapDBTableName() + "';");
	db->getAdapter()->exec(sqltimemodel);
	QSqlQuery query2 = db->getAdapter()->lastQSqlQuery();
	query2.next();
	EXPECT_EQ(query2.size(), 1);

	for (int i = 0; i < query2.size(); i++)
	{
		auto val = query2.value(i).toString();
		EXPECT_STRCASEEQ(timemodel.getMapDBTableName().toStdString().c_str(), val.toStdString().c_str());
	}
}

TEST(UnitTest_MySQL, t2_SampleTable_2_Save)
{
	EXPECT_TRUE(ORMDatabaseFactory::getInstance()->registerDatabase(dbName, hostName, userName, password));
	ORMDatabaseFactory::getInstance()->dropDatabase(dbName);
	EXPECT_TRUE(ORMDatabaseFactory::getInstance()->registerDatabase(dbName, hostName, userName, password));
	auto db = ORMDatabaseFactory::getInstance()->getDefaultDatabase();


	QDateTime dateTime = QDateTime::currentDateTime();
	QString timemodelStr = "ORM_Model" + dateTime.toString("yyyyMMdd_hhmmss");

	ORM_Model model, timemodel(timemodelStr);
	QTime time = QTime::currentTime();
	EXPECT_TRUE(model.createTable());

	model.setnameBool(true);
	model.setnameBlob(QByteArray(10000, '1'));
	model.setnameChar('A');
	model.setnameDate(QDate(2024, 03, 05));
	model.setnameDatetime(QDateTime(QDate(2024, 03, 05), time));
	model.setnamedouble(0.1);
	model.setnameInt(10);
	model.setnameLonglong(1234567890);
	model.setnameString("Hello world!");
	model.setnameTime(time);
	model.setnameUint(60000);
	model.setnameUlonglong(123456789123456789);
	EXPECT_EQ(model.save(), true);


	db->getAdapter()->exec("SELECT * FROM "+ model .getMapDBTableName()+";");
	QSqlQuery query = db->getAdapter()->lastQSqlQuery();
	query.next();
	for (int i = 0; i < query.size(); i++)
	{
		if (query.record().fieldName(i) != model.getColumnNameOf_id())
			EXPECT_EQ(query.value(i), model.property(query.record().fieldName(i).toLocal8Bit().constData()));
		else
			EXPECT_EQ(query.value(i).toInt(), model.getId());
	}

	EXPECT_TRUE(timemodel.createTable());
	timemodel.setnameBool(true);
	timemodel.setnameBlob(QByteArray(10000, '1'));
	timemodel.setnameChar('A');
	timemodel.setnameDate(QDate(2013, 03, 26));
	timemodel.setnameDatetime(QDateTime(QDate(2013, 03, 26), time));
	timemodel.setnamedouble(0.001);
	timemodel.setnameInt(1000);
	timemodel.setnameLonglong(1234567890);
	timemodel.setnameString("Hello world!");
	timemodel.setnameTime(time);
	timemodel.setnameUint(60000);
	timemodel.setnameUlonglong(1234567891234567891);
	EXPECT_EQ(timemodel.save(), true);


	db->getAdapter()->exec("SELECT * FROM " + timemodel.getMapDBTableName() + ";");
	QSqlQuery query2 = db->getAdapter()->lastQSqlQuery();
	query2.next();
	for (int i = 0; i < query2.size(); i++)
	{
		if (query2.record().fieldName(i) != timemodel.getColumnNameOf_id())
			EXPECT_EQ(query2.value(i), timemodel.property(query2.record().fieldName(i).toLocal8Bit().constData()));
		else
			EXPECT_EQ(query2.value(i).toInt(), timemodel.getId());
	}
}
TEST(UnitTest_MySQL, t89_RelationTable_2_CreateTable)
{
	EXPECT_TRUE(ORMDatabaseFactory::getInstance()->registerDatabase(dbName, hostName, userName, password));
	ORMDatabaseFactory::getInstance()->dropDatabase(dbName);
	EXPECT_TRUE(ORMDatabaseFactory::getInstance()->registerDatabase(dbName, hostName, userName, password));

	ORM_Model model;
	CarDriver driver;

	EXPECT_TRUE(model.createTable());
	EXPECT_EQ(driver.createTable(), true);
}
TEST(UnitTest_MySQL, t3_SampleTable_3_AlterEmptyTable1)
{
	EXPECT_TRUE(ORMDatabaseFactory::getInstance()->registerDatabase(dbName, hostName, userName, password));
	ORMDatabaseFactory::getInstance()->dropDatabase(dbName);
	EXPECT_TRUE(ORMDatabaseFactory::getInstance()->registerDatabase(dbName, hostName, userName, password));

	auto db = ORMDatabaseFactory::getInstance()->getDefaultDatabase();

	QString oldcreateSQL = "CREATE TABLE if not exists ORM_Model(id BIGINT AUTO_INCREMENT, namedouble int, nameTime TIME, nameString TEXT, nameLonglong long, m_Priority INT, nameUlonglong BIGINT UNSIGNED, nameDatetime DATETIME, m_SafeHardware INT, nameDate DATE, nameUint INT UNSIGNED, nameChar CHAR(1), nameInt INT, PRIMARY KEY(id));";
	db->getAdapter()->exec(oldcreateSQL);
	ORM_Model model;
	ASSERT_ANY_THROW(EXPECT_TRUE(model.createTable()));
	ASSERT_NO_THROW(EXPECT_TRUE(model.alterTable()));
}

TEST(UnitTest_MySQL, t3_SampleTable_3_AlterEmptyTable2)
{
	EXPECT_TRUE(ORMDatabaseFactory::getInstance()->registerDatabase(dbName, hostName, userName, password));
	ORMDatabaseFactory::getInstance()->dropDatabase(dbName);
	EXPECT_TRUE(ORMDatabaseFactory::getInstance()->registerDatabase(dbName, hostName, userName, password));

	auto db = ORMDatabaseFactory::getInstance()->getDefaultDatabase();

	ORM_Model_ToBeAlert model_ToBeAlert;
	ASSERT_NO_THROW(EXPECT_TRUE(model_ToBeAlert.createTable()));

	ORM_Model model(model_ToBeAlert.getMapDBTableName());
	ASSERT_ANY_THROW(EXPECT_TRUE(model.createTable()));
	ASSERT_NO_THROW(EXPECT_TRUE(model.alterTable()));
}

TEST(UnitTest_MySQL, t5_ORM_HAS_ONE)
{
	EXPECT_TRUE(ORMDatabaseFactory::getInstance()->registerDatabase(dbName, hostName, userName, password));
	ORMDatabaseFactory::getInstance()->dropDatabase(dbName);
	EXPECT_TRUE(ORMDatabaseFactory::getInstance()->registerDatabase(dbName, hostName, userName, password));


	CarDriver driver1, driver2;
	DriverLicense license;
	driver1.createTable();
	/*driver1.setMapDBTableName(driver1.metaObject()->className() + QString("TBName"));
	driver2.setMapDBTableName(driver2.metaObject()->className() + QString("TBName"));
	license.setMapDBTableName(license.metaObject()->className() + QString("TBName"));*/

	std::shared_ptr<DriverLicense> pointer;
	driver1.removeAll();
	license.removeAll();
	driver1.setName("Alex");
	driver2.setName("Paul");
	EXPECT_TRUE(driver1.save());
	EXPECT_TRUE(driver2.save());
	EXPECT_TRUE(driver1.getDriverLicense() == 0);
	QHash<QString, QVariant> info;
	info.insert("Number", 123);
	pointer = driver1.createDriverLicense(info);
	EXPECT_NE(pointer, nullptr);
	EXPECT_EQ(pointer->getNumber(), 123);
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

TEST(UnitTest_MySQL, t6_ORM_HAS_MANY)
{
	EXPECT_TRUE(ORMDatabaseFactory::getInstance()->registerDatabase(dbName, hostName, userName, password));
	ORMDatabaseFactory::getInstance()->dropDatabase(dbName);
	EXPECT_TRUE(ORMDatabaseFactory::getInstance()->registerDatabase(dbName, hostName, userName, password));



	CarDriver driver1, driver2;
	Car car1, car2, car3;
	driver1.createTable();

	/*driver1.setMapDBTableName(driver1.metaObject()->className() + QString("TBName"));
	driver2.setMapDBTableName(driver2.metaObject()->className() + QString("TBName"));
	car1.setMapDBTableName(car1.metaObject()->className() + QString("TBName"));
	car2.setMapDBTableName(car2.metaObject()->className() + QString("TBName"));
	car3.setMapDBTableName(car3.metaObject()->className() + QString("TBName"));*/

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

TEST(UnitTest_MySQL, t4_includes)
{
	EXPECT_TRUE(ORMDatabaseFactory::getInstance()->registerDatabase(dbName, hostName, userName, password));
	ORMDatabaseFactory::getInstance()->dropDatabase(dbName);
	EXPECT_TRUE(ORMDatabaseFactory::getInstance()->registerDatabase(dbName, hostName, userName, password));


	Car car1, car2, car3;
	DriverLicense license1, license2;
	CarDriver driver1, driver2;
	driver1.createTable();
	/*license1.setMapDBTableName(driver1.metaObject()->className() + QString("TBName"));
	license2.setMapDBTableName(driver2.metaObject()->className() + QString("TBName"));
	driver1.setMapDBTableName(driver1.metaObject()->className() + QString("TBName"));
	driver2.setMapDBTableName(driver2.metaObject()->className() + QString("TBName"));
	car1.setMapDBTableName(car1.metaObject()->className() + QString("TBName"));
	car2.setMapDBTableName(car2.metaObject()->className() + QString("TBName"));
	car3.setMapDBTableName(car3.metaObject()->className() + QString("TBName"));*/
	car1.removeAll();
	license1.removeAll();
	driver1.removeAll();
	driver1.setName("Peter");
	driver2.setName("Paul");
	car1.setNumber("1");
	car2.setNumber("2");
	car3.setNumber("3");
	license1.setNumber(123);
	license2.setNumber(456);
	driver1.save();
	driver2.save();
	car1.save();
	car2.save();
	car3.save();
	license1.save();
	license2.save();
	driver1.setDriverLicense(license1.getId());
	driver2.setDriverLicense(license2.getId());
	driver1.addCar(car1.getId());
	driver2.addCar(car2.getId());
	driver2.addCar(car3.getId());
	QStringList models;
	models << license1.getMapDBTableName() << car1.getMapDBTableName();
	auto list = driver1.includes(models);
	EXPECT_EQ(list.size(), 2);
	EXPECT_EQ(list.value(0)->getCarAfterIncludes().size(), 1);
	EXPECT_EQ(list.value(0)->getName(), QString("Peter"));
	EXPECT_EQ(list.value(1)->getName(), QString("Paul"));
	EXPECT_EQ(list.value(0)->getCarAfterIncludes().first()->getNumber(), QString("1"));
	EXPECT_EQ(list.value(1)->getCarAfterIncludes().size(), 2);
	EXPECT_EQ(list.value(1)->getCarAfterIncludes().first()->getNumber(), QString("2"));
	EXPECT_EQ(list.value(1)->getCarAfterIncludes().value(1)->getNumber(), QString("3"));
	EXPECT_EQ(list.first()->getDriverLicense()->getNumber(), 123);
	EXPECT_EQ(list.value(1)->getDriverLicense()->getNumber(), 456);
	car1.remove();
	car2.remove();

	auto list2 = driver1.includes(models);
	EXPECT_EQ(list2.first()->getCarAfterIncludes().size(), 0);
	EXPECT_EQ(list2.value(1)->getCarAfterIncludes().size(), 1);
	models.removeAt(1);
	auto list3 = driver1.includes(models);
	EXPECT_EQ(list3.first()->getCarAfterIncludes().size(), 0);
	EXPECT_EQ(list3.value(1)->getCarAfterIncludes().size(), 0);
	auto list4 = driver1.includes(models, ORMWhere(driver1.getColumnNameOf_id(), ORMWhere::Equals, driver1.getId()));
	EXPECT_EQ(list4.size(), 1);
	EXPECT_EQ(list4.first()->getName(), QString("Peter"));
}

TEST(UnitTest_MySQL, t7_pluck)
{
	EXPECT_TRUE(ORMDatabaseFactory::getInstance()->registerDatabase(dbName, hostName, userName, password));
	ORMDatabaseFactory::getInstance()->dropDatabase(dbName);
	EXPECT_TRUE(ORMDatabaseFactory::getInstance()->registerDatabase(dbName, hostName, userName, password));

	ORM_Model model1, model2, model3;
	model1.createTable();
	/*model1.setMapDBTableName(model1.metaObject()->className() + QString("TBName"));
	model2.setMapDBTableName(model2.metaObject()->className() + QString("TBName"));
	model3.setMapDBTableName(model3.metaObject()->className() + QString("TBName"));*/

	model1.removeAll();
	model1.setnameInt(-2);
	model1.setnameString("abc");
	model2.setnameInt(1);
	model2.setnameString("def");
	model3.setnameInt(5);
	model3.setnameString("ghi");
	model1.save();
	model2.save();
	model3.save();
	QList<QVariant> list = model1.pluck(model1.getColumnNameOf_id());
	EXPECT_EQ(list.size(), 3);
	EXPECT_EQ(list.value(0).toInt(), model1.getId());
	EXPECT_EQ(list.value(1).toInt(), model2.getId());
	EXPECT_EQ(list.value(2).toInt(), model3.getId());
	list = model1.pluck(model1.getColumnNameOf_nameInt(), ORMWhere(model1.getColumnNameOf_nameString(), ORMWhere::Equals, "abc") ||
		ORMWhere(model1.getColumnNameOf_nameString(), ORMWhere::Equals, "ghi"));
	EXPECT_EQ(list.size(), 2);
	EXPECT_EQ(list.value(0).toInt(), -2);
	EXPECT_EQ(list.value(1).toInt(), 5);
	model1.setnameString("def");
	model1.update();
	list = model1.pluck(model1.getColumnNameOf_nameString(), ORMWhere(), ORMGroupBy(model1.getColumnNameOf_nameString()));
	EXPECT_EQ(list.size(), 2);
}