// MoveVisioDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "UW.h"
#include "MoveVisioDlg.h"
#include "AutoFocusMgr.h"

#include "DbgMsg.h"

#define A_CHANNEL 0
#define B_CHANNEL 1
#define C_CHANNEL 2
#define D_CHANNEL 3

IMPLEMENT_DYNAMIC(CMoveVisioDlg, CPropertyPage)

CMoveVisioDlg::CMoveVisioDlg(CWnd* pParent /*=NULL*/)
: CPropertyPage(CMoveVisioDlg::IDD)
	, m_nZPos(0)
{		
	m_nTotialStep = 0;
	m_XTotalStep = 0;
	m_YTotalStep = 0;
	m_nChannelNums = A_CHANNEL;
	Clear();
	m_oldPostion.SetPoint(0, 0);
	m_bLedLightOn = false;
	m_bInAdjustFocusMode = false;
	m_nMicroLens = 10;
}

CMoveVisioDlg::~CMoveVisioDlg()
{
}

void CMoveVisioDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_10X_STATIC, m_10x_static);
	DDX_Control(pDX, IDC_40X_STATIC, m_40x_static);
	DDX_Control(pDX, IDC_ERR_INFO_STATIC, m_err_info_static);
	DDX_Control(pDX, IDC_MORE_RIGHT_BTN, m_more_right_btn);
	DDX_Control(pDX, IDC_RIGHT_BTN, m_right_btn);
	DDX_Control(pDX, IDC_UP_BTN, m_up_btn);
	DDX_Control(pDX, IDC_DOWN_BTN, m_down_btn);
	DDX_Control(pDX, IDC_LEFT_BTN, m_left_btn);
	DDX_Control(pDX, IDC_MORE_LEFT_BTN, m_more_left_btn);
	DDX_Control(pDX, IDC_SET_EDIT, m_set_edit);
	DDX_Control(pDX, IDC_STATIC_NOTICE, m_static_notice);
	DDX_Text(pDX, IDC_EDIT1, m_nZPos);
	DDX_Control(pDX, IDC_10X_BUTTON, m_btn_post_10X);
	DDX_Control(pDX, IDC_40X_BUTTON, m_btn_post_40X);
}


BEGIN_MESSAGE_MAP(CMoveVisioDlg, CDialog)
	ON_BN_CLICKED(IDC_CHANNELA_BTN, &CMoveVisioDlg::OnBnClickedChannelaBtn)
	ON_BN_CLICKED(IDC_CHANNELB_BTN, &CMoveVisioDlg::OnBnClickedChannelbBtn)
	ON_BN_CLICKED(IDC_CHANNELC_BTN, &CMoveVisioDlg::OnBnClickedChannelcBtn)
	ON_BN_CLICKED(IDC_CHANNELD_BTN, &CMoveVisioDlg::OnBnClickedChanneldBtn)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_UP_BTN, &CMoveVisioDlg::OnBnClickedUpBtn)
	ON_BN_CLICKED(IDC_DOWN_BTN, &CMoveVisioDlg::OnBnClickedDownBtn)
	ON_BN_CLICKED(IDC_RIGHT_BTN, &CMoveVisioDlg::OnBnClickedRightBtn)
	ON_BN_CLICKED(IDC_MORE_RIGHT_BTN, &CMoveVisioDlg::OnBnClickedMoreRightBtn)
	ON_BN_CLICKED(IDC_LEFT_BTN, &CMoveVisioDlg::OnBnClickedLeftBtn)
	ON_BN_CLICKED(IDC_MORE_LEFT_BTN, &CMoveVisioDlg::OnBnClickedMoreLeftBtn)
	ON_BN_CLICKED(IDOK, &CMoveVisioDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CMoveVisioDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_SAVE_CAMERA_POS_BTN, &CMoveVisioDlg::OnBnClickedSaveCameraPosBtn)
	ON_BN_CLICKED(IDC_RESET_CAMERA_BUTTON, &CMoveVisioDlg::OnBnClickedResetCameraButton)  // not use
	ON_WM_MOUSEWHEEL()
	ON_BN_CLICKED(IDC_10X_BUTTON, &CMoveVisioDlg::OnBnClicked10xButton)
	ON_BN_CLICKED(IDC_40X_BUTTON, &CMoveVisioDlg::OnBnClicked40xButton)
	ON_BN_CLICKED(IDC_BUTTON6, &CMoveVisioDlg::OnBnClickedButton6)  // not use
	ON_BN_CLICKED(IDC_BUTTON7, &CMoveVisioDlg::OnBnClickedButton7)  // not use
	ON_BN_CLICKED(IDC_MOVE_Z, &CMoveVisioDlg::OnBnClickedMoveZ)
	ON_BN_CLICKED(IDC_BTN_MOVETO, &CMoveVisioDlg::OnBnClickedBtnMoveto)
	ON_BN_CLICKED(IDC_BTN_LED_CTRL, &CMoveVisioDlg::OnBnClickedBtnLedCtrl)
	ON_BN_CLICKED(IDC_BTN_ADJUST_FOCUS, &CMoveVisioDlg::OnBnClickedBtnAdjustFocus)
	ON_BN_CLICKED(IDC_SAVE_Z_POS, &CMoveVisioDlg::OnBnClickedSaveZPos)
	ON_BN_CLICKED(IDC_BUTTON9, &CMoveVisioDlg::OnBnClickedButtonAction1)
	ON_BN_CLICKED(IDC_BUTTON10, &CMoveVisioDlg::OnBnClickedButtonAction2)
	ON_BN_CLICKED(IDC_BUTTON11, &CMoveVisioDlg::OnBnClickedButtonAction3)
	ON_BN_CLICKED(IDC_BUTTON12, &CMoveVisioDlg::OnBnClickedButtonAction4)
	ON_BN_CLICKED(IDC_BUTTON14, &CMoveVisioDlg::OnBnClickedButtonAction5)
	ON_BN_CLICKED(IDC_BUTTON15, &CMoveVisioDlg::OnBnClickedButtonAction6)
	ON_BN_CLICKED(IDC_BUTTON16, &CMoveVisioDlg::OnBnClickedButtonAction7)
	ON_EN_CHANGE(IDC_EDIT_STEPCOUNT, &CMoveVisioDlg::OnEnChangeEditStepcount)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

 
BOOL CMoveVisioDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	//CButton* pBtn =(CButton*) GetDlgItem(IDC_CHANNELA_BTN);
	//pBtn->SetCheck(1);

	EnableToolTips(TRUE);
	m_tooltipctrl.Create(this);
	m_tooltipctrl.AddTool(this,_T("windows"));
	m_tooltipctrl.SetDelayTime(100);
	m_tooltipctrl.Activate(true);

	InitializeCtrls();
	//OnBnClickedChannelaBtn();
	EnablePostBtns(TRUE);
	SetBtnsStatus(false);

	char buf[MAX_PATH] = { 0 };
	GetModuleFileNameA(NULL, buf, MAX_PATH);
	PathRemoveFileSpecA(buf);
	PathAppendA(buf, "Config\\Focus.ini");
	m_strIniFocus = buf;


	((CButton *)GetDlgItem(IDC_RADIO_X))->SetCheck(TRUE);
	((CComboBox*)GetDlgItem(IDC_COMBO_SPEED))->SetCurSel(0);
	GetDlgItem(IDC_EDIT_STEPCOUNT)->SetWindowText(_T("10"));

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CMoveVisioDlg::InitializeCtrls(void)
{
	m_40x_static.ShowWindow(SW_HIDE);
	m_selected_10x_pos.nColumn = -1;
	m_selected_10x_pos.nLines  = -1;
    m_10xIndex = -1;
	m_selected_40x_pos.nColumn = -1;
	m_selected_40x_pos.nLines  = -1;
	m_40xIndex = -1;

	m_10x_static.GetClientRect(&m_10x_rect_info.m_client_rect);
	m_10x_static.GetWindowRect(&m_10x_rect_info.m_window_rect);
	ScreenToClient(&m_10x_rect_info.m_window_rect);

	m_10x_rect_info.m_width = m_10x_rect_info.m_client_rect.Width();
	m_10x_rect_info.m_height = m_10x_rect_info.m_client_rect.Height();

	UW2000_VISIO_LINE_COLUMN info;
	info.nChannel = m_nChannelNums;
	theApp.GetConfigVisioLineAndColumn(&info);
	m_10x_rect_info.m_Lines = info.nLines;
	m_10x_rect_info.m_Column = info.nColumn;
	
	m_10x_rect_info.m_Max_Lines = MAX_10X_LINES;
	m_10x_rect_info.m_Max_Column = MAX_10X_COLUMN;
	m_10x_rect_info.m_nCounts = MAX_10X_LINES*MAX_10X_COLUMN;

	m_10x_rect_info.m_nType = 0;
	InitializeRect(&m_10x_rect_info);


	m_40x_static.GetClientRect(&m_40x_rect_info.m_client_rect);
	m_40x_static.GetWindowRect(&m_40x_rect_info.m_window_rect);
	ScreenToClient(&m_40x_rect_info.m_window_rect);

	m_40x_rect_info.m_width = m_40x_rect_info.m_client_rect.Width();
	m_40x_rect_info.m_height = m_40x_rect_info.m_client_rect.Height();

	m_40x_rect_info.m_Lines = MAX_40X_LINES;
	m_40x_rect_info.m_Column = MAX_40X_COLUMN;

	m_40x_rect_info.m_Max_Lines = MAX_40X_LINES;
	m_40x_rect_info.m_Max_Column = MAX_40X_COLUMN;

	m_40x_rect_info.m_nCounts = MAX_40X_LINES*MAX_40X_COLUMN;

	m_40x_rect_info.m_nType = 1;
	InitializeRect(&m_40x_rect_info);

}

void CMoveVisioDlg::InitializeRect(UW2000_RECT_INFO* pinfo)
{
	int movex = pinfo->m_window_rect.left ;
	int movey = pinfo->m_window_rect.top;
	int yCounts = pinfo->m_Max_Column;
	int xCounts = pinfo->m_Max_Lines;

	int nIndex = 0;
	for ( int i = 0 ; i < yCounts ; i ++)
	{
		for ( int j = 0 ; j < xCounts ; j ++)
		{
			pinfo->m_visio_info.m_VisioRect[i*xCounts+j].left = pinfo->m_width * j / xCounts;
			pinfo->m_visio_info.m_VisioRect[i*xCounts+j].top  =  pinfo->m_height * i / yCounts;
			pinfo->m_visio_info.m_VisioRect[i*xCounts+j].right = pinfo->m_width * (j +1)/xCounts;
			pinfo->m_visio_info.m_VisioRect[i*xCounts+j].bottom = pinfo->m_height * (i + 1)/yCounts;
			pinfo->m_visio_info.m_nIndex[i*xCounts+j] = i*xCounts+j;
			pinfo->m_visio_info.m_nSelected[i*xCounts+j] = 0;

			if(pinfo->m_nType == 0)
			{
				UW2000_VISIO_INFO info;
				info.nChannel = m_nChannelNums;
				info.nIndex = pinfo->m_visio_info.m_nIndex[i*xCounts+j] ;
				theApp.GetConfigVisioVaule(&info);
				pinfo->m_visio_info.m_bIsDefault[i*xCounts+j] = info.nSelect;
			}
			else if(pinfo->m_nType == 1)
			{
				pinfo->m_visio_info.m_bIsDefault[i*xCounts+j] = TRUE;
			}
			else{}

		}
	}

	for ( int i = 0 ; i < yCounts ; i ++)
	{
		for ( int j = 0 ; j < xCounts ; j ++)
		{
			pinfo->m_window_visio_info.m_VisioRect[i*xCounts+j].CopyRect(pinfo->m_visio_info.m_VisioRect[i*xCounts+j]);
			pinfo->m_window_visio_info.m_VisioRect[i*xCounts+j].InflateRect(-movex,-movey,movex,movey);
			pinfo->m_window_visio_info.m_bIsDefault[i*xCounts+j] = pinfo->m_visio_info.m_bIsDefault[i*xCounts+j];
			pinfo->m_window_visio_info.m_nIndex[i*xCounts+j] = i*xCounts+j;
			pinfo->m_window_visio_info.m_nSelected[i*xCounts+j] = 0;
		}
	}
}


void CMoveVisioDlg::Clear()
{

	m_10x_rect_info.m_client_rect.SetRectEmpty();
	m_10x_rect_info.m_height = 0;
	m_10x_rect_info.m_width  = 0;
	m_10x_rect_info.m_nCounts = 0;
	m_10x_rect_info.m_Lines = 0;
	m_10x_rect_info.m_Column = 0;
	m_10x_rect_info.m_Max_Lines = 0;
	m_10x_rect_info.m_Max_Column = 0;
	m_10x_rect_info.m_nType = 0;

	m_40x_rect_info.m_client_rect.SetRectEmpty();
	m_40x_rect_info.m_height = 0;
	m_40x_rect_info.m_width = 0;
	m_40x_rect_info.m_nCounts = 0;
	m_40x_rect_info.m_Lines = 0;
	m_40x_rect_info.m_Column = 0;
	m_40x_rect_info.m_Max_Lines = 0;
	m_40x_rect_info.m_Max_Column = 0;
	m_40x_rect_info.m_nType = 1;

	for ( int i = 0 ; i < MAX_VISIO_ITEMCOUNTS ; i ++)
	{
		m_10x_rect_info.m_visio_info.m_nSelected[i] = 0;
		m_10x_rect_info.m_visio_info.m_bIsDefault[i] = 0;
		m_10x_rect_info.m_visio_info.m_nIndex[i] = 0;
		m_10x_rect_info.m_visio_info.m_VisioRect[i].SetRectEmpty();

		m_10x_rect_info.m_window_visio_info.m_nSelected[i] = 0;
		m_10x_rect_info.m_window_visio_info.m_bIsDefault[i] = 0;
		m_10x_rect_info.m_window_visio_info.m_nIndex[i] = 0;
		m_10x_rect_info.m_window_visio_info.m_VisioRect->SetRectEmpty();

		m_40x_rect_info.m_visio_info.m_nSelected [i] = 0;
		m_40x_rect_info.m_visio_info.m_bIsDefault [i] = 0;
		m_40x_rect_info.m_visio_info.m_nIndex[i] = 0;
		m_40x_rect_info.m_visio_info .m_VisioRect[i].SetRectEmpty();

		m_40x_rect_info.m_window_visio_info.m_nSelected[i] = 0;
		m_40x_rect_info.m_window_visio_info.m_bIsDefault[i] = 0;
		m_40x_rect_info.m_window_visio_info.m_nIndex[i] = 0;
		m_40x_rect_info.m_window_visio_info.m_VisioRect->SetRectEmpty();
	}
}

void CMoveVisioDlg::DrawIndex(HDC hdc,UW2000_RECT_INFO info)
{

	int nMode = SetBkMode(hdc,TRANSPARENT);
	COLORREF dwColor = SetTextColor(hdc,RGB(255,140,0));

	for (int i = 0 ; i < info.m_nCounts ; i ++)
	{
		CString sData;

		if(info.m_visio_info.m_bIsDefault[i] == TRUE)
		{
			if (info.m_window_visio_info.m_nSelected[i] == 1)
			{
				SetTextColor(hdc, RGB(255, 255, 255));
			}
			else
			{
				SetTextColor(hdc, RGB(255, 140, 0));
			}
			sData.Format(_T("%d"),info.m_visio_info.m_nIndex[i]+1);
			DrawText(hdc, sData, -1, &info.m_visio_info.m_VisioRect[i], DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		}
		
	}

	SetBkMode(hdc,nMode);
	SetTextColor(hdc,dwColor);
}

void CMoveVisioDlg::DrawVisioRgn(int nID)
{

	CWnd* pWnd = GetDlgItem(nID);

	if(pWnd == NULL)
		return ;

	CDC* pDC = pWnd->GetDC();

	if(pDC == NULL)
		return;

	HDC hdc = pDC->GetSafeHdc();

	pWnd->Invalidate(); 
	pWnd->UpdateWindow(); 

	if(nID == IDC_10X_STATIC)
	{
		for ( int i = 0 ; i < m_10x_rect_info.m_nCounts ; i ++)
		{
			CBrush *OldBrush = NULL; 
			CBrush NewBrush; 

			if(m_10x_rect_info.m_window_visio_info.m_bIsDefault[i] == TRUE)
			{
				if(m_10x_rect_info.m_window_visio_info.m_nSelected[i] == 1)
				{
					NewBrush.CreateSolidBrush(RGB(255,165,0)); 
				}
				else
				{
					NewBrush.CreateSolidBrush(RGB(255,255,255));
				}
			}
			else
			{
				NewBrush.CreateSolidBrush(RGB(255,255,255)); 
			}

			OldBrush = pDC-> SelectObject(&NewBrush); 
			pDC->Rectangle(&m_10x_rect_info.m_visio_info.m_VisioRect[i]);
			pDC->SelectObject(OldBrush); 
		}

		DrawIndex(hdc,m_10x_rect_info);
	}
	else if(nID == IDC_40X_STATIC)
	{
		for ( int i = 0 ; i < m_40x_rect_info.m_nCounts ; i ++)
		{
			CBrush *OldBrush = NULL; 
			CBrush NewBrush; 

			if(m_40x_rect_info.m_window_visio_info.m_bIsDefault[i] == TRUE)
			{
				if(m_40x_rect_info.m_window_visio_info.m_nSelected[i] == 1)
				{
					NewBrush.CreateSolidBrush(RGB(255,165,0)); 
				}
				else
				{
					NewBrush.CreateSolidBrush(RGB(240,255,240)); 
				}
			}
			else
			{
				NewBrush.CreateSolidBrush(RGB(255,255,255)); 
			}

			OldBrush = pDC-> SelectObject(&NewBrush); 
			pDC->Rectangle(&m_40x_rect_info.m_visio_info.m_VisioRect[i]);
			pDC->SelectObject(OldBrush); 
		}

		DrawIndex(hdc,m_40x_rect_info);
	}

	ReleaseDC(pDC); 
}


void CMoveVisioDlg::OnBnClickedChannelaBtn()
{
	m_XTotalStep = 0;
	m_YTotalStep = 0;

	EnablePostBtns(TRUE);

	m_nChannelNums = A_CHANNEL;
	Clear();

	InitializeCtrls();
	m_40x_static.ShowWindow(SW_SHOW);
	DrawVisioRgn(IDC_10X_STATIC);
	DrawVisioRgn(IDC_40X_STATIC);

	//ConvertToAChannel();	
	COUNT_POOL_CHANNEL cpc;
	ChangeChannel(COUNT_POOL_CHANNEL::CHANNEL_A, cpc);
}

void CMoveVisioDlg::OnBnClickedChannelbBtn()
{
	m_XTotalStep = 0;
	m_YTotalStep = 0;

	EnablePostBtns(TRUE);

	m_nChannelNums = B_CHANNEL;
	Clear();
	InitializeCtrls();
	m_40x_static.ShowWindow(SW_SHOW);
	DrawVisioRgn(IDC_10X_STATIC);
	DrawVisioRgn(IDC_40X_STATIC);

	COUNT_POOL_CHANNEL cpc;
	ChangeChannel(COUNT_POOL_CHANNEL::CHANNEL_B, cpc);
}

void CMoveVisioDlg::OnBnClickedChannelcBtn()
{
	m_XTotalStep = 0;
	m_YTotalStep = 0;

	EnablePostBtns(TRUE);

	m_nChannelNums = C_CHANNEL;
	Clear();
	InitializeCtrls();
	m_40x_static.ShowWindow(SW_SHOW);
	DrawVisioRgn(IDC_10X_STATIC);
	DrawVisioRgn(IDC_40X_STATIC);
	
	COUNT_POOL_CHANNEL cpc;
	ChangeChannel(COUNT_POOL_CHANNEL::CHANNEL_C, cpc);
}

void CMoveVisioDlg::OnBnClickedChanneldBtn()
{
	m_XTotalStep = 0;
	m_YTotalStep = 0;

	EnablePostBtns(TRUE);

	m_nChannelNums = D_CHANNEL;
	Clear();
	InitializeCtrls();
	m_40x_static.ShowWindow(SW_SHOW);
	DrawVisioRgn(IDC_10X_STATIC);
	DrawVisioRgn(IDC_40X_STATIC);
	
	COUNT_POOL_CHANNEL cpc;
	ChangeChannel(COUNT_POOL_CHANNEL::CHANNEL_D, cpc);
}

void CMoveVisioDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	DrawVisioRgn(IDC_10X_STATIC);
	DrawVisioRgn(IDC_40X_STATIC);
}

void CMoveVisioDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	CString str;
	CRect rt1, rt2;
	int nIndex = -1;
	bool bIs40x = false;

	if (point == m_oldPostion)
	{
		CDialog::OnMouseMove(nFlags, point);
		return;
	}
	m_oldPostion = point;

	m_10x_static.GetWindowRect(rt1);
	ScreenToClient(rt1);
	m_40x_static.GetWindowRect(rt2);
	ScreenToClient(rt2);

	if (rt1.PtInRect(point) || rt2.PtInRect(point))
	{
		for (int i = 0; i <= m_10x_rect_info.m_nCounts; i++)
		{
			if (m_10x_rect_info.m_window_visio_info.m_VisioRect[i].PtInRect(point))
			{
				nIndex = m_10x_rect_info.m_window_visio_info.m_nIndex[i];
				bIs40x = false;
			}
		}

		for (int i = 0; i <= m_40x_rect_info.m_nCounts; i++)
		{
			if (m_40x_rect_info.m_window_visio_info.m_VisioRect[i].PtInRect(point))
			{
				nIndex = m_40x_rect_info.m_window_visio_info.m_nIndex[i];
				bIs40x = true;
			}
		}

		if (nIndex != -1)
		{
			if (bIs40x == false)
			{
				/*str.Format(_T("10倍窗口%d 坐标:%d,%d"),
					nIndex+1,
					point.x-m_10x_rect_info.m_window_rect.left
					,point.y-m_10x_rect_info.m_window_rect.top);*/
				str.Format(_T("10倍窗口%d"), nIndex + 1);

				if (m_10x_rect_info.m_window_visio_info.m_bIsDefault[nIndex] == TRUE)
				{
					m_tooltipctrl.UpdateTipText(str, this);
				}
			}
			else
			{
				/*str.Format(_T("40倍窗口%d 坐标:%d,%d"),
					nIndex+1,
					point.x-m_40x_rect_info.m_window_rect.left
					,point.y-m_40x_rect_info.m_window_rect.top);*/
				str.Format(_T("40倍窗口%d"), nIndex + 1);

				if (m_40x_rect_info.m_window_visio_info.m_bIsDefault[nIndex] == TRUE)
				{
					m_tooltipctrl.UpdateTipText(str, this);
				}
			}
		}
	}
	else
	{
		m_tooltipctrl.UpdateTipText(_T(""), this);
	}

	CDialog::OnMouseMove(nFlags, point);
}

BOOL CMoveVisioDlg::PreTranslateMessage(MSG* pMsg)
{
	m_tooltipctrl.RelayEvent(pMsg);
	return CDialog::PreTranslateMessage(pMsg);
}

void CMoveVisioDlg::OnLButtonDown(UINT nFlags, CPoint point)
{

	for ( int j = 0 ; j < m_10x_rect_info.m_Max_Column ; j ++)
	{
		for(int i = 0 ; i < m_10x_rect_info.m_Max_Lines ; i ++)
		{
			if(m_10x_rect_info.m_window_visio_info.m_VisioRect[m_10x_rect_info.m_Max_Lines*j+i].PtInRect(point))
			{
				if(m_10xIndex != -1)
				{
					m_10x_rect_info.m_window_visio_info.m_nSelected[m_10xIndex] = 0;	
				}

				m_10xIndex = m_10x_rect_info.m_Max_Lines*j+i;
				m_10x_rect_info.m_window_visio_info.m_nSelected[m_10x_rect_info.m_Max_Lines*j+i] = 1;	

				m_selected_10x_pos.nColumn = j;
				m_selected_10x_pos.nLines = i;

				if(m_40xIndex != -1)
				{
					m_40x_rect_info.m_window_visio_info.m_nSelected[m_40xIndex] = 0;	
					m_40xIndex != -1;
				}
				DrawVisioRgn(IDC_40X_STATIC);
				DrawVisioRgn(IDC_10X_STATIC);
				SetBtnsStatus(TRUE);
				Send10xCommandToMachine();
				CDialog::OnLButtonDown(nFlags, point);
				return ;
			}
		}
	}


	for ( int j = 0 ; j < m_40x_rect_info.m_Max_Column ; j ++)
	{
		for(int i = 0 ; i < m_40x_rect_info.m_Max_Lines ; i ++)
		{
			if(m_40x_rect_info.m_window_visio_info.m_VisioRect[m_40x_rect_info.m_Max_Lines*j+i].PtInRect(point))
			{
				if(m_40xIndex != -1)
				{
					m_40x_rect_info.m_window_visio_info.m_nSelected[m_40xIndex] = 0;	
				}

				m_40xIndex = m_40x_rect_info.m_Max_Lines*j+i;
				m_40x_rect_info.m_window_visio_info.m_nSelected[m_40x_rect_info.m_Max_Lines*j+i] = 1;	

				m_selected_40x_pos.nColumn = j;
				m_selected_40x_pos.nLines = i;

				DrawVisioRgn(IDC_40X_STATIC);
				SetBtnsStatus(TRUE);
				Send40xCommandToMachine();
				CDialog::OnLButtonDown(nFlags, point);
				return ;
			}
		}
	}
	
}

void CMoveVisioDlg::SetBtnsStatus(bool bIsEnable)
{
	m_more_right_btn.EnableWindow(bIsEnable);
	//m_right_btn.EnableWindow(bIsEnable);
	m_up_btn.EnableWindow(bIsEnable);
	m_down_btn.EnableWindow(bIsEnable);
	//m_left_btn.EnableWindow(bIsEnable);
	m_more_left_btn.EnableWindow(bIsEnable);
	
}

void CMoveVisioDlg::Send10xCommandToMachine()
{
	//unsigned char chPara[80] = {0};
	//chPara[0] = m_selected_10x_pos.nLines;
	//chPara[1] = m_selected_10x_pos.nColumn;
	//chPara[2] = m_nChannelNums;
	//chPara[3] = 0;

	//theApp.m_pDevice->SendCommand(CCMD_10LOCATION, chPara, 4);

	/*CLocate10MicroscopeInfo mic10;
	mic10.XPicNum = m_selected_10x_pos.nLines;
	mic10.YPicNum = m_selected_10x_pos.nColumn;
	mic10.ChannelNum= m_nChannelNums;
	mic10.IsReturnTakePicCommand =0;
	Locate10Microscope(mic10);*/

	CAPTURE_IMAGE_AREA_CONTEXT ctx = { 0 }, retCtx = { 0 };
	ctx.AreaX = m_selected_10x_pos.nLines;
	ctx.AreaY = m_selected_10x_pos.nColumn;
	ChangeM10LensArea(ctx, retCtx);
}

void CMoveVisioDlg::Send40xCommandToMachine()
{
	//unsigned char chPara[80] = {0};
	//chPara[0] = m_selected_10x_pos.nLines;
	//chPara[1] = m_selected_10x_pos.nColumn;
	//chPara[2] = m_40xIndex;
	//chPara[3] = 0;
	//chPara[4] = 0;
	//chPara[5] = m_nChannelNums;
	//chPara[6] = 0;
	//theApp.m_pDevice->SendCommand(CCMD_40LOCATION, chPara, 7);

	/*CLocate40MicroscopeInfo mic40;
	mic40.XPicNumFor10 = m_selected_10x_pos.nLines;
	mic40.YPicNumFor10 = m_selected_10x_pos.nColumn;
	mic40.PicNumFor40 = m_40xIndex;
	mic40.XOffset = 0;
	mic40.YOffset = 0;
	mic40.ChannelNum= m_nChannelNums;
	mic40.IsReturnTakePicCommand =0;
		Locate40Microscope(mic40);*/

	CAPTURE_IMAGE_AREA_CONTEXT ctx = { 0 }, retCtx = { 0 };
	ctx.AreaX = m_selected_40x_pos.nLines;
	ctx.AreaY = m_selected_40x_pos.nColumn;
	ChangeM40LensArea(ctx, retCtx);
}

void CMoveVisioDlg::OnBnClickedUpBtn()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	m_set_edit.GetWindowText(str);
	int m_Nums = _wtoi(str);
	/*unsigned char chPara[8] = {0};
	chPara[0] = 9;
	chPara[1] = 0;
	chPara[2] = m_Nums & 0x00ff;
	chPara[3] = (m_Nums>>8) & 0x00ff;*/

	//theApp.m_pDevice->SendCommand(CCMD_MICRO_MOVE, chPara, 4);

	m_YTotalStep -= m_Nums;

	MoveScanPlatform(YMotorID,Backward,m_Nums);
}

void CMoveVisioDlg::OnBnClickedDownBtn()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	m_set_edit.GetWindowText(str);
	int m_Nums = _wtoi(str);
	//unsigned char chPara[8] = {0};
	//chPara[0] = 9;
	//chPara[1] = 1;
	//chPara[2] = m_Nums & 0x00ff;
	//chPara[3] = (m_Nums>>8) & 0x00ff;
	//theApp.m_pDevice->SendCommand(CCMD_MICRO_MOVE, chPara, 4);
		DBG_MSG("m_YTotalStep:%d",m_YTotalStep);
	m_YTotalStep += m_Nums;
	MoveScanPlatform(YMotorID,Foreward,m_Nums);
}

void CMoveVisioDlg::OnBnClickedMoreRightBtn()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	m_set_edit.GetWindowText(str);
	int m_Nums = _wtoi(str);
	//unsigned char chPara[8] = {0};
	//chPara[0] = 5;
	//chPara[1] = 1;
	m_Nums = m_Nums*2;
	//chPara[2] = m_Nums & 0x00ff;
	//chPara[3] = (m_Nums>>8) & 0x00ff;
	//theApp.m_pDevice->SendCommand(CCMD_MICRO_MOVE, chPara, 4);
	m_XTotalStep += m_Nums;
	MoveScanPlatform(XMotorID,Foreward,m_Nums);
}

void CMoveVisioDlg::OnBnClickedMoreLeftBtn()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	m_set_edit.GetWindowText(str);
	int m_Nums = _wtoi(str);
	//unsigned char chPara[8] = {0};
	//chPara[0] = 5;
	//chPara[1] = 0;
	m_Nums = m_Nums*2;
	//chPara[2] = m_Nums & 0x00ff;
	//chPara[3] = (m_Nums>>8) & 0x00ff;

	//theApp.m_pDevice->SendCommand(CCMD_MICRO_MOVE, chPara, 4);
	m_XTotalStep -= m_Nums;
	MoveScanPlatform(XMotorID,Backward,m_Nums);
}


void CMoveVisioDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码

	CDialog::OnOK();
}

void CMoveVisioDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	OnCancel();
}

void CMoveVisioDlg::OnBnClickedSaveCameraPosBtn()
{
	// TODO: 在此添加控件通知处理程序代码

	WORD ZPos = 0;
	//CZPosType ZPosType  = AChannel10;
	CZPosType ZPosType  = m_ZPosType;
	QueryZPos(ZPosType, ZPos);
	CAutoFocusMgr::SaveCameraConfigInfo(ZPosType, m_XTotalStep, m_YTotalStep, ZPos);

	DBG_MSG("ZPosType :%d,m_XTotalStep: %d,m_YTotalStep: %d,ZPos: %d\n",ZPosType,m_XTotalStep,m_YTotalStep,ZPos);
}


void CMoveVisioDlg::OnBnClickedResetCameraButton()
{
	// TODO: 在此添加控件通知处理程序代码
	XAxisReset();
	YAxisReset();
	ZAxisReset();

	m_XTotalStep = 0;
	m_YTotalStep = 0;
}


bool CMoveVisioDlg::LocateCameraTo10X()
{
	int XStep = 0;
	int YStep = 0;
	int ZPos = 0;

	if( !CAutoFocusMgr::MoveToStartPos( (CChannelType)m_nChannelNums) )
		return false;

	if (m_nChannelNums == A_CHANNEL)
	{
		if( !CAutoFocusMgr::GetCameraConfigInfo(AChannel10, XStep, YStep, ZPos) )
			return true;

		if( !CAutoFocusMgr::MoveCameraByConfigInfo( AChannel10) )
			return false;		
	}
	else 
	{
		if( !CAutoFocusMgr::GetCameraConfigInfo(BChannel10, XStep, YStep, ZPos) )
			return true;

		if( !CAutoFocusMgr::MoveCameraByConfigInfo( BChannel10) )
			return false;		
	}

	m_XTotalStep = XStep;
	m_YTotalStep = YStep;
	DBG_MSG("m_XTotalStep:%d,m_YTotalStep:%d\n",m_XTotalStep,m_YTotalStep);
	return true;
}




void CMoveVisioDlg::OnBnClickedButton6()
{
	// TODO: 在此添加控件通知处理程序代码
	int ZPos = 0;
	m_XTotalStep = 0;
	m_YTotalStep = 0;
 	if (m_nChannelNums == A_CHANNEL)
 	{
 		CAutoFocusMgr::MoveCameraByConfigInfo( AChannel10);	
 	}
 	else 
 	{
 		CAutoFocusMgr::MoveCameraByConfigInfo( BChannel10);		
 	}
}

void CMoveVisioDlg::OnBnClickedButton7()
{
	int ZPos = 0;
	m_XTotalStep = 0;
	m_YTotalStep = 0;
	// TODO: 在此添加控件通知处理程序代码
	if (m_nChannelNums == A_CHANNEL)
	{
		CAutoFocusMgr::MoveCameraByConfigInfo( AChannel40);
		
	}
	else 
	{
		CAutoFocusMgr::MoveCameraByConfigInfo( BChannel40);		
	}
}

bool CMoveVisioDlg::LocateCameraTo40X()
{
	int XStep = 0;
	int YStep = 0;
	int ZPos = 0;
	
	if (m_nChannelNums == A_CHANNEL)
	{
			DBG_MSG("A 40X定位");
		if( !CAutoFocusMgr::GetCameraConfigInfo(AChannel40, XStep, YStep, ZPos) )
			return true;

		if( !CAutoFocusMgr::MoveCameraByConfigInfo( AChannel40,false) )
			return false;
	}
	else 
	{
		DBG_MSG("B 40X定位");
		if( !CAutoFocusMgr::GetCameraConfigInfo(BChannel40, XStep, YStep, ZPos) )
			return true;

		if( !CAutoFocusMgr::MoveCameraByConfigInfo( BChannel40,false) )
			return false;
	}

	m_XTotalStep = XStep;
	m_YTotalStep = YStep;
	DBG_MSG("m_XTotalStep:%d,m_YTotalStep:%d\n",m_XTotalStep,m_YTotalStep);
	return true;
}
void CMoveVisioDlg::OnBnClickedButton4()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData();

	//MessageBox(L"Z Move");

	MoveZAxis(m_nZPos);
}

void CMoveVisioDlg::OnBnClickedMoveZ()
{
	OnBnClickedButton4();
}

void CMoveVisioDlg::OnBnClickedBtnMoveto()
{
	// TODO: 在此添加控件通知处理程序代码
	int XStep = 0;
	int YStep = 0;
	int ZPos = 0;

	

	

	if( !CAutoFocusMgr::MoveTo40XStartPos( (CChannelType)m_nChannelNums) )
	{
		AfxMessageBox(L"移镜40倍镜初始位置错误!");
		return ;
	}		

	if (m_nChannelNums == A_CHANNEL)
	{
		CAutoFocusMgr::GetCameraConfigInfo(AChannel40, XStep, YStep, ZPos);
	}
	else 
	{		
		CAutoFocusMgr::GetCameraConfigInfo(BChannel40, XStep, YStep, ZPos);	
	}


	CDirection Direction = Foreward;

	if(XStep < 0)
	{
		XStep = -XStep;
		Direction = Backward;
	}

	if( ! MoveScanPlatform(XMotorID,Direction, XStep) ) 
	{
		MessageBoxW(NULL,_T("移动x轴失败"),0);
		return ; 
	}


	Direction = Foreward;

	if(YStep < 0)
	{
		YStep = -YStep;
		Direction = Backward;
	}
	
	if( !MoveScanPlatform(YMotorID,Direction, YStep) )
	{
		MessageBoxW(NULL,_T("移动y轴失败"),0);
		return ; 
	}
}

void CMoveVisioDlg::EnablePostBtns( BOOL bEnable )
{
	m_btn_post_10X.EnableWindow(bEnable);
	m_btn_post_40X.EnableWindow(!bEnable);
}

void CMoveVisioDlg::OnBnClickedBtnLedCtrl()
{
	if (MicrosopeLedSwitch(!m_bLedLightOn))
	{
		m_bLedLightOn = !m_bLedLightOn;
		if (m_bLedLightOn)
		{
			SetDlgItemText(IDC_BTN_LED_CTRL, _T("关灯"));
		}
		else
		{
			SetDlgItemText(IDC_BTN_LED_CTRL, _T("开灯"));
		}
	}
}


void CMoveVisioDlg::OnBnClickedBtnAdjustFocus()
{
	if (m_bInAdjustFocusMode)
	{
		m_bInAdjustFocusMode = FocusEndAdjust() ? !m_bInAdjustFocusMode : m_bInAdjustFocusMode;
	}
	else
	{
		m_bInAdjustFocusMode = FocusBeginAdjust() ? !m_bInAdjustFocusMode : m_bInAdjustFocusMode;
	}
	if (m_bInAdjustFocusMode)
	{
		SetDlgItemText(IDC_BTN_ADJUST_FOCUS, _T("结束调焦"));
		GetDlgItem(IDC_SAVE_Z_POS)->EnableWindow(TRUE);
	}
	else
	{
		SetDlgItemText(IDC_BTN_ADJUST_FOCUS, _T("开始调焦"));
		GetDlgItem(IDC_SAVE_Z_POS)->EnableWindow(FALSE);
	}
}

void CMoveVisioDlg::OnBnClickedSaveZPos()
{
	bool ret = FocusSave();
	MessageBox((ret ? _T("保存Z轴位置成功!") : _T("保存Z轴位置失败!")), _T("提示"), MB_OK | (ret ? MB_ICONINFORMATION : MB_ICONWARNING));
}

BOOL CMoveVisioDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

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

	return CDialog::OnMouseWheel(nFlags, zDelta, pt);
}

void CMoveVisioDlg::OnBnClicked10xButton()
{
	// TODO: 在此添加控件通知处理程序代码
	/*if( m_nChannelNums == A_CHANNEL )
	m_ZPosType = AChannel10;
	else if( m_nChannelNums == B_CHANNEL )
	m_ZPosType = BChannel10;*/

	m_nMicroLens = 10;

	if (!PerformCommonAction(LC_DBG_GOTO_10X_LENS_END))
	{
		AfxMessageBox(L"切换到10倍镜失败，请检查下位机是否正常", MB_OK | MB_ICONWARNING);
		return;
	}

	/*if( !LocateCameraTo10X() )
	{
	AfxMessageBox(L"移镜失败，请检查下位机是否正常", MB_OK | MB_ICONWARNING);
	return;
	}*/

	SetBtnsStatus(TRUE);
	m_btn_post_10X.EnableWindow(FALSE);
	m_btn_post_40X.EnableWindow(TRUE);
}

void CMoveVisioDlg::OnBnClicked40xButton()
{
	// TODO: 在此添加控件通知处理程序代码
	/*if( m_nChannelNums == A_CHANNEL )
	m_ZPosType = AChannel40;
	else if( m_nChannelNums == B_CHANNEL )
	m_ZPosType = BChannel40;*/

	/*ConvertTo40Microscope();

	if( !CAutoFocusMgr::MoveTo40XStartPos( (CChannelType)m_nChannelNums) )
	{
	DBG_MSG("移镜40倍镜初始位错误");
	//AfxMessageBox(L"移镜40倍镜错误!");
	return ;
	}

	//AfxMessageBox(L"移镜40倍镜到初市位置");

	if( !LocateCameraTo40X() )
	{
	AfxMessageBox(L"移镜失败，请检查下位机是否正常");
	return;
	}*/

	m_nMicroLens = 40; 

// 	if (!PerformCommonAction(LC_DBG_GOTO_40X_LENS_END))
// 	{
// 		AfxMessageBox(L"切换到40倍镜失败，请检查下位机是否正常", MB_OK | MB_ICONWARNING);
// 		return;
// 	}

	SetBtnsStatus(TRUE);
	m_btn_post_10X.EnableWindow(TRUE);
	m_btn_post_40X.EnableWindow(FALSE);
}

void CMoveVisioDlg::OnBnClickedRightBtn()
{
	// TODO: 在此添加控件通知处理程序代码

	CString str;
	GetDlgItem(IDC_EDIT_STEPCOUNT)->GetWindowText(str);
	int nStepCount = _ttoi(str);
	MOTOR_ID id = MOTOR_ID::XAXIS_MOTOR;
	if (((CButton*)GetDlgItem(IDC_RADIO_Y))->GetCheck())
	{
		id = MOTOR_ID::YAXIS_MOTOR;
	}
	else if (((CButton*)GetDlgItem(IDC_RADIO_Z))->GetCheck())
	{
		id = MOTOR_ID::ZAXIS_MOTOR;
	}

	MotorTest(id,
		MOTOR_ROTATION_DIRECTION::MOTOR_DIRECTION_REVERSE,
		(MOTOR_SPEED)(((CComboBox*)GetDlgItem(IDC_COMBO_SPEED))->GetCurSel()),
		nStepCount);
}
void CMoveVisioDlg::OnBnClickedLeftBtn()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	GetDlgItem(IDC_EDIT_STEPCOUNT)->GetWindowText(str);
	int nStepCount = _ttoi(str);
	MOTOR_ID id = MOTOR_ID::XAXIS_MOTOR;
	if (((CButton*)GetDlgItem(IDC_RADIO_Y))->GetCheck())
	{
		id = MOTOR_ID::YAXIS_MOTOR;
	}
	else if (((CButton*)GetDlgItem(IDC_RADIO_Z))->GetCheck())
	{
		id = MOTOR_ID::ZAXIS_MOTOR;
	}

	MotorTest(id,
		MOTOR_ROTATION_DIRECTION::MOTOR_DIRECTION_FORWARD, 
		(MOTOR_SPEED)(((CComboBox*)GetDlgItem(IDC_COMBO_SPEED))->GetCurSel()), 
		nStepCount);
}

void CMoveVisioDlg::OnBnClickedButtonAction1()
{
	PerformCommonAction(UC_DBG_MIROSCOPE_RESET_START);
}


void CMoveVisioDlg::OnBnClickedButtonAction2()
{
	PerformCommonAction(UC_DBG_X_AXIS_RESET_START);
}


void CMoveVisioDlg::OnBnClickedButtonAction3()
{
	PerformCommonAction(UC_DBG_Y_AXIS_AB_CHANNEL_RESET_START);
}


void CMoveVisioDlg::OnBnClickedButtonAction4()
{
	PerformCommonAction(UC_DBG_Y_AXIS_CD_CHANNEL_RESET_START);
}


void CMoveVisioDlg::OnBnClickedButtonAction5()
{
	PerformCommonAction(UC_DBG_Z_AXIS_RESET_START);
}


void CMoveVisioDlg::OnBnClickedButtonAction6()
{
	PerformCommonAction(UC_DBG_GOTO_10X_LENS_START);
}


void CMoveVisioDlg::OnBnClickedButtonAction7()
{
/*	PerformCommonAction(UC_DBG_GOTO_40X_LENS_START);*/
}


void CMoveVisioDlg::OnEnChangeEditStepcount()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CPropertyPage::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}


void CMoveVisioDlg::OnShowWindow(BOOL bShow, UINT nStatus)
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
