#include <QtCore/QCoreApplication>
#include "../../src/ORM4Qt/ORMDatabase.h"
#include <Qdebug>
#if defined(_MSC_VER) && (_MSC_VER >= 1600)    
# pragma execution_character_set("utf-8")    
#endif
#include "../ORM_Model/ORM_Model.h"
int main(int argc, char* argv[])
{
	QCoreApplication a(argc, argv);
	auto db = ORMDatabase::addORMDatabase("QMYSQL");
	db.setUserName("root");
	db.setHostName("localhost");
	db.setPassword("123456");
	//Test open
	if (!db.open())
	{
		QString temp = db.lastError().text();
		qDebug() << "open fail" << "connect to mysql error" << temp;
		return -1;
	}
	else
	{
		auto res = db.createDatabase("Test_ORMDatabase");
		ORM_Model model;

		res = model.createTableWithRelation();
		qDebug() << "createTable:" << res;

	}
	return a.exec();
}
