#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(HXNTFSPLUGIN_LIB)
#  define HXNTFSPLUGIN_EXPORT Q_DECL_EXPORT
# else
#  define HXNTFSPLUGIN_EXPORT Q_DECL_IMPORT
#pragma comment(lib, "../x64/Debug/HXNTFSPLUGINU.lib")
# endif
#else
# define HXNTFSPLUGIN_EXPORT
#endif
