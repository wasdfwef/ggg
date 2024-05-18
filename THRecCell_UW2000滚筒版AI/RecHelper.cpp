#include "StdAfx.h"

#include  <SHLWAPI.H>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include "DbgMsg.h"
/*#include "..\..\inc\THDBUW_Access.h"*/
#include "RecHelper.h"

#define  ADDITION_SECTION "AdditionInfo"
#define  END_INDEX_KEY    "EndIndex"
#define  GRAP_SIZE_KEY    "GrapSize"
#define  DEFAULT_GRAP_SIZE 40


#define  IDENTIFY_SAME(s,d) (memcmp(&s,&d,sizeof(CELL_IDENTIFY)) == 0)
#define  VAILD_RECT(rc) (rc.left < rc.right && rc.top < rc.bottom)

#include "..\\..\\inc\\OpenCV2_1/include/cxcore.h"
#include "..\\..\\inc\\OpenCV2_1/include/highgui.h"
#include "..\\..\\inc\\OpenCV2_1/include/cv.h"

#pragma comment(lib, "..\\..\\lib\\OpenCV2_1/lib/highgui210.lib")
#pragma comment(lib, "..\\..\\lib\\OpenCV2_1/lib/cxcore210.lib")
#pragma comment(lib, "..\\..\\lib\\OpenCV2_1/lib/cv210.lib")
#pragma comment(lib, "..\\..\\lib\\QueueManager.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "Shlwapi.lib")



KSTATUS  CRecHelper::QueryTaskInfo(PREC_GRAP_FULL_RECORD   grap_rc, PTASK_REC_INFO *pTaskInfo)
{
	int                nRet, nCounter = 0, nPrevCount = 0, nIndex;
	char               IniPath[MAX_PATH], GrapPath[MAX_PATH],FileName[50];
	ULONG              nGrapSize;
	KSTATUS            status = STATUS_SUCCESS;
	PREC_GRAP_RECORD   pre_rc;
	PTASK_REC_INFO     t_info;
	
	if( grap_rc->ReslutRelativePath[0] == 0 || PathFileExists(grap_rc->ReslutRelativePath) == FALSE )
		return STATUS_INVALID_PARAMETERS;
	
	strcpy(IniPath, grap_rc->ReslutRelativePath);
	PathAppend(IniPath, RESLUT_INI); 
	
	if( PathFileExists(IniPath) == FALSE )
		return STATUS_NOT_FOUND_FILE;

	nGrapSize  = GetPrivateProfileInt(ADDITION_SECTION, GRAP_SIZE_KEY, DEFAULT_GRAP_SIZE, IniPath);
	nPrevCount = GetPrivateProfileInt(ADDITION_SECTION, END_INDEX_KEY, 0, IniPath);

	pre_rc = (PREC_GRAP_RECORD)malloc(sizeof(REC_GRAP_RECORD) * (nPrevCount + 1));
	if( pre_rc == NULL )
		return STATUS_ALLOC_MEMORY_ERROR;
	
	ZeroMemory(pre_rc, sizeof(REC_GRAP_RECORD) * (nPrevCount + 1));

	strcpy(GrapPath, grap_rc->ReslutRelativePath);
	for( nIndex = 0 ; nIndex <= nPrevCount ; nIndex ++ )
	{
		sprintf(FileName, "%.3u_%.4u.jpg", nGrapSize, nIndex);
		PathAppend(GrapPath, FileName);
		nRet = PathFileExists(GrapPath);
		PathRemoveFileSpec(GrapPath);
		if( nRet == FALSE )
			continue;

		pre_rc[nCounter].rGrapSize = (UCHAR)nGrapSize;
		pre_rc[nCounter].rIndex    = nIndex;
		nCounter ++;
	}
	if( nCounter == 0 )
	{
		status = STATUS_NOT_FOUND_VALUE;
e1_loc:
		free(pre_rc);
		return status;
	}

	t_info = (PTASK_REC_INFO)FMMC_CreateBuffer(sizeof(TASK_REC_INFO) + nCounter * sizeof(REC_GRAP_RECORD));
	if( t_info == NULL )
	{
		status = STATUS_ALLOC_MEMORY_ERROR;
		goto e1_loc;
	}

	t_info->tGrapCounter = nCounter;
	memcpy(t_info->gInfo, pre_rc, nCounter * sizeof(REC_GRAP_RECORD));
	free(pre_rc);
	*pTaskInfo = t_info;
	return STATUS_SUCCESS;
}

#define  SESSION_BUFFER_SIZE 4 * 4096




KSTATUS CRecHelper::QueryCellInfo(PREC_GRAP_FULL_RECORD   rc, PCELL_FULL_INFO *pCellInfo)
{
	int                  nCounter, j, nIndex = 0, nClass;
	char                 IniPath[MAX_PATH], SessionName[40], str[40], va[512] = "", Jpg_Path[MAX_PATH];
	PCELL_DETAIL_INFO    d_info;




	if( rc->ReslutRelativePath[0] == 0 || PathFileExists(rc->ReslutRelativePath) == FALSE )
		return STATUS_INVALID_PARAMETERS;

	strcpy(IniPath, rc->ReslutRelativePath);
	PathAppend(IniPath,RESLUT_INI);

	if( PathFileExists(IniPath) == FALSE )
		return STATUS_NOT_FOUND_FILE;

	strcpy(Jpg_Path, rc->ReslutRelativePath);
	sprintf(SessionName, "%.3u_%.4u", rc->g_rc.rGrapSize, rc->g_rc.rIndex);
	PathAppend(Jpg_Path, SessionName);
	PathAddExtension(Jpg_Path, ".jpg");
	if( PathFileExists(Jpg_Path) == FALSE )
		return STATUS_NOT_FOUND_FILE;



	nCounter = GetPrivateProfileInt(SessionName, CELL_COUNT, 0, IniPath);
	if( nCounter == 0 )
		return STATUS_NOT_FOUND_VALUE;

	j = sizeof(CELL_FULL_INFO) + sizeof(CELL_DETAIL_INFO) * (nCounter - 1);

	*pCellInfo = (PCELL_FULL_INFO)FMMC_CreateBuffer(j);
	if( *pCellInfo == NULL )
		return STATUS_ALLOC_MEMORY_ERROR;

	ZeroMemory(*pCellInfo, j);
// 	//如果theApp.nAttribute == UsQCByN || theApp.nAttribute == UsQCByP就开启质控球识别
// 	if (rc->testType == 20)//测试类型为质控
// 	{
// 		QueryQCInfo(Jpg_Path, *pCellInfo);
// 		return STATUS_SUCCESS;
// 	}


	(*pCellInfo)->cCellCounter = nCounter;
	d_info = (*pCellInfo)->cInfo;
	
	for( j = 0 ; j < nCounter ; j ++ )
	{
		sprintf(str, "%u", j); 

		if( GetPrivateProfileString(SessionName, str, NULL, va, 512, IniPath) == 0 )
		{
			(*pCellInfo)->cCellCounter--;
			continue;
		}

		try
		{
			sscanf(va,"%d,%d,%d,%d,%d,%d",
				   &nClass, 
				   &d_info[nIndex].rc.left, 
				   &d_info[nIndex].rc.top,
				   &d_info[nIndex].rc.right,
				   &d_info[nIndex].rc.bottom,
				   &d_info[nIndex].score);

			d_info[nIndex].cType      = (USHORT)nClass;
			BuildIdentify(&d_info[nIndex]);
			nIndex ++;
		}
		catch(...)
		{
			(*pCellInfo)->cCellCounter--;
		}
	}
	return STATUS_SUCCESS;
}

KSTATUS  CRecHelper::ModifyCellInfo(PCHANGE_MODIFY_INFO   cModifyInfo, ULONG nModifyCellItem)
{
	int    cType;
	CHAR   IniPath[MAX_PATH], str[40], SessionName[40], va[512];


	if( cModifyInfo->rc_info.ReslutRelativePath[0] == 0 || PathFileExists(cModifyInfo->rc_info.ReslutRelativePath) == FALSE )
		return STATUS_INVALID_PARAMETERS;
	
	strcpy(IniPath, cModifyInfo->rc_info.ReslutRelativePath);
	PathAppend(IniPath,RESLUT_INI);
	if( PathFileExists(IniPath) == FALSE )
		return STATUS_NOT_FOUND_FILE;

	sprintf(SessionName, "%.3u_%.4u", cModifyInfo->rc_info.g_rc.rGrapSize, cModifyInfo->rc_info.g_rc.rIndex);
	sprintf(str, "%u", nModifyCellItem);
	cType = cModifyInfo->cInfo.cType;
	sprintf(va, "%d,%d,%d,%d,%d,%d", 
		    cType,
			cModifyInfo->cInfo.rc.left,
			cModifyInfo->cInfo.rc.top,
			cModifyInfo->cInfo.rc.right,
			cModifyInfo->cInfo.rc.bottom,
			cModifyInfo->cInfo.score);

	if( WritePrivateProfileString(SessionName, str, va, IniPath) == FALSE )
		return STATUS_WRITE_FAIL;

	return STATUS_SUCCESS;
}


KSTATUS CRecHelper::QueryQCInfo(char* pPath, PCELL_FULL_INFO pCellInfo)
{
// 	if (theApp.nAttribute == UsQCByN || theApp.nAttribute == UsQCByP)
// 	{
		IplImage* srcImg, *RoiImg;
		srcImg = cvLoadImage(pPath, 0);
		DBG_MSG("pPath %s\n", pPath);
		cvSmooth(srcImg, srcImg, CV_GAUSSIAN, 7, srcImg->nChannels);
		CvMemStorage* storage = cvCreateMemStorage(0);
		CvSeq* circles = 0;
		//第四个参数，double类型的dp，用来检测圆心的累加器图像的分辨率于输入图像之比的倒数，且此参数允许创建一个比输入图像分辨率低的累加器。上述文字不好理解的话，来看例子吧。例如，如果dp= 1时，累加器和输入图像具有相同的分辨率。如果dp=2，累加器便有输入图像一半那么大的宽度和高度。
		//第五个参数，double类型的minDist，为霍夫变换检测到的圆的圆心之间的最小距离，即让我们的算法能明显区分的两个不同圆之间的最小距离。这个参数如果太小的话，多个相邻的圆可能被错误地检测成了一个重合的圆。反之，这个参数设置太大的话，某些圆就不能被检测出来了。
		//第六个参数，double类型的param1，有默认值100。它是第三个参数method设置的检测方法的对应的参数。对当前唯一的方法霍夫梯度法CV_HOUGH_GRADIENT，它表示传递给canny边缘检测算子的高阈值，而低阈值为高阈值的一半。
		//第七个参数，double类型的param2，也有默认值100。它是第三个参数method设置的检测方法的对应的参数。对当前唯一的方法霍夫梯度法CV_HOUGH_GRADIENT，它表示在检测阶段圆心的累加器阈值。它越小的话，就可以检测到更多根本不存在的圆，而它越大的话，能通过检测的圆就更加接近完美的圆形了。
		//第八个参数，int类型的minRadius,有默认值0，表示圆半径的最小值。
		//第九个参数，int类型的maxRadius,也有默认值0，表示圆半径的最大值。

		circles = cvHoughCircles(srcImg, storage, CV_HOUGH_GRADIENT, 1, 40, 10, 38, 30, 65);

		pCellInfo->cCellCounter = circles->total;

		float* p = NULL;
		for (size_t i = 0; i < pCellInfo->cCellCounter; i++)
		{
			p = (float*)cvGetSeqElem(circles, i);
			if (p != NULL)
			{
				pCellInfo->cInfo->rc.left = (LONG)p[0] - (LONG)p[2];
				pCellInfo->cInfo->rc.right = (LONG)p[0] + (LONG)p[2];
				pCellInfo->cInfo->rc.top = (LONG)p[1] - (LONG)p[2];
				pCellInfo->cInfo->rc.bottom = (LONG)p[1] + (LONG)p[2];

				CvPoint pt = cvPoint(cvRound(p[0]), cvRound(p[1]));
				cvCircle(srcImg, pt, cvRound(p[2]), CV_RGB(255, 255, 255), 3);
			}
		}
		pCellInfo->cInfo->cType = 20;//质控球
		cvSaveImage(pPath, srcImg);
		cvReleaseMemStorage(&storage);

		cvReleaseImage(&srcImg);
/*	}*/
		return STATUS_SUCCESS;
}

KSTATUS  CRecHelper::DelRecResult(PDEL_REC_EX_INFO del_rec)
{
	char                      ResPath[MAX_PATH], SessionName[40], str[40], value_str[40];
	USHORT                    j, nIndex = 0;
	BOOLEAN                   bDeleteCell = TRUE;
	KSTATUS                   status;
	PCELL_FULL_INFO           cell_full_info;
	CHANGE_MODIFY_INFO        modify_info = {0};
	REC_GRAP_FULL_RECORD      rgf = {0};
	

	
	strcpy(modify_info.rc_info.ReslutRelativePath, del_rec->RelativePath);
	modify_info.rc_info.g_rc.rGrapSize = del_rec->rgr.rGrapSize;
	modify_info.rc_info.g_rc.rIndex    = del_rec->rgr.rIndex;

	if( del_rec->cdi.identify.i1.QuadPart == 0 && del_rec->cdi.identify.i2 == 0 )
		bDeleteCell = FALSE;
		
	
	if( bDeleteCell == TRUE )
	{
		status = QueryCellInfo(&modify_info.rc_info, &cell_full_info);
		if( status != STATUS_SUCCESS )
			return status;
	}
	else
		status = STATUS_SUCCESS;
	
	strcpy(ResPath, del_rec->RelativePath);
	PathAppend(ResPath,RESLUT_INI);
	sprintf(SessionName, "%.3u_%.4u", modify_info.rc_info.g_rc.rGrapSize, modify_info.rc_info.g_rc.rIndex);
	
	if( bDeleteCell == TRUE )
	{
		for( j = 0 ; j < cell_full_info->cCellCounter ; j ++ )
		{
			if( IDENTIFY_SAME(cell_full_info->cInfo[j].identify,del_rec->cdi.identify) )
			{
				cell_full_info->cInfo[j].cType = 0;
				break;
			}
		}
		
		if( j == cell_full_info->cCellCounter )
			return STATUS_INVALID_PARAMETERS;
		
		for( j ++ ; j < cell_full_info->cCellCounter ; j ++ )
		{
			memcpy(&modify_info.cInfo, &cell_full_info->cInfo[j], sizeof(CELL_DETAIL_INFO));
			ModifyCellInfo(&modify_info, j-1);
		}
		
		cell_full_info->cCellCounter --;
		if( cell_full_info->cCellCounter )
		{
			sprintf(str, "%u", cell_full_info->cCellCounter);
			WritePrivateProfileString(SessionName, str, NULL,  ResPath);
			sprintf(value_str, "%u", cell_full_info->cCellCounter);
			WritePrivateProfileString(SessionName, "Count", value_str,  ResPath);
		}
		else
			WritePrivateProfileString(SessionName, NULL, NULL, ResPath);
		
		FMMC_FreeBuffer(cell_full_info);
		return STATUS_SUCCESS;
	}
	else 
	{
		WritePrivateProfileString(SessionName, NULL, NULL, ResPath);	
		PathRemoveFileSpec(ResPath);
		PathAppend(ResPath,SessionName);
		PathAddExtension(ResPath, ".jpg");
		DeleteFile(ResPath);
		return STATUS_SUCCESS;
	}

}

KSTATUS  CRecHelper::GetGrapFileName(PREC_GRAP_FULL_RECORD  grap_rc, PCHAR  pGrapFileName)
{
	sprintf(pGrapFileName, "%.3u_%.4u.jpg", grap_rc->g_rc.rGrapSize, grap_rc->g_rc.rIndex);
	return STATUS_SUCCESS;
}


KSTATUS CRecHelper::DelRecTaskInfo( PDEL_REC_EX_INFO del_rec )
{
	char  cmd_info[1024] = "", short_path[MAX_PATH] = "";

	GetShortPathName(del_rec->RelativePath, short_path, MAX_PATH);
	sprintf(cmd_info, "cmd.exe /c del %s /Q", short_path);
	WinExec(cmd_info, SW_HIDE);
	return STATUS_SUCCESS;
}


KSTATUS  CRecHelper::DelRecGrap(PDEL_REC_EX_INFO   del_rec)
{
	
	if( del_rec->RelativePath[0] == 0 || PathFileExists(del_rec->RelativePath) == FALSE )
		return STATUS_INVALID_PARAMETERS;
	
	if( del_rec->bDelAll == TRUE )
		return DelRecTaskInfo(del_rec);

	return DelRecResult(del_rec);

	
}

KSTATUS  CRecHelper::AddRecCellGrap(PCHANGE_MODIFY_INFO  pChangeModifyInfo)
{
    char                 ResPath[MAX_PATH], SessionName[40],  value_str[40];
	ULONG                nItemIndex = 0;
	KSTATUS              status;
	PCELL_FULL_INFO      cell_full_info;

	if( pChangeModifyInfo->rc_info.ReslutRelativePath[0] == 0 || PathFileExists(pChangeModifyInfo->rc_info.ReslutRelativePath) == FALSE || VAILD_RECT(pChangeModifyInfo->cInfo.rc) == FALSE )
		return STATUS_INVALID_PARAMETERS;

	status = QueryCellInfo(&pChangeModifyInfo->rc_info, &cell_full_info);
	if( status != STATUS_SUCCESS )
	{
		if( status != STATUS_NOT_FOUND_VALUE )
			return status;

	}
	else
	{
		nItemIndex = cell_full_info->cCellCounter;
		FMMC_FreeBuffer(cell_full_info);
	}

	strcpy(ResPath, pChangeModifyInfo->rc_info.ReslutRelativePath);
	PathAppend(ResPath,RESLUT_INI);
	sprintf(SessionName, "%.3u_%.4u", pChangeModifyInfo->rc_info.g_rc.rGrapSize, pChangeModifyInfo->rc_info.g_rc.rIndex);
	sprintf(value_str, "%u", nItemIndex+1);
	WritePrivateProfileString(SessionName, "Count", value_str,  ResPath);

	status = ModifyCellInfo(pChangeModifyInfo, nItemIndex);
	if( status == STATUS_SUCCESS )
		BuildIdentify(&pChangeModifyInfo->cInfo);

	return status;
}

void  CRecHelper::BuildIdentify(PCELL_DETAIL_INFO  CellDetailInfo)
{
	CellDetailInfo->identify.i1.u.LowPart  = ((CellDetailInfo->rc.bottom) << 16) | CellDetailInfo->rc.top;
	CellDetailInfo->identify.i1.u.HighPart = ((CellDetailInfo->rc.right) << 16) | CellDetailInfo->rc.left;
	CellDetailInfo->identify.i2            = (UCHAR)CellDetailInfo->cType;
	
}

KSTATUS CRecHelper::ModifyCellInfoEntry(PCHANGE_MODIFY_INFO   cModifyInfo)
{
	ULONG                nItemIndex;
	KSTATUS              status;
	PCELL_FULL_INFO      cell_full_info;

	if( VAILD_RECT(cModifyInfo->cInfo.rc) == FALSE )
		return STATUS_INVALID_PARAMETERS;

	status = QueryCellInfo(&cModifyInfo->rc_info, &cell_full_info);
	if( status != STATUS_SUCCESS )
		return status;

	for( nItemIndex = 0 ; nItemIndex < cell_full_info->cCellCounter ; nItemIndex ++ )
	{
		if( IDENTIFY_SAME(cell_full_info->cInfo[nItemIndex].identify, cModifyInfo->cInfo.identify) )
			break;
	}
	
	if( nItemIndex == cell_full_info->cCellCounter )
		status = STATUS_NOT_FOUND_VALUE;

	FMMC_FreeBuffer(cell_full_info);

	if( status != STATUS_SUCCESS )
		return status;

	status = ModifyCellInfo(cModifyInfo, nItemIndex);
	if( status == STATUS_SUCCESS )
		BuildIdentify(&cModifyInfo->cInfo);

	return status;
}


CRecHelper::CRecHelper()
{
	
}
   
CRecHelper::~CRecHelper()
{
	
}

