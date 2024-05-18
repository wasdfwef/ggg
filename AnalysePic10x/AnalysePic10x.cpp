// AnalysePic10x.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "interal.h"
#include <shlwapi.h>
#include "DbgMsg.h"


#include "..\\..\\inc\\OpenCV2_1/include/cxcore.h"
#include "..\\..\\inc\\OpenCV2_1/include/highgui.h"
#include "..\\..\\inc\\OpenCV2_1/include/cv.h"
#pragma comment(lib, "..\\..\\lib\\OpenCV2_1/lib/highgui210.lib")
#pragma comment(lib, "..\\..\\lib\\OpenCV2_1/lib/cxcore210.lib")
#pragma comment(lib, "..\\..\\lib\\OpenCV2_1/lib/cv210.lib")
#pragma comment(lib, "..\\..\\lib\\QueueManager.lib")
#pragma comment(lib, "shlwapi.lib")

#include "..\..\inc\Recognition.h"

#define CELL_RECOGNITION_DLL_NAME  "Screening.dll"
#define CELL_RECOGNITION_FUN_NAME  "ScreeningPositive"
#define TH_FLAG          MAKEWORD('T','H')




typedef int (*pScreeningPositive)(unsigned char *pImageData, int nWidth, int nHeight, int nWidthStep,
								  double dThreshold1, double dThreshold2,
								  PCOBJECTINFO pObjectInfo,             
								  int nMaxObject,                       
								  PCCLASSINFO pClassInfo,          
								  int nClassNumber);  


static HMODULE    hRec10xMod     = NULL;
static CCLASSINFO g_class_info[] = {{1,0,{0,0},{25.0,200.0},{0.0,1.75}},{2,0,{0,0},{200.0,10000.0},{1.0,2.5}},{3,0,{0,0},{100.0,10000.0},{2.5,20.0}},{4,0,{0,0},{10000.0,480000.0},{0.0,0.0}}};
static pScreeningPositive  g_scrpos = NULL;

#define FULL_40X_MAX_SAMPLE_NUMBER  (32 * 16)  //最大采图数量
#define OPTION_40X_SAMPLE_NUMBER     50        //考虑性能优化,40倍镜采图最大数量上限
#define S_40X_MAX_NUMBER     16
#define S_40X_MIN_NUMBER     10
#define PRPE_SAMPLE_MASK     0X1
#define MUST_SAMPLE_MASK     0X2
#define DONT_CORRECT_MASK    0X4
#define SAMPLE_CLASS_NUMBER  2

#define ARRAY_MUST_SAMPLE_INDEX 0
#define ARRAY_BIG_CELL_INDEX    1



static
void
LoadRec10xMod(
)
{
	char  mod_path[MAX_PATH];

	GetModuleFileName(NULL, mod_path, MAX_PATH);
	PathRemoveFileSpec(mod_path);
	PathAppend(mod_path, CELL_RECOGNITION_DLL_NAME);

	hRec10xMod = LoadLibrary(mod_path);
	if( hRec10xMod == NULL )
	{
		DBG_MSG("Not Found %s dll file\n", mod_path);
		return;
	}

	g_scrpos = (pScreeningPositive)GetProcAddress(hRec10xMod, CELL_RECOGNITION_FUN_NAME);
	if( g_scrpos == NULL )
	{
		FreeLibrary(hRec10xMod);
		hRec10xMod = NULL;
		DBG_MSG("Not Found CELL_RECOGNITION_FUN_NAME\n");
		return;
	}
}



BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	if(ul_reason_for_call == DLL_PROCESS_ATTACH)
		LoadRec10xMod();


    return TRUE;
}



#define MAX_CELL_NUMBER 4096

static
ULONG
RedoVaildCellInfo(   //重新处理有效细胞,去除nClass为4的细胞
PCOBJECTINFO         c_obj,
ULONG                n10xCellNums
)
{
	ULONG   nVaildCellCounter = 0;

	for( ULONG j = 0 ; j < n10xCellNums ; j ++ )
	{
		if( c_obj[j].nClass != 4 )
		{
			nVaildCellCounter ++;
			continue;
		}

		memcpy(&c_obj[j], &c_obj[j+1], n10xCellNums - j - 1);
		n10xCellNums --;
		j--;
	}

	return nVaildCellCounter;

}

static
void
CalcBigCellPos(
PCOBJECTINFO      c_obj,
PSAMPLE_40X_INFO  s_40x_info,
PSIZE             one_piece,
BOOL              bMustSample40x
)
{
	int start_x_loc, col, start_y_loc, line, i, j, l, n;

	start_x_loc = c_obj->nLeft / one_piece->cx;

	col = (c_obj->nRight / one_piece->cx) - start_x_loc + 1;

	start_y_loc = c_obj->nTop / one_piece->cy;

	line = (c_obj->nBottom / one_piece->cy) - start_y_loc + 1;

	for( i = 0 ; i < line ; i ++ )
	{
		l = (start_y_loc + i) << 2;

		for( j = 0 ; j < col ; j ++ )	
		{
			n = l + j + start_x_loc;
			s_40x_info[n].nAbleCellNum++;

			if( bMustSample40x == TRUE )
				s_40x_info[n].s_mask = MUST_SAMPLE_MASK | DONT_CORRECT_MASK;
			else
				s_40x_info[n].s_mask = PRPE_SAMPLE_MASK | DONT_CORRECT_MASK;
		}
	}
}

static
void
CalcSmallCellPos(
PCOBJECTINFO      c_obj,
PSAMPLE_40X_INFO  s_40x_info,
USHORT            c_idx,
PSIZE             one_piece
)
{
	int start_x_loc,start_y_loc, k;

	start_x_loc = c_obj->nLeft / one_piece->cx;
	start_y_loc = c_obj->nTop / one_piece->cy;

	k = (start_y_loc << 2) + start_x_loc;

	s_40x_info[k].nAbleCellNum ++;
	s_40x_info[k].c_idx  = c_idx;
	s_40x_info[k].s_mask = MUST_SAMPLE_MASK;  //将小细胞纳入为必采的范围
}

static
void
Calc40xPosInfo(
PCOBJECTINFO      c_obj,
ULONG             n10xCellNums,
PSAMPLE_40X_INFO  s_40x_info,
PSIZE             one_piece,
PSIZE             grap_sz
)
{

	for( ULONG j = 0 ; j < n10xCellNums ; j ++ )
	{
		if( c_obj[j].nLeft < 0 || c_obj[j].nTop < 0 || c_obj[j].nRight > grap_sz->cx || c_obj[j].nBottom > grap_sz->cy )
			continue;

		if( c_obj[j].nClass == 2 || c_obj[j].nClass == 3 ) //大细胞
			CalcBigCellPos(&c_obj[j], s_40x_info, one_piece, FALSE); //由于识别精度问题,不再将(c_obj[j].nClass == 3)管形作为必采项目.
		else
			CalcSmallCellPos(&c_obj[j], s_40x_info, (USHORT)j,one_piece);

	}

}

static
BOOL
CorrectPos(
PSAMPLE_40X_INFO  s_40x_info,
PCOBJECTINFO      c_obj,
PSIZE             one_piece
)
{
	int    x, y;
	int    x_l, x_h;
	int    y_l, y_h;
	BOOL   bRet = FALSE;

	x  = c_obj->nLeft % one_piece->cx;
	y  = c_obj->nTop % one_piece->cy;

	x_l = (int)((float)one_piece->cx * 0.1f);
	x_h = (int)((float)one_piece->cx * 0.9f);
	y_l = (int)((float)one_piece->cy * 0.1f);
	y_h = (int)((float)one_piece->cy * 0.9f);

	if( x <= x_l )
	{
		s_40x_info->dX = 50;
		bRet = TRUE;
	}
	else if( x >= x_h )
	{
		s_40x_info->dX = -50;
		bRet = TRUE;
	}

	if( y <= y_l )
	{
		s_40x_info->dy = 50;
		bRet = TRUE;
	}
	else if( y >= y_h )
	{
		s_40x_info->dy = -50;
		bRet = TRUE;
	}

	return bRet;
	
}

static
void
LaterThing(                        //后期相关处理,例如纠偏,数据填充
PANALYSE_10X_HANDLE  a10x,
PSAMPLE_40X_INFO     s_40x_info,
PCOBJECTINFO         c_obj,
UCHAR                nIdx10x,
PSIZE                one_piece
)
{
	for( UCHAR j = 0 ; j < S_40X_MAX_NUMBER ; j ++ )
	{
		s_40x_info[j].idx_10x = nIdx10x;
		s_40x_info[j].idx_40x = j;

		if( s_40x_info[j].nAbleCellNum == 0 || Que_InsertData(a10x->s40x_que, &s_40x_info[j]) == FALSE )
			continue;

		if( s_40x_info[j].s_mask & DONT_CORRECT_MASK )
		{
			a10x->nBigSample40xCorrectNumber++;   //上皮
			continue;
		}

		if( s_40x_info[j].nAbleCellNum == 1  ) //颗粒为1个的情况考虑纠偏
			CorrectPos(&s_40x_info[j], &c_obj[s_40x_info[j].c_idx], one_piece);

		a10x->nSmallSample40xNumber++; //普通粒子
	}

}

static 
void
Calc40xPosEntry(
PANALYSE_10X_HANDLE  a10x,
UCHAR                nIdx10x,   
PCOBJECTINFO         c_obj,
ULONG                n10xCellNums,
PSIZE                grap_sz
)
{
	SIZE               one_piece;
	SAMPLE_40X_INFO    s_40[S_40X_MAX_NUMBER] = {0};

	if( n10xCellNums == 0 ) //没有找到任何粒子
		return;

	n10xCellNums = RedoVaildCellInfo(c_obj, n10xCellNums); //重新修正n10xCellNums细胞数量
	if( n10xCellNums == 0 )
		return;

	one_piece.cx = grap_sz->cx >> 2;
	one_piece.cy = grap_sz->cy >> 2;

	a10x->m_CellCounter += n10xCellNums;
	//计算40倍采图坐标
	Calc40xPosInfo(c_obj, n10xCellNums, s_40, &one_piece, grap_sz);

	//后期处理
	LaterThing(a10x,s_40, c_obj, nIdx10x, &one_piece);
	
	//if( n10xCellNums == 1 )   //如果细胞只有一个,则考虑进行纠偏

}

extern "C"
RECOGNITION_API
BOOL 
WINAPI
AnalysePic10x_Entry( 
HANDLE hA10x, 
PCHAR pGrapPath,
UCHAR nIdx10x
)
{
	SIZE                    sz = {0};
	ULONG                   n10xCellNums;
	IplImage               *pImage;

	PCOBJECTINFO           c_obj;
	PANALYSE_10X_HANDLE    a_10x = (PANALYSE_10X_HANDLE)hA10x;

	if( g_scrpos == NULL || hA10x == NULL || pGrapPath == NULL || IsBadWritePtr(a_10x, sizeof(ANALYSE_10X_HANDLE)) == TRUE )
		return FALSE;

	try
	{
		if( a_10x->th_flag != TH_FLAG || a_10x->bFinished != FALSE )
			return FALSE;

		pImage = cvLoadImage(pGrapPath, CV_LOAD_IMAGE_GRAYSCALE);
		if( pImage == NULL )
		{
			DBG_MSG("Load pGrapPath = %s Fail...\n", pGrapPath);
			return FALSE;
		}

		c_obj = new COBJECTINFO[MAX_CELL_NUMBER];
		if( c_obj == NULL )
		{
			DBG_MSG("new COBJECTINFO[MAX_CELL_NUMBER] err...\n");
			cvReleaseImage(&pImage);
			return FALSE;
		}

		sz.cx = pImage->width;
		sz.cy = pImage->height;

		n10xCellNums = 	g_scrpos((unsigned char*)pImage->imageData,
			                      pImage->width,
			                      pImage->height,
			                      pImage->widthStep,
			                      64,
			                      128,
			                      c_obj,
			                      MAX_CELL_NUMBER,
			                      g_class_info,
			                      4);

		DBG_MSG("10倍镜识别出来的粒子数量=%u\n",n10xCellNums);
		cvReleaseImage(&pImage);
		Calc40xPosEntry(a_10x, nIdx10x, c_obj, n10xCellNums, &sz);
		delete [] c_obj;
		return TRUE;
	}
	catch(...)
	{
		DBG_MSG("AnalysePic10x_Entry Exception...\n");
		return FALSE;

	}
	
}

extern "C"
RECOGNITION_API
HANDLE 
WINAPI
AnalysePic10x_Create(
USHORT   nExpressSample40x
)
{
	//if( nExpressSample40x < S_40X_MIN_NUMBER )
	//	return NULL;

	PANALYSE_10X_HANDLE    a_10x = new ANALYSE_10X_HANDLE;

	if( a_10x == NULL )
	{
		DBG_MSG("new ANALYSE_10X_HANDLE handle err...\n");
		return a_10x;
	}

	ZeroMemory(a_10x, sizeof(ANALYSE_10X_HANDLE));
	a_10x->s40x_que = Que_Initialize(sizeof(SAMPLE_40X_INFO));
	if( a_10x->s40x_que == NULL )
	{
		DBG_MSG("new ANALYSE_10X_HANDLE Create Que err...\n");
		delete a_10x;
		return NULL;
	}
	a_10x->th_flag	= TH_FLAG;
	return a_10x;
}

static
void
CalcFactSample40xNumber(
PANALYSE_10X_HANDLE  a_10x,
PUSHORT              pFactArraySample40x
)
{
	USHORT     nRefNumber;
	PUSHORT    pAbleArraySample40x = &a_10x->nSmallSample40xNumber;

	nRefNumber  = OPTION_40X_SAMPLE_NUMBER;
	a_10x->nFactSample40xNumber = 0;

	for( USHORT j = 0 ; j < SAMPLE_CLASS_NUMBER; j ++ )
	{
		if( nRefNumber >= pAbleArraySample40x[j] )
		{
			pFactArraySample40x[j] = pAbleArraySample40x[j];
			nRefNumber -= pFactArraySample40x[j];
		}
		else
		{
			pFactArraySample40x[j] = nRefNumber;
			nRefNumber = 0;
		}

		a_10x->nFactSample40xNumber += pFactArraySample40x[j];
		if( nRefNumber == 0 )
			break;
	}

}

static
BOOL
GetFactSample40xInfo(
PANALYSE_10X_HANDLE  a_10x,
PUSHORT              pFactArraySample40x
)
{
	BOOL              bVaild, bCorrect;
	USHORT            nVaildCounter = 0;
	SAMPLE_40X_INFO   s_40;

	a_10x->pFactSample40xArrayInfo  = new SAMPLE_40X_INFO[a_10x->nFactSample40xNumber];

	if( a_10x->pFactSample40xArrayInfo == NULL )
		return FALSE;

	ZeroMemory(a_10x->pFactSample40xArrayInfo, a_10x->nFactSample40xNumber * sizeof(SAMPLE_40X_INFO));

	while( Que_GetHeaderData(a_10x->s40x_que, &s_40) )
	{
		bVaild = FALSE;

		if( (s_40.s_mask & MUST_SAMPLE_MASK) && pFactArraySample40x[0] )
		{
			pFactArraySample40x[0] --;
			bVaild = TRUE;
			goto copy_vaild_40x_info;
		}
		if( !(s_40.s_mask & DONT_CORRECT_MASK) ) //小粒子
		{
			bCorrect = (s_40.dX != 0 || s_40.dy != 0);
			if( bCorrect == FALSE && pFactArraySample40x[1] )
			{
				bVaild = TRUE;
				pFactArraySample40x[1] --;
				goto copy_vaild_40x_info;
			}
			else if( bCorrect == TRUE && pFactArraySample40x[2] )
			{
				bVaild = TRUE;
				pFactArraySample40x[2] --;
				goto copy_vaild_40x_info;
			}
		}
		if( (s_40.s_mask & DONT_CORRECT_MASK) && pFactArraySample40x[3] )  //上皮
		{
			bVaild = TRUE;
			pFactArraySample40x[3] --;
			goto copy_vaild_40x_info;
		}

copy_vaild_40x_info:

		if( bVaild == FALSE )
			continue;

		memcpy(&a_10x->pFactSample40xArrayInfo[nVaildCounter], &s_40, sizeof(s_40));
		nVaildCounter++;
		if( nVaildCounter == a_10x->nFactSample40xNumber )
			break;
	}

	a_10x->nFactSample40xNumber = nVaildCounter;
	Que_ReleaseList(a_10x->s40x_que);
	a_10x->s40x_que = NULL;

	return TRUE;
}

static
BOOL
GetFactSample40xInfo1(
PANALYSE_10X_HANDLE  a_10x,
PUSHORT              pFactArraySample40x
)
{
	ULONG             nVaildCounter = 0;
	SAMPLE_40X_INFO   s_40;

	a_10x->nNew40LastIdx = FULL_40X_MAX_SAMPLE_NUMBER - 1;
	a_10x->nFactSample40xNumber = 0;
	a_10x->pFactSample40xArrayInfo  = new SAMPLE_40X_INFO[OPTION_40X_SAMPLE_NUMBER];

	while( Que_GetHeaderData(a_10x->s40x_que, &s_40) )
	{
		if( nVaildCounter < OPTION_40X_SAMPLE_NUMBER )
		{
			memcpy(&a_10x->pFactSample40xArrayInfo[nVaildCounter], &s_40, sizeof(s_40));
			a_10x->nFactSample40xNumber++;
		}

		nVaildCounter++;
		if( nVaildCounter > OPTION_40X_SAMPLE_NUMBER )
		{
			a_10x->nNew40LastIdx = (a_10x->pFactSample40xArrayInfo[OPTION_40X_SAMPLE_NUMBER - 1].idx_10x * 16) + a_10x->pFactSample40xArrayInfo[OPTION_40X_SAMPLE_NUMBER - 1].idx_40x;
			break;
		}
	}

	DBG_MSG("最后一个采图位置 = %u, 新算法放大系数:%.2f\n", a_10x->nNew40LastIdx, 512.0f / (float)a_10x->nNew40LastIdx);
	return TRUE;

}

static
BOOL
AnalysePic10x_FillArray(  //填充4宫格
PANALYSE_10X_HANDLE  a_10x,
PSAMPLE_40X_INFO     s_40_i,
USHORT              &start,
USHORT              &end
)
{
	UCHAR   n10x_idx = a_10x->pFactSample40xArrayInfo[start].idx_10x;
	UCHAR   nCol, nLine, nArrayIdx;

	end = start;

	for( ; end < a_10x->nFactSample40xNumber ; end ++ )
	{
		if( a_10x->pFactSample40xArrayInfo[end].idx_10x != n10x_idx )
			break;
		
		nArrayIdx = a_10x->pFactSample40xArrayInfo[end].idx_40x;
		nCol      = nArrayIdx >> 2;
		if( nCol & 0x1 ) //奇数排(从序号0开始)
		{
			nLine = nArrayIdx & 0x3;
			nLine = 3 - nLine;
			nArrayIdx = (nCol << 2) +  nLine;
		}

		memcpy(&s_40_i[nArrayIdx], &a_10x->pFactSample40xArrayInfo[end], sizeof(SAMPLE_40X_INFO));
	}

	if( end == start ) //不要重新调整镜头路径
		return FALSE;

	return TRUE;
}

static
void
AnalysePic10x_FixLoc(
PANALYSE_10X_HANDLE  a_10x,
PSAMPLE_40X_INFO     s_40_i,
USHORT               s
)
{
	for( USHORT j = 0 ; j < S_40X_MAX_NUMBER ; j ++ )
	{
	    if( s_40_i[j].nAbleCellNum == 0 )
			continue;
		memcpy(&a_10x->pFactSample40xArrayInfo[s], &s_40_i[j], sizeof(SAMPLE_40X_INFO));
		s++;
	}
}

static
void
AnalysePic10x_Reloc(     //重新进行40x走位路线调整
PANALYSE_10X_HANDLE  a_10x
)
{
	BOOL              bReloc;
	USHORT            s = 0, e;
	SAMPLE_40X_INFO   s_40_i[S_40X_MAX_NUMBER];

	while( TRUE )
	{
		if( s >= a_10x->nFactSample40xNumber )
			break;

		ZeroMemory(s_40_i, sizeof(s_40_i));
		bReloc = AnalysePic10x_FillArray(a_10x, s_40_i, s, e);
		if( bReloc == TRUE )
			AnalysePic10x_FixLoc(a_10x, s_40_i, s);

		s = e + 1;
	}
}



static
BOOL
AnalysePic10x_Decide(
PANALYSE_10X_HANDLE  a_10x
)
{
	BOOL       bRet;
	USHORT     nFactArraySample40x[4] = {0};

	a_10x->nAvalibSample40xNumber = Que_GetCounter(a_10x->s40x_que);

	CalcFactSample40xNumber(a_10x, nFactArraySample40x);  //计算粒子信息
	bRet = GetFactSample40xInfo(a_10x,nFactArraySample40x);
	if( bRet == FALSE )
		return bRet;

	//DBG_MSG("总共粒子数量(10倍镜包含修正)=%u\n", a_10x->m_CellCounter);
	AnalysePic10x_Reloc(a_10x); //重新设计镜头走位路线
	return bRet;
}


extern "C"
RECOGNITION_API
BOOL 
WINAPI
AnalysePic10x_Finish( 
HANDLE hA10x 
)
{
	PANALYSE_10X_HANDLE    a_10x = (PANALYSE_10X_HANDLE)hA10x;

	if( g_scrpos == NULL || hA10x == NULL ||  IsBadWritePtr(a_10x, sizeof(ANALYSE_10X_HANDLE)) == TRUE )
		return FALSE;

	try
	{
		if( a_10x->th_flag != TH_FLAG || a_10x->bFinished == TRUE )
			return FALSE;

		a_10x->bFinished = TRUE;
		return AnalysePic10x_Decide(a_10x);
	}
	catch(...)
	{
		return FALSE;
	}

}

extern "C"
RECOGNITION_API
ULONG 
WINAPI
AnalysePic10x_GetSample40xGrapCounter( 
HANDLE hA10x 
)
{
	PANALYSE_10X_HANDLE    a_10x = (PANALYSE_10X_HANDLE)hA10x;

	if( g_scrpos == NULL || hA10x == NULL ||  IsBadWritePtr(a_10x, sizeof(ANALYSE_10X_HANDLE)) == TRUE )
		return FALSE;

	try
	{
		if( a_10x->th_flag != TH_FLAG || a_10x->bFinished == FALSE )
			return 0;
		
		return a_10x->nFactSample40xNumber;
	}
	catch(...)
	{
		return 0;
	}

}

extern "C"
RECOGNITION_API
BOOL 
WINAPI
AnalysePic10x_GetSample40xGrapInfo( 
HANDLE hA10x, 
PSAMPLE_40X_INFO s_40_info, 
ULONG nCounter 
)
{
	PANALYSE_10X_HANDLE    a_10x = (PANALYSE_10X_HANDLE)hA10x;

	if( g_scrpos == NULL || hA10x == NULL ||  IsBadWritePtr(a_10x, sizeof(ANALYSE_10X_HANDLE)) == TRUE || s_40_info == NULL || nCounter == 0 )
		return FALSE;

	try
	{
		if( a_10x->th_flag != TH_FLAG || a_10x->bFinished == FALSE || nCounter < a_10x->nFactSample40xNumber )
			return 0;

		memcpy(s_40_info, a_10x->pFactSample40xArrayInfo, a_10x->nFactSample40xNumber * sizeof(SAMPLE_40X_INFO));
		return TRUE;
	}
	catch(...)
	{
		return FALSE;
	}
}

extern "C"
RECOGNITION_API
void 
WINAPI
AnalysePic10x_Destory( 
HANDLE hA10x 
)
{
	PANALYSE_10X_HANDLE    a_10x = (PANALYSE_10X_HANDLE)hA10x;

	if( g_scrpos == NULL || hA10x == NULL ||  IsBadWritePtr(a_10x, sizeof(ANALYSE_10X_HANDLE)) == TRUE )
		return;

	try
	{
		if( a_10x->th_flag != TH_FLAG )
			return;

		if( a_10x->s40x_que )
			Que_ReleaseList(a_10x->s40x_que);
		if( a_10x->pFactSample40xArrayInfo )
			delete [] a_10x->pFactSample40xArrayInfo;
		
		delete a_10x;
	}
	catch(...)
	{
		return;
	}

}

extern "C"
RECOGNITION_API
BOOL 
WINAPI
AnalysePic10x_RecCellNumber(
HANDLE hA10x, 
PLONG pRecCellNumber 
)
{
	PANALYSE_10X_HANDLE    a_10x = (PANALYSE_10X_HANDLE)hA10x;

	if( g_scrpos == NULL || hA10x == NULL ||  IsBadWritePtr(a_10x, sizeof(ANALYSE_10X_HANDLE)) == TRUE || pRecCellNumber == NULL )
		FALSE;

	try
	{
		if( a_10x->th_flag != TH_FLAG )
			return FALSE;

		*pRecCellNumber = a_10x->m_CellCounter;
		return  TRUE;
	}
	catch(...)
	{
		return FALSE;
	}

	
}


extern "C"
RECOGNITION_API
ULONG
WINAPI
AnalysePic10x_GetSample40xTotaleGrapCounter(
HANDLE  hA10x    //获取有细胞的40x图片的数量
)
{
	PANALYSE_10X_HANDLE    a_10x = (PANALYSE_10X_HANDLE)hA10x;

	if( g_scrpos == NULL || hA10x == NULL ||  IsBadWritePtr(a_10x, sizeof(ANALYSE_10X_HANDLE)) == TRUE )
		return 0;

	try
	{
		if( a_10x->th_flag != TH_FLAG )
			return 0;

		return  a_10x->nAvalibSample40xNumber;
	}
	catch(...)
	{
		return 0;
	}


}


