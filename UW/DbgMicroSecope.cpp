// DbgMicroSecope.cpp : 实现文件
//

#include "stdafx.h"
#include "DbgMicroSecope.h"
#include "afxdialogex.h"
#include "CmdHelper.h"
#include "THInterface.h"
#include "..\..\..\inc\CommDef.h"


// CDbgMicroSecope 对话框

IMPLEMENT_DYNAMIC(CDbgMicroSecope, CPropertyPage)

CDbgMicroSecope::CDbgMicroSecope()
	: CPropertyPage(CDbgMicroSecope::IDD)
{
	m_channel = CHANNEL_A;
	m_mlens = MLENS_10X;
	m_bInAdjustFocusMode = false;
}

CDbgMicroSecope::~CDbgMicroSecope()
{
}

void CDbgMicroSecope::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}

static
VOID
CALLBACK
UWPrviewCB(
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

	for (int j = 0; j < 4; j++)
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
	LineTo(hDC, sSize.cx, sSize.cy - 1);
	// 划竖线
	MoveToEx(hDC, sSize.cx - 1, 0, NULL);
	LineTo(hDC, sSize.cx - 1, sSize.cy);

	SelectObject(hDC, hOldPen);
	DeleteObject(hPen);
}

bool CDbgMicroSecope::SetCameraPreview(int nCameraIndex)
{
	int nWidth = 0, nHeight = 0;
	CAMERA_PRVIEW_CONFIG   cam_cfg = { 0 };
	cam_cfg.nCaIndex = nCameraIndex;

	CRect rc;
	CWnd *pPreviewWnd = GetDlgItem(IDC_STATIC_CAMERA);
	if (pPreviewWnd == NULL || !::IsWindow(pPreviewWnd->GetSafeHwnd()))
	{
		return false;
	}
	pPreviewWnd->GetClientRect(&rc);
	rc.DeflateRect(1, 1, 1, 1);

	if (rc.Width()*3 > rc.Height()*4)
	{
		nHeight = rc.Height();
		nWidth = (int)(((float)nHeight) / 3 * 4);
	}
	else
	{
		nWidth = rc.Width();
		nHeight = (int)(((float)nWidth) / 4 * 3);
	}
	cam_cfg.cPrviewWnd = pPreviewWnd->GetSafeHwnd();
	cam_cfg.cPrviewRC.left = rc.left + (rc.Width() - nWidth) >> 1;
	cam_cfg.cPrviewRC.top = rc.top + (rc.Height() - nHeight) >> 1;
	cam_cfg.cPrviewRC.bottom = nHeight + cam_cfg.cPrviewRC.top;
	cam_cfg.cPrviewRC.right = nWidth + cam_cfg.cPrviewRC.left;

	if (THInterface_SetCameraPrviewWindow(&cam_cfg) != STATUS_SUCCESS)
	{
		return false;
	}

	//////////////////
	PRVIEW_OUTPUT_CB  prv_cb = { 0 };
	prv_cb.nCaIndex = nCameraIndex;
	prv_cb.pCB = UWPrviewCB;

	return (THInterface_SetDevCaps(T_DEV_CAMERA_TYPE, DevCB, (PCHAR)&prv_cb, sizeof(prv_cb), 0)== STATUS_SUCCESS);
}

BEGIN_MESSAGE_MAP(CDbgMicroSecope, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON1, &CDbgMicroSecope::OnBnClickedLedTurnOn)
	ON_BN_CLICKED(IDC_BUTTON13, &CDbgMicroSecope::OnBnClickedLedTurnOff)
	ON_BN_CLICKED(IDC_BUTTON15, &CDbgMicroSecope::OnBnClickedChangeChannel)
	ON_BN_CLICKED(IDC_BUTTON31, &CDbgMicroSecope::OnBnClickedChangeCaptureImageArea)
	ON_BN_CLICKED(IDC_RADIO1, &CDbgMicroSecope::OnBnClickedChannelA)
	ON_BN_CLICKED(IDC_RADIO2, &CDbgMicroSecope::OnBnClickedChannelB)
	ON_BN_CLICKED(IDC_RADIO3, &CDbgMicroSecope::OnBnClickedOnBnClickedChannelC)
	ON_BN_CLICKED(IDC_RADIO4, &CDbgMicroSecope::OnBnClickedChannelD)
	ON_BN_CLICKED(IDC_RADIO5, &CDbgMicroSecope::OnBnClickedM10Lens)
	ON_BN_CLICKED(IDC_RADIO6, &CDbgMicroSecope::OnBnClickedM40Lens)
	ON_BN_CLICKED(IDC_BUTTON2, &CDbgMicroSecope::OnBnClickedFocusOption)
	ON_BN_CLICKED(IDC_BUTTON3, &CDbgMicroSecope::OnBnClickedSaveZAxisPos)
	ON_WM_MOUSEWHEEL()
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// CDbgMicroSecope 消息处理程序


BOOL CDbgMicroSecope::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	((CButton *)GetDlgItem(IDC_RADIO1))->SetCheck(TRUE);
	((CButton *)GetDlgItem(IDC_RADIO5))->SetCheck(TRUE);
	((CComboBox *)GetDlgItem(IDC_COMBO1))->SetCurSel(0);
	((CComboBox *)GetDlgItem(IDC_COMBO2))->SetCurSel(0);

	// 设置相机预览
	//SetCameraPreview(0);

	char buf[MAX_PATH] = { 0 };
	GetModuleFileNameA(NULL, buf, MAX_PATH);
	PathRemoveFileSpecA(buf);
	PathAppendA(buf, "Config\\Focus.ini");
	m_strIniFocus = buf;

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

void CDbgMicroSecope::OnDestroy()
{
	CPropertyPage::OnDestroy();

	//PRVIEW_OUTPUT_CB  prv_cb = { 0 };
	//prv_cb.nCaIndex = 0;
	//THInterface_SetDevCaps(T_DEV_CAMERA_TYPE, DevCB, (PCHAR)&prv_cb, sizeof(prv_cb), 0);
}

void CDbgMicroSecope::OnBnClickedLedTurnOn()
{
	MicrosopeLedSwitch();
}


void CDbgMicroSecope::OnBnClickedLedTurnOff()
{
	MicrosopeLedSwitch(false);
}


void CDbgMicroSecope::OnBnClickedChangeChannel()
{
	COUNT_POOL_CHANNEL result;
	ChangeChannel(m_channel, result);
}

void CDbgMicroSecope::OnBnClickedChannelA()
{
	m_channel = CHANNEL_A;
}

void CDbgMicroSecope::OnBnClickedChannelB()
{
	m_channel = CHANNEL_B;
}

void CDbgMicroSecope::OnBnClickedOnBnClickedChannelC()
{
	m_channel = CHANNEL_C;
}


void CDbgMicroSecope::OnBnClickedChannelD()
{
	m_channel = CHANNEL_D;
}

void CDbgMicroSecope::OnBnClickedChangeCaptureImageArea()
{
	CAPTURE_IMAGE_AREA_CONTEXT dstArea = { 0 }, resultArea = { 0 };
	dstArea.AreaX = ((CComboBox *)GetDlgItem(IDC_COMBO1))->GetCurSel();
	dstArea.AreaY = ((CComboBox *)GetDlgItem(IDC_COMBO2))->GetCurSel();
	if (m_mlens == MLENS_10X)
	{
		ChangeM10LensArea(dstArea, resultArea);
	}
	else
	{
		ChangeM40LensArea(dstArea, resultArea);
	}
}

void CDbgMicroSecope::OnBnClickedM10Lens()
{
	m_mlens = MLENS_10X;
	CComboBox* cbx1 = ((CComboBox *)GetDlgItem(IDC_COMBO1));
	CComboBox* cbx2 = ((CComboBox *)GetDlgItem(IDC_COMBO2));
	cbx1->ResetContent();
	cbx2->ResetContent();
	for (int i = 0; i < 8; i++)
	{
		CString str;
		str.Format(_T("%d"), i);
		cbx1->AddString(str);
	}
	for (int i = 0; i < 3; i++)
	{
		CString str;
		str.Format(_T("%d"), i);
		cbx2->AddString(str);
	}
	cbx1->SetCurSel(0);
	cbx2->SetCurSel(0);
}


void CDbgMicroSecope::OnBnClickedM40Lens()
{
	m_mlens = MLENS_40X;
	CComboBox* cbx1 = ((CComboBox *)GetDlgItem(IDC_COMBO1));
	CComboBox* cbx2 = ((CComboBox *)GetDlgItem(IDC_COMBO2));
	cbx1->ResetContent();
	cbx2->ResetContent();
	for (int i = 0; i < 4; i++)
	{
		CString str;
		str.Format(_T("%d"), i);
		cbx1->AddString(str);
		cbx2->AddString(str);
	}
	cbx1->SetCurSel(0);
	cbx2->SetCurSel(0);
}


void CDbgMicroSecope::OnBnClickedFocusOption()
{
	if (m_bInAdjustFocusMode)
	{
		FocusEndAdjust();
		SetDlgItemText(IDC_BUTTON2, _T("开始调焦"));
		GetDlgItem(IDC_BUTTON3)->EnableWindow(FALSE);
	}
	else
	{
		FocusBeginAdjust();
		SetDlgItemText(IDC_BUTTON2, _T("结束调焦"));
		GetDlgItem(IDC_BUTTON3)->EnableWindow(TRUE);
	}
	m_bInAdjustFocusMode = !m_bInAdjustFocusMode;

	
}


void CDbgMicroSecope::OnBnClickedSaveZAxisPos()
{
	bool ret = FocusSave();
	MessageBox(( ret ? _T("保存Z轴位置成功!") : _T("保存Z轴位置失败!")), _T("提示"), MB_OK | (ret ? MB_ICONINFORMATION : MB_ICONWARNING));
}


BOOL CDbgMicroSecope::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (m_bInAdjustFocusMode)
	{
		int nStep = GetPrivateProfileIntA("Focus", "step", 8, m_strIniFocus.c_str());

		if (zDelta < 0)
		{
			FocusAdjust(FOCUS_DIRECTION::DIRECTION_BACKWARD, nStep);
		}
		else
		{
			FocusAdjust(FOCUS_DIRECTION::DIRECTION_FOREWARD, nStep);
		}
	}

	return CPropertyPage::OnMouseWheel(nFlags, zDelta, pt);
}


void CDbgMicroSecope::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO:  在此处添加消息处理程序代码
	// 不为绘图消息调用 CPropertyPage::OnPaint()

	CRect rt;
	GetDlgItem(IDC_STATIC_CAMERA)->GetClientRect(rt);
	rt.DeflateRect(1, 1, 1, 1);
	CDC* pDC = GetDlgItem(IDC_STATIC_CAMERA)->GetDC();
	pDC->FillSolidRect(rt, RGB(0, 0, 0));
	ReleaseDC(pDC);
}





void CDbgMicroSecope::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CPropertyPage::OnShowWindow(bShow, nStatus);
// 	RECT rect;
// 	GetParent()->GetWindowRect(&rect);
// 	int nWidth = rect.right - rect.left;
// 	int nHeight = rect.bottom - rect.top;
// 	if (bShow)
// 	{
// 		GetParent()->GetParent()->SetWindowPos(NULL, 0, 0, nWidth, nHeight, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
// 	}
	// TODO:  在此处添加消息处理程序代码
}
