#include "StdAfx.h"
#include "GrapProc.h"
#include "\CommonAPI\ca_inc\GrapIFace.h"
#include "\CommonAPI\ca_inc\HVDAILT.h"
#include "\CommonAPI\ca_inc\Raw2Rgb.h"

#pragma comment(lib, "\\CommonAPI\\lib\\Raw2Rgb.lib")

#ifndef MIN
#define MIN(a,b)  ((a) > (b) ? (b) : (a))
#endif


void SetLutTable(double dRatioR,double dRatioG,double dRatioB, PBYTE m_pLutR, PBYTE m_pLutG, PBYTE m_pLutB)
{
	if( dRatioR <= 0.0 )
		return;
	
	if( dRatioG <= 0.0 )
		return;
	
	if(dRatioB <= 0.0 )
		return;
	
	for(int i=0;i<256;i++)
	{
		m_pLutR[i] = MIN((int)(i*dRatioR),255);
		m_pLutG[i] = MIN((int)(i*dRatioG),255);
		m_pLutB[i] = MIN((int)(i*dRatioB),255);
	}
	
	return;
}



BOOLEAN    CGrapProc::PreFrameData(int s_cx, int s_cy, int z_cx, int z_cy, PCHAR  pRawBuffer, PVOID pMatBuffer, HV_BAYER_LAYOUT nLayout)
{
	HDC         hDC = NULL;
	BYTE        m_pLutR[256];
	BYTE        m_pLutG[256];
	BYTE        m_pLutB[256];
	BOOL        bRet;
	double      m_dRatioR = 1.0f, m_dRatioG = 1.0f, m_dRatioB = 1.0f;
	PCHAR       m_pData;
	ZOOM_PAR    z_par = {0};
	
	
	m_pData = (PCHAR)VirtualAlloc(NULL, s_cx * s_cy * 3 , MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	if( m_pData == NULL )
		return FALSE;
	
	for( int i = 0 ; i < 256 ; i ++ )
	{
		m_pLutR[i] = i;
		m_pLutG[i] = i;
		m_pLutB[i] = i;
	}
	
	ConvertBayer2Rgb((PUCHAR)m_pData, (PUCHAR)pRawBuffer, s_cx, s_cy, BAYER2RGB_NEIGHBOUR, m_pLutR, m_pLutG, m_pLutB, true, nLayout);
	GetWhiteBalanceRatio((PUCHAR)m_pData, s_cx, s_cy, &m_dRatioR,  &m_dRatioG, &m_dRatioB);
	SetLutTable(m_dRatioR, m_dRatioG, m_dRatioB, m_pLutR, m_pLutG, m_pLutB);
	ConvertBayer2Rgb((PUCHAR)m_pData, (PUCHAR)pRawBuffer, s_cx, s_cy, BAYER2RGB_NEIGHBOUR, m_pLutR, m_pLutG, m_pLutB, true, nLayout);
	
	z_par.nSrcWidth   = s_cx;
	z_par.nSrcHeight  = s_cy;
	z_par.nChannels   = 3;
	z_par.pMatBuffer  = pMatBuffer;
	z_par.pSrcBuffer  = (PBYTE)m_pData;
	z_par.nZoomWidth  = z_cx;
	z_par.nZoomHeight = z_cy;
	
	bRet =  GrapInterface_Zoom(&z_par);
	VirtualFree(m_pData, 0, MEM_RELEASE);
	return bRet;
}