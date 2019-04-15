#ifndef SDK_QS_UTILS_H
#define SDK_QS_UTILS_H

#include "vp_api.h"

#define QS_DEBUG printf //("%s %d\n",__FILE__,__LINE__);printf

extern char*
MapStatus(
    VpStatusType status);

extern void
UtilPrintEvent(
    VpEventType *pEvent);

#endif /* SDK_QS_UTILS_H */
