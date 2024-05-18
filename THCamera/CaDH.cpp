#include "StdAfx.h"
#include "CaDH.h"
#include <SHLWAPI.H>
#include <stdio.h>
#include <malloc.h>
#include "GrapProc.h"
#include "\CommonAPI\ca_inc\GrapIFace.h"
#include "\CommonAPI\inc\THInterface.h"
#include "DbgMsg.h"
#include "\CommonAPI\ca_inc\HVUtil.h"

#pragma comment(lib, "\\CommonApi\\lib\\Hvdailt.lib")
#pragma comment(lib, "\\CommonApi\\lib\\HVUtil.lib")




#define   MAKE_CONENT


#define   DH_RELATIVE_PATH  "Config"
#define   DH_CFG_FILE_NAME  "CaDHCfg.ini"
#define   CA_DEFAULT_CFG_NAME "CameraDefCfg.ini"

#define   DH_SECTION   "DHCamera"

#define   DH_HIGH_WIDTH   1280
#define   DH_HIGH_HEIGHT  1024

#define  RECT_WIDTH(x) (x.right - x.left)
#define  RECT_HEIGHT(x) (x.bottom - x.top)




CCADH::CCADH()
{
	m_hkv = NULL;
	ZeroMemory(&m_CfgInfo, sizeof(m_CfgInfo));
	ZeroMemory(&m_PrviewRC, sizeof(m_PrviewRC));
	m_hThread    = NULL;
	m_DisplayWnd = NULL;
	bSetPrviewMode = FALSE;
	GetModuleFileName(NULL, m_CfgPath, MAX_PATH);
	PathRemoveFileSpec(m_CfgPath);
	PathAppend(m_CfgPath, DH_RELATIVE_PATH);
	PathAppend(m_CfgPath, DH_CFG_FILE_NAME);
	m_RawBuffer                 = NULL;
	m_Rentry    = m_hPauseEvent = NULL;
	m_ClosePrviewThread         = 0;
	m_PickRawBuffer             = m_PrvRawBuffer = NULL;
	bSupExposeTime              = FALSE;
	bSupRGBChannel              = FALSE;
	m_MinExpTime                = 0;
	m_MaxExpTime                = 0;
	m_MinGain                   = 0;
	m_MaxGain                   = 0;
	m_Layout                    = BAYER_GR;
}

CCADH::~CCADH()
{
	DBG_MSG("m_hkv close = %x\n", m_hkv);
	
	if( m_hkv )
	{
		HVStopSnap(m_hkv);
		HVCloseSnap(m_hkv);
		EndHVDevice(m_hkv);
	}

	if( m_hThread )
	{
		InterlockedExchangeAdd(&m_ClosePrviewThread, 1);
		SetEvent(m_hPauseEvent);
		WaitForSingleObject(m_hThread, INFINITE);
		CloseHandle(m_hThread);
		CloseHandle(m_hPauseEvent);
	}

	if( m_RawBuffer )
		VirtualFree(m_RawBuffer, 0, MEM_RELEASE);

	if( m_Rentry )
		CloseHandle(m_Rentry);

	if( m_PickRawBuffer )
		delete m_PickRawBuffer;

	if( m_PrvRawBuffer )
		delete m_PrvRawBuffer;
	

	DeleteCriticalSection(&cs);

}

static
int 
DHPrviewThread(
CCADH  *ca_dh
)
{
	ca_dh->Preview();

	return 1;
}

int 
CALLBACK 
DHSnapThreadCallback(
HV_SNAP_INFO *pInfo
)
{

	CCADH  *ca_dh = (CCADH *)pInfo->pParam;
	
	
	ca_dh->CopyPickRawBuffer();
	ca_dh->CopyPrvRawBuffer();

	return 1;
	
}


BOOLEAN  CCADH::QuerySupRGBChannel()
{
	HVSTATUS   status;
	HVAPI_CONTROL_PARAMETER  p;
	HV_RES_QUERY_ADV_FEATURE_PRESENCE ResPres;

	ZeroMemory(&p, sizeof(p));
	ZeroMemory(&ResPres, sizeof(ResPres));
	p.code = ORD_QUERY_ADV_FEATURE_PRESENCE;
	p.pOutBuf = &ResPres;
	p.dwOutBufSize = sizeof(ResPres);
	status = HVCommand(m_hkv, CMD_HVAPI_CONTROL, &p, NULL);
	if( status != STATUS_OK )
	{
		DBG_MSG("QuerySupRGBChannel->HVCommand fail...\n");
		return FALSE;
	}

	return (BOOLEAN)!ResPres.PixelsSeparateGain;

}

BOOLEAN   CCADH::QuerySupExposeTime()
{
	HVSTATUS                               status;
	HVAPI_CONTROL_PARAMETER                p;
	HV_RES_QUERY_ADV_FEATURE_PRESENCE      ResAdvFeaturePres = {0};

	ZeroMemory(&p, sizeof(p));


	p.code         = ORD_QUERY_ADV_FEATURE_PRESENCE;
	p.pOutBuf      = &ResAdvFeaturePres;
	p.dwOutBufSize = sizeof(ResAdvFeaturePres);
	status = HVCommand(m_hkv, CMD_HVAPI_CONTROL, &p, 0);
	if( status != STATUS_OK )
		return FALSE;

	return (BOOLEAN)ResAdvFeaturePres.ShutterUnit;
	
}

BOOLEAN   CCADH::QueryExposeTimeRange(int &nMinExpTime, int &nMaxExpTime)
{
	HVSTATUS                                  status;
	HVAPI_CONTROL_PARAMETER                   p;
	HV_ARG_QUERY_FEATURE_DESCRIPTOR           ArgDes;
	HV_RES_QUERY_FEATURE_DESCRIPTOR           ResDes;

	ZeroMemory(&ArgDes, sizeof(ArgDes));
	ZeroMemory(&ResDes, sizeof(ResDes));
	ZeroMemory(&p, sizeof(p));

	ArgDes.FeatureId = FEATURE_ID_SHUTTER;
	p.code = ORD_QUERY_FEATURE_DESCRIPTOR;
	p.pInBuf = &ArgDes;
	p.dwInBufSize = sizeof(ArgDes);
	p.pOutBuf = &ResDes;
	p.dwOutBufSize = sizeof(ResDes);
	status = HVCommand(m_hkv, CMD_HVAPI_CONTROL, &p, NULL);
	if( status == STATUS_OK )
	{
		nMinExpTime = ResDes.Scalar.MinValue;
		nMaxExpTime = ResDes.Scalar.MaxValue;
	}
	else
		nMaxExpTime = nMinExpTime = 0;

	return (status==STATUS_OK);
}

void   CCADH::CopyPickRawBuffer()
{
	m_PickRawBuffer->CopySynBuffer(m_RawBuffer, DH_HIGH_HEIGHT * DH_HIGH_WIDTH);
}

void   CCADH::CopyPrvRawBuffer()
{
	m_PrvRawBuffer->CopySynBuffer(m_RawBuffer,  DH_HIGH_HEIGHT * DH_HIGH_WIDTH);
}


void   CCADH::SetPacketLong(ULONG nPacketSize)
{
	HVSTATUS            status;
	HV_CMD_PACKET_SIZE cmdPacketSize = {0};

	cmdPacketSize.dwSize = nPacketSize;//设置包长大小，例如3200 Byte

	status = HVCommand(m_hkv,CMD_SET_BYTE_PER_PACKET,&cmdPacketSize,0);

	if( status != STATUS_SUCCESS )
		DBG_MSG("SetPacketLong Fail...\n");
		


}

#define R_CHANNEL_MASK 0X1
#define G_CHANNEL_MASK 0X2
#define B_CHANNEL_MASK 0X4

void   CCADH::ReadDefaultCfgFile()
{
	char       DefaultCaCfg[MAX_PATH], SessionName[MAX_PATH];
	ULONG      hv_type, nSize = sizeof(ULONG), rgb_mask;
	HVSTATUS   status;
	
	strcpy(DefaultCaCfg, m_CfgPath);
	PathRemoveFileSpec(DefaultCaCfg);
	PathAppend(DefaultCaCfg, CA_DEFAULT_CFG_NAME);

	status = HVGetDeviceInfo(m_hkv, DESC_DEVICE_TYPE, &hv_type, (int*)&nSize);
	if( status != STATUS_OK )
		return;

	DBG_MSG("HVGetDeviceInfo->>>hv_type = %u, status = %x\n", hv_type, status);

	if( hv_type == SV1421FCTYPE )
		strcpy(SessionName, "SV1421FC");
	else
		return;

	rgb_mask = GetPrivateProfileInt(SessionName, "RGBCtrl", 0, DefaultCaCfg);
	if( rgb_mask )
	{
		int r,g,b;

		r = GetPrivateProfileInt(SessionName, "r", 0, DefaultCaCfg);
		g = GetPrivateProfileInt(SessionName, "g", 0, DefaultCaCfg);
		b = GetPrivateProfileInt(SessionName, "b", 0, DefaultCaCfg);
		DBG_MSG("r = %u, b = %u, rgb_mask = %u\n",r,b, rgb_mask);


		if( rgb_mask & R_CHANNEL_MASK )
		{
			//HVAGCControl(m_hkv, AGC_GAIN_RED_CHANNEL, r);

			

			DBG_MSG("r set...status = %x\n", HVAGCControl(m_hkv, RED_CHANNEL, r));
		}
		if( rgb_mask & G_CHANNEL_MASK )
		{
			HVAGCControl(m_hkv, AGC_GAIN_GREEN_CHANNEL1, g);
			HVAGCControl(m_hkv, AGC_GAIN_GREEN_CHANNEL2, g);
		}
		if( rgb_mask & B_CHANNEL_MASK )
		{
			//HVAGCControl(m_hkv, AGC_GAIN_BLUE_CHANNEL, b);
			//HVAGCControl(m_hkv, BLUE_CHANNEL, b);
			DBG_MSG("b set..status = %x\n", HVAGCControl(m_hkv, BLUE_CHANNEL, b));
		}

		DBG_MSG("ReadDefaultCfgFile....r = %u\n", r);

	}



}

KSTATUS   CCADH::Create(PINIT_CAMERA_PARAMETERS   pParInit)
{
	char     Index_Str[20];
	PBYTE    ppBuf[1];
	DWORD    dwThreadID;
	HVSTATUS status;

	PathRemoveExtension(m_CfgPath);
	sprintf(Index_Str, "%u", pParInit->nHDevice);
	strcat(m_CfgPath, Index_Str);
	PathAddExtension(m_CfgPath, ".ini");
	ReadConfig(m_CfgPath);
	m_CfgInfo.cHeader.nDHIndex = pParInit->nHDevice;
	
	InitializeCriticalSection(&cs);
	
	status = BeginHVDevice(pParInit->nHDevice, &m_hkv);
	if( HV_SUCCESS(status) == FALSE )
	{
		DBG_MSG("CCADH::Create->BeginHVDevice Fail...err = %x\n", status);
		return STATUS_OPEN_CAMERA_FAIL;
	}

	HVSetResolution(m_hkv, RES_MODE0); //高清
	HVSetSnapMode(m_hkv, CONTINUATION);
	HVSetSnapSpeed(m_hkv, HIGH_SPEED);
	HVADCControl(m_hkv, ADC_BITS, ADC_LEVEL3);
	HVSetOutputWindow(m_hkv, 0, 0, DH_HIGH_WIDTH, DH_HIGH_HEIGHT);

	SetPacketLong(2000);


	m_RawBuffer = (PCHAR)VirtualAlloc(NULL, DH_HIGH_WIDTH * DH_HIGH_HEIGHT, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	if( m_RawBuffer == NULL )
		return STATUS_ALLOC_MEMORY_ERROR;

	m_PickRawBuffer = new CSynBuffer;
	m_PrvRawBuffer  = new CSynBuffer;
	if( m_PickRawBuffer == NULL || m_PrvRawBuffer == NULL )
		return STATUS_ALLOC_MEMORY_ERROR;

	if( m_PickRawBuffer->Create() == FALSE || m_PrvRawBuffer->Create() == FALSE )
		return STATUS_CREATE_EVENT_FAIL;


	m_hPauseEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if( m_hPauseEvent == NULL )
		return STATUS_CREATE_EVENT_FAIL;

	m_Rentry     = CreateEvent(NULL,  FALSE, TRUE, NULL);
	if( m_Rentry == NULL )
		return STATUS_CREATE_EVENT_FAIL;

	if( HV_SUCCESS(HVOpenSnap(m_hkv, DHSnapThreadCallback, this)) == FALSE )
	     return STATUS_UNKNOW_ERROR;

	
	ppBuf[0] = (PBYTE)m_RawBuffer;

	if( HV_SUCCESS(HVStartSnap(m_hkv, ppBuf, 1)) == FALSE )
		return STATUS_UNKNOW_ERROR;
	
	bSupExposeTime = QuerySupExposeTime();
	if( bSupExposeTime )
	{
		if( HVAECControl(m_hkv, AEC_SHUTTER_UNIT, 1) == STATUS_OK ) //设置快门时间为ms
		{
			if( QueryExposeTimeRange(m_MinExpTime, m_MaxExpTime) == FALSE )
				bSupExposeTime = FALSE;
		}
	}

	m_Layout  = HVGetDeviceInfo_Bayer_Layout();
	QueryGainRange(m_MinGain, m_MaxGain);
	bSupRGBChannel = QuerySupRGBChannel();
	
//	if( bSupRGBChannel == FALSE )
//		ReadDefaultCfgFile();

	SetGain();
	SetExposureTime();

	m_hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)DHPrviewThread, this, 0, &dwThreadID);
	if( m_hThread == NULL )
		return STATUS_CREATE_THREAD_FAIL;

	DBG_MSG("DH_CA->Create Success...\n");

	return STATUS_SUCCESS;
}

void  CCADH::ReadConfig(PCHAR  pCfgPath)
{
	m_CfgInfo.dGrapWidth      = (int) GetPrivateProfileInt(DH_SECTION, "Width",      640, pCfgPath);
	m_CfgInfo.dGrapHeight     = (int) GetPrivateProfileInt(DH_SECTION, "Height",     480, pCfgPath);
	m_CfgInfo.dGain           = (int) GetPrivateProfileInt(DH_SECTION, "Gain",       2,   pCfgPath);
	m_CfgInfo.dTime           = (int) GetPrivateProfileInt(DH_SECTION, "Time",       34,  pCfgPath);
	m_CfgInfo.dGamma          = (int) GetPrivateProfileInt(DH_SECTION, "Gamma",      50,  pCfgPath);
}

void  CCADH::SetGain()
{
/*	if( bSupRGBChannel )
	{
		for( int i = 0 ; i < 4 ; i ++ )
			HVAGCControl(m_hkv, AGC_GAIN_RED_CHANNEL + i, m_CfgInfo.dGain);
	}*/

	HVAGCControl(m_hkv, AGC_GAIN, m_CfgInfo.dGain);

}

void  CCADH::SetExposureTime()
{
	HVSTATUS  status;
	if( bSupExposeTime )
	{
		//status = HVAECControl(m_hkv, AEC_EXPOSURE_TIME , m_CfgInfo.dTime);
		status = HVAECControl(m_hkv, AEC_SHUTTER_SPEED , m_CfgInfo.dTime);
	}

}

KSTATUS   CCADH::SetParameters(PCAMERA_CONFIG_INFO  cfg_info)
{
	char   str[40] = "";

	if( m_hkv == NULL )
		return STATUS_UNINIT_STATUS;

	if( cfg_info->dGrapWidth == 0 || cfg_info->dGrapHeight == 0 )
		return STATUS_INVALID_PARAMETERS;


	if( cfg_info->dTime != m_CfgInfo.dTime )
	{
		sprintf(str,"%d",cfg_info->dTime);
		WritePrivateProfileString(DH_SECTION, "Time", str, m_CfgPath);
		m_CfgInfo.dTime = cfg_info->dTime;

		DBG_MSG("SetParameters->OrgExposeTime = %u, DesExposeTime = %u, bSupExposeTime=%u\n", m_CfgInfo.dTime, cfg_info->dTime, bSupExposeTime);
		
		SetExposureTime();
	}



	if( cfg_info->dGain != m_CfgInfo.dGain )
	{
		sprintf(str,"%d",cfg_info->dGain);
		WritePrivateProfileString(DH_SECTION, "Gain", str, m_CfgPath);
		m_CfgInfo.dGain = cfg_info->dGain;
		SetGain();
	}

	if( cfg_info->dGamma != m_CfgInfo.dGamma )
	{
		sprintf(str,"%d",cfg_info->dGamma);
		WritePrivateProfileString(DH_SECTION, "Gamma", str, m_CfgPath);
		m_CfgInfo.dGamma = cfg_info->dGamma;
	}


	EnterCriticalSection(&cs);
	if( cfg_info->dGrapHeight != m_CfgInfo.dGrapHeight )
	{
		m_CfgInfo.dGrapWidth  = cfg_info->dGrapWidth;
		sprintf(str,"%d",cfg_info->dGrapHeight);
		WritePrivateProfileString(DH_SECTION, "Height", str, m_CfgPath);
	}

	if( cfg_info->dGrapWidth != m_CfgInfo.dGrapWidth )
	{
		m_CfgInfo.dGrapHeight = cfg_info->dGrapHeight;
		sprintf(str,"%d",cfg_info->dGrapWidth);
		WritePrivateProfileString(DH_SECTION, "Width", str, m_CfgPath);
	}

    LeaveCriticalSection(&cs);

	return STATUS_SUCCESS;
}

#define  STOP_PRVIEW_RETRY 3


KSTATUS    CCADH::SetPrview(PCAMERA_PRVIEW_CONFIG  pPrvConfig)
{
	KSTATUS   status = STATUS_INVALID_PARAMETERS;

	EnterCriticalSection(&cs);

	if( m_DisplayWnd )
	{
		ResetEvent(m_hPauseEvent);
		Sleep(150);
	}

	
	if( pPrvConfig->cPrviewWnd == NULL ) //关闭预览
	{
		bSetPrviewMode = FALSE;
		status = STATUS_SUCCESS;
		ZeroMemory(&m_PrviewRC, sizeof(m_PrviewRC));
		m_DisplayWnd = NULL;
		ResetEvent(m_hPauseEvent);
		DBG_MSG("关闭预览...\n");
		goto set_exit;
	}

	status = STATUS_SUCCESS;
	m_DisplayWnd = pPrvConfig->cPrviewWnd;
	CopyRect(&m_PrviewRC, &pPrvConfig->cPrviewRC);
	bSetPrviewMode = TRUE;
	SetEvent(m_hPauseEvent);
	DBG_MSG("启动预览...\n");

set_exit:

	LeaveCriticalSection(&cs);

	return status;
  
}


BOOLEAN    CCADH::AllocResource(int cx, int cy, PCHAR &pRawBuffer, PVOID &pMatBuffer)
{
	pRawBuffer = (PCHAR)VirtualAlloc(NULL, DH_HIGH_WIDTH * DH_HIGH_HEIGHT, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	if( pRawBuffer == NULL )
		return FALSE;
	
	pMatBuffer = GrapInterface_CreateMatBuffer(cx, cy, 3);
	if( pMatBuffer == NULL )
	{
		VirtualFree(pRawBuffer, 0, MEM_RELEASE);
		pRawBuffer = NULL;
		return FALSE;
	}

	return TRUE;
}



void  CCADH::Preview()
{
	int         cx = 0, cy = 0, i;
	HDC         hDC = NULL;
	HWND        hWnd;
	BOOL        bRet;
	PBYTE       ppbuf[1];
	PCHAR       m_ZoomBuffer;
	PCHAR       pRawBuffer = NULL;
	RECT        rc;
	PVOID       pMatBuffer = NULL;
	BITMAPINFO *pBitmapInfo =NULL;

	pRawBuffer = (PCHAR)VirtualAlloc(NULL, DH_HIGH_HEIGHT * DH_HIGH_WIDTH, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	if( pRawBuffer == NULL )
		return;

	pBitmapInfo =  (BITMAPINFO *)malloc(sizeof(BITMAPINFO) + 256 * sizeof(RGBQUAD));
	if( pBitmapInfo == NULL )
		goto thread_exit;


	pBitmapInfo->bmiHeader.biSize		        = sizeof(BITMAPINFOHEADER);
	pBitmapInfo->bmiHeader.biWidth	  	        = cx;
	pBitmapInfo->bmiHeader.biHeight		        = cy;
	pBitmapInfo->bmiHeader.biPlanes		        = 1;
	pBitmapInfo->bmiHeader.biBitCount		    = 24;
	pBitmapInfo->bmiHeader.biCompression	    = BI_RGB;
	pBitmapInfo->bmiHeader.biSizeImage	        = 0;
	pBitmapInfo->bmiHeader.biXPelsPerMeter      = 0;
	pBitmapInfo->bmiHeader.biYPelsPerMeter      = 0;
	pBitmapInfo->bmiHeader.biClrUsed		    = 0;
	pBitmapInfo->bmiHeader.biClrImportant	    = 0;
	
	for (i = 0; i < 256; i++)
	{
		pBitmapInfo->bmiColors[i].rgbBlue		= (BYTE)i;
		pBitmapInfo->bmiColors[i].rgbGreen	    = (BYTE)i;
		pBitmapInfo->bmiColors[i].rgbRed		= (BYTE)i;
		pBitmapInfo->bmiColors[i].rgbReserved	= 0;	
	}

	ppbuf[0] = (PUCHAR)pRawBuffer;


	while( TRUE )
	{
		WaitForSingleObject(m_hPauseEvent, INFINITE);
		if( InterlockedExchangeAdd(&m_ClosePrviewThread, 0) )
		{
			DBG_MSG("预览线程:退出...\n");
			break;
		}

		//Sleep(200);

		if( m_PrvRawBuffer->SetSynBufferAndWait(pRawBuffer) == FALSE )
			continue;

		EnterCriticalSection(&cs);
		CopyRect(&rc, &m_PrviewRC);
		hWnd = m_DisplayWnd;
		LeaveCriticalSection(&cs);

		if( RECT_WIDTH(rc) != cx ||  RECT_HEIGHT(rc) != cy )
		{
			cx = RECT_WIDTH(rc);
			cy = RECT_HEIGHT(rc);

			if( cx == 0 || cy == 0 )
			{
				cx = cy = 0;
				if( pMatBuffer )
					GrapInterface_FreeMatBuffer(pMatBuffer);
				pMatBuffer = NULL;
				continue;
			}

			if( pMatBuffer )
				GrapInterface_FreeMatBuffer(pMatBuffer);

			pMatBuffer = GrapInterface_CreateMatBuffer(cx,cy, 3);
			if( pMatBuffer == NULL )
			{
				VirtualFree(pRawBuffer, 0, MEM_RELEASE);
				DBG_MSG("预览线程:错误->>>退出...\n");
				return;
			}
			pBitmapInfo->bmiHeader.biWidth	  	        = cx;
			pBitmapInfo->bmiHeader.biHeight		        = cy;
		}

		bRet = CGrapProc::PreFrameData(DH_HIGH_WIDTH, DH_HIGH_HEIGHT, cx, cy, pRawBuffer, pMatBuffer, m_Layout);

		if( bRet == TRUE )
		{
			if( IsWindow(hWnd) )
				hDC = GetDC(hWnd);
			else
				hDC = NULL;
		}
		else
			hDC = NULL;
		
		
		if( bRet == FALSE || hDC == NULL )
			goto prv_exit;
		
		m_ZoomBuffer = GrapInterface_GetContentBuffer(pMatBuffer);
		if( m_ZoomBuffer == NULL )
			goto prv_exit;


		StretchDIBits(hDC,
			          rc.left,
			          rc.top,
			          cx,
			          cy,
			          0,
			          0,
			          cx,
			          cy,
			          m_ZoomBuffer,
			          pBitmapInfo,
			          DIB_RGB_COLORS,
					  SRCCOPY); 

	
prv_exit:
		
		if( hDC )
			ReleaseDC(hWnd, hDC);

	}

thread_exit:

	if( pBitmapInfo )
		free(pBitmapInfo);

	if( pMatBuffer )
		GrapInterface_FreeMatBuffer(pMatBuffer);

	if( pRawBuffer )
		VirtualFree(pRawBuffer, 0, MEM_RELEASE);


}

KSTATUS   CCADH::QueryParameters(PCAMERA_CONFIG_INFO  cfg_info)
{
	ZeroMemory(cfg_info, sizeof(CAMERA_CONFIG_INFO));
	memcpy(cfg_info, &m_CfgInfo, sizeof(m_CfgInfo));
	DBG_MSG("QueryParameters...cfg_info.dtime = %u\n",cfg_info->dTime);
	return STATUS_SUCCESS;
}

#define  OVER_TIME 3 * 1000

KSTATUS   CCADH::ReadRawGrapBuffer(HANDLE  *handle)
{

	BOOL                        bRet = FALSE;
	PCHAR                       pRawBuffer = NULL;
	KSTATUS                     status = STATUS_SUCCESS;

	//首先要防止重入
	WaitForSingleObject(m_Rentry, INFINITE);

	pRawBuffer = (PCHAR)VirtualAlloc(NULL, DH_HIGH_WIDTH * DH_HIGH_HEIGHT, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	if( pRawBuffer == NULL )
	{
		SetEvent(m_Rentry);
		return STATUS_ALLOC_MEMORY_ERROR;
	}

	//开始置换BUFFER

	if( m_PickRawBuffer->SetSynBufferAndWait(pRawBuffer) == FALSE )
		status = STATUS_UNKNOW_ERROR;


	if( status == STATUS_SUCCESS )
		*handle = pRawBuffer;

	SetEvent(m_Rentry);
	
	return status;

}

#define WIDTH_BYTES(bits)	(((bits) + 31) / 32 * 4)

BOOL SaveBMPFile(LPCSTR lpFileName, BITMAPINFO *pBmpInfo, BYTE *pImageBuffer)
{
	BOOL bRVal				= TRUE;
	DWORD dwBytesRead		= 0;
	DWORD dwSize			= 0;
	BITMAPFILEHEADER bfh	= {0};
	int nTable				= 0;
	DWORD dwImageSize		= 0;
	
	if (pBmpInfo->bmiHeader.biBitCount > 8) {
		nTable = 0;
	}
	else{
		nTable = 256;
	}
	
	dwImageSize =  WIDTH_BYTES(pBmpInfo->bmiHeader.biWidth * pBmpInfo->bmiHeader.biBitCount) * 
		pBmpInfo->bmiHeader.biHeight;
	
	if (dwImageSize <= 0) {
		bRVal = FALSE;
	}
	else{
		bfh.bfType		= (WORD)'M' << 8 | 'B';
		bfh.bfOffBits	= sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + nTable * sizeof(RGBQUAD);
		bfh.bfSize		= bfh.bfOffBits + dwImageSize;			
		
		HANDLE hFile = ::CreateFile(lpFileName,
			GENERIC_WRITE ,
			0,
			NULL,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL
			);
		if (hFile == INVALID_HANDLE_VALUE) {
			bRVal = FALSE;
		}
		else{
			dwSize = sizeof(BITMAPFILEHEADER);
			::WriteFile(hFile, &bfh, dwSize, &dwBytesRead, NULL );
			
			dwSize = sizeof(BITMAPINFOHEADER) + nTable * sizeof(RGBQUAD);
			::WriteFile(hFile, pBmpInfo, dwSize, &dwBytesRead, NULL );
			
			dwSize = dwImageSize;
			WriteFile(hFile, pImageBuffer, dwSize, &dwBytesRead, NULL );
			
			CloseHandle(hFile);
		}
	}
	
	return bRVal;
}

KSTATUS   CCADH::SaveFrameFile(PFRAME_FILE_INFO  FrameFileInfo)
{
	int         z_cx, z_cy;
	BOOL        bRet;
	PVOID       pMatBuffer = NULL;
	KSTATUS     status = STATUS_SUCCESS;

	EnterCriticalSection(&cs);
	z_cx = m_CfgInfo.dGrapWidth;
	z_cy = m_CfgInfo.dGrapHeight;
	LeaveCriticalSection(&cs);

	pMatBuffer = GrapInterface_CreateMatBuffer(z_cx, z_cy, 3);

	if( pMatBuffer == NULL )
	{
		status = STATUS_ALLOC_MEMORY_ERROR;
		goto save_exit;
	}


	bRet = CGrapProc::PreFrameData(DH_HIGH_WIDTH, DH_HIGH_HEIGHT, m_CfgInfo.dGrapWidth, m_CfgInfo.dGrapHeight, (PCHAR)FrameFileInfo->hRawGrapBuffer, pMatBuffer, m_Layout);
	if( bRet == FALSE )
	{
		status = STATUS_UNKNOW_ERROR;
		goto save_exit;
	}

	
	if( HVSaveJPEG(FrameFileInfo->FrameFilePath,(PUCHAR)GrapInterface_GetContentBuffer(pMatBuffer), m_CfgInfo.dGrapWidth, m_CfgInfo.dGrapHeight, 24, TRUE, 100) != STATUS_SUCCESS )
		status = STATUS_UNKNOW_ERROR;
	
	
//	if( GrapInterface_SaveFile(FrameFileInfo->FrameFilePath, pMatBuffer) == FALSE )
//		status = STATUS_UNKNOW_ERROR;

	//SaveBMPFile(FrameFileInfo->FrameFilePath, pBitmapInfo, (PUCHAR)GrapInterface_GetContentBuffer(pMatBuffer));

save_exit:

	if( pMatBuffer )
		GrapInterface_FreeMatBuffer(pMatBuffer);

	if( FrameFileInfo->hRawGrapBuffer )
		VirtualFree(FrameFileInfo->hRawGrapBuffer, 0, MEM_RELEASE);

	return status;
}


KSTATUS   CCADH::QueryDHDevSN(PCHAR pSNBuffer, ULONG nSNSize)
{
	int   nSize = 0;

	HVGetDeviceInfo(m_hkv,DESC_DEVICE_SERIESNUM,NULL,&nSize);

	if( nSize == 0 )
		return STATUS_UNKNOW_ERROR;

	if( nSNSize < nSize )
		return STATUS_BUFFER_TOO_SMALL;
	
	ZeroMemory(pSNBuffer, nSNSize);
	if( HV_SUCCESS(HVGetDeviceInfo(m_hkv,DESC_DEVICE_SERIESNUM,pSNBuffer,&nSize)) == FALSE )
		return STATUS_UNKNOW_ERROR;

	return STATUS_SUCCESS;
}


KSTATUS  CCADH::QueryCaDevablity(ULONG nQueryType, PCHAR pOutputBuffer, ULONG nOutLength)
{
	KSTATUS   status = STATUS_INVALID_PARAMETERS;

	if( nQueryType == QUERY_EXPOSE_TIME && nOutLength == sizeof(CA_EXPOSE_TIME_RANGE) )
	{
		PCA_EXPOSE_TIME_RANGE  range = (PCA_EXPOSE_TIME_RANGE)pOutputBuffer;

		range->bSupportExposeTime = bSupExposeTime;
		range->nMinExposeTime     = m_MinExpTime;
		range->nMaxExposeTime     = m_MaxExpTime;

		status = STATUS_SUCCESS;

	}
	else if( nQueryType == QUERY_GAIN && nOutLength == sizeof(CA_GAIN_RANGE) )
	{
		PCA_GAIN_RANGE    range = (PCA_GAIN_RANGE)pOutputBuffer;

		range->nMinGain = m_MinGain;
		range->nMaxGain = m_MaxGain;
		status = STATUS_SUCCESS;
	}



	return status;
}

void     CCADH::QueryGainRange(int &nMinGain, int &nMaxGain)
{
	HVSTATUS                             status;
	HVAPI_CONTROL_PARAMETER              p;
	HV_ARG_QUERY_FEATURE_DESCRIPTOR      ArgDes;
	HV_RES_QUERY_FEATURE_DESCRIPTOR      ResDes;

	ZeroMemory(&p, sizeof(p));
	ZeroMemory(&ArgDes, sizeof(ArgDes));
	ZeroMemory(&ResDes, sizeof(ResDes));

	ArgDes.FeatureId = FEATURE_ID_GAIN;
	p.code = ORD_QUERY_FEATURE_DESCRIPTOR;
	p.pInBuf = &ArgDes;
	p.dwInBufSize = sizeof(ArgDes);
	p.pOutBuf = &ResDes;
	p.dwOutBufSize = sizeof(ResDes);
	status = HVCommand(m_hkv, CMD_HVAPI_CONTROL, &p, NULL);
	if( status == STATUS_OK )
	{
		nMinGain = ResDes.Scalar.MinValue;
		nMaxGain = ResDes.Scalar.MaxValue;
	}
	else
		nMaxGain = nMinGain = 0;

}


HV_BAYER_LAYOUT CCADH::HVGetDeviceInfo_Bayer_Layout()
{
	int nBayerLayout_Size;  ///English:    ///Chinese: 获得DESC_DEVICE_BAYER_LAYOUT所需空间的大小
	
	HV_BAYER_LAYOUT Layout; ///English:    ///Chinese: Bayer格式
	HVSTATUS status = HVGetDeviceInfo(m_hkv, DESC_DEVICE_BAYER_LAYOUT, NULL,&nBayerLayout_Size);	          
    if(STATUS_OK != status)
		return BAYER_GR;
	else
	{
		BYTE *pbBayerLayout = NULL;
		pbBayerLayout = new BYTE[nBayerLayout_Size];
		DWORD *pdBayerLayout = (DWORD *)pbBayerLayout;
        status = HVGetDeviceInfo(m_hkv, DESC_DEVICE_BAYER_LAYOUT, pdBayerLayout,&nBayerLayout_Size);
		if(STATUS_OK != status)
		{
			if (NULL != pbBayerLayout) {
				delete []pbBayerLayout; ///English:    ///Chinese: 释放空间
				pbBayerLayout=NULL;
			}
			
			return BAYER_GR;
		}
		else
		{			
			Layout = (HV_BAYER_LAYOUT)*pdBayerLayout;///English:    ///Chinese: 得到具体的Bayer格式信息
		}
		
		if (NULL != pbBayerLayout) {
			delete []pbBayerLayout; ///English:    ///Chinese: 释放空间
			pbBayerLayout=NULL;
		}
	}
	return Layout;
}