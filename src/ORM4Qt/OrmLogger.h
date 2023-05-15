#pragma once

#include <QTextStream>
#include <QSqlQuery>
#include <QTime>
#include <QSqlError>

#include "orm4qt_global.h"

class ORM4QT_EXPORT OrmLogger
{
public:
    enum LogDeep
    {
        ldAll,
        ldWarningsOnly,
        ldNone
    };
    OrmLogger();
    bool exec(QSqlQuery query);
    bool exec(QSqlQuery &query, const QString &queryString);
    void setLogDeep(OrmLogger::LogDeep deep);

private:
    LogDeep m_logDeep;
};

 
