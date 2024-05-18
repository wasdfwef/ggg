#include "stdafx.h"
#include "MainFrm.h"
#include "..\..\..\inc\THInterface.h"
#include "UW.h"
#include "Camera.h"

#include "MoveVisioDlg.h"
#include "MyWaitCursor.h"
#include "DbgMsg.h"

#define  VIDEO_WIDTH    800
#define  VIDEO_HIEHGT   600

#define IDC_BTN_FOUCS 11119
 
#define WM_JUDGELIGHT WM_USER + 540
const static TCHAR *strFoucs[] = { _T("开始显微镜设置"), _T("完成显微镜设置") };

IMPLEMENT_DYNCREATE(CMicroscopeCameraView, CView)
extern 
std::wstring GetIdsString(const std::wstring &sMain,const std::wstring &sId);

BEGIN_MESSAGE_MAP(CMicroscopeCameraView, CView)
	ON_WM_SIZE()
	ON_WM_MOUSEWHEEL()
	ON_WM_RBUTTONDOWN()
	ON_UPDATE_COMMAND_UI_RANGE(ID_XYADJUST, ID_US_MIC_EXIT, &CMicroscopeCameraView::OnUpdateMicConfig)
	ON_COMMAND_RANGE(ID_XYADJUST, ID_US_MIC_EXIT, &CMicroscopeCameraView::OnMicConfig)
	ON_WM_LBUTTONDBLCLK()
	ON_BN_CLICKED(IDC_BTN_FOUCS, &CMicroscopeCameraView::OnFoucs)
	ON_WM_DESTROY()
	ON_COMMAND(ID_FOCUS_BEGIN, &CMicroscopeCameraView::OnFocusBegin)
	ON_COMMAND(ID_FOCUS_END, &CMicroscopeCameraView::OnFocusEnd)
	ON_COMMAND(ID_FOCUS_CANCEL, &CMicroscopeCameraView::OnFocusEnd)
	ON_COMMAND(ID_FOCUS_SAVE_Z, &CMicroscopeCameraView::OnFocusSaveZ)
	ON_MESSAGE(WM_CANCEL_FOCUS, &CMicroscopeCameraView::OnFocusEnd)
	ON_MESSAGE(WM_SAVEZ_FOCUS, &CMicroscopeCameraView::OnFocusSaveZ)
	ON_MESSAGE(WM_RESETZ_FOCUS, &CMicroscopeCameraView::OnFocusResetZ)
	ON_MESSAGE(WM_BEGIN_FOCUS, &CMicroscopeCameraView::OnFocusBegin)
	ON_MESSAGE(WM_JUDGELIGHT, &CMicroscopeCameraView::OnReceiveLight)
	ON_MESSAGE(WM_SHOW_LIGHT, &CMicroscopeCameraView::OnShowLight)
	ON_COMMAND(ID_FOCUS_RESET_Z, &CMicroscopeCameraView::OnFocusResetZ)
END_MESSAGE_MAP()

CMicroscopeCameraView::CMicroscopeCameraView()
{
	m_bPrv = FALSE;
	m_IsMicSetting = FALSE;	
	m_bInMicConfig = FALSE;
	noResetZMode = TRUE;
	m_bInAdjustFocusMode = false;
}

void CMicroscopeCameraView::OnDraw( CDC* pDC )
{
	/*CRect rc;
	int left = 0, top = 0;
	GetClientRect(&rc);
	if (rc.right > VIDEO_WIDTH)
		left = (rc.right - VIDEO_WIDTH) >> 1;
	if (rc.bottom > VIDEO_HIEHGT)
		top = (rc.bottom - VIDEO_HIEHGT) >> 1;

	pDC->FillSolidRect(left, top, VIDEO_WIDTH, VIDEO_HIEHGT,  0);*/

	int nWidth = 0, nHeight = 0;
	CRect rc;
	GetClientRect(&rc);
	rc.DeflateRect(5, 5, 5, 5);

	if (rc.Width() * 3 > rc.Height() * 4)
	{
		nHeight = rc.Height();
		nWidth = (int)(((float)nHeight) / 3 * 4);
	}
	else
	{
		nWidth = rc.Width();
		nHeight = (int)(((float)nWidth) / 4 * 3);
	}
	pDC->FillSolidRect(
		rc.left+(rc.Width() - nWidth) / 2, 
		rc.top + (rc.Height() - nHeight) / 2,
		nWidth, nHeight, 0);
}

void CMicroscopeCameraView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	//  创建按钮
	//m_btnFocus.Create(strFoucs[0], WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(5, 40, 95, 75), this, IDC_BTN_FOUCS);
	//m_font.CreatePointFont(90, L"宋体", NULL);
	//m_btnFocus.SetFont(&m_font);

	menu_usmic.LoadMenu(IDR_MENU_FOCUS);
	SetMenu(&menu_usmic);
	menu_usmic.ModifyMenu(0,MF_BYPOSITION, 0, GetIdsString(_T("us_mic_config"),_T("title")).c_str());
	CMenu *usmic = menu_usmic.GetSubMenu(0);
	if (usmic)
	{
		usmic->EnableMenuItem(ID_FOCUS_BEGIN, MF_BYCOMMAND | MF_ENABLED);
		usmic->EnableMenuItem(ID_FOCUS_CANCEL, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		usmic->EnableMenuItem(ID_FOCUS_RESET_Z, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		//usmic->EnableMenuItem(ID_FOCUS_END, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		usmic->DeleteMenu(ID_FOCUS_END, MF_BYCOMMAND);
		usmic->EnableMenuItem(ID_FOCUS_SAVE_Z, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);

	}
	/*CMenu *usmic = menu_usmic.GetSubMenu(0);
	if (usmic)
	{
		usmic->ModifyMenu(ID_XYADJUST, MF_STRING |MF_BYCOMMAND, ID_XYADJUST,GetIdsString(_T("us_mic_config"),_T("XYADJUST")).c_str());
		usmic->ModifyMenu(ID_ZADJUST, MF_STRING |MF_BYCOMMAND, ID_ZADJUST,GetIdsString(_T("us_mic_config"),_T("ZADJUST")).c_str());
		usmic->ModifyMenu(ID_X_RESET, MF_STRING |MF_BYCOMMAND, ID_X_RESET,GetIdsString(_T("us_mic_config"),_T("X_RESET")).c_str());
		usmic->ModifyMenu(ID_Y_RESET, MF_STRING |MF_BYCOMMAND, ID_Y_RESET,GetIdsString(_T("us_mic_config"),_T("Y_RESET")).c_str());
		usmic->ModifyMenu(ID_Z_RESET, MF_STRING |MF_BYCOMMAND, ID_Z_RESET,GetIdsString(_T("us_mic_config"),_T("Z_RESET")).c_str());
		usmic->ModifyMenu(ID_US_MIC_EXIT, MF_STRING |MF_BYCOMMAND, ID_US_MIC_EXIT,GetIdsString(_T("us_mic_config"),_T("exit")).c_str());
	}*/

	char buf[MAX_PATH] = { 0 };
	GetModuleFileNameA(NULL, buf, MAX_PATH);
	PathRemoveFileSpecA(buf);
	PathAppendA(buf, "Config\\Focus.ini");
	m_strIniFocus = buf;
}

void CMicroscopeCameraView::OnDestroy()
{
	CView::OnDestroy();

	SetVideoPrview(FALSE);
}

BOOL CMicroscopeCameraView::SetVideoPrview( BOOL bPrv )
{
	BOOL bRet;

	bRet = InteralVideoPrview(bPrv);
	if (bRet)
	{
		m_bPrv = bPrv;
	}
	
	return bRet;
}

void CMicroscopeCameraView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	if( m_bPrv )
		InteralVideoPrview(m_bPrv);

}

void CMicroscopeCameraView::OnFoucs()
{
	if (m_bInMicConfig)
	{
		m_btnFocus.SetWindowText(strFoucs[0]);
	}
	else
	{
		m_btnFocus.SetWindowText(strFoucs[1]);
	}
	((CMainFrame*)AfxGetMainWnd())->MicSet(m_bInMicConfig);
}

BOOL CMicroscopeCameraView::InteralVideoPrview(BOOL bPrv)
{
	/*CAMERA_PRVIEW_CONFIG   cam_cfg = {0};
	cam_cfg.nCaIndex = 0;

	/ *if( m_bPrv )
		return TRUE;* /

	if( bPrv == TRUE )
	{
		RECT   rc;

		GetClientRect(&rc);

		if( rc.right > VIDEO_WIDTH )
			cam_cfg.cPrviewRC.left = (rc.right - VIDEO_WIDTH) >> 1;

		if( rc.bottom > VIDEO_HIEHGT )
			cam_cfg.cPrviewRC.top = (rc.bottom - VIDEO_HIEHGT) >> 1;

		cam_cfg.cPrviewWnd = GetSafeHwnd();
		cam_cfg.cPrviewRC.bottom = VIDEO_HIEHGT + cam_cfg.cPrviewRC.top;
		cam_cfg.cPrviewRC.right = VIDEO_WIDTH + cam_cfg.cPrviewRC.left ;
	}

	return (THInterface_SetCameraPrviewWindow(&cam_cfg) == STATUS_SUCCESS);*/

	int nWidth = 0, nHeight = 0;
	CAMERA_PRVIEW_CONFIG   cam_cfg = { 0 };
	cam_cfg.nCaIndex = 0;

	if (bPrv == TRUE)
	{
		CRect rc;
		GetClientRect(&rc);
		rc.DeflateRect(5, 5, 5, 5);

		if (rc.Width() * 3 > rc.Height() * 4)
		{
			nHeight = rc.Height();
			nWidth = (int)(((float)nHeight) / 3 * 4);
		}
		else
		{
			nWidth = rc.Width();
			nHeight = (int)(((float)nWidth) / 4 * 3);
		}
		cam_cfg.cPrviewWnd = GetSafeHwnd();
		cam_cfg.cPrviewRC.left = rc.left + (rc.Width() - nWidth) / 2;
		cam_cfg.cPrviewRC.top = rc.top + (rc.Height() - nHeight) / 2;
		cam_cfg.cPrviewRC.bottom = nHeight + cam_cfg.cPrviewRC.top;
		cam_cfg.cPrviewRC.right = nWidth + cam_cfg.cPrviewRC.left;
	}

	return (THInterface_SetCameraPrviewWindow(&cam_cfg) == STATUS_SUCCESS);
}

void CMicroscopeCameraView::ControlUSSample( BOOL bPause )
{
	SYS_WORK_STATUS   sys_work_status = {0};

	sys_work_status.nType = USSampleCtrl;
	*((PBOOL)sys_work_status.Reserved) = bPause;

	THInterface_SetSystemWorkStatusEx(&sys_work_status);
}

BOOL CMicroscopeCameraView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
	if(m_bInMicConfig == FALSE)
	{
		return FALSE;
	}
	else
	{
		if(pMain->m_bPicPause != FALSE)
		{

		}
		else
		{
			return FALSE;
		}
	}

	unsigned char chPara[8] = {0};

	char temp[MAX_PATH] = {0};

	
	if (m_bInAdjustFocusMode)
	{
		int nStep = GetPrivateProfileIntA("Focus", "step", 6, m_strIniFocus.c_str());

		if (zDelta < 0)
		{
			FocusAdjust(FOCUS_DIRECTION::DIRECTION_BACKWARD, nStep);
		}
		else
		{
			FocusAdjust(FOCUS_DIRECTION::DIRECTION_FOREWARD, nStep);
		}
	}

	return CView::OnMouseWheel(nFlags, zDelta, pt);
}

#define  FG_SETP 4


static
VOID
CALLBACK
UWPrviewCB(
HDC   hDC,
SIZE  sSize
)
{
	/*
	int   cx, cy;
	HPEN  hPen, hOldPen;

	cx = sSize.cx >> 2;
	cy = sSize.cy >> 2;

	hPen = CreatePen(PS_SOLID, 0, RGB(255,0,0));
	hOldPen = (HPEN)SelectObject(hDC, hPen);

	for( int j = 0 ; j  < FG_SETP ; j ++ )
	{
		// 划横线
		MoveToEx(hDC, j * cx, 0, NULL);
		LineTo(hDC, j * cx, sSize.cy);
		// 划竖线
		MoveToEx(hDC, 0, j * cy, NULL);
		LineTo(hDC, sSize.cx, j * cy);
	}
	// 划横线
	MoveToEx(hDC, 0, sSize.cy-1, NULL);
	LineTo(hDC, sSize.cx, sSize.cy-1);
	// 划竖线
	MoveToEx(hDC, sSize.cx-1, 0, NULL);
	LineTo(hDC, sSize.cx-1, sSize.cy);

	SelectObject(hDC, hOldPen);
	DeleteObject(hPen);
	*/
}

bool CMicroscopeCameraView::StartUsMicConfig()
{
	KSTATUS               status;
	PRVIEW_OUTPUT_CB      prv_cb = {0};		
	prv_cb.nCaIndex = 0;

	if (!m_bInMicConfig)
		prv_cb.pCB = UWPrviewCB;

	status = THInterface_SetDevCaps(T_DEV_CAMERA_TYPE, DevCB, (PCHAR)&prv_cb, sizeof(prv_cb), 0);
	//if( status != STATUS_SUCCESS )
	//	return false;

	m_bInMicConfig = !m_bInMicConfig;
	m_IsMicSetting = !m_IsMicSetting;

	return true;
}

void CMicroscopeCameraView::EndUsMicConfig()
{
	m_bInMicConfig = FALSE;
}

void CMicroscopeCameraView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	
// 	CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
// 	pMain->OutputMessage(L"错误信息");
	/*if(m_bInMicConfig == FALSE)
	{
		return ;
	}
	else
	{
		if(pMain->m_bPicPause != FALSE)
		{

		}
		else
		{
			return ;
		}
	}*/

	CMenu *usmic = menu_usmic.GetSubMenu(0);
	if (usmic)
	{
		if (m_bInAdjustFocusMode)
		{
			usmic->EnableMenuItem(ID_FOCUS_BEGIN, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			usmic->EnableMenuItem(ID_FOCUS_END, MF_BYCOMMAND | MF_ENABLED);
			usmic->EnableMenuItem(ID_FOCUS_SAVE_Z, MF_BYCOMMAND | MF_ENABLED);
			usmic->EnableMenuItem(ID_FOCUS_CANCEL, MF_BYCOMMAND | MF_ENABLED);
		}
		else
		{
			usmic->EnableMenuItem(ID_FOCUS_BEGIN, MF_BYCOMMAND | MF_ENABLED);
			usmic->EnableMenuItem(ID_FOCUS_END, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			usmic->EnableMenuItem(ID_FOCUS_SAVE_Z, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			usmic->EnableMenuItem(ID_FOCUS_CANCEL, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		}

		// 右键菜单
		GetCursorPos(&point);
		//theApp.GetContextMenuManager()->ShowPopupMenu(usmic->GetSafeHmenu(), point.x, point.y, this, TRUE);
		usmic->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
		usmic->Detach();
	}

	CView::OnRButtonDown(nFlags, point);
}

void CMicroscopeCameraView::OnUpdateMicConfig( CCmdUI *pCmdUI )
{
	pCmdUI->Enable(TRUE);
}

void CMicroscopeCameraView::OnMicConfig( UINT nID )
{
	if(!m_bInMicConfig)
		return;

	CZPosType zpostype = AChannel10;
	WORD stepcount =0, xoffset = 0, yoffset = 0;
	switch (nID)
	{
		// XY轴调整
	case ID_XYADJUST:
		{
			Save40XYSetting();
		}
		break;
		// Z轴调整
	case ID_ZADJUST:
		{
			SaveZSetting();	

			if( QueryZPos(zpostype, stepcount) )
			{
				SaveZPosToIni(zpostype, stepcount);
			}
		}
		break;
		// Z轴复位
	case ID_Z_RESET:
		{
			ZAxisReset();
		}
		break;
	case ID_X_RESET:
		{
		   XAxisReset();
		}
		break;
	case ID_Y_RESET:
		{
			YAxisReset();
		}
		break;
		// 退出显微镜调整
	case ID_US_MIC_EXIT:
		{
//			m_bInMicConfig = FALSE;
		}
		break;
	}
}

void CMicroscopeCameraView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CView::OnLButtonDblClk(nFlags, point);
    theApp.EnterDebugMode(true);

	if (m_IsMicSetting == TRUE)
	{
		//CMoveVisioDlg mdlg;
		//mdlg.DoModal();
	}
	else
	{
		CMicroscopeCameraView* pAtiveView = ((CMainFrame*)theApp.GetMainWnd())->GetUWView()->m_pMicroscopeCamera;	
 		StartSetCameraParaDlg(NULL, 0, pAtiveView);
	}	

    theApp.EnterDebugMode(false);
}

void CMicroscopeCameraView::SaveZPosToIni( CZPosType zPostype,int nPos )
{
	char FilePah[MAX_PATH] = {0},temp[MAX_PATH] = {0},FocusPath[MAX_PATH] = {0};
	char *SectionName = NULL;
	char *KeyName = NULL;

	GetModuleFileNameA(NULL, FilePah, MAX_PATH);
	PathRemoveFileSpecA(FilePah);
	strcpy_s(FocusPath,FilePah);
	PathAppendA(FilePah, "config\\setting.ini");
	PathAppendA(FocusPath, "config\\Focus.ini");

	if( zPostype == AChannel10 )
	{
		SectionName = "A10";
		KeyName     = "Ashift10";
	}
	else if( zPostype == BChannel10 )
	{
		SectionName = "B10";
		KeyName     = "Bshift10";
	}
	else if( zPostype == AChannel40 )
	{
		SectionName = "A40";
		KeyName     = "Ashift40";
	}
	else
	{
		SectionName = "B40";
		KeyName     = "Bshift40";
	}

	if( nPos > 3800 || nPos < 300 )
		nPos = 2000;
	sprintf_s(temp,"%d",nPos);
	WritePrivateProfileStringA(SectionName, "z_pos", temp, FilePah);		

	//WritePrivateProfileStringA("Focus", KeyName, temp, FocusPath);
}

void CMicroscopeCameraView::OnFocusBegin()
{
	CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
	if (!m_bInAdjustFocusMode)
	{
		pMain->OnCaiTuConfig(ID_END1_TEST);
		pMain->OnCaiTuConfig(ID_US1_MIC_CONFIG);
		if (FocusBeginAdjust())
		{
			CMenu *usmic = menu_usmic.GetSubMenu(0);
			if (usmic)
			{
				usmic->EnableMenuItem(ID_FOCUS_BEGIN, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
				//usmic->EnableMenuItem(ID_FOCUS_END, MF_BYCOMMAND | MF_ENABLED);
				usmic->EnableMenuItem(ID_FOCUS_RESET_Z, MF_BYCOMMAND | MF_ENABLED);
				usmic->EnableMenuItem(ID_FOCUS_SAVE_Z, MF_BYCOMMAND | MF_ENABLED);
				usmic->EnableMenuItem(ID_FOCUS_CANCEL, MF_BYCOMMAND | MF_ENABLED);
			}
			m_bInAdjustFocusMode = true;
		}
		else
		{
			pMain->OnCaiTuConfig(ID_US1_MIC_CONFIG);
			pMain->OnCaiTuConfig(ID_START1_TEST);
		}
	}
}

LRESULT CMicroscopeCameraView::OnFocusBegin(WPARAM,LPARAM)
{
	CMicroscopeCameraView *pView = ((CMainFrame*)theApp.GetMainWnd())->GetUWView()->m_pMicroscopeCamera;
	pView->SetFocus();
	DBG_MSG("CMicroscopeCameraView::receive message");
	OnFocusBegin();
	return 1;
}

void CMicroscopeCameraView::OnFocusEnd()
{
	CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
	if (m_bInAdjustFocusMode)
	{
		if (FocusEndAdjust())
		{
			CMenu *usmic = menu_usmic.GetSubMenu(0);
			if (usmic)
			{
				usmic->EnableMenuItem(ID_FOCUS_BEGIN, MF_BYCOMMAND | MF_ENABLED);
				//usmic->EnableMenuItem(ID_FOCUS_END, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
				usmic->EnableMenuItem(ID_FOCUS_SAVE_Z, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
				usmic->EnableMenuItem(ID_FOCUS_RESET_Z, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			}
			m_bInAdjustFocusMode = false;

			pMain->OnCaiTuConfig(ID_US1_MIC_CONFIG);
			pMain->OnCaiTuConfig(ID_START1_TEST);
		}
	}
}

LRESULT CMicroscopeCameraView::OnFocusEnd(WPARAM, LPARAM)
{
	OnFocusEnd();
	return 3;
}

void CMicroscopeCameraView::OnFocusSaveZ()
{
	bool ret = FocusSave();
	MessageBox((ret ? _T("保存Z轴位置成功!") : _T("保存Z轴位置失败!")), _T("提示"), MB_OK | (ret ? MB_ICONINFORMATION : MB_ICONWARNING));
	OnFocusEnd();
}


LRESULT CMicroscopeCameraView::OnFocusResetZ(WPARAM, LPARAM)
{
	OnFocusResetZ();
	return 4;
}

//当前正在自动调整亮度
LRESULT CMicroscopeCameraView::OnReceiveLight(WPARAM, LPARAM)
{

	return 0;
}

LRESULT CMicroscopeCameraView::OnShowLight(WPARAM w, LPARAM l) 
{
	CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();

	KSTATUS               status;
	PRVIEW_OUTPUT_CB      prv_cb = { 0 };
	prv_cb.nCaIndex = 0;

	if (w)
		prv_cb.pCB = UWPrviewCB;

	status = THInterface_SetDevCaps(T_DEV_CAMERA_TYPE, DevCB, (PCHAR)&prv_cb, sizeof(prv_cb), 0);
	return 5;
}

LRESULT CMicroscopeCameraView::OnFocusSaveZ(WPARAM,LPARAM)
{
	OnFocusSaveZ();
	return 2;
}

DWORD CMicroscopeCameraView::ResetZAdjustFun(LPVOID param)
{
	CMicroscopeCameraView* p = (CMicroscopeCameraView*)param;
	p->noResetZMode = FALSE;
	int num = 0;
	bool ret = false;
	do
	{
		theApp.continueResetZ = FALSE;
		ret = ResetZAdjust(num++);		
	} while (theApp.continueResetZ);

	p->MessageBox((ret ? _T("Z轴复位成功!") : _T("Z轴复位失败!")), _T("提示"), MB_OK | (ret ? MB_ICONINFORMATION : MB_ICONWARNING));
	p->noResetZMode = TRUE;
	

	return 0;
}

//////////////////////////////////////CharacterCamera////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CCharactorCameraView, CView)
BEGIN_MESSAGE_MAP(CCharactorCameraView, CView)
	ON_WM_SIZE()
	ON_WM_MOUSEWHEEL()
	ON_WM_RBUTTONDOWN()
	ON_UPDATE_COMMAND_UI_RANGE(ID_XYADJUST, ID_US_MIC_EXIT, &CCharactorCameraView::OnUpdateMicConfig)
	ON_COMMAND_RANGE(ID_XYADJUST, ID_US_MIC_EXIT, &CCharactorCameraView::OnMicConfig)
	ON_WM_LBUTTONDBLCLK()
	ON_BN_CLICKED(IDC_BTN_FOUCS, &CCharactorCameraView::OnFoucs)
END_MESSAGE_MAP()

CCharactorCameraView::CCharactorCameraView()
{
	m_bPrv = FALSE;
	m_IsMicSetting = FALSE;
	m_bInMicConfig = FALSE;
}

void CCharactorCameraView::OnDraw(CDC* pDC)
{
	/*CRect rc;
	int left = 0, top = 0;
	GetClientRect(&rc);
	if (rc.right > VIDEO_WIDTH)
		left = (rc.right - VIDEO_WIDTH) >> 1;
	if (rc.bottom > VIDEO_HIEHGT)
		top = (rc.bottom - VIDEO_HIEHGT) >> 1;

	pDC->FillSolidRect(left, top, VIDEO_WIDTH, VIDEO_HIEHGT, 0);*/

	int nWidth = 0, nHeight = 0;
	CRect rc;
	GetClientRect(&rc);
	rc.DeflateRect(5, 5, 5, 5);

	if (rc.Width() * 3 > rc.Height() * 4)
	{
		nHeight = rc.Height();
		nWidth = (int)(((float)nHeight) / 3 * 4);
	}
	else
	{
		nWidth = rc.Width();
		nHeight = (int)(((float)nWidth) / 4 * 3);
	}
	pDC->FillSolidRect(
		rc.left + (rc.Width() - nWidth) / 2,
		rc.top + (rc.Height() - nHeight) / 2,
		nWidth, nHeight, 0);
}

void CCharactorCameraView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	//  创建按钮
	//m_btnFocus.Create(strFoucs[0], WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(5, 40, 95, 75), this, IDC_BTN_FOUCS);
	//m_font.CreatePointFont(90, L"宋体", NULL);
	//m_btnFocus.SetFont(&m_font);

	menu_usmic.LoadMenu(IDR_MENU_FOCUS);
	SetMenu(&menu_usmic);
	menu_usmic.ModifyMenu(0, MF_BYPOSITION, 0, GetIdsString(_T("us_mic_config"), _T("title")).c_str());
	/*CMenu *usmic = menu_usmic.GetSubMenu(0);
	if (usmic)
	{
		usmic->ModifyMenu(ID_XYADJUST, MF_STRING | MF_BYCOMMAND, ID_XYADJUST, GetIdsString(_T("us_mic_config"), _T("XYADJUST")).c_str());
		usmic->ModifyMenu(ID_ZADJUST, MF_STRING | MF_BYCOMMAND, ID_ZADJUST, GetIdsString(_T("us_mic_config"), _T("ZADJUST")).c_str());
		usmic->ModifyMenu(ID_X_RESET, MF_STRING | MF_BYCOMMAND, ID_X_RESET, GetIdsString(_T("us_mic_config"), _T("X_RESET")).c_str());
		usmic->ModifyMenu(ID_Y_RESET, MF_STRING | MF_BYCOMMAND, ID_Y_RESET, GetIdsString(_T("us_mic_config"), _T("Y_RESET")).c_str());
		usmic->ModifyMenu(ID_Z_RESET, MF_STRING | MF_BYCOMMAND, ID_Z_RESET, GetIdsString(_T("us_mic_config"), _T("Z_RESET")).c_str());
		usmic->ModifyMenu(ID_US_MIC_EXIT, MF_STRING | MF_BYCOMMAND, ID_US_MIC_EXIT, GetIdsString(_T("us_mic_config"), _T("exit")).c_str());
	}*/

	SetVideoPrview(TRUE);
}

BOOL CCharactorCameraView::SetVideoPrview(BOOL bPrv)
{
	BOOL bRet;

	bRet = InteralVideoPrview(bPrv);
	if (bRet)
	{
		m_bPrv = bPrv;
	}

	return bRet;
}

void CCharactorCameraView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	if (m_bPrv)
		InteralVideoPrview(m_bPrv);
}

void CCharactorCameraView::OnFoucs()
{
	if (m_bInMicConfig)
	{
		m_btnFocus.SetWindowText(strFoucs[0]);
	}
	else
	{
		m_btnFocus.SetWindowText(strFoucs[1]);
	}
	((CMainFrame*)AfxGetMainWnd())->MicSet(m_bInMicConfig);
}

BOOL CCharactorCameraView::InteralVideoPrview(BOOL bPrv)
{
	/*CAMERA_PRVIEW_CONFIG   cam_cfg = { 0 };
	cam_cfg.nCaIndex = 1;

	/ *if (m_bPrv)
		return TRUE;* /

	if (bPrv == TRUE)
	{
		RECT   rc;

		GetClientRect(&rc);

		if (rc.right > VIDEO_WIDTH)
			cam_cfg.cPrviewRC.left = (rc.right - VIDEO_WIDTH) >> 1;

		if (rc.bottom > VIDEO_HIEHGT)
			cam_cfg.cPrviewRC.top = (rc.bottom - VIDEO_HIEHGT) >> 1;

		cam_cfg.cPrviewWnd = GetSafeHwnd();
		cam_cfg.cPrviewRC.bottom = VIDEO_HIEHGT + cam_cfg.cPrviewRC.top;
		cam_cfg.cPrviewRC.right = VIDEO_WIDTH + cam_cfg.cPrviewRC.left;
	}

	return (THInterface_SetCameraPrviewWindow(&cam_cfg) == STATUS_SUCCESS);*/

	int nWidth = 0, nHeight = 0;
	CAMERA_PRVIEW_CONFIG   cam_cfg = { 0 };
	cam_cfg.nCaIndex = 1;

	if (bPrv == TRUE)
	{
		CRect rc;
		GetClientRect(&rc);
		rc.DeflateRect(5,5,5,5);

		if (rc.Width() * 3 > rc.Height() * 4)
		{
			nHeight = rc.Height();
			nWidth = (int)(((float)nHeight) / 3 * 4);
		}
		else
		{
			nWidth = rc.Width();
			nHeight = (int)(((float)nWidth) / 4 * 3);
		}
		cam_cfg.cPrviewWnd = GetSafeHwnd();
		cam_cfg.cPrviewRC.left = rc.left + (rc.Width() - nWidth) / 2;
		cam_cfg.cPrviewRC.top = rc.top + (rc.Height() - nHeight) / 2;
		cam_cfg.cPrviewRC.bottom = nHeight + cam_cfg.cPrviewRC.top;
		cam_cfg.cPrviewRC.right = nWidth + cam_cfg.cPrviewRC.left;
	}

	return (THInterface_SetCameraPrviewWindow(&cam_cfg) == STATUS_SUCCESS);
}

BOOL CCharactorCameraView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
	if (m_bInMicConfig == FALSE)
	{
		return FALSE;
	}
	else
	{
		if (pMain->m_bPicPause != FALSE)
		{

		}
		else
		{
			return FALSE;
		}
	}

	unsigned char chPara[8] = { 0 };

	char temp[MAX_PATH] = { 0 };


	if (zDelta < 0)
	{

		MoveScanPlatform(ZMotorID, Backward, theApp.GetFocusStep());
		sprintf_s(temp, "-%d", theApp.GetFocusStep());

	}
	else
	{
		MoveScanPlatform(ZMotorID, Foreward, theApp.GetFocusStep());
		sprintf_s(temp, "%d", theApp.GetFocusStep());
	}

	CString str;
	str = temp;
	//pMain->OtherInfo((TCHAR*)str.GetBuffer());

	return CView::OnMouseWheel(nFlags, zDelta, pt);
}

static
VOID
CALLBACK
UWPrviewCB2(
HDC   hDC,
SIZE  sSize
)
{
	int   cx, cy;
	HPEN  hPen, hOldPen;

	cx = sSize.cx >> 2;
	cy = sSize.cy >> 2;

	hPen = CreatePen(PS_SOLID, 0, RGB(255, 0, 0));
	hOldPen = (HPEN)SelectObject(hDC, hPen);

	for (int j = 0; j < FG_SETP; j++)
	{
		// 划横线
		MoveToEx(hDC, j * cx, 0, NULL);
		LineTo(hDC, j * cx, sSize.cy);
		// 划竖线
		MoveToEx(hDC, 0, j * cy, NULL);
		LineTo(hDC, sSize.cx, j * cy);
	}
	// 划横线
	MoveToEx(hDC, 0, sSize.cy - 1, NULL);
	LineTo(hDC, sSize.cx, sSize.cy * 2);
	// 划竖线
	MoveToEx(hDC, sSize.cx - 1, 0, NULL);
	LineTo(hDC, sSize.cx, sSize.cy * 2);

	SelectObject(hDC, hOldPen);
	DeleteObject(hPen);
}

void CCharactorCameraView::StartUsMicConfig()
{
	BOOL                  bUSSet;
	KSTATUS               status;
	PRVIEW_OUTPUT_CB      prv_cb = { 0 };
	prv_cb.nCaIndex = 0;

	bUSSet = !m_bInMicConfig;

	if (bUSSet == TRUE)
		prv_cb.pCB = UWPrviewCB2;

	status = THInterface_SetDevCaps(T_DEV_CAMERA_TYPE, DevCB, (PCHAR)&prv_cb, sizeof(prv_cb), 0);
	if (status != STATUS_SUCCESS)
		return;

	m_bInMicConfig = bUSSet;
	m_IsMicSetting = !m_IsMicSetting;
}

void CCharactorCameraView::EndUsMicConfig()
{
	m_bInMicConfig = FALSE;
}

void CCharactorCameraView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
	if (m_bInMicConfig == FALSE)
	{
		return;
	}
	else
	{
		if (pMain->m_bPicPause != FALSE)
		{

		}
		else
		{
			return;
		}
	}

	// 右键菜单
	GetCursorPos(&point);
	theApp.GetContextMenuManager()->ShowPopupMenu(menu_usmic, point.x, point.y, this, TRUE);
	//TrackPopupMenu(menu_usmic,TPM_LEFTALIGN|TPM_LEFTBUTTON,point.x, point.y,0,GetSafeHwnd(),NULL);

	CView::OnRButtonDown(nFlags, point);
}

void CCharactorCameraView::OnUpdateMicConfig(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

void CCharactorCameraView::OnMicConfig(UINT nID)
{
	if (!m_bInMicConfig)
		return;

	CZPosType zpostype = AChannel10;
	WORD stepcount = 0, xoffset = 0, yoffset = 0;
	switch (nID)
	{
		// XY轴调整
	case ID_XYADJUST:
	{
		Save40XYSetting();
	}
	break;
	// Z轴调整
	case ID_ZADJUST:
	{
		SaveZSetting();

		if (QueryZPos(zpostype, stepcount))
		{
			SaveZPosToIni(zpostype, stepcount);
		}
	}
	break;
	// Z轴复位
	case ID_Z_RESET:
	{
		ZAxisReset();
	}
	break;
	case ID_X_RESET:
	{
		XAxisReset();
	}
	break;
	case ID_Y_RESET:
	{
		YAxisReset();
	}
	break;
	// 退出显微镜调整
	case ID_US_MIC_EXIT:
	{
		//			m_bInMicConfig = FALSE;
	}
	break;
	}
}

void CCharactorCameraView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CView::OnLButtonDblClk(nFlags, point);
	StartSetCameraParaDlg(NULL, 1); // 相机设置
}

void CCharactorCameraView::SaveZPosToIni(CZPosType zPostype, int nPos)
{
	char FilePah[MAX_PATH] = { 0 }, temp[MAX_PATH] = { 0 }, FocusPath[MAX_PATH] = { 0 };
	char *SectionName = NULL;
	char *KeyName = NULL;

	GetModuleFileNameA(NULL, FilePah, MAX_PATH);
	PathRemoveFileSpecA(FilePah);
	strcpy_s(FocusPath, FilePah);
	PathAppendA(FilePah, "config\\setting.ini");
	PathAppendA(FocusPath, "config\\Focus.ini");

	if (zPostype == AChannel10)
	{
		SectionName = "A10";
		KeyName = "Ashift10";
	}
	else if (zPostype == BChannel10)
	{
		SectionName = "B10";
		KeyName = "Bshift10";
	}
	else if (zPostype == AChannel40)
	{
		SectionName = "A40";
		KeyName = "Ashift40";
	}
	else
	{
		SectionName = "B40";
		KeyName = "Bshift40";
	}

	if (nPos > 3800 || nPos < 300)
		nPos = 2000;
	sprintf_s(temp, "%d", nPos);
	WritePrivateProfileStringA(SectionName, "z_pos", temp, FilePah);

	//WritePrivateProfileStringA("Focus", KeyName, temp, FocusPath);
}





void CMicroscopeCameraView::OnFocusResetZ()
{
	// TODO:  在此添加命令处理程序代码
	HANDLE resetZHandle;
	resetZHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ResetZAdjustFun, this, 0, NULL);
	if (resetZHandle == NULL)
	{
		DBG_MSG("resetZHandle create failed");
		return;
	}
}


BOOL CMicroscopeCameraView::PreTranslateMessage(MSG* pMsg)
{
	// TODO:  在此添加专用代码和/或调用基类
	if (!noResetZMode)
	{
		AfxGetApp()->BeginWaitCursor();
	}
	if (noResetZMode == false)
	{
		if (pMsg->message == WM_MOUSEWHEEL || 
			pMsg->message == WM_LBUTTONDOWN || 
			pMsg->message == WM_RBUTTONDOWN)
		{
			return TRUE;
		}
	}

	return CView::PreTranslateMessage(pMsg);
}
