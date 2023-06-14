#include "mysqladapter.h"

MySqlAdapter::MySqlAdapter(std::shared_ptr<QSqlDatabase> db) :SqlAdapter(db)
{
	fillTableTypes();
}

bool MySqlAdapter::createDatabase(const QString& name)
{
	QString sql = QString("CREATE DATABASE %1;").arg(name);
	m_DBName= name;

	bool result = exec(sql);
	if (result)
		initDB(name);
	return result;
}
QHash<QString, QString> MySqlAdapter::getTableColumns(const QString& tableName)
{
	QHash<QString, QString> databaseColumns;

	bool res = exec("SHOW COLUMNS FROM " + tableName);

	while (m_query.next())
	{
		QString columnName = m_query.value(0).toString();
		QString dataType = m_query.value(1).toString();
		databaseColumns[columnName] = dataType;
	}
	return databaseColumns;
}
bool MySqlAdapter::createTable(const QString& tableName, const QHash<QString, QString>& info)
{
	bool res = false;

	exec(QString("SHOW TABLES LIKE '%1';").arg(tableName));
	if (m_query.next())
	{
		auto databaseColumns = getTableColumns(tableName);

		QStringList matchtableError;
		matchtableError << tableName;
		QHash<QString, QString>::iterator i;
		for (auto i = info.begin(); i != info.end(); ++i)
		{
			QString columnName = i.key();
			QString dataType = m_tableTypes.value(i.value());

			if (!databaseColumns.contains(columnName))
			{
				matchtableError << "数据库没有字段" << columnName << "类型" << dataType << "\n";
			}
			else if (databaseColumns[columnName].toLower() != dataType.toLower())
			{
				matchtableError << "数据库字段" << columnName << "类型不匹配" << databaseColumns[columnName] << "--" << dataType << "\n";
			}
		}

		for (auto i = databaseColumns.begin(); i != databaseColumns.end(); ++i)
		{
			QString columnName = i.key();

			//id为每个表的自增id,_id为外键表id.不用比较
			if (columnName.toLower() == "id" || columnName.toLower().contains("_id"))
			{
				continue;
			}
			else if (!info.contains(columnName))
			{
				matchtableError << "程序中没有字段" << columnName << "类型" << "\n";
			}
			else
			{
				QString dataType = m_tableTypes.value(info[columnName]);

				if (i.value().toLower() != dataType.toLower())
				{
					matchtableError << "程序中字段" << columnName << "类型不匹配" << info[columnName] << "--" << dataType << "\n";
				}
			}
		}
		m_query.clear();
		if (matchtableError.count() > 1)
		{
			throw std::exception(qPrintable(matchtableError.join(" ")));
		}
		res = true;
	}
	else
	{
		QString sql = QString("CREATE TABLE if not exists %1(id BIGINT AUTO_INCREMENT, ")
			.arg(tableName);
		QString name;

		foreach(name, info.keys())
		{
			QString value = info.value(name);
			if (m_tableTypes.contains(value))
			{
				sql += QString("%1 %2, ")
					.arg(name)
					.arg(m_tableTypes.value(value));
			}
			else
			{
				throw QString("字段格式异常");
			}
		}
		sql += QString("PRIMARY KEY (id));");
		m_query.clear();

		res = exec(sql);
	}
	return res;
}

bool MySqlAdapter::createTableRelations(const QString& parent, ORMAbstractAdapter::Relation rel, const QString& child)
{
	QString sql = QString("select * from information_schema.TABLE_CONSTRAINTS t ") +
		QString(" where constraint_schema = '%1' and t.TABLE_NAME = '%2' and CONSTRAINT_TYPE = 'FOREIGN KEY'; ").arg(m_DBName).arg(child);
	exec(sql);
	bool res = false;
	if (m_query.next())
	{
		m_query.clear();
		res = true;
	}
	else
	{
		if (rel == ORMAbstractAdapter::Relation::HasMany)
		{

			sql = QString("ALTER TABLE %1 ADD %2_id BIGINT AFTER id, ADD FOREIGN KEY(%2_id) REFERENCES %2(id);")
				.arg(child)
				.arg(parent);
		}
		else if (rel == ORMAbstractAdapter::Relation::HasOne)
		{
			sql = QString("ALTER TABLE %1 ADD %2_id BIGINT AFTER id, ADD FOREIGN KEY(%2_id) REFERENCES %2(id),"
						  "ADD UNIQUE(%2_id);")
				.arg(child)
				.arg(parent);
		}
		res = exec(sql);
	}

	return res;
}

bool MySqlAdapter::initDB(const QString& name)
{
	bool result = false;
	QString sql = QString("SHOW DATABASES LIKE '%1';").arg(name);
	bool checkresult = exec(sql);

	bool existDB = false;
	while (m_query.next())
	{
		QString name = m_query.value(0).toString();
		if (name.toLower() == name.toLower())
		{
			existDB = true;
			result = true;
		}
	}
	if (!existDB)
	{
		sql = QString("CREATE DATABASE %1").arg(name);
		result = exec(sql);
	}

	if (result)
	{
		auto sql = QString("USE %1;").arg(name);
		result = exec(sql);
	}
	return result;
}
