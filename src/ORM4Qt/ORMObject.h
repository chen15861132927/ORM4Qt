﻿#pragma once

#include <QObject>
#include <QMetaProperty>
#include <QSqlRecord>
#include <QVector>
#include <QMetaProperty>
#include <QSqlRelationalTableModel>
#include "ormdatabase.h"
#include "ormwhere.h"
#include "ormgroupby.h"
#include "ormorderby.h"
#include <memory>
#include <qDebug>
#include <QThread>
#include "ORMDatabaseFactory.h"
#if defined(_MSC_VER) && (_MSC_VER >= 1600)    
# pragma execution_character_set("utf-8")    
#endif
/*!
   \class ORMObject
   \brief The ORMObject is a base class for all your models.

   For create model, you must inherits by this class.
   Example:
   \code
   Car : public ORMObject<Car>
   {
	Car();
	ORM_PROPERTY(QString, brand, "")
	ORM_PROPERTY(QDate, year, new QDate())
	ORM_PROPERTY(QString, color, "")
   }
   \endcode
 */


template<class ModelName>
class ORMObject : public QObject
{
public:
	/*!
	   Creates empty ORMObject with \a parent QObject.
	 */
	explicit ORMObject(QObject* parent = nullptr) : QObject(parent)
	{
		id = -1;
		m_hasUnsavedChanges = false;

		auto defaultdb = ORMDatabaseFactory::getInstance()->getDefaultDatabase();
		if (defaultdb != nullptr)
		{
			setAdapter(defaultdb->getAdapter());
		}
	}
	/*!
	Creates table associated with model with Relation(one or many).

	Returns true if table created, otherwise return false.
	*/
	bool createTableWithRelation()
	{
		bool res = createTableNoRelation();
		if (res)
		{
			LoadCurrentTableRelations();

			for (QString item : m_oneRelations)
			{
				bool itemres = true;
				QString methodName = QString(CreateTableWithRelationStr(item)).arg(item);
				try
				{
					QMetaObject::invokeMethod(this, qPrintable(methodName), Qt::DirectConnection, Q_RETURN_ARG(bool, itemres));
				}
				catch (std::exception ex)
				{
					itemres = false;
				}
				res = res & itemres & createTableRelation(ORMAbstractAdapter::Relation::HasOne, item);
			}

			for (QString item : m_manyRelations)
			{
				bool itemres = true;
				QString methodName = QString(CreateTableWithRelationStr(item)).arg(item);
				try
				{
					QMetaObject::invokeMethod(this, qPrintable(methodName), Qt::DirectConnection, Q_RETURN_ARG(bool, itemres));
				}
				catch (std::exception ex)
				{
					itemres = false;
				}
				res = res & itemres & createTableRelation(ORMAbstractAdapter::Relation::HasMany, item);
			}
		}

		return res;
	}

	bool alterTable()
	{
		QHash<QString, QString> info; //QHash<name, type>
		for (int i = metaObject()->propertyOffset(); i < metaObject()->propertyCount(); i++)
		{
			auto pro = metaObject()->property(i);
			if (pro.isEnumType())
			{
				info.insert(pro.name(), "int");
			}
			else
			{
				info.insert(pro.name(), pro.typeName());
			}
		}

		bool res = false;
		try
		{
			res = alertTableNoRelation();
		}
		catch (std::exception ex)
		{
			qDebug(ex.what());
			res = false;
		}

		//父表不成功,子表依然需要修改
		if (res)
		{
			LoadCurrentTableRelations();

			for (QString item : m_oneRelations)
			{
				bool itemres = true;

				QString methodName = QString("alter%1Table").arg(item);
				QMetaObject::invokeMethod(this, qPrintable(methodName), Qt::DirectConnection, Q_RETURN_ARG(bool, itemres));
				res = res & itemres & createTableRelation(ORMAbstractAdapter::Relation::HasOne, item);

			}

			for (QString item : m_manyRelations)
			{
				bool itemres = true;

				QString methodName = QString("alter%1Table").arg(item);
				QMetaObject::invokeMethod(this, qPrintable(methodName), Qt::DirectConnection, Q_RETURN_ARG(bool, itemres));
				res = res & itemres & createTableRelation(ORMAbstractAdapter::Relation::HasOne, item);

			}
		}
		return res;
	}
	/*!
	   Creates table relation \a rel to child model \a childModelName.

	   Returns true if relation created, otherwise return false.
	 */
	bool createTableRelation(ORMAbstractAdapter::Relation rel, QString childModelName) const
	{
		return m_adapter->createTableRelations(getMapDBTableName(), rel, childModelName);
	}
	/*!
	   Deletes table associated with model.

	   Returns true if table deleted, otherwise return false.
	 */
	bool dropTable() const
	{
		return m_adapter->dropTable(getMapDBTableName());
	}
	/*!
	   Sets new id.

	   It must be used carefully, because now model will be linked to another record in the table.
	 */
	void setId(qlonglong newId)
	{
		id = newId;
		m_hasUnsavedChanges = true;
	}
	/*!
	   Returns object id.
	 */
	qlonglong getId() const
	{
		return id;
	}
	bool hasUnsavedChanges()
	{
		return m_hasUnsavedChanges;
	}
	/*!
	   Creates new record in table. Read all meta-property from model that have be changed and save their into table.

	   Returns true if success, otherwise return false.
	 */
	bool save()
	{
		QHash<QString, QVariant> info;
		for (int i = 0; i < m_propertiesForUpdate.size(); i++)
			info.insert(m_propertiesForUpdate.value(i), property(qPrintable(m_propertiesForUpdate.value(i))));
		id = m_adapter->addRecord(getMapDBTableName(), info);
		if (id >= 0)
		{
			m_propertiesForUpdate.clear();
			m_hasUnsavedChanges = false;
		}
		return (id >= 0);
	}
	/*!
	   Updates existing record in table with object's \a id.

	   Returns true if success, otherwise return false.
	 */
	bool update()
	{
		if (id < 0)
			return false;
		if (m_propertiesForUpdate.isEmpty())
			return true;
		QHash<QString, QVariant> info;
		for (int i = 0; i < m_propertiesForUpdate.size(); i++)
			info.insert(m_propertiesForUpdate.value(i), property(qPrintable(m_propertiesForUpdate.value(i))));
		if (m_adapter->updateRecord(getMapDBTableName(), id, info))
		{
			m_propertiesForUpdate.clear();
			m_hasUnsavedChanges = false;
			return true;
		}
		else
			return false;
	}
	/*!
	   Finds object by \a id.

	   Returns pointer to found model or 0 if no object found.
	 */
	std::shared_ptr<ModelName> find(qlonglong id)
	{
		QList<QSqlRecord> list;
		list = m_adapter->find(getMapDBTableName(), "*", ORMWhere(getColumnNameOf_id(), ORMWhere::Equals, id).getWhereString());
		if (list.isEmpty())
			return 0;
		else
			return translateRecToObj<ModelName>(list.first(), m_adapter);
	}
	/*!
	   Finds all records in table.

	   Returns list of found objects.
	 */
	QList<std::shared_ptr<ModelName>> findAll(ORMGroupBy group = ORMGroupBy(), ORMOrderBy order = ORMOrderBy())
	{
		QList<QSqlRecord> list = m_adapter->find(getMapDBTableName(), "*", group.getGroupString() + " " + order.getOrderString());
		QList<std::shared_ptr<ModelName>> returnList;
		for (int i = 0; i < list.size(); i++)
			returnList.append(translateRecToObj<ModelName>(list.value(i), m_adapter));
		return returnList;
	}
	/*!
	   Finds first object in table.

	   Returns pointer to found model or 0 if no object found.
	 */
	std::shared_ptr<ModelName> first()
	{
		QSqlRecord record = m_adapter->first(getMapDBTableName());
		if (record.isNull(getColumnNameOf_id()))
			return 0;
		else
			return translateRecToObj<ModelName>(record, m_adapter);
	}
	/*!
	   Finds last object in table.

	   Returns pointer to found model or 0 if no object found.
	 */
	std::shared_ptr<ModelName> last()
	{
		QSqlRecord record = m_adapter->last(getMapDBTableName());
		if (record.isNull(getColumnNameOf_id()))
			return 0;
		else
			return translateRecToObj<ModelName>(record, m_adapter);
	}
	/*!
	   Finds objects by some field and value.

	   Returns list of found objects.
	 */
	QList<std::shared_ptr<ModelName>> findBy(const QString fieldName, const QVariant value, ORMGroupBy group = ORMGroupBy(), ORMOrderBy order = ORMOrderBy())
	{
		QList<QSqlRecord> list = m_adapter->find(getMapDBTableName(), "*",
												 ORMWhere(fieldName, ORMWhere::Equals, value).getWhereString() + " " +
												 group.getGroupString() + " " + order.getOrderString());
		QList<std::shared_ptr<ModelName>> returnList;
		for (int i = 0; i < list.size(); i++)
			returnList.append(translateRecToObj<ModelName>(list.value(i), m_adapter));
		return returnList;
	}
	/*!
	   Finds objects by vector of values.

	   Returns list of found objects.
	 */
	QList<std::shared_ptr<ModelName>> findBy(const QString fieldName, const QVector<QVariant>& values, ORMGroupBy group = ORMGroupBy(), ORMOrderBy order = ORMOrderBy())
	{
		QList<std::shared_ptr<ModelName>> returnList;
		if (values.isEmpty())
			return returnList;
		QString whereString = "WHERE ";
		QVariant value;
		foreach(value, values)
			whereString += QString("%1 = '%2' OR ")
			.arg(fieldName)
			.arg(value.toString());
		whereString.resize(whereString.size() - 4);
		QList<QSqlRecord> list = m_adapter->find(getMapDBTableName(), "*", whereString + " " + group.getGroupString() + " " +
												 order.getOrderString());
		for (int i = 0; i < list.size(); i++)
			returnList.append(translateRecToObj<ModelName>(list.value(i), m_adapter));
		return returnList;
	}
	/*!
	   Finds object by many fields and values.
	   \a params - QHash<fieldName, value>, of which will be searched.

	   Returns list of found objects.
	 */
	QList<std::shared_ptr<ModelName>> findBy(const QHash<QString, QVariant>& params)
	{
		QList<std::shared_ptr<ModelName>> returnList;
		if (params.isEmpty())
			return returnList;
		QString key;
		QString whereString = "WHERE ";
		foreach(key, params.keys())
			whereString += QString("%1 = '%2' OR ")
			.arg(key)
			.arg(params.value(key).toString());
		whereString.resize(whereString.size() - 4);
		QList<QSqlRecord> list = m_adapter->find(getMapDBTableName(), "*", whereString);
		for (int i = 0; i < list.size(); i++)
			returnList.append(translateRecToObj<ModelName>(list.value(i), m_adapter));
		return returnList;
	}
	/*!
	   Finds object with use WHERE expression.
	   For ORMWhere defined && and || operators, so you use next code:
	   \code
	   Model.where(ORMWhere(...) || ORMWhere(...));
	   \endcode
	   or
	   \code
	   Model.where(ORMWhere(...) && ORMWhere(...));
	   \endcode

	   Returns list of found objects.
	 */
	QList<std::shared_ptr<ModelName>> where(ORMWhere condition, ORMGroupBy group = ORMGroupBy(), ORMOrderBy order = ORMOrderBy())
	{
		QList<QSqlRecord> list = m_adapter->find(getMapDBTableName(), "*", condition.getWhereString() + " " + group.getGroupString() + " " +
												 order.getOrderString());
		QList<std::shared_ptr<ModelName>> returnList;
		for (int i = 0; i < list.size(); i++)
			returnList.append(translateRecToObj<ModelName>(list.value(i), m_adapter));
		return returnList;
	}
	/*!
	   Returns true if table is empty, otherwise return false.
	 */
	bool exists()
	{
		return !m_adapter->find(getMapDBTableName(), "*", "").isEmpty();
	}
	/*!
	   Returns true if object with given \a id exist, otherwise return false.
	 */
	bool exists(qlonglong id)
	{
		return !m_adapter->find(getMapDBTableName(), "*", ORMWhere(getColumnNameOf_id(), ORMWhere::Equals, id).getWhereString()).isEmpty();
	}
	/*!
	   Verification existence of objects with specified conditions.

	   Returns true if exist at least one object, otherwise return false.
	 */
	bool exists(ORMWhere condition)
	{
		return !m_adapter->find(getMapDBTableName(), "*", condition.getWhereString()).isEmpty();
	}
	/*!
	   Immediately updates object field in table.

	   Returns true if property updated, otherwise return false.
	 */
	bool updateProperty(QString fieldName, QVariant value)
	{
		if (id < 0)
			return false;
		QHash<QString, QVariant> info;
		info.insert(fieldName, value);
		if (m_adapter->updateRecord(getMapDBTableName(), id, info))
		{
			setProperty(qPrintable(fieldName), value);
			m_propertiesForUpdate.removeAt(m_propertiesForUpdate.indexOf(fieldName));
			return true;
		}
		else
			return false;
	}
	/*!
	   Removes table's record with object's id.

	   Returns true if success, otherwise return false.
	 */
	bool remove()
	{
		if (m_adapter->remove(getMapDBTableName(), ORMWhere(getColumnNameOf_id(), ORMWhere::Equals, id).getWhereString()))
		{
			id = -1;
			return true;
		}
		else
			return false;
	}
	/*!
	   Removes all objects, that match \a condition.

	   Returns true if success, otherwise return false.
	 */
	bool removeBy(ORMWhere condition)
	{
		return m_adapter->remove(getMapDBTableName(), condition.getWhereString());
	}
	/*!
	   Removes all records from table.

	   Returns true is success, otherwise return false.
	 */
	bool removeAll() const
	{
		return m_adapter->remove(getMapDBTableName(), "");
	}
	/*!
	   Returns number of objects in table.
	 */
	qlonglong count() const
	{
		return m_adapter->count(getMapDBTableName(), "*");
	}
	/*!
	   Returns number of not null fields with given \a fieldName column.
	 */
	qlonglong count(QString fieldName) const
	{
		return m_adapter->count(getMapDBTableName(), fieldName);
	}
	/*!
	   Returns number of objects that match \a condition.
	 */
	qlonglong count(ORMWhere condition) const
	{
		return m_adapter->countBy(getMapDBTableName(), condition.getWhereString());
	}
	/*!
	   Returns average value from given \a fieldName column that match \a condition.
	 */
	double average(QString fieldName, ORMWhere condition = ORMWhere()) const
	{
		return m_adapter->calculation(ORMAbstractAdapter::Average, getMapDBTableName(),
									  fieldName, condition.getWhereString());
	}
	/*!
	   Returns maximum value from given \a fieldName column that match \a condition.
	 */
	double maximum(QString fieldName, ORMWhere condition = ORMWhere()) const
	{
		return m_adapter->calculation(ORMAbstractAdapter::Maximum, getMapDBTableName(),
									  fieldName, condition.getWhereString());
	}
	/*!
	   Returns minimum value from given \a fieldName column that match \a condition.
	 */
	double minimum(QString fieldName, ORMWhere condition = ORMWhere()) const
	{
		return m_adapter->calculation(ORMAbstractAdapter::Minimum, getMapDBTableName(),
									  fieldName, condition.getWhereString());
	}
	/*!
	   Returns sum of values \a fieldName column that match \a condition.
	 */
	double sum(QString fieldName, ORMWhere condition = ORMWhere()) const
	{
		return m_adapter->calculation(ORMAbstractAdapter::Sum, getMapDBTableName(),
									  fieldName, condition.getWhereString());
	}
	/*!
	   Find all models that match \a condition and immediately load all relations that contains in \a list.
	   This method sovle n+1 problem, ORM execute 1 + list.size() queries. Child objects may be get with
	   get<em>ChildModelName</em>AfterInclude method.

	   Returns list of found models.
	 */
	QList<std::shared_ptr<ModelName>> includes(const QStringList& list, ORMWhere condition = ORMWhere())
	{
		QHash<QString, QList<QSqlRecord> > hash = m_adapter->includes(getMapDBTableName(), list,
																	  condition.getWhereString());
		QHash<int, int> idIndexHash;
		QList<std::shared_ptr<ModelName>> result;
		QList<QSqlRecord> currentList = hash.take(getMapDBTableName());
		QString currentModelName;
		QString fieldName = (QString("%1_") + getColumnNameOf_id()).arg(getMapDBTableName());
		for (int i = 0; i < currentList.size(); i++)
		{
			result.append(translateRecToObj<ModelName>(currentList.value(i), m_adapter));
			idIndexHash.insert(currentList.value(i).field(getColumnNameOf_id()).value().toInt(), i);
		}
		foreach(currentModelName, hash.keys())
		{
			QString methodName = QString("add%1AfterIncludes").arg(currentModelName);
			currentList = hash.take(currentModelName);
			for (int i = 0; i < currentList.size(); i++)
			{
				auto itemindex = idIndexHash.value(currentList.value(i).field(fieldName).value().toInt());
				QMetaObject::invokeMethod(result.value(itemindex).get(),
										  qPrintable(methodName), Q_ARG(QSqlRecord, currentList.value(i)));
			}
		}
		return result;
	}
	/*!
	   Select \a fieldName from object that match \a condition.

	   Returns list of found \a fieldName.
	 */
	QList<QVariant> pluck(QString fieldName, ORMWhere condition = ORMWhere(), ORMGroupBy group = ORMGroupBy())
	{
		QList<QSqlRecord> list = m_adapter->find(getMapDBTableName(), fieldName, condition.getWhereString() + " "
												 + group.getGroupString());
		QList<QVariant> result;
		for (int i = 0; i < list.size(); i++)
			result.append(list.value(i).value(0));
		return result;
	}
	QString toString()
	{
		QString str;
		str += getMapDBTableName();
		str += '\n';
		str.append("    " + getColumnNameOf_id() + " : " + QString::number(id) + '\n');
		for (int i = 1; i < metaObject()->propertyCount(); i++)
			str.append(QString("    ") + metaObject()->property(i).name() +
					   QString(" : ") + property(metaObject()->property(i).name()).toString() + '\n');
		return str;
	}
	void clearUpdateList()
	{
		m_propertiesForUpdate.clear();
		m_hasUnsavedChanges = false;
	}
	void setMapDBTableName(QString mapDBTableName)
	{
		m_isSettingTableNameBySelf = true;
		m_mapDBTableName = mapDBTableName;
	}

	const QString& getMapDBTableName()const
	{
		if (!m_isSettingTableNameBySelf)
		{
			m_mapDBTableName = metaObject()->className();
		}
		return m_mapDBTableName;
	}
	const QString& getColumnNameOf_id() const
	{
		return m_idColumnName;
	}

	void LoadCurrentTableRelations()
	{
		if (m_isGotRelations)
			return;
		for (int i = metaObject()->methodOffset(); i < metaObject()->methodCount(); i++)
		{
			QMetaMethod method = metaObject()->method(i);
			QString methodName = method.name();
			//qDebug() << methodName;
			if (method.name().startsWith(ManyRelation))
			{
				QString retVal;
				method.invokeOnGadget(this, Q_RETURN_ARG(QString, retVal));//true
				m_manyRelations.append(retVal);
			}
			if (method.name().startsWith(OneRelation))
			{
				QString retVal;
				method.invokeOnGadget(this, Q_RETURN_ARG(QString, retVal));//true
				m_oneRelations.append(retVal);
			}
		}
		m_isGotRelations = true;
	}
	void setAdapter(std::shared_ptr<ORMAbstractAdapter> adapter)
	{
		m_adapter = adapter;
	}

protected:
	qlonglong id;
	bool m_hasUnsavedChanges;
	QStringList m_propertiesForUpdate;
	template<class T>
	std::shared_ptr<T> translateRecToObj(const QSqlRecord& record, std::shared_ptr<ORMAbstractAdapter> adapter) const
	{
		std::shared_ptr<T> result = std::make_shared<T>();
		QString currentFieldName;
		for (int i = 1; i < result->metaObject()->propertyCount(); i++)
		{
			currentFieldName = QString(result->metaObject()->property(i).name());
			QMetaProperty itemProperty = result->metaObject()->property(i);
			if (record.contains(currentFieldName))
				itemProperty.write(result.get(), record.value(currentFieldName));
			else if (record.contains(currentFieldName.toLower()))
				itemProperty.write(result.get(), record.value(currentFieldName.toLower()));
		}
		result->setId(record.value(result->getColumnNameOf_id()).toInt());
		result->clearUpdateList();
		result->setAdapter(adapter);
		return result;
	}

	std::shared_ptr<ORMAbstractAdapter> getAdapter()
	{
		return m_adapter;
	}

	std::shared_ptr<ORMAbstractAdapter> m_adapter;


private:
	/*!
	   Creates table associated with model with out Relation(one or many).

	   Returns true if table created, otherwise return false.
	 */
	bool createTableNoRelation() const
	{
		QHash<QString, QString> info; //QHash<name, type>
		for (int i = 1; i < metaObject()->propertyCount(); i++)
		{
			auto columnProperty = metaObject()->property(i);
			if (columnProperty.isEnumType())
			{
				info.insert(columnProperty.name(), "int");
			}
			else
			{
				info.insert(columnProperty.name(), columnProperty.typeName());
			}
		}
		return m_adapter->createTable(getMapDBTableName(), info);
	}

	bool alertTableNoRelation()
	{
		QHash<QString, QString> info; //QHash<name, type>
		for (int i = metaObject()->propertyOffset(); i < metaObject()->propertyCount(); i++)
		{
			auto pro = metaObject()->property(i);
			if (pro.isEnumType())
			{
				info.insert(pro.name(), "int");
			}
			else
			{
				info.insert(pro.name(), pro.typeName());
			}
		}

		bool res = false;

		res = m_adapter->alterTable(getMapDBTableName(), info);

		return res;
	}
	mutable QString m_mapDBTableName = "";

	bool m_isSettingTableNameBySelf = false;

	QString m_idColumnName = idColumnName;

	bool m_isGotRelations = false;

	QList<QString> m_oneRelations;
	QList<QString> m_manyRelations;


};
