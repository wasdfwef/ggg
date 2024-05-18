#ifndef   _GRAPPROC_INCLUDE
#define   _GRAPPROC_INCLUDE

#include "\CommonAPI\ca_inc\HVDAILT.h"
#include "\CommonAPI\ca_inc\Raw2Rgb.h"

class    CGrapProc
{
public:
	static   BOOLEAN   PreFrameData(int s_cx, int s_cy, int z_cx, int z_cy, PCHAR  pRawBuffer, PVOID pMatBuffer, HV_BAYER_LAYOUT nLayout);
};





#endif