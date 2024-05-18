// Thumbnail1.cpp : 实现文件
//

#include "stdafx.h"

#include "thumbnail.h"
#include "Thumbnail1.h"

#include "UserInfoDlg.h"	
#include "PicPreviewWnd.h"

#include <algorithm>

#include <gdiplus.h>
using namespace Gdiplus;

#define	THUMBNAIL_WIDTH		100
#define	THUMBNAIL_HEIGHT	75

#define NO_GRAP_INDEX       0XFFFFFFFF

#define ID_DELETE_CELL_1    0X9001


void SplitString(const std::string& s, const std::string& c, std::vector<std::string>& v, bool bSplitOnce)
{
	std::string::size_type pos1, pos2;
	pos2 = s.find(c);
	pos1 = 0;
	if (bSplitOnce)
	{
		if (pos2 == string::npos)
		{
			v.push_back(s);
			//v.push_back("");
		}
		else
		{
			v.push_back(s.substr(pos1, pos2 - pos1));
			v.push_back(s.substr(pos2));
		}
	}
	else
	{
		while (std::string::npos != pos2)
		{
			v.push_back(s.substr(pos1, pos2 - pos1));

			pos1 = pos2 + c.size();
			pos2 = s.find(c, pos1);
		}
		if (pos1 != s.length())
			v.push_back(s.substr(pos1));
	}
}


// CThumbnail1 对话框

static  CThumbnail1  *g_th = NULL;


LRESULT 
CALLBACK 
ListGroupWndProc(
				 HWND hwnd, 
				 UINT uMsg, 
				 WPARAM wParam, 
				 LPARAM lParam 
				 )
{

	if( uMsg == WM_COMMAND )
	{
		DBG_MSG("ListGroupWndProc WM_COMMAND g_th = %x\n", g_th);
		g_th->MenuCommand(LOWORD(wParam));
	}

	//DBG_MSG("ListGroupWndProc g_th = %x, g_th->m_OldWndProc = %x\n", g_th, g_th->m_OldWndProc);

	return CallWindowProc(g_th->m_OldWndProc, hwnd, uMsg, wParam, lParam);
}


IMPLEMENT_DYNAMIC(CThumbnail1, CDialog)

CThumbnail1::CThumbnail1(CWnd* pParent /*=NULL*/)
	: CDialog(CThumbnail1::IDD, pParent)
	, m_strImageDir(_T(""))	
{	
	m_nID = 0;
	ZeroMemory(&m_AssoicateDC, sizeof(m_AssoicateDC));
	m_OldWndProc      = NULL;
	m_ptask_rec_info  = NULL;
	m_cell_full_info  = NULL;
	m_CurSelCellIndex = -1;
	m_RectTracker     = NULL;
	m_AddMode         = FALSE;
	m_nSelectedItem   = 0;
	m_GrapIndex       = NO_GRAP_INDEX;
	m_CellTypeArray   = NULL;
	m_CellTypeCounter = 0;	
	memset(&m_ListGInfo,0,sizeof(LIST_GROUP_INFO));
	ZeroMemory(&m_CellIdentify, sizeof(m_CellIdentify));
	ZeroMemory(m_ImageFolder, sizeof(m_ImageFolder));
	ZeroMemory(m_TaskPath, sizeof(m_TaskPath));
	ZeroMemory(&get_us_info, sizeof(get_us_info));	
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CThumbnail1::~CThumbnail1()
{
	if( m_gdiplusToken )
		GdiplusShutdown(m_gdiplusToken);

	m_gdiplusToken = 0;

	if( m_ptask_rec_info )
		THInterface_FreeSysBuffer(m_ptask_rec_info);
	m_ptask_rec_info = NULL;

	if( m_AssoicateDC.hMemDC )
		m_rec_task.DestoryAssoicateDC(m_AssoicateDC);

	ZeroMemory(&m_AssoicateDC, sizeof(m_AssoicateDC));

	if( m_cell_full_info )	
		m_rec_task.FreeCellFullInfo(m_cell_full_info);	
	m_cell_full_info = NULL;

	if( m_CellTypeCounter && m_CellTypeArray )
		free(m_CellTypeArray);

	m_CellTypeArray   = NULL;
	m_CellTypeCounter = 0;

	RestoreNormalMode();

	if(m_MenuDisp_Des)
		m_MenuDisp_Des = NULL;
	if(m_MenuCmd_Des)
		m_MenuCmd_Des = NULL;
	pen.DeleteObject();
		
	if(m_ParentWnd)
		m_ParentWnd =NULL;
	
}

void CThumbnail1::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LIST_THUMB, m_ListThumbnail);

	DDX_Control(pDX, IDC_IMAGE_RECT, m_ImageRect);
	DDX_Text(pDX, IDC_EDIT_PATH, m_strImageDir);
}


BEGIN_MESSAGE_MAP(CThumbnail1, CDialog)
	ON_COMMAND(ID_CELLNEW_NEW, &CThumbnail1::OnCellNew)
	ON_COMMAND(ID_POP_RBC, &CThumbnail1::OnPopRbc)
	ON_COMMAND(ID_POP_WBC, &CThumbnail1::OnPopWbc)
	ON_COMMAND(ID_POP_EPITHELIALCELL, &CThumbnail1::OnPopEpithelialcell)
	ON_COMMAND(ID_POP_CASTS, &CThumbnail1::OnPopCasts)
	ON_COMMAND(ID_POP_C, &CThumbnail1::OnPopC)
	ON_COMMAND(ID_POP_MUCUS, &CThumbnail1::OnPopMucus)
	ON_COMMAND(ID_POP_BACTREIA, &CThumbnail1::OnPopBactreia)
	ON_COMMAND(ID_POP_BYST, &CThumbnail1::OnPopByst)


	ON_COMMAND(ID_DELETE_CELL_1, &CThumbnail1::OnPopDelete)

	ON_COMMAND_RANGE(ID_AC_CELL_FRIST, ID_AC_CELL_LAST,&CThumbnail1::OnACCell)

	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	
	ON_WM_MOUSEWHEEL()
	ON_WM_SETCURSOR()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_THUMB, &CThumbnail1::OnLvnItemchangedListThumb)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_THUMB, &CThumbnail1::OnLvnKeydownListThumb)
	ON_NOTIFY(NM_CLICK, IDC_LIST_THUMB, &CThumbnail1::OnNMClickListThumb)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_THUMB, &CThumbnail1::OnNMRClickListThumb)
	ON_WM_LBUTTONUP()
	ON_WM_DESTROY()
	ON_STN_CLICKED(IDC_IMAGE_RECT, &CThumbnail1::OnStnClickedImageRect)
	ON_WM_QUERYDRAGICON()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_COMMAND(ID_ADD_PRINTPIC, &CThumbnail1::OnAddPrintpic)
	ON_COMMAND(ID_DEL_PRINTPIC, &CThumbnail1::OnDelPrintpic)
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()


// CThumbnail1 消息处理程序

std::string GetIdsString(std::string sMain,std::string sId)
{
	char sModuleFile[MAX_PATH]={0};
	char sDrive[MAX_PATH]={0};
	char sDir[MAX_PATH]={0};

	GetModuleFileName(NULL, sModuleFile, MAX_PATH);

	_splitpath(sModuleFile, sDrive, sDir, NULL,NULL);

	char s_inifile[MAX_PATH]={0};

	sprintf(s_inifile,"%s%s%d.ini",sDrive,sDir,2052);


	std::string str;
	TCHAR szLine[MAX_PATH*8] = {0};

	if( 0 !=GetPrivateProfileString(sMain.c_str(),
		sId.c_str(),
		_T("0"),
		szLine,
		MAX_PATH*8,
		s_inifile))
	{
		str = szLine;
	}
	else
	{
		str = "";
	}

	return str;
}

BOOL CThumbnail1::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

//	_asm int 3
	//BeginWaitCursor();	

	CloneCellType(m_ListGInfo.gn, m_ListGInfo.gCounter);

	KSTATUS     status= STATUS_SUCCESS;

	CONFIG_RECORD_INFO ConfigInfo={0};
	ConfigInfo.nRecordType = US_TYPE_INFO;

	ConfigInfo.nConfigLength = sizeof(GET_US_INFO);

	ConfigInfo.pConfigConent = new GET_US_INFO;

	/*if(ConfigInfo.pConfigConent == NULL)
		return FALSE;*/
	SetConfigIniPath(NULL);
	status = QueryRecordConfig(&ConfigInfo);
	if(status != STATUS_SUCCESS)
	{
		DBG_MSG("status = %x\n",status);
		CString str;
		str.Format("%s",GetIdsString("USCfgSet","USCfgQueryErr").c_str());
		AfxMessageBox(str);
		if(ConfigInfo.pConfigConent != NULL)
			delete[]ConfigInfo.pConfigConent;
		ConfigInfo.pConfigConent = NULL;
		//return FALSE;
	}
	memcpy(&get_us_info,ConfigInfo.pConfigConent,sizeof(GET_US_INFO));

	if(ConfigInfo.pConfigConent != NULL)
		delete[]ConfigInfo.pConfigConent;
	ConfigInfo.pConfigConent = NULL;
		
	UpdateTaskPath(m_nID);	

	status = m_rec_task.GetRecGrapTaskInfoWhenFinish((PCHAR)m_TaskPath,m_ptask_rec_info);

	if(status != STATUS_SUCCESS || (m_ptask_rec_info && m_ptask_rec_info->tGrapCounter == 0) )
	{
		if( m_ptask_rec_info )
			THInterface_FreeSysBuffer(m_ptask_rec_info);
		m_ptask_rec_info = NULL;

		CDialog::OnOK();
		return TRUE;
		
	}

	GetWhetherShowOther();

	m_ImageListThumb.Create(THUMBNAIL_WIDTH, THUMBNAIL_HEIGHT, ILC_COLOR24, 0, 1);
	m_ListThumbnail.SetImageList(&m_ImageListThumb, LVSIL_NORMAL);
	SetWindowText(GetIdsString("thumbnaildlg","title").c_str());

	m_ImageRect.GetWindowRect(&m_DrawRect);
	
	ScreenToClient(&m_DrawRect);

	ResizeWindow(&m_picsize);

	pen.CreatePen(PS_DOT,1,RGB(220,0,0));
	OnBtnLoad();

//	_asm int 3
	g_th = this;

	m_OldWndProc = (WNDPROC)SetWindowLong(GetSafeHwnd(), GWL_WNDPROC, (LONG)ListGroupWndProc);

	//EndWaitCursor();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CThumbnail1::OnBtnLoad()
{
	UpdateData(TRUE);
	// show hour glass cursor
	BeginWaitCursor();

	// get the names of bitmap files
	if ( !GetImageFileNames() )
	{
		
		EndWaitCursor();
		return;
	}

	char szPrintImages[255] = { 0 };
	::SendMessage(m_ParentWnd, WM_GET_PRINT_PATH, DWORD(szPrintImages), DWORD(m_nID));
	vector<string> tmpVector;
	SplitString(szPrintImages, "|", tmpVector, false);
	//for (auto itr = tmpVector.begin(); itr != tmpVector.end(); itr++)
	for (auto item : tmpVector)
	{
		if (!item.empty())
		{
			m_VectorPrintImages.push_back(item.c_str());
		}
	}

	// draw thumbnail images in list control
	DrawThumbnails();



	if( m_GrapIndex != NO_GRAP_INDEX && (m_CellIdentify.i1.QuadPart || m_CellIdentify.i2) )
	{
		char    szFileName[MAX_PATH], tFileName[MAX_PATH];
		KSTATUS status;

		DBG_MSG(" THInterface_GetRecFileName....\n");

		/*status = THInterface_GetRecFileName(m_GrapIndex,
			40,
			(PCHAR)szFileName);*/


		ULONG  nGrapMask = MAKE_REC_FILE_NAME(m_GrapIndex,40);
		status = THInterface_QueryDevCaps(T_DEV_REC_TYPE,RecGrapFileName,(PCHAR)&nGrapMask ,sizeof(ULONG),(PCHAR)szFileName,MAX_PATH,0);
		if( status != STATUS_SUCCESS )
			goto default_load;

		for( ULONG j = 0 ; j < m_ListThumbnail.GetItemCount() ; j ++ )
		{
			m_ListThumbnail.GetItemText(j, 0, tFileName, MAX_PATH);

			if( strcmp(szFileName, tFileName) == 0 )
			{
				m_nSelectedItem = j;

				//				m_ListThumbnail.SetFocus();
				m_ListThumbnail.SetItemState(j, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
				m_ListThumbnail.EnsureVisible(j, TRUE);

				break;
			}
		}

		LoadGrapAndCellInfo();		

		for( ULONG j = 0 ; j < m_cell_full_info->cCellCounter ; j ++ )
		{
			if( memcmp(&m_cell_full_info->cInfo[j].identify, &m_CellIdentify, sizeof(CELL_IDENTIFY)) == 0 )
			{
				m_CurSelCellIndex = j;
				break;
			}
		}
	}
	else
	{
default_load:
		LoadGrapAndCellInfo();
		//		m_ListThumbnail.SetFocus();
		m_ListThumbnail.SetItemState(0, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);	
	}
	// draw the selected image in its full size
	DrawSelectedImage();

	// set focus and select the first thumbnail in the list control


	EndWaitCursor();


}

BOOL CThumbnail1::GetImageFileNames()
{
	CString	strExt;
	CString	strName;
	CString	strPattern;
	BOOL	bRC = TRUE;

	std::vector<CString>	VectorImageNames;

	if(m_ptask_rec_info != NULL)
	{

		
		for(int i = 0 ; i < m_ptask_rec_info->tGrapCounter ; i ++)
		{
			KSTATUS status = STATUS_SUCCESS;
			char szFileName[MAX_PATH]={0};

		/*	status = THInterface_GetRecFileName(m_ptask_rec_info->gInfo[i].rIndex,
				m_ptask_rec_info->gInfo[i].rGrapSize,
				(PCHAR)szFileName)*/;

			ULONG  nGrapMask = MAKE_REC_FILE_NAME(m_ptask_rec_info->gInfo[i].rIndex,
				m_ptask_rec_info->gInfo[i].rGrapSize);
			
			status = THInterface_QueryDevCaps(T_DEV_REC_TYPE,RecGrapFileName,(PCHAR)&nGrapMask ,sizeof(ULONG),(PCHAR)szFileName,MAX_PATH,0);
			


			if(status == STATUS_SUCCESS)
			{
				//char szFullName[MAX_PATH]={0};
				//sprintf(szFullName,"%s\\%s",m_full_task_info_1.MainInfo1.sImageFoler,szFileName);
				VectorImageNames.push_back(szFileName);
			}
		}
	}


	// update the names, if any
	if ( !VectorImageNames.empty() )
	{
		// reset the image name vector
		m_VectorImageNames.clear();
		m_VectorImageNames = VectorImageNames;
		return TRUE;
	}

	return FALSE;

}

void CThumbnail1::DrawThumbnails()
{
	CBitmap*    pImage = NULL;
	HBITMAP		hBmp = NULL;
	POINT		pt;
	CString		strPath;
	int			i;

	// no images
	if (m_VectorImageNames.empty())
		return;

	// sort and comb
	/*std::sort(m_VectorImageNames.begin(), m_VectorImageNames.end());
	std::sort(m_VectorPrintImages.begin(), m_VectorPrintImages.end());
	std::vector<CString> tmpVec;
	std::set_difference(m_VectorImageNames.begin(), m_VectorImageNames.end(), m_VectorPrintImages.begin(), m_VectorPrintImages.end(), std::inserter(tmpVec, tmpVec.begin()));
	m_VectorImageNames.clear();
	std::merge(m_VectorPrintImages.begin(), m_VectorPrintImages.end(), tmpVec.begin(), tmpVec.end(), std::back_inserter(m_VectorImageNames));
	m_VectorImageNames = m_VectorPrintImages;
	for (auto i : tmpVec)
	{
		m_VectorImageNames.push_back(i);
	}*/

	// set the length of the space between thumbnails
	// you can also calculate and set it based on the length of your list control
	int nGap = 6;

	// hold the window update to avoid flicking
	m_ListThumbnail.SetRedraw(FALSE);

	// reset our image list
	for (i = 0; i<m_ImageListThumb.GetImageCount(); i++)
		m_ImageListThumb.Remove(i);	

	// remove all items from list view
	if (m_ListThumbnail.GetItemCount() != 0)
		m_ListThumbnail.DeleteAllItems();

	// set the size of the image list
	m_ImageListThumb.SetImageCount(m_VectorImageNames.size());
	i = 0;

	// draw the thumbnails
	std::vector<CString>::iterator	iter;
	for (iter = m_VectorImageNames.begin(); iter != m_VectorImageNames.end(); iter++)
	{	
		
		// load the bitmap
		strPath.Format( TEXT("%s\\%s"), m_TaskPath, *iter );
		//strPath = *iter;

		USES_CONVERSION;
		Bitmap img( A2W(strPath) );
		Bitmap* pThumbnail = static_cast<Bitmap*>(img.GetThumbnailImage(THUMBNAIL_WIDTH, THUMBNAIL_HEIGHT, NULL, NULL));

		if (std::find(m_VectorPrintImages.begin(), m_VectorPrintImages.end(), *iter) != m_VectorPrintImages.end())
		{
			Gdiplus::Graphics g(pThumbnail);
			FontFamily fontFamily(L"宋体");
			Gdiplus::Font font(&fontFamily, 18, FontStyleBold, UnitPixel);
			StringFormat stringformat;
			//stringformat.SetAlignment(StringAlignmentCenter);
			//stringformat.SetLineAlignment(StringAlignmentCenter); 
			stringformat.SetAlignment(StringAlignmentFar);
			stringformat.SetLineAlignment(StringAlignmentNear);
			SolidBrush brush(Color(255, 255, 0, 0));
			g.SetTextRenderingHint(TextRenderingHintAntiAlias);
			g.DrawString(L"打印", -1, &font, RectF(0, 0, THUMBNAIL_WIDTH, THUMBNAIL_HEIGHT), &stringformat, &brush);
		}

		// attach the thumbnail bitmap handle to an CBitmap object
		pThumbnail->GetHBITMAP(NULL, &hBmp);
		pImage = new CBitmap();		 
		pImage->Attach(hBmp);

		// add bitmap to our image list
		m_ImageListThumb.Replace(i, pImage, NULL);

		// put item to display
		// set the image file name as item text
		m_ListThumbnail.InsertItem(i, m_VectorImageNames[i], i);

		// get current item position	 
		m_ListThumbnail.GetItemPosition(i, &pt);	 

		// shift the thumbnail to desired position
		pt.x = nGap + i*(THUMBNAIL_WIDTH + nGap);
		m_ListThumbnail.SetItemPosition(i, pt);
		i++;

		delete pImage;
		delete pThumbnail;
	}

	// let's show the new thumbnails
	m_ListThumbnail.SetRedraw(); 

}

KSTATUS CThumbnail1::LoadGrapAndCellInfo()
{
	CString		        strPath;
	KSTATUS             status;
	PCELL_FULL_INFO_EX  tCellFullInfo;
	

	strPath.Format( TEXT("%s\\%s"),
		m_TaskPath, 
		m_VectorImageNames[m_nSelectedItem] );

	m_CurSelCellIndex = -1;


	if( m_AssoicateDC.hMemDC )
	{
		m_rec_task.DestoryAssoicateDC(m_AssoicateDC);
		ZeroMemory(&m_AssoicateDC, sizeof(m_AssoicateDC));
	}

	status = m_rec_task.CreateAssociateDC(strPath.GetBuffer(), m_AssoicateDC);
	if( status != STATUS_SUCCESS )
		return status;

	

	status = m_rec_task.GetPickupRecCellGrap((PCHAR)m_TaskPath,
		m_ptask_rec_info->gInfo[m_nSelectedItem].rIndex,
		m_ptask_rec_info->gInfo[m_nSelectedItem].rGrapSize,
		tCellFullInfo,m_CellTypeCounter,m_CellTypeArray,FALSE);

	if( status != STATUS_SUCCESS )
	{
		m_rec_task.DestoryAssoicateDC(m_AssoicateDC);
		ZeroMemory(&m_AssoicateDC, sizeof(m_AssoicateDC));

		
		return status;
	}

	if( m_cell_full_info )
		m_rec_task.FreeCellFullInfo(m_cell_full_info);

	m_cell_full_info = tCellFullInfo;
	return status;

}

void CThumbnail1::DrawSelectedImage()
{
	/*if( m_ptask_rec_info == NULL || m_cell_full_info == NULL )
	{
		return;
	}	

	m_rec_task.DrawPickupRecDC(this->m_hWnd,m_AssoicateDC,&m_DrawRect);

	
	DrawCellText();
	DrawSelectRC();

	if( m_RectTracker )
	{
		HDC  hDC = ::GetDC(m_hWnd);
		m_RectTracker->Draw(CDC::FromHandle(hDC),&pen);
		::ReleaseDC(m_hWnd, hDC);
	}*/

	
	m_rec_task.DrawPickupRecDC(this->m_hWnd,m_AssoicateDC,&m_DrawRect);
	m_fx = (float)(m_DrawRect.right-m_DrawRect.left)/(float)(m_AssoicateDC.rc.right-m_AssoicateDC.rc.left);//增加细胞位置偏移量x 20180730
	m_fy = (float)(m_DrawRect.bottom-m_DrawRect.top)/(float)(m_AssoicateDC.rc.bottom-m_AssoicateDC.rc.top);//增加细胞位置偏移量y
	
	if( m_RectTracker )
	{
		HDC  hDC = ::GetDC(m_hWnd);
		m_RectTracker->Draw(CDC::FromHandle(hDC),&pen);
		::ReleaseDC(m_hWnd, hDC);
	}

	if( m_ptask_rec_info == NULL || m_cell_full_info == NULL )
		return;

	DrawCellText();
	DrawSelectRC();

}

void CThumbnail1::DrawCellText()
{
	int      r, iMode;
	HDC      hDC;
	char     txt[100] = "";
	RECT     rc;
	SIZE     sz;
	HBRUSH   hBrush, hOldBrush;

	hDC   = ::GetDC(m_hWnd);
	iMode =   SetBkMode(hDC,TRANSPARENT);

	hBrush     =  (HBRUSH)GetStockObject(NULL_BRUSH);
	hOldBrush  =  (HBRUSH)SelectObject(hDC, hBrush);	

	for(ULONG j = 0; j< m_cell_full_info->cCellCounter ; j++)
	{

		int k = 0;

		/*if( CellDisableDisaply(m_cell_full_info->cInfo[j].cType) )
			continue;*/		
		
		if( (m_cell_full_info->cInfo[j].cType == CELL_OTHER_TYPE) &&  (m_nIsShowOther == 0) )
		{ 
			continue;
		}

		for(ULONG i = 0; i < m_ListGInfo.gCounter;i++)
			{				
				if(m_cell_full_info->cInfo[j].cType == m_ListGInfo.gn[i].nCellType)
				{					
					if(m_cell_full_info->cInfo[j].cType >= CELL_RED_TYPE && m_cell_full_info->cInfo[j].cType <= (CELL_RED_TYPE+44))
					{						
						k = m_cell_full_info->cInfo[j].cType - 1;						
						
						r = ::SetTextColor(hDC,RGB(atoi(get_us_info.par[k].m_CellNameColoer_R),atoi(get_us_info.par[k].m_CellNameColoer_G),atoi(get_us_info.par[k].m_CellNameColoer_B)));
					}
					else
					{					
						r = ::SetTextColor(hDC,RGB(0,255,0));							
					}		

					strcpy(txt,m_ListGInfo.gn[i].GroupName);
					break;					
				}
			}	
		
		CopyRect(&rc, &m_cell_full_info->cInfo[j].rc);
		rc.left *=m_fx;//20180730增加偏移量                             
		rc.right *=m_fx;
		rc.top  *=m_fy;
		rc.bottom*=m_fy;
		rc.left  += m_DrawRect.left;
		rc.right += m_DrawRect.left;
		rc.top   += m_DrawRect.top;
		rc.bottom += m_DrawRect.top;
		GetTextShowLoc(hDC,rc, txt, sz);	

		TextOut(hDC, 
			sz.cx,
			sz.cy,
			txt, 
			strlen(txt));

		::SetTextColor(hDC,r);

		if(get_us_info.bIsShowFrame == true)
		{		
			Rectangle(hDC, rc.left, rc.top, rc.right, rc.bottom);
		}		
	}

	SetBkMode(hDC, iMode);
	SelectObject(hDC, hOldBrush);
	DeleteObject(hBrush);
	::ReleaseDC(m_hWnd, hDC);

}

void CThumbnail1::DrawSelectRC()
{
	HDC      hDC;
	HPEN     hPen = CreatePen(PS_SOLID,5,RGB(255,0,0)), hOldPen;
	HBRUSH   hOldBrush, hBrush;

	if( m_ptask_rec_info == NULL || m_cell_full_info == NULL || m_CurSelCellIndex == -1 )
		return;

	hDC        = ::GetDC(m_hWnd);
	hBrush     =  (HBRUSH)GetStockObject(NULL_BRUSH);
	hOldBrush  =  (HBRUSH)SelectObject(hDC, hBrush);
	hOldPen    =  (HPEN)SelectObject(hDC, hPen);

	/*if( CellDisableDisaply(m_cell_full_info->cInfo[m_CurSelCellIndex].cType) )
		goto lc;*/

	if( (m_cell_full_info->cInfo[m_CurSelCellIndex].cType == CELL_OTHER_TYPE) && (m_nIsShowOther == 0)  )
	{

	}
	else
		Rectangle(hDC, 
		m_cell_full_info->cInfo[m_CurSelCellIndex].rc.left*m_fx + m_DrawRect.left,
		m_cell_full_info->cInfo[m_CurSelCellIndex].rc.top*m_fy + m_DrawRect.top, 
		m_cell_full_info->cInfo[m_CurSelCellIndex].rc.right*m_fx + m_DrawRect.left, 
		m_cell_full_info->cInfo[m_CurSelCellIndex].rc.bottom*m_fy + m_DrawRect.top);

	DBG_MSG("\nDrawSelectRC  %f,%f",m_cell_full_info->cInfo[m_CurSelCellIndex].rc.left*m_fx + m_DrawRect.left,m_cell_full_info->cInfo[m_CurSelCellIndex].rc.right*m_fx + m_DrawRect.left);

lc:
	SelectObject(hDC, hOldBrush);
	SelectObject(hDC, hOldPen);
	DeleteObject(hBrush);
	DeleteObject(hPen);
	::ReleaseDC(m_hWnd, hDC);

}

void CThumbnail1::GetTextShowLoc( HDC hDC, RECT &rc, PCHAR pStr, SIZE &off_loc )
{
	int   nLen = strlen(pStr), cx;

	SIZE  sz;

	GetTextExtentPoint(hDC, pStr, nLen, &sz);


	if( rc.top - sz.cy >= m_DrawRect.top )
		off_loc.cy = rc.top - sz.cy;
	else
		off_loc.cy = rc.bottom + 3;

	/*if( rc.left - sz.cx >= m_DrawRect.left )
	off_loc.cx = rc.left - sz.cx;
	else
	off_loc.cx = rc.right + sz.cx;*/

	cx = rc.right - rc.left;
	if( cx >= sz.cx )
	{
		cx -= sz.cx;
		cx >>= 1;
		off_loc.cx = rc.left + cx;
	}
	else
	{
		cx = sz.cx - cx;
		cx >>= 1;
		off_loc.cx = rc.left - cx;
	}

	if( off_loc.cx < m_DrawRect.left )
		off_loc.cx = m_DrawRect.left;
	else if( off_loc.cx + sz.cx > m_DrawRect.right )
		off_loc.cx = m_DrawRect.right - sz.cx;

}

BOOL CThumbnail1::SetSelectInfo( ITEM_CELL_INFO* pSelectPicInfo )
{
	memcpy(&m_CellIdentify, &pSelectPicInfo->identify, sizeof(m_CellIdentify));
	m_GrapIndex   = (int)pSelectPicInfo->nGrapIndex;
	DBG_MSG("m_GrapIndex = %d\n",m_GrapIndex);
	return TRUE;

}

BOOL CThumbnail1::SetImageSize(unsigned int nID,PSIZE PicSize )
{
	
	m_nID = nID;	
	
	m_picsize.cx = PicSize->cx;
	m_picsize.cy = PicSize->cy;	
	
	return TRUE;	

}

void CThumbnail1::OnACCell( UINT nID )
{
	/*int   nCellType = nID - ID_AC_CELL_FRIST + CELL_RED_TYPE;

	if( nCellType >= CELL_RED_TYPE && nCellType <= (CELL_RED_TYPE + 44) )
		CellDothing(nCellType);*/

}

void CThumbnail1::OnCellNew()
{
	// TODO: 在此添加命令处理程序代码
	

	CreateNewMyTracker();
}

void CThumbnail1::OnPopRbc()
{
	// TODO: 在此添加命令处理程序代码
}

void CThumbnail1::OnPopWbc()
{
	// TODO: 在此添加命令处理程序代码
}

void CThumbnail1::OnPopEpithelialcell()
{
	// TODO: 在此添加命令处理程序代码
}

void CThumbnail1::OnPopCasts()
{
	// TODO: 在此添加命令处理程序代码
}

void CThumbnail1::OnPopC()
{
	// TODO: 在此添加命令处理程序代码
}

void CThumbnail1::OnPopMucus()
{
	// TODO: 在此添加命令处理程序代码
}

void CThumbnail1::OnPopBactreia()
{
	// TODO: 在此添加命令处理程序代码
}

void CThumbnail1::OnPopByst()
{
	// TODO: 在此添加命令处理程序代码
}

void CThumbnail1::OnPopDelete()
{
	// TODO: 在此添加命令处理程序代码
	if( m_AddMode == TRUE )
	{
		
		RestoreNormalMode();
	}
		
	else
	{
		
		CELL_DETAIL_INFO ci = {0};

		if( m_CurSelCellIndex == -1 )
			return;

		/*if( m_cell_full_info->cInfo[m_CurSelCellIndex].cType == CELL_OTHER_TYPE )
		{
			if( DeleteOthCell() == FALSE )
				return;

			goto sub_cell;
		}*/

		memcpy(&ci, &m_cell_full_info->cInfo[m_CurSelCellIndex], sizeof(ci) - 8);
		
		//::SendMessage(m_ParentWnd,WM_SELECT_CELL,DWORD(&ci),DWORD(&m_ptask_rec_info->gInfo[m_nSelectedItem]));	
		
sub_cell:
		m_cell_full_info->cCellCounter --;
		if( m_cell_full_info->cCellCounter == 0 )
		{
			/*m_rec_task.FreeCellFullInfo(m_cell_full_info);
			m_cell_full_info = NULL;*/
			ZeroMemory(m_cell_full_info,sizeof(PCELL_FULL_INFO_EX));
			
			m_CurSelCellIndex = -1;
		}
		else
		{
			memcpy(&m_cell_full_info->cInfo[m_CurSelCellIndex], &m_cell_full_info->cInfo[m_CurSelCellIndex+1], sizeof(CELL_DETAIL_INFO_EX) * (m_cell_full_info->cCellCounter - m_CurSelCellIndex));
			m_CurSelCellIndex = -1;
		}
	}

	DrawSelectedImage();
}

void CThumbnail1::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	BOOL  bResult;

	/*CDialog::OnLButtonDown(nFlags, point);*/

	if( m_AddMode == FALSE )
	{
		if( PtInRect(&m_DrawRect, point) == FALSE || HitTest(point) == TRUE )
			return;
	}
	else
	{
		if( m_RectTracker )
		{
			bResult = m_RectTracker->HitTest (point) != CRectTracker::hitNothing;	
			if (bResult)
			{
				m_RectTracker->Track (this, point, FALSE);
				DrawSelectedImage();
			}
		}
	}

	CDialog::OnLButtonDown(nFlags, point);
}

void CThumbnail1::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDialog::OnMouseMove(nFlags, point);
}

void CThumbnail1::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	BOOL  bMenuStatus = FALSE;
	POINT pt;
	CMenu top, *pMenu;

	pt.x = point.x;
	pt.y = point.y;	
	if( m_AddMode == FALSE )
	{
		if( HitTest(point) == TRUE )
		{	
			DBG_MSG("C1");
			ClientToScreen(&pt);
			TrackPopupMenu(m_MenuDisp_Des[0].hMenu,TPM_LEFTALIGN|TPM_RIGHTBUTTON,pt.x,pt.y,0,GetSafeHwnd(),NULL);
			bMenuStatus = TRUE;
		}
		else
		{
			if( PtInRect(&m_DrawRect, pt) )
			{
				DBG_MSG("C2");
				top.LoadMenu(IDR_MENU_AC);
				pMenu = top.GetSubMenu(0);
				ClientToScreen(&pt);
				pMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,pt.x,pt.y,this);
			}
			
		}
	}
	else
	{
		ClientToScreen(&pt);
		TrackPopupMenu(m_MenuDisp_Des[0].hMenu,TPM_LEFTALIGN|TPM_RIGHTBUTTON,pt.x,pt.y,0,GetSafeHwnd(),NULL);
	}
		


	CDialog::OnRButtonDown(nFlags, point);
}

void CThumbnail1::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDialog::OnLButtonUp(nFlags, point);
}


BOOL CThumbnail1::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	POSITION  pos = m_ListThumbnail.GetFirstSelectedItemPosition();


	if( pos )
	{
		int nSel = m_ListThumbnail.GetNextSelectedItem(pos);

		DBG_MSG("nSel = %u, zDelta = %d", nSel, zDelta);

		if( nSel != -1 )
		{
			if(m_AddMode == TRUE)
				RestoreNormalMode();
			if( zDelta < 0 && nSel + 1 < m_ListThumbnail.GetItemCount() )
			{
				ClearSelectItemStatus(nSel);
				m_nSelectedItem = nSel + 1;
				m_ListThumbnail.SetItemState(m_nSelectedItem, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
				m_ListThumbnail.EnsureVisible(m_nSelectedItem, TRUE);
				LoadGrapAndCellInfo();
				DrawSelectedImage();

				return TRUE;
			}
			else if( zDelta > 0 && nSel > 0 )
			{
				ClearSelectItemStatus(nSel);
				m_nSelectedItem = nSel-1;
				m_ListThumbnail.SetItemState(m_nSelectedItem, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
				m_ListThumbnail.EnsureVisible(m_nSelectedItem, TRUE);
				LoadGrapAndCellInfo();
				DrawSelectedImage();
				return TRUE;
			}
		}
	}


	return CDialog::OnMouseWheel(nFlags, zDelta, pt);
}

BOOL CThumbnail1::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if( m_RectTracker )
	{
		SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));	
		if(m_RectTracker->SetCursor(pWnd,nHitTest))
			return TRUE;
	}

	return CDialog::OnSetCursor(pWnd, nHitTest, message);
}

void CThumbnail1::OnLvnItemchangedListThumb(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码



	*pResult = 0;
}

void CThumbnail1::OnLvnKeydownListThumb(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	if (pLVKeyDow->wVKey == VK_LEFT || pLVKeyDow->wVKey == VK_RIGHT)
	{	
		// get total number of items in the list
		int nTotal = m_ListThumbnail.GetItemCount();

		// rule out the situation of an empty list
		if (nTotal == 0)
		{
			*pResult = 1; // current record stays selected
			return;
		}

		// reset selected item index
		int  nNewItem = -1;

		// set the selection
		if (pLVKeyDow->wVKey == VK_LEFT)	// left arrow
		{
			if (m_nSelectedItem > 0)
				nNewItem = m_nSelectedItem - 1;
		}
		else	// right arrow
		{
			if (m_nSelectedItem < nTotal-1)
				nNewItem = m_nSelectedItem + 1;
		}

		// update the selection
		if (nNewItem != -1)
		{
			// update the selected item index
			m_nSelectedItem = nNewItem;

			LoadGrapAndCellInfo();
			// draw the selected image
			DrawSelectedImage();
		}	 
	}

	*pResult = 0;
}

void CThumbnail1::OnNMClickListThumb(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	LVHITTESTINFO  hitTest;
	ZeroMemory(&hitTest, sizeof(LVHITTESTINFO));
	hitTest.pt = pNMItemActivate->ptAction;
	m_ListThumbnail.SendMessage(LVM_SUBITEMHITTEST, 0, (LPARAM)&hitTest);

	if(m_AddMode == TRUE)
		RestoreNormalMode();

	// draw the selected image
	if (hitTest.iItem != m_nSelectedItem && hitTest.iItem >= 0)
	{
		m_nSelectedItem = hitTest.iItem;
		LoadGrapAndCellInfo();
		DrawSelectedImage();
	}

	*pResult = 0;
}

void CThumbnail1::OnNMRClickListThumb(NMHDR *pNMHDR, LRESULT *pResult)
{
	/*
		修改：20230109
		从鼠标右键弹出菜单选择设置\取消打印图片，改为鼠标右键点击直射设置\取消打印图片
	 */

	*pResult = 0;

	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	int nRClickItem = pNMListView->iItem;
	if (nRClickItem < 0) return;

	std::vector<CString> VectorPrintImages= m_VectorPrintImages;
	bool isPrintImage = false;
	auto itr = std::find(VectorPrintImages.begin(), VectorPrintImages.end(), m_VectorImageNames[nRClickItem]);
	if (itr != VectorPrintImages.end())
	{
		isPrintImage = true;
		VectorPrintImages.erase(itr);
	}
	else
	{
		VectorPrintImages.push_back(m_VectorImageNames[nRClickItem]);
		std::sort(VectorPrintImages.begin(), VectorPrintImages.end());
	}
	if (!isPrintImage)
	{
		if (VectorPrintImages.size() > 20)
		{
			AfxMessageBox("选择打印图片不允许超过20张");
			return;
		}
	}
	// 设置打印图片
	{
		char MultiPrintPicPath[255] = { 0 };
		KSTATUS status = STATUS_SUCCESS;
		UPDATE_PRINT_PATH1 update_print_path = { 0 };
		update_print_path.nTaskID = m_nID;
		for (auto itr = VectorPrintImages.begin(); itr != VectorPrintImages.end(); itr++)
		{
			char temp[128] = { 0 };
			sprintf(temp, "%s|", itr->GetBuffer());
			strcat(MultiPrintPicPath, temp);
		}
		strcpy(update_print_path.PrintPath, MultiPrintPicPath);

		DBG_MSG("update_print_path.PrintPath=%s\n", update_print_path.PrintPath);

		if (!::SendMessage(m_ParentWnd, WM_SET_PRINT_PATH, DWORD(&update_print_path), NULL))
		{
			AfxMessageBox("设置打印图片失败");
			return;
		}
	}
	// 更新显示
	{
		CString path = CString(m_TaskPath) + "\\" + m_VectorImageNames[nRClickItem];
		USES_CONVERSION;
		Bitmap img(A2W(path));
		Bitmap* pThumbnail = static_cast<Bitmap*>(img.GetThumbnailImage(THUMBNAIL_WIDTH, THUMBNAIL_HEIGHT, NULL, NULL));
		if (!isPrintImage)
		{
			Gdiplus::Graphics g(pThumbnail);
			FontFamily fontFamily(L"宋体");
			Gdiplus::Font font(&fontFamily, 18, FontStyleBold, UnitPixel);
			StringFormat stringformat;
			//stringformat.SetAlignment(StringAlignmentCenter);
			//stringformat.SetLineAlignment(StringAlignmentCenter); 
			stringformat.SetAlignment(StringAlignmentFar);
			stringformat.SetLineAlignment(StringAlignmentNear);
			SolidBrush brush(Color(255, 255, 0, 0));
			g.SetTextRenderingHint(TextRenderingHintAntiAlias);
			g.DrawString(L"打印", -1, &font, RectF(0, 0, THUMBNAIL_WIDTH, THUMBNAIL_HEIGHT), &stringformat, &brush);
		}
		HBITMAP hBmp = 0;
		pThumbnail->GetHBITMAP(NULL, &hBmp);
		CBitmap bitmap;
		bitmap.Attach(hBmp);
		m_ImageListThumb.Replace(nRClickItem, &bitmap, NULL);
		delete pThumbnail;
		m_ListThumbnail.RedrawItems(nRClickItem, nRClickItem);
	}
	// 更新内存中保存的打印图片集合
	m_VectorPrintImages = VectorPrintImages;

	return;

	//////////////////////////////////////////////////////////////////////
	// 弹出菜单选择设置打印图片
	POSITION pos = m_ListThumbnail.GetFirstSelectedItemPosition(); //pos选中的首行位置
	if (pos == NULL)
	{
		return;
	}
	CPoint point;
	::GetCursorPos(&point);
	CMenu menu;
	VERIFY(menu.LoadMenu(IDR_MENU1));      //IDR_MENU_POPUP是新建菜单ID
	CMenu* popup = menu.GetSubMenu(0);
	ASSERT(popup != NULL);
	popup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
	return;

	/*char temp1[MAX_PATH],MultiPrintPicPath[MAX_PATH] = "";
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	int iItem = -1 ;
	POSITION pos;
	int nCounter = 0;
	pos = m_ListThumbnail.GetFirstSelectedItemPosition();
	while (NULL != pos)
	{
		// 取消数量限制
		/ *if( nCounter >= 2 )
		{
			 
			break;
		}* /
			
		iItem = m_ListThumbnail.GetNextSelectedItem(pos);
		if( iItem == -1 )
			break;

		sprintf(temp1,"%s|", m_VectorImageNames[iItem]);		
		strcat(MultiPrintPicPath,temp1);
		nCounter++;
		
	}



	if( nCounter)
	{
		KSTATUS status = STATUS_SUCCESS;
		UPDATE_PRINT_PATH1 update_print_path={0};
		update_print_path.nTaskID = m_nID;

		strcpy(update_print_path.PrintPath,MultiPrintPicPath);

	/ *	status = THInterface_DBUpdateRecordInfo(PRN_PATH_INFO_TYPE,
			(PCHAR)&update_print_path,sizeof(update_print_path));* /

DBG_MSG("update_print_path.PrintPath=%s\n",update_print_path.PrintPath);

	if(::SendMessage(m_ParentWnd,WM_SET_PRINT_PATH,DWORD(&update_print_path),NULL))
		{
			CUserInfoDlg dlg;
			dlg.SetInfo(GetIdsString(RES_STRING_NAME,"add_print_pis_success").c_str());
			DBG_MSG("%s\n",GetIdsString(RES_STRING_NAME,"add_print_pis_success").c_str());
			dlg.DoModal();
		}

	}*/
	
}
void CThumbnail1::OnDestroy()
{

	if( m_OldWndProc )
		(WNDPROC)SetWindowLong(GetSafeHwnd(), GWL_WNDPROC, (LONG)m_OldWndProc);

	
	//RestoreNormalMode();
	
	CDialog::OnDestroy();
	
}

void CThumbnail1::RestoreNormalMode()
{
	if( m_AddMode == TRUE )
	{
		if( m_RectTracker )
			delete m_RectTracker;
		m_AddMode = FALSE;
		m_RectTracker = NULL;
	}

}

void CThumbnail1::ClearSelectItemStatus( int nSel )
{
	m_ListThumbnail.SetItemState(nSel, 0, LVIS_SELECTED|LVIS_FOCUSED);

}

void CThumbnail1::CellDothing( int cCellType, BOOL bDrawImage /*= TRUE*/ )
{
	if( m_AddMode == FALSE )
	{
		//AfxMessageBox("ModifyCell");
		ModifyCell(cCellType, bDrawImage);
	}
		
	else
	{
		//AfxMessageBox("AddCell");
		AddCell(cCellType);
	}
		

}

void CThumbnail1::ModifyCell( USHORT CellType,BOOL bDrawImage /*= TRUE*/ )
{
	CELL_DETAIL_INFO ci = {0};

	if( m_CurSelCellIndex == -1 )
		return;
	memcpy(&ci, &m_cell_full_info->cInfo[m_CurSelCellIndex], sizeof(ci) - 8);
	//	if( ci.cType != CELL_OTHER_TYPE )
	{		
		// ::SendMessage(m_ParentWnd,WM_SELECT_CELL,DWORD(&ci),DWORD(&m_ptask_rec_info->gInfo[m_nSelectedItem]));
		{		
			m_cell_full_info->cInfo[m_CurSelCellIndex].cType = CellType;			
		}
	}
	/*	else   //对于杂质来说在标本界面相当于是增加
	{
	HBITMAP              hBitmap = NULL;
	ITEM_CELL_INFO       ItemCellInfo = {0};

	CopyRect(&ItemCellInfo.rc, &ci.rc);
	m_rec_task.GetPickupRecCellGrap(m_AssoicateDC, hBitmap, ci.rc);
	if( hBitmap == NULL )
	return;

	DeleteOthCell();			
	ItemCellInfo.cType = CellType;
	ItemCellInfo.nGrapIndex = m_ptask_rec_info->gInfo[m_nSelectedItem].rIndex;
	ItemCellInfo.nGrapSize  = m_ptask_rec_info->gInfo[m_nSelectedItem].rGrapSize;
	strcpy(ItemCellInfo.sRecRelativePath, m_full_task_info_1.MainInfo1.sImageFoler);
	if( m_BbDlg->AddCell(&ItemCellInfo, hBitmap) == TRUE )
	{
	memcpy(&m_cell_full_info->cInfo[m_CurSelCellIndex].identify, &ItemCellInfo.identify, sizeof(CELL_IDENTIFY));
	m_cell_full_info->cInfo[m_CurSelCellIndex].cType = CellType;


	}
	else
	DeleteObject(hBitmap);

	}*/
	DrawSelectedImage();

}

void CThumbnail1::AddCell( USHORT CellType )
{
	RECT             rc;
	KSTATUS          status;
	HBITMAP          hBitmap = NULL;
	ITEM_CELL_INFO   ItemCellInfo = {0};	

	if( m_RectTracker == NULL || m_AddMode == FALSE || m_AssoicateDC.hMemDC == NULL || m_ptask_rec_info == NULL )
		return;
	m_RectTracker->GetTrueRect(&rc);
	rc.left -= m_DrawRect.left;
	rc.right -= m_DrawRect.left;
	rc.top -= m_DrawRect.top;
	rc.bottom -= m_DrawRect.top;



	float fx = (float)(m_DrawRect.right-m_DrawRect.left)/(float)(m_AssoicateDC.rc.right-m_AssoicateDC.rc.left);
	float fy = (float)(m_DrawRect.bottom-m_DrawRect.top)/(float)(m_AssoicateDC.rc.bottom-m_AssoicateDC.rc.top);
	rc.left /=fx;
	rc.right /=fx;
	rc.top  /=fy;
	rc.bottom/=fy;


	m_rec_task.GetPickupRecCellGrap(m_AssoicateDC, hBitmap, rc);
	if( hBitmap == NULL )
		return; 

	ItemCellInfo.cType = CellType;
	ItemCellInfo.nGrapIndex = m_ptask_rec_info->gInfo[m_nSelectedItem].rIndex;
	ItemCellInfo.nGrapSize  = m_ptask_rec_info->gInfo[m_nSelectedItem].rGrapSize;
	CopyRect(&ItemCellInfo.rc, &rc);
	strcpy(ItemCellInfo.sRecRelativePath, (PCHAR)m_TaskPath);

	if(::SendMessage(m_ParentWnd,WM_ADD_CELL,DWORD(&ItemCellInfo),(LPARAM)hBitmap))	
	{
		if( m_cell_full_info )
		{			
			m_rec_task.FreeCellFullInfo(m_cell_full_info);
			m_cell_full_info = NULL;
		}
		status = m_rec_task.GetPickupRecCellGrap((PCHAR)m_TaskPath,
			m_ptask_rec_info->gInfo[m_nSelectedItem].rIndex,
			m_ptask_rec_info->gInfo[m_nSelectedItem].rGrapSize,
			m_cell_full_info,m_CellTypeCounter,m_CellTypeArray,FALSE);

			

		if( status != STATUS_SUCCESS )
		{
			
			m_cell_full_info = NULL;
		}
			
	}
	m_CurSelCellIndex = -1;
	RestoreNormalMode();
	DrawSelectedImage();
}

BOOL CThumbnail1::DeleteOthCell()
{
	KSTATUS          status;
	REC_GRAP_RECORD  grap = {0};

	if( m_CurSelCellIndex == -1 )
		return FALSE;

	grap.rGrapSize = m_ptask_rec_info->gInfo[m_nSelectedItem].rGrapSize;
	grap.rIndex    = m_ptask_rec_info->gInfo[m_nSelectedItem].rIndex;
	status = THInterface_DeleteRecCellInfo(m_TaskPath,
											&grap,
											&m_cell_full_info->cInfo[m_CurSelCellIndex].identify,
											FALSE);

	if( status == STATUS_SUCCESS )
		return TRUE;

	return FALSE;
	return TRUE;
	

}

int CThumbnail1::GetCurSelCell( POINT &pt )
{
	ULONG  j;
	CRect rect;

	if( m_ptask_rec_info == NULL || m_cell_full_info == NULL )
	{
		
		return 0; //未击中
	}

	for( j = 0 ; j < m_cell_full_info->cCellCounter ; j ++ )
	{
		

		CopyRect(&rect,&m_cell_full_info->cInfo[j].rc);
		rect.left *=m_fx;
		rect.right *=m_fx;
		rect.top  *=m_fy;
		rect.bottom*=m_fy;

		if( ::PtInRect(&rect,pt))
		{

    
			if( m_CurSelCellIndex == j )
				return 1; //不需要刷新

			m_CurSelCellIndex = j;
			return 2; //需要刷新

		}
	}

	return 0;

}

BOOL CThumbnail1::HitTest( POINT pt )
{
	int   nRet = 0;

	if( PtInRect(&m_DrawRect, pt) )
	{
		LoadGrapAndCellInfo();

		m_ImageRect.SetFocus();
		pt.x -= m_DrawRect.left;
		pt.y -= m_DrawRect.top;
		nRet = GetCurSelCell(pt);
		
		if( nRet == 2 )
		{
			CELL_DETAIL_INFO ci = {0};

			memcpy(&ci, &m_cell_full_info->cInfo[m_CurSelCellIndex], sizeof(ci) - 8);
			
			::SendMessage(m_ParentWnd,WM_SELECT_CELL,DWORD(&ci),DWORD(&m_ptask_rec_info->gInfo[m_nSelectedItem]));

			DrawSelectedImage();
		}
	}

	return (nRet != 0);

}

void CThumbnail1::CreateNewMyTracker()
{
	RECT  NewLocRC = {0};
	POINT pt;

	m_RectTracker = new CMyTracker;
	if( m_RectTracker == NULL )
		return;

#define  DEFAULT_CX 20
#define  DEFAULT_CY 20

	m_RectTracker->m_nStyle=CRectTracker::resizeInside|CRectTracker::dottedLine;
	m_RectTracker->m_sizeMin.cx=10;
	m_RectTracker->m_sizeMin.cy=10; 
	m_RectTracker->m_nHandleSize = 4;
	m_RectTracker->SetRectTrackerRange(m_DrawRect, m_hWnd);

	GetCursorPos(&pt);
	ScreenToClient(&pt);

	if( pt.x + DEFAULT_CX < m_DrawRect.right )
	{
		NewLocRC.left = pt.x;
		NewLocRC.right = pt.x + DEFAULT_CX;
	}
	else
	{
		NewLocRC.right = pt.x;
		NewLocRC.left  = pt.x - DEFAULT_CX;
	}

	if( pt.y + DEFAULT_CY < m_DrawRect.bottom )
	{
		NewLocRC.top = pt.y;
		NewLocRC.bottom = pt.y + DEFAULT_CY;
	}
	else
	{
		NewLocRC.bottom = pt.y;
		NewLocRC.top    = NewLocRC.bottom - DEFAULT_CY;
	}
	CopyRect(&m_RectTracker->m_rect, &NewLocRC);
	m_AddMode = TRUE;
	::SendMessage(m_ParentWnd,WM_SELECT_CELL,NULL,DWORD(&m_ptask_rec_info->gInfo[m_nSelectedItem]));
	m_CurSelCellIndex = -1;
	DrawSelectedImage();

}
void CThumbnail1::OnStnClickedImageRect()
{
	// TODO: 在此添加控件通知处理程序代码
	int i = 0;
}

HCURSOR CThumbnail1::OnQueryDragIcon()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	return (HCURSOR) m_hIcon;

	return CDialog::OnQueryDragIcon();
}

BOOL CThumbnail1::CellDisableDisaply( ULONG cType )
{
	/*int  nIndex = CellMapUsIndex(cType);
	if( nIndex == -1 )
		return TRUE;

	if( cType != CELL_OTHER_TYPE )
		return (m_us_info.par[nIndex].bIsDelete);
	else	
		return FALSE;*/
	if(cType >= 1 && cType <= 45)
		return (get_us_info.par[cType - 1].bIsDelete);
	else if (cType == CELL_OTHER_TYPE)
		return  FALSE;
	else
		return TRUE;


}

int CThumbnail1::CellMapUsIndex( USHORT nCellType )
{
	/*if( nCellType >= 1 && nCellType <= 45 )
		return (nCellType - 1);

	else if( nCellType == CELL_OTHER_TYPE )
		return CELL_OTHER_TYPE;*/

	return -1;

}

void CThumbnail1::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;


		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);

	}
	else
	{
		CDialog::OnPaint();

	}
}



void CThumbnail1::RMenuStatus( CMenu *SubMenu )
{
	/*USHORT  CellArray[] = {CELL_RED_TYPE,CELL_WHITE_TYPE,CELL_EP_TYPE,CELL_CAST_TYPE,CELL_CRY_TYPE,CELL_MUCS_TYPE,CELL_BACT_TYPE,CELL_BYST_TYPE};


	for( int j = 0 ; j < 8 ; j ++ )
	{
		if( CellDisableDisaply(CellArray[j]) )
			SubMenu->EnableMenuItem(j,MF_BYPOSITION|MF_GRAYED|MF_DISABLED);
	}*/

}

void CThumbnail1::ResizeWindow( PSIZE PicSize )
{
	
	int nDrawRectWidth = m_DrawRect.right - m_DrawRect.left;
	int nDrawRectHeight = m_DrawRect.bottom - m_DrawRect.top;

	DBG_MSG("nDrawRectWidth =%d,nDrawRectHeight = %d,PicSize->cx=%d,PicSize->cy=%d\n",nDrawRectWidth,nDrawRectHeight,PicSize->cx,PicSize->cy);

	CRect rect;
	this->GetWindowRect(&rect);

	RECT m_thumbnaillist;
	m_ListThumbnail.GetWindowRect(&m_thumbnaillist);
	ScreenToClient(&m_thumbnaillist);

	if( nDrawRectWidth == PicSize->cx  && nDrawRectHeight == PicSize->cy )
	{
		//DBG_MSG("CCC1");
		return;
	}
	if( nDrawRectWidth > PicSize->cx )
	{
		//DBG_MSG("CCC2");
		/*m_DrawRect.left += (nDrawRectWidth - PicSize->cx) >> 1;
		m_DrawRect.right = m_DrawRect.left + PicSize->cx;*/

		m_thumbnaillist.right -= nDrawRectWidth - PicSize->cx;

		m_DrawRect.right -= nDrawRectWidth - PicSize->cx;

		rect.right -= nDrawRectWidth - PicSize->cx;
	}

	if(nDrawRectHeight > PicSize->cy )
	{
		//DBG_MSG("CCC3");
		/*m_DrawRect.top  += (nDrawRectHeight - PicSize->cy) >> 1;
		m_DrawRect.bottom = m_DrawRect.top + PicSize->cy;*/
		m_DrawRect.bottom -= nDrawRectHeight - PicSize->cy;
		rect.bottom -= nDrawRectHeight - PicSize->cy;
	}

	if(nDrawRectWidth < PicSize->cx)
	{
		//DBG_MSG("CCC4");
		int xoff = PicSize->cx - nDrawRectWidth ;

		rect.right += xoff;		

		m_DrawRect.right += xoff;
				
		m_thumbnaillist.right += xoff;
	}
	if(nDrawRectHeight < PicSize->cy )
	{
		//DBG_MSG("CCC5");
		int yoff = PicSize->cy - nDrawRectHeight ;

		rect.bottom += yoff;		

		//DBG_MSG("m_DrawRect.bottom1=%d\n",m_DrawRect.bottom);
		m_DrawRect.bottom += yoff;

		//DBG_MSG("m_DrawRect.bottom2=%d\n",m_DrawRect.bottom);
	}


	this->MoveWindow(rect,TRUE);
	this->Invalidate();

	m_ListThumbnail.MoveWindow(&m_thumbnaillist);
	m_ListThumbnail.Invalidate();

	m_ImageRect.MoveWindow(&m_DrawRect);
	m_ImageRect.Invalidate();


}
void CThumbnail1::OnSize(UINT nType, int cx, int cy)
{

	
	CDialog::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
}



WNDPROC CThumbnail1::GetOldListGroupWndProc()
{
	//DBG_MSG("m_OldWndProc = %x\n", m_OldWndProc);

	return  m_OldWndProc;

}




void CThumbnail1::HookListGroupWindow( HWND  HookWnd,PMENU_DISP_DESCRIPTION pMenuDispDes,USHORT nMenuDispDesCounter,PMENU_COMMAND_DESCRIPTION pMenuCmdDes, USHORT nMenuCmdDesCounter )
{


	m_MenuDisp_Des  =  pMenuDispDes;
	m_MenuCmd_Des   =  pMenuCmdDes;

	m_MenuDisp_Counter  = nMenuDispDesCounter;
	m_MenuCmdDesCounter = nMenuCmdDesCounter;


	//m_OldWndProc = (WNDPROC)SetWindowLong(GetSafeHwnd(), GWL_WNDPROC, (LONG)ListGroupWndProc);

}

void CThumbnail1::MenuCommand( DWORD nMenuCmdID )
{
	MENU_COMMAND_DESCRIPTION   MenuCmdDes;

	
	


	if( SearchMenuCmd(nMenuCmdID, MenuCmdDes) == FALSE )
		return;
DBG_MSG("CThumbnail1 nMenuCmdID = %d\n",nMenuCmdID);
	

	if( MenuCmdDes.nMAction == CHANGED_CELL_TYPE_ACTION )
	{
		
		CellDothing(MenuCmdDes.nAdditionData);
	}
		
	else if( MenuCmdDes.nMAction == DELETE_CELL_TYPE_ACTION )
	{
		OnPopDelete();
	}

	::SendMessage(m_ParentWnd,WM_COMMAND,LOWORD(nMenuCmdID),NULL);
		

}

BOOL CThumbnail1::SearchMenuCmd( DWORD nMenuCmdID, MENU_COMMAND_DESCRIPTION & MenuCmdDes )
{
	for( ULONG j = 0 ; j < m_MenuCmdDesCounter ; j ++ )
	{
		if( (m_MenuCmd_Des[j].nMCommandID & 0xffff )== nMenuCmdID )
		{			
			memcpy(&MenuCmdDes, &m_MenuCmd_Des[j], sizeof(MENU_COMMAND_DESCRIPTION));
			return TRUE;
		}
	}

	return FALSE;

}

void CThumbnail1::CloneCellType( PGROUP_NAME pGroupName, ULONG nCounter )
{
	m_CellTypeArray = (PULONG)malloc(sizeof(ULONG) * nCounter);
	if( m_CellTypeArray == NULL )
		return;

	for( m_CellTypeCounter = 0 ; m_CellTypeCounter < nCounter; m_CellTypeCounter ++ )
		m_CellTypeArray[m_CellTypeCounter] = pGroupName[m_CellTypeCounter].nCellType;

}

BOOL CThumbnail1::VaildCellType( USHORT nCellType )
{
	for( ULONG j  = 0 ; j < m_CellTypeCounter ; j ++ )
	{
		if( m_CellTypeArray[j] == nCellType )
			return TRUE;
	}

	return FALSE;


}

void CThumbnail1::UpdateTaskPath( DWORD nTaskID )
{
	char  Temp[40];
	sprintf(Temp, "%u", nTaskID);
	strcpy(m_TaskPath, m_ImageFolder);
	PathAppend(m_TaskPath, Temp);
}

void CThumbnail1::GetWhetherShowOther()
{
	char path[MAX_PATH]={0};
	GetModuleFileName(NULL, path, MAX_PATH);
	PathRemoveFileSpec(path);	
	PathAppend(path,"Conf.ini");
	m_nIsShowOther = GetPrivateProfileInt("th_us","bIsShowOther",1,path);

	DBG_MSG("m_nIsShowOther = %d\n",m_nIsShowOther);
}

void CThumbnail1::OnAddPrintpic()
{
	// TODO:  在此添加命令处理程序代码
	if ((m_VectorPrintImages.size() + m_ListThumbnail.GetSelectedCount()) > 20)
	{
		AfxMessageBox("选择打印图片不允许超过20张");
		return;
	}
	POSITION pos = m_ListThumbnail.GetFirstSelectedItemPosition(); //pos选中的首行位置
	while (pos)   //如果你选择多行
	{
		int nIdx = -1;
		nIdx = m_ListThumbnail.GetNextSelectedItem(pos);
		if (nIdx < 0)
		{
			continue;
		}
		CString path = CString(m_TaskPath)+"\\"+m_VectorImageNames[nIdx];

		USES_CONVERSION;
		Bitmap img(A2W(path));
		Bitmap* pThumbnail = static_cast<Bitmap*>(img.GetThumbnailImage(THUMBNAIL_WIDTH, THUMBNAIL_HEIGHT, NULL, NULL));

		Gdiplus::Graphics g(pThumbnail);
		FontFamily fontFamily(L"宋体");
		Gdiplus::Font font(&fontFamily, 18, FontStyleBold, UnitPixel);
		StringFormat stringformat;
		//stringformat.SetAlignment(StringAlignmentCenter);
		//stringformat.SetLineAlignment(StringAlignmentCenter); 
		stringformat.SetAlignment(StringAlignmentFar);
		stringformat.SetLineAlignment(StringAlignmentNear);
		SolidBrush brush(Color(255, 255, 0, 0));
		g.SetTextRenderingHint(TextRenderingHintAntiAlias);
		g.DrawString(L"打印", -1, &font, RectF(0, 0, THUMBNAIL_WIDTH, THUMBNAIL_HEIGHT), &stringformat, &brush);

		HBITMAP hBmp = 0;
		pThumbnail->GetHBITMAP(NULL, &hBmp);
		CBitmap bitmap;
		bitmap.Attach(hBmp);
		m_ImageListThumb.Replace(nIdx, &bitmap, NULL);
		delete pThumbnail;
		m_ListThumbnail.RedrawItems(nIdx, nIdx);

		auto itr = std::find(m_VectorPrintImages.begin(), m_VectorPrintImages.end(), m_VectorImageNames[nIdx]);
		if (itr == m_VectorPrintImages.end())
		{
			m_VectorPrintImages.push_back(m_VectorImageNames[nIdx]);
		}
	}

	//DrawThumbnails();

	{
		char MultiPrintPicPath[255] = { 0 };
		KSTATUS status = STATUS_SUCCESS;
		UPDATE_PRINT_PATH1 update_print_path = { 0 };
		update_print_path.nTaskID = m_nID;
		for (auto itr = m_VectorPrintImages.begin(); itr != m_VectorPrintImages.end(); itr++)
		{
			char temp[128] = { 0 };
			sprintf(temp, "%s|", itr->GetBuffer());
			strcat(MultiPrintPicPath, temp);
		}
		strcpy(update_print_path.PrintPath, MultiPrintPicPath);

		DBG_MSG("update_print_path.PrintPath=%s\n", update_print_path.PrintPath);

		if (::SendMessage(m_ParentWnd, WM_SET_PRINT_PATH, DWORD(&update_print_path), NULL))
		{
			CUserInfoDlg dlg;
			dlg.SetInfo(GetIdsString(RES_STRING_NAME, "add_print_pis_success").c_str());
			DBG_MSG("%s\n", GetIdsString(RES_STRING_NAME, "add_print_pis_success").c_str());
			dlg.DoModal();
		}
	}
}


void CThumbnail1::OnDelPrintpic()
{
	// TODO:  在此添加命令处理程序代码
	POSITION pos = m_ListThumbnail.GetFirstSelectedItemPosition(); //pos选中的首行位置
	while (pos)   //如果你选择多行
	{
		int nIdx = -1;
		nIdx = m_ListThumbnail.GetNextSelectedItem(pos);
		if (nIdx < 0)
		{
			continue;
		}
		CString path = CString(m_TaskPath) + "\\" + m_VectorImageNames[nIdx];

		USES_CONVERSION;
		Bitmap img(A2W(path));
		Bitmap* pThumbnail = static_cast<Bitmap*>(img.GetThumbnailImage(THUMBNAIL_WIDTH, THUMBNAIL_HEIGHT, NULL, NULL));

		HBITMAP hBmp = 0;
		pThumbnail->GetHBITMAP(NULL, &hBmp);
		CBitmap bitmap;
		bitmap.Attach(hBmp);
		m_ImageListThumb.Replace(nIdx, &bitmap, NULL);
		delete pThumbnail;
		m_ListThumbnail.RedrawItems(nIdx, nIdx);

		auto itr = std::find(m_VectorPrintImages.begin(), m_VectorPrintImages.end(), m_VectorImageNames[nIdx]);
		if (itr != m_VectorPrintImages.end())
		{
			m_VectorPrintImages.erase(itr);
		}
	}

	// DrawThumbnails();

	{
		char MultiPrintPicPath[255] = { 0 };
		KSTATUS status = STATUS_SUCCESS;
		UPDATE_PRINT_PATH1 update_print_path = { 0 };
		update_print_path.nTaskID = m_nID;
		for (auto itr = m_VectorPrintImages.begin(); itr != m_VectorPrintImages.end(); itr++)
		{
			char temp[128] = { 0 };
			sprintf(temp, "%s|", itr->GetBuffer());
			strcat(MultiPrintPicPath, temp);
		}
		strcpy(update_print_path.PrintPath, MultiPrintPicPath);

		DBG_MSG("update_print_path.PrintPath=%s\n", update_print_path.PrintPath);

		if (::SendMessage(m_ParentWnd, WM_SET_PRINT_PATH, DWORD(&update_print_path), NULL))
		{

		}
	}
}


void CThumbnail1::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	CDialog::OnLButtonDblClk(nFlags, point);


	CRect rt;
	CWnd *picWnd = GetDlgItem(IDC_IMAGE_RECT);
	picWnd->GetClientRect(rt);
	::MapWindowPoints(m_hWnd, picWnd->GetSafeHwnd(), &point, 1);
	if (PtInRect(rt, point))
	{
		CPicPreviewWnd wnd(m_VectorImageNames, CString(m_TaskPath), m_nSelectedItem);
		wnd.DoModal();
	}
}
