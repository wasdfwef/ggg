#ifndef   _INTERAL_INCLUDE
#define   _INTERAL_INCLUDE

#include "..\..\inc\QueueManager.h"
#include "..\..\inc\AnalysePic10x.h"

typedef struct COBJECTINFO
{
	int       nClass;                  // 类别
	int       nLeft;
	int       nTop;
	int       nRight;
	int       nBottom;
	double    dLength;                 // 周长
	double    dArea;                   // 面积
	double    dAspectRatio;            // 长宽比

}COBJECTINFO, *PCOBJECTINFO;

typedef struct CCLASSINFO
{

	int       nClass;                  // 类别
	int       nNumber;                 // 数量
	int       dLength[2];              // 周长
	double    dArea[2];                // 面积
	double    dAspectRatio[2];         // 长宽比

}CCLASSINFO, *PCCLASSINFO;

#pragma pack(1)



typedef  struct _ANALYSE_10X_HANDLE
{
	USHORT               th_flag;
	USHORT               nNew40LastIdx;
	QUEUE_HANDLE         s40x_que;       //40x采图序列
	ULONG                m_CellCounter;           //总共数胞个数
	ULONG                nAvalibSample40xNumber;  //有细胞的40x图片总数量
	USHORT               nSmallSample40xNumber;  //颗粒(包括红细胞，白细胞，杂质), 同时将该成员的值定为必采图片.因为识别最关心其颗粒
	USHORT               nBigSample40xCorrectNumber;
	USHORT               nFactSample40xNumber;
	BOOL                 bFinished;
	PSAMPLE_40X_INFO     pFactSample40xArrayInfo;
	

}ANALYSE_10X_HANDLE, *PANALYSE_10X_HANDLE;


#pragma pack()


#endif