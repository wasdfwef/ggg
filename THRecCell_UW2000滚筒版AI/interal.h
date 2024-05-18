#ifndef  _INTERAL_INCLUDE
#define  _INTERAL_INCLUDE

#include "RecHelper.h"

#define RECOGNISE_DLL_1   "UsRecognise.dll"
#define RECOGNISE_DLL_2   "urinedll.dll"

#define SUPPORT_MAX_RECDEV_COUNTER 4

#pragma pack(1)

typedef  struct _REC_CELL_EXTENSION
{
	PVOID    pInterface;

}REC_CELL_EXTENSION, *PREC_CELL_EXTENSION;



#pragma pack()


#endif