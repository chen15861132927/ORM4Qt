#pragma once

#include "orm_model_global.h"
#include "../../src/ORM4Qt/ORM4Qt.h"

class ORM_MODEL_EXPORT ORM_Model : public ORMObject<ORM_Model>
{
	Q_OBJECT;

	ORM_PROPERTY(bool, nameBool, false);
	ORM_PROPERTY(int, nameInt, 0);
	ORM_PROPERTY(uint, nameUint, 0);
	ORM_PROPERTY(qlonglong, nameLonglong, 0);
	ORM_PROPERTY(qulonglong, nameUlonglong, 0);
	ORM_PROPERTY(double, namedouble, 0);
	ORM_PROPERTY(QByteArray, nameBlob, "");
	ORM_PROPERTY(QChar, nameChar, 0);
	ORM_PROPERTY(QDate, nameDate, QDate());
	ORM_PROPERTY(QTime, nameTime, QTime());
	ORM_PROPERTY(QDateTime, nameDatetime, QDateTime());
	ORM_PROPERTY(QString, nameString, "");

public:
	ORM_Model() {};
};

class ORM_MODEL_EXPORT Car : public ORMObject<Car>
{
	Q_OBJECT;
	ORM_PROPERTY(QString, Number, "");

public:
	Car() {}
};

class ORM_MODEL_EXPORT DriverLicense : public ORMObject<DriverLicense>
{
	Q_OBJECT;
	ORM_PROPERTY(int, Number, 0);

public:
	DriverLicense() {}
};

class ORM_MODEL_EXPORT CarDriver : public ORMObject<CarDriver>
{
	Q_OBJECT;
	ORM_HAS_ONE(DriverLicense);
	ORM_HAS_MANY(Car);
	ORM_PROPERTY(QString, Name, "");
	
public:
	CarDriver() {}
};