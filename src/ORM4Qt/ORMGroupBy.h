#pragma once

#include <QVector>
#include <QString>

/*!
   \class ORMGroupBy
   \brief The ORMGroupBy class use for generate \a GROUP \a BY expressions.

   You must create object and pass them \a fieldName.
 */

#include "orm4qt_global.h"

class ORM4QT_EXPORT ORMGroupBy
{
public:
    /*!
       Constructs empty ORMGroupBy object.
     */
    ORMGroupBy();
    /*!
       Constructs ORMGroupBy object with the given \a fieldName.
     */
    ORMGroupBy(QString field);
    /*!
       Returns current \a GROUP \a BY expression.
     */
    QString getGroupString() const;
    /*!
       Сombines two \a GROUP \a BY into a single expression.
     */
    ORMGroupBy& operator&& (ORMGroupBy b);
    /*!
       Assigns the b object to this object.
     */
    void operator= (ORMGroupBy b);

private:
    QString m_groupString;
};

 