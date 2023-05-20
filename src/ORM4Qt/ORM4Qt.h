#pragma once
#include <memory>
#include "ormdatabase.h"
#include "ormobject.h"
#include "adapters/mysqladapter.h"
#include "adapters/postgresqladapter.h"
#include "adapters/sqliteadapter.h"
#include "ormwhere.h"
#include "ormgroupby.h"
#include "ormorderby.h"

#include "orm4qt_global.h"

class ORM4QT_EXPORT ORM4Qt
{
public:
    ORM4Qt();
};
