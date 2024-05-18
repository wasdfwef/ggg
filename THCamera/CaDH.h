#ifndef        _CADH_INCLUDE
#define        _CADH_INCLUDE

#include "CaInteral.h"
#include "\CommonAPI\ca_inc\HVDAILT.h"
#include "\CommonAPI\ca_inc\Raw2Rgb.h"
#include "\CommonAPI\inc\THFrameDef.h"
#include "SynBuffer.h"


class  CCADH
{
private:

	
	 HHV                                 m_hkv;
	 char                                m_CfgPath[MAX_PATH];
	 HWND                                m_DisplayWnd;
	 RECT                                m_PrviewRC;
	 PCHAR                               m_RawBuffer;
	 BOOLEAN                             bSetPrviewMode;
	 BOOLEAN                             bSupExposeTime;
	 BOOLEAN                             bSupRGBChannel;
	 int                                 m_MinExpTime,m_MaxExpTime, m_MinGain, m_MaxGain;
	 HV_BAYER_LAYOUT                     m_Layout;


	 

	 CRITICAL_SECTION                    cs;          //只是用于锁图形大小
	 CAMERA_CONFIG_INFO                  m_CfgInfo;
	 
	 void                                QueryGainRange(int &nMinGain, int &nMaxGain);
	 void                                SetGain();
	 void                                ReadConfig(PCHAR  pCfgPath);
	 void                                SetExposureTime();
	 void                                SetLutTable(double dRatioR,double dRatioG,double dRatioB);
	 void                                SetPacketLong(ULONG nPacketSize);
	 void                                ReadDefaultCfgFile();
	 LONG                                m_ClosePrviewThread;
	 HANDLE                              m_hThread;
	 HANDLE                              m_hPauseEvent;
	 HANDLE                              m_Rentry;
	 BOOLEAN                             AllocResource(int cx, int cy,PCHAR &pRawBuffer, PVOID &pMatBuffer);
	 BOOLEAN                             QuerySupExposeTime();
	 BOOLEAN                             QueryExposeTimeRange(int &nMinExpTime, int &nMaxExpTime);
	 BOOLEAN                             QuerySupRGBChannel();
	 HV_BAYER_LAYOUT                     HVGetDeviceInfo_Bayer_Layout();
	 
	 
	 
	 

public:
	 CCADH();
	~CCADH();

	 
	 void                                Preview();
	 void                                CopyPickRawBuffer();
	 void                                CopyPrvRawBuffer();
	 KSTATUS                             Create(PINIT_CAMERA_PARAMETERS   pParInit);
	 KSTATUS                             SetParameters(PCAMERA_CONFIG_INFO  cfg_info);
	 KSTATUS                             QueryParameters(PCAMERA_CONFIG_INFO  cfg_info);
	 KSTATUS                             SetPrview(PCAMERA_PRVIEW_CONFIG  pPrvConfig);
	 KSTATUS                             SaveFrameFile(PFRAME_FILE_INFO  FrameFileInfo);
	 KSTATUS                             ReadRawGrapBuffer(HANDLE  *handle);
	 KSTATUS                             QueryDHDevSN(PCHAR pSNBuffer, ULONG nSNSize);
	 KSTATUS                             QueryCaDevablity(ULONG nQueryType, PCHAR pOutputBuffer, ULONG nOutLength);
	 CSynBuffer                         *m_PickRawBuffer, *m_PrvRawBuffer;

};


#endif