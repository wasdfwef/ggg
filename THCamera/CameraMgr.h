#ifndef  _CAMERAMGR_INCLUDE
#define  _CAMERAMGR_INCLUDE

//#include "..\..\\CommonAPI\inc\DrvAPIDef.h"
#include "..\..\inc\DrvAPIDef.h"

class   CCameraMgr
{

#pragma pack(1)

typedef struct _DRIVER_API
{
	CA_DRV_QUERY_DEV_COUNT           DrvQueryDevCounter;
	CA_DRV_START                     DrvStart;
	CA_QUERY_DEV_SN                  DrvQuerySN;
	CA_SET_PRVIEW                    DrvSetPrview;
	CA_QUERY_DEV_GAIN_RANGE          DrvQueryGainRange;
	CA_QUERY_DEV_EXPOSE_TIME_RANGE   DrvQueryETRange;
	CA_QUERY_DEV_CONFIG              DrvQueryConfig;
	CA_SET_DEV_CONFIG                DrvSetConfig;
	CA_READ_RAW_GRAP_BUFFER          DrvReadRawGrapBuffer;
	CA_CATCH_GRAP                    DrvCatchGrap;
	CA_SET_OUTPUT_CB                 DrvOutputCB;
	CA_CLOSE_ALL                     DrvCloseAll;
	CA_SET_BALANCE                   DrvSetBalance;
	CA_CATCH_BITMAP					 DrvCatchBitmap;

}DRIVER_API, *PDRIVER_API;

typedef struct _CA_DRIVER
{
	USHORT       nCameraNumber;
	HMODULE      hMod;
	DRIVER_API   nDrvAPI;
	
}CA_DRIVER, *PCA_DRIVER;

typedef  struct  _CA_DEVICE
{
	USHORT       nRelativeCaIndex;
	PCA_DRIVER   pRelativeDriver;
	LONG         bStart;

}CA_DEVICE, *PCA_DEVICE;


typedef struct _FULL_CA_INFO
{
	USHORT        nDrvCounter;
	PCA_DRIVER    pCaDriver;
	USHORT        nDevCounter;
	PCA_DEVICE    pCaDevice;


}FULL_CA_INFO, *PFULL_CA_INFO;

#pragma pack()

private:

	void                         LoadAllCaDrv();
	void                         QueryCaDevInfo();
	void                         FreeCaDriverInfo(PCA_DRIVER pCaDriver, USHORT nDrvCount);
	void                         FreeCaDrv(PCA_DRIVER  pCaDriver);
	BOOL                         GetAPIInfo(PCA_DRIVER  pCaDriver);
	KSTATUS                      GetRDriverObjectInfo(UCHAR  nGlobalCaIndex,  PUSHORT pRelativeCaIndex = NULL, PCA_DEVICE *pCaDevice = NULL);

	FULL_CA_INFO                 m_CaInfo;

public:

	CCameraMgr();
   ~CCameraMgr();

    ULONG                         QueryUseageDevCounter();
	ULONG                         QueryStartDevCounter();
	KSTATUS                       CloseAllDevice();
    KSTATUS                       StartCaDev(UCHAR nGlobalCaIndex);
	KSTATUS                       QueryCaSN(UCHAR nGlobalCaIndex, PCHAR pDevSN, int nDevSNLength);
	KSTATUS                       SetPrviewWindow(UCHAR  nGlobalCaIndex, HWND hPrvWnd, RECT nPrvRC);
	KSTATUS                       QueryGainRange(PCA_GAIN_RANGE  pGainRange);
	KSTATUS                       QueryETRange(PCA_EXPOSE_TIME_RANGE   pETRange);
	KSTATUS                       QueryConfig(PCAMERA_CONFIG_INFO    pCaCfg);
	KSTATUS                       SetConfig(PCAMERA_CONFIG_INFO    pCaCfg);
	KSTATUS                       ReadRawGrapBuffer(USHORT nGlobalCaIndex, HANDLE  *pHandle);
	KSTATUS		                  CatchGrap(USHORT  nCaIndex,PCHAR  pGrapPath,HANDLE    hRawGrapBuffer);
	KSTATUS                       SetPrivewCB(PPRVIEW_OUTPUT_CB   prv_out_cb);
	KSTATUS                       SetWBalance(USHORT nCaIndex);
	KSTATUS		                  CatchBitmap(USHORT  nCaIndex,HANDLE  *pHbitmap,HANDLE    hRawGrapBuffer);

};


#endif