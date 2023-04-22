#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(ORM4QT_LIB)
#  define ORM4QT_EXPORT Q_DECL_EXPORT
# else
#  define ORM4QT_EXPORT Q_DECL_IMPORT
# endif
#else
# define ORM4QT_EXPORT
#endif
