#ifndef HANDLE_FUNC_H_ALDSFHALDF
#define HANDLE_FUNC_H_ALDSFHALDF

#include "fmmc.h"


KSTATUS StartLC(PROGRAM_OBJECT  PrgObject, PRP prp);

KSTATUS QueryDevInfo(PROGRAM_OBJECT  PrgObject, PRP prp);

KSTATUS SetRelation(PROGRAM_OBJECT  PrgObject, PRP prp);

KSTATUS SetDevInfo(PROGRAM_OBJECT  PrgObject, PRP prp);

KSTATUS DispatchCmd(PROGRAM_OBJECT  PrgObject, PRP prp);

KSTATUS RecvFromSport(PROGRAM_OBJECT  PrgObject, PRP prp);

KSTATUS StopLC(PROGRAM_OBJECT  PrgObject, PRP prp);

KSTATUS QueryDeviceCap(PROGRAM_OBJECT  PrgObject, PRP prp);

#endif  //HANDLE_FUNC_H_ALDSFHALDF
