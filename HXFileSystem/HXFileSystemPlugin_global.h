#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(HXFILESYSTEM_LIB)
#  define HXFILESYSTEM_EXPORT Q_DECL_EXPORT
# else
#  define HXFILESYSTEM_EXPORT Q_DECL_IMPORT
# endif
#else
# define HXFILESYSTEM_EXPORT
#endif

