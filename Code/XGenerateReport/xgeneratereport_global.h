#ifndef XGENERATEREPORT_GLOBAL_H
#define XGENERATEREPORT_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef XGENERATEREPORT_LIB
# define XGENERATEREPORT_EXPORT Q_DECL_EXPORT
#else
# define XGENERATEREPORT_EXPORT Q_DECL_IMPORT
#endif

#endif // XGENERATEREPORT_GLOBAL_H
