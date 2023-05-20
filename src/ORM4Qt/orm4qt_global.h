#pragma once

#include <QtCore/qglobal.h>
#if defined(_MSC_VER) && (_MSC_VER >= 1600)    
# pragma execution_character_set("utf-8")    
#endif
#ifndef BUILD_STATIC
# if defined(ORM4QT_LIB)
#  define ORM4QT_EXPORT Q_DECL_EXPORT
# else
#  define ORM4QT_EXPORT Q_DECL_IMPORT
# endif
#else
# define ORM4QT_EXPORT
#endif

#define idColumnName  "id"

/*! \file
*/

/*!
   \def ORM_PROPERTY(type, name)
   Declares property of model. Macros declare private property
   and public getter and setter.
 */

#define ORM_PROPERTY(type, name, defaultvalue) \
    Q_PROPERTY(type name READ get##name WRITE set##name) \
    private: \
    type name=defaultvalue; \
    public: \
    type get##name() const \
    { \
        return name; \
    } \
    void set##name(type input_##name)  \
    { \
        name = input_##name; \
        m_hasUnsavedChanges = true; \
        if(!m_propertiesForUpdate.contains(#name)) \
            m_propertiesForUpdate.append(#name); \
    }\
    QString getColumnNameOf_##name##() const \
    { \
        return #name; \
    } \

 /*!
	\def ORM_HAS_ONE(ClassName)
	Declares HAS_ONE relation. Example:
	\code
	class DriverLicense : public ORMObject<DriverLicense>
	{
		...
	};

	class CarDriver : public ORMObject<CarDriver>
	{
		...
		ORM_HAS_ONE(DriverLicense)

	 public:
		CarDriver() {}
	};
	\endcode
	Macros generate next methods:

	<b>\a ClassName get\a ClassName() \n</b>
	Returns child \a ClassName, associated with model or 0, if no such child object.

	<b>void set\a ClassName(const \a Classname &object) \n</b>
	Assigns \a object with model. If the child exists, it is updated with new values.\n
	\b NOTE: \a object must exists in table!

	<b>\a std::shared_ptr<ClassName> create\a ClassName(const QHash<QString, QVariant> &values) \n</b>
	Creates new \a ClassName object from QHash<fieldName, value> \a values and call save() method. After assigns \a object with model
	and return pointer to it.

	<b>\a std::shared_ptr<ClassName> get<em>ClassName</em>AfterIncludes() const \n </b>
	Returns pointer to \a ClassName that be loaded by includes(..) method.

  */

#define ORM_HAS_ONE(ClassName) \
    private: \
    QSqlRecord m_##ClassName##AfterIncludes; \
    Q_INVOKABLE void add##ClassName##AfterIncludes(QSqlRecord rec) \
    { \
        m_##ClassName##AfterIncludes = rec; \
    } \
    public: \
    std::shared_ptr<ClassName> get##ClassName() \
    { \
        if(id < 0) \
            return 0; \
        QString whereString = QString("WHERE %1_"+getColumnNameOf_id()+" = %2") \
                                .arg(getMapDBTableName()) \
                                .arg(id); \
        QList<QSqlRecord> list = ORMDatabase::adapter->find(#ClassName, "*", whereString); \
        if(list.isEmpty()) \
            return 0; \
        else \
            return translateRecToObj<ClassName>(list.first()); \
    } \
    void set##ClassName(const qlonglong childId) \
    { \
        if(childId < 0 || id < 0) \
            return; \
        QHash<QString, QVariant> hash; \
        hash.insert(QString(getMapDBTableName()) + "_"+getColumnNameOf_id(), id); \
        ORMDatabase::adapter->updateRecord(#ClassName, childId, hash); \
    } \
    std::shared_ptr<ClassName> create##ClassName(QHash<QString, QVariant> &values) \
    { \
        if(id < 0) \
            return 0; \
        values.insert(QString("%1_"+getColumnNameOf_id()).arg(getMapDBTableName()), id); \
        int childId = ORMDatabase::adapter->addRecord(#ClassName, values); \
        return translateRecToObj<ClassName>(ORMDatabase::adapter->find(#ClassName, "*","WHERE "+getColumnNameOf_id()+" = " + QString::number(childId)).first()); \
    } \
    std::shared_ptr<ClassName> get##ClassName##AfterIncludes() const\
    { \
        return translateRecToObj<ClassName>(m_##ClassName##AfterIncludes); \
    } \
    bool remove##ClassName(qlonglong id) \
    { \
        return ORMDatabase::adapter->setNull(#ClassName, QString("%1_"+getColumnNameOf_id()).arg(getMapDBTableName()), \
            QString("WHERE "+getColumnNameOf_id()+" = '%1'") \
                .arg(id)); \
    }

  /*!
	 \def ORM_HAS_MANY(ClassName)
	 Declares HAS_MANY relation. Example:
	 \code
	 class Car : public ORMObject<Car>
	 {
		 ...
	 };

	 class CarDriver : public ORMObject<CarDriver>
	 {
		 ...
		 ORM_HAS_ONE(DriverLicense)
		 ORM_HAS_MANY(Car)

	  public:
		 CarDriver() {}
	 };
	 \endcode
	 Macros generate next methods:

	 <b> QList<\a std::shared_ptr<ClassName>> getAll\a ClassName() \n</b>
	 Returns all \a ClassName, associated with model or empty QList, if no such child object.

	 <b>void add\a ClassName(const \a Classname &object) \n</b>
	 Assigns \a object with model.\n
	 \b NOTE: \a object must exists in table!

	 <b>\a std::shared_ptr<ClassName> create\a ClassName(const QHash<QString, QVariant> &values) \n</b>
	 Creates new \a ClassName object from QHash<fieldName, value> \a values and call save() method. After assigns \a object with model
	 and return pointer to it.

	 <b>QList<\a std::shared_ptr<ClassName>> find<em>ClassName</em>Where(ORMWhere where, ORMGroupBy group = ORMGroupBy(), ORMOrderBy order = ORMOrderBy()) \n</b>
	 Find child objects appropriate given \a where and returns them in accordance with \a group and \a order.

	 <b>void get<em>ClassName</em>AfterIncludes() const \n </b>
	 Returns list of \a ClassName that be loaded by includes(..) method.

   */

#define ORM_HAS_MANY(ClassName) \
    private: \
    QList<QSqlRecord> m_##ClassName##AfterIncludes; \
    Q_INVOKABLE void add##ClassName##AfterIncludes(QSqlRecord rec) \
    { \
        m_##ClassName##AfterIncludes.append(rec); \
    } \
    public: \
    QList<std::shared_ptr<ClassName>> getAll##ClassName(ORMGroupBy group = ORMGroupBy(), ORMOrderBy order = ORMOrderBy()) \
    { \
        QList<std::shared_ptr<ClassName>> result; \
        if(id < 0) \
            return result; \
        QString whereString = QString("WHERE %1_"+ QString(idColumnName) +" = %2") \
                                .arg(getMapDBTableName()) \
                                .arg(id); \
        QList<QSqlRecord> list = ORMDatabase::adapter->find( #ClassName, "*", whereString + " " + group.getGroupString() + \
                                                            " " + order.getOrderString()); \
        if(!list.isEmpty()) \
            for(int i = 0; i < list.size(); i++) \
                result.append(translateRecToObj<ClassName>(list.value(i))); \
        return result; \
    } \
    void add##ClassName(const qlonglong childId) \
    { \
        if(childId < 0 || id < 0) \
            return; \
        QHash<QString, QVariant> hash; \
        hash.insert(QString("%1_"+getColumnNameOf_id()).arg(getMapDBTableName()), QString::number(id)); \
        ORMDatabase::adapter->updateRecord(#ClassName, childId, hash); \
    } \
    std::shared_ptr<ClassName> create##ClassName(QHash<QString, QVariant> &values) \
    { \
        if(id < 0) \
            return 0; \
        values.insert(QString("%1_"+getColumnNameOf_id()).arg(getMapDBTableName()), QString::number(id)); \
        int childId = ORMDatabase::adapter->addRecord(#ClassName, values); \
        return translateRecToObj<ClassName>(ORMDatabase::adapter->find(#ClassName, "*", "WHERE "+getColumnNameOf_id()+" = " + QString::number(childId)).first()); \
    } \
    QList<std::shared_ptr<ClassName>> find##ClassName##Where(ORMWhere where, ORMGroupBy group = ORMGroupBy(), ORMOrderBy order = ORMOrderBy()) \
    { \
        QList<std::shared_ptr<ClassName>> result; \
        if(id < 0) \
            return result; \
        QString whereString = QString("WHERE %1_"+getColumnNameOf_id()+" = %2") \
                                .arg(getMapDBTableName()) \
                                .arg(id); \
        whereString += " AND " + where.getWhereString().remove(0, 6); \
        QList<QSqlRecord> list = ORMDatabase::adapter->find( #ClassName, "*",whereString + " " + group.getGroupString() + \
                                                            " " + order.getOrderString()); \
        if(!list.isEmpty()) \
            for(int i = 0; i < list.size(); i++) \
                result.append(translateRecToObj<ClassName>(list.value(i))); \
        return result; \
    } \
    QList<std::shared_ptr<ClassName>> get##ClassName##AfterIncludes() const \
    { \
        QList<std::shared_ptr<ClassName>> list; \
        for(int i = 0; i < m_##ClassName##AfterIncludes.size(); i++) \
            list.append(translateRecToObj<ClassName>(m_##ClassName##AfterIncludes.value(i))); \
        return list; \
    } \
    bool remove##ClassName(qlonglong id) \
    { \
        return ORMDatabase::adapter->setNull(#ClassName, QString("%1_"+getColumnNameOf_id()).arg(getMapDBTableName()), \
            QString("WHERE "+getColumnNameOf_id()+" = '%1'") \
                .arg(id)); \
    } \
    bool removeAll##ClassName() \
    { \
        return ORMDatabase::adapter->setNull(#ClassName, QString("%1_"+getColumnNameOf_id()).arg(getMapDBTableName()), \
            QString("WHERE %1_"+getColumnNameOf_id()+" = '%2'") \
                .arg(getMapDBTableName()) \
                .arg(id)); \
    }
