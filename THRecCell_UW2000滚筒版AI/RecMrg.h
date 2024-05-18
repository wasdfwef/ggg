#ifndef _REC_MGR_INCLUDE
#define _REC_MGR_INCLUDE

#include "..\..\inc\THFrameDef.h"
#include "..\..\inc\RecCBDef.h"

extern "C"
typedef
RecDevTypeDes
(WINAPI *REC_DEV_QUERY_TYPE)(
);

extern "C"
typedef
KSTATUS
(WINAPI*REC_DEV_START)(
REC_QUERY_CELL_INFO_CB  rec_query_cell_cb,
REC_MODIFY_CELL_INFO_CB rec_modify_cell_cb,
PVOID                   pContext
);

extern "C"
typedef 
KSTATUS  
(WINAPI*REC_BUILD_RESULT)(
PREC_GRAP_FULL_RECORD   rec_reslut,
PCHAR                   pSessionName,
PCHAR                   pIniPath
);

extern "C"
typedef
KSTATUS
(WINAPI*REC_PIC_DISTINCT)(
PCHAR  pPicPath,
float* pDistinct
);


class  CRecMgr
{

#pragma pack(1)

typedef struct _DRIVER_API
{
	REC_DEV_QUERY_TYPE      DrvRecDevQueryType;
	REC_DEV_START           DrvRecDevStart;
	REC_BUILD_RESULT        DrvRecBuildResult;
	REC_PIC_DISTINCT        DrvRecPicDistinct;
		
}DRIVER_API, *PDRIVER_API;

typedef struct _REC_DRIVER
{
	LONG             bStart;
	HMODULE          hMod;
	DRIVER_API       nDrvAPI;

}REC_DRIVER, *PREC_DRIVER;

typedef struct _FULL_REC_INFO
{
	USHORT         nRecDrvCount;
	PREC_DRIVER    pRecDriver;


}FULL_REC_INFO, *PFULL_REC_INFO;


#pragma pack()


private:

	FULL_REC_INFO    m_rec_info;

	void             LoadAllCaDrv();
	BOOL             GetAPIInfo(PREC_DRIVER  pRecDriver);
	void             FreeRecDrv(PREC_DRIVER pRecDriver);
	KSTATUS          GetRDriverObjectInfo(UCHAR  nGlobalRecIndex, PREC_DRIVER *pRecDriver = NULL);
	KSTATUS          BuildRecPre(PREC_GRAP_FULL_RECORD   grap_rc, PCHAR SessionName, PCHAR  pIniPath);

public:
	CRecMgr();
   ~CRecMgr();


   ULONG             GetStartRecCount();
   ULONG             GetRecCount();
   KSTATUS           StartRecDev(UCHAR nGlobalRecIndex, PVOID pContext);
   KSTATUS           RecDevBuildResult(PREC_GRAP_FULL_RECORD   rec_reslut);
   KSTATUS           QueryPicDis(UCHAR nRecDevIndex, PCHAR pPicPath, float* pDist);
   RecDevTypeDes     RecDevQueryType(UCHAR nGlobalRecIndex);

    
    

};



#endif