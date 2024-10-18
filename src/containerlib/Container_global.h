#ifndef CONTAINER_GLOBAL_H
#define CONTAINER_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(CONTAINER_LIBRARY)
#define CONTAINER_EXPORT Q_DECL_EXPORT
#else
#define CONTAINER_EXPORT Q_DECL_IMPORT
#endif

#endif // CONTAINER_GLOBAL_H
