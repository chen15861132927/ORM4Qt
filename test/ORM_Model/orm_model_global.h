#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(ORM_MODEL_LIB)
#  define ORM_MODEL_EXPORT Q_DECL_EXPORT
# else
#  define ORM_MODEL_EXPORT Q_DECL_IMPORT
# endif
#else
# define ORM_MODEL_EXPORT
#endif
