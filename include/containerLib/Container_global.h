#ifndef CONTAINER_GLOBAL_H
#define CONTAINER_GLOBAL_H

#include <QtCore/qglobal.h>

// Avoid redefinition issues
#ifdef CONTAINER_LIBRARY
    #undef CONTAINER_EXPORT
    #define CONTAINER_EXPORT Q_DECL_EXPORT
#else
    #undef CONTAINER_EXPORT
    #define CONTAINER_EXPORT Q_DECL_IMPORT
#endif

#endif // CONTAINER_GLOBAL_H
