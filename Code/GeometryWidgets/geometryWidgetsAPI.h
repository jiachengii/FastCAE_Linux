#ifndef _GEOMETRYWIDGETSEAPI_H_
#define _GEOMETRYWIDGETSEAPI_H_

#include <QtCore/QtGlobal>


#if defined(GEOMETRYWIDGETS_API)
#define GEOMETRYWIDGETSAPI Q_DECL_EXPORT
#else
#define  GEOMETRYWIDGETSAPI Q_DECL_IMPORT
#endif

#endif
