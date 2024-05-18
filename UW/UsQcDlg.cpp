// UsQcDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "UsQcDlg.h"
#include "afxdialogex.h"
#include "MainFrm.h"
#include "UW.h"
#include "include/common/String.h"
#include "DbgMsg.h"
#include "MainFrm.h"

#include "../.././../inc/OpenCV2_1/include/cxcore.h"
#include "../.././../inc/OpenCV2_1/include/highgui.h"
#include "../.././../inc/OpenCV2_1/include/cv.h"

#pragma comment(lib, "../.././../lib/OpenCV2_1/lib/highgui210.lib")
#pragma comment(lib, "../.././../lib/OpenCV2_1/lib/cxcore210.lib")
#pragma comment(lib, "../.././../lib/OpenCV2_1/lib/cv210.lib")

using namespace Common;

static bool compare_id(const TASK_INFO *t1, const TASK_INFO *t2){
    return t1->main_info.nID < t2->main_info.nID;
}

static double variance_of_laplacian(const IplImage* img)
{
	if (img == NULL)
	{
		return 0.0;
	}
	const int imgrows = img->height;
	const int imgcols = img->width;
	IplImage* gray = cvCreateImage(cvSize(imgcols, imgrows), IPL_DEPTH_8U, 1);
	if (img->nChannels == 3)
	{
		cvCvtColor(img, gray, CV_BGR2GRAY);
	}
	else if (img->nChannels == 1)
	{
		cvCopy(img, gray, NULL);
	}
	else
	{
		cvCvtColor(img, gray, CV_BGRA2GRAY);
	}

	IplImage* lap = cvCreateImage(cvSize(imgcols, imgrows), IPL_DEPTH_16S, 1);
	cvLaplace(gray, lap, 3);
	cvReleaseImage(&gray);

	CvScalar lap_mean, lap_stddev;
	cvAvgSdv(lap, &lap_mean, &lap_stddev, NULL);
	cvReleaseImage(&lap);

	return lap_stddev.val[0];
}

// UsQcDlg 对话框

IMPLEMENT_DYNAMIC(UsQcDlg, CDialogEx)

UsQcDlg::UsQcDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(UsQcDlg::IDD, pParent)
	, m_max(0)
	, m_min(0)
    , m_max2(0)
    , m_min2(0)
    , m_max3(0)
    , m_min3(0)
{
	m_Sn = 1;
    days = 1;
}

UsQcDlg::~UsQcDlg()
{
}

void UsQcDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list);
	DDX_Check(pDX, IDC_CHECK15, m_conf);
	DDX_Control(pDX, IDC_CHECK15, m_confCon);
	DDX_Control(pDX, IDC_TAB1, m_tab);
	DDX_Text(pDX, IDC_EDIT11, m_max);
	DDX_Text(pDX, IDC_EDIT12, m_min);
    DDX_Text(pDX, IDC_EDIT13, m_max2);
    DDX_Text(pDX, IDC_EDIT14, m_min2);
    DDX_Text(pDX, IDC_EDIT15, m_max3);
    DDX_Text(pDX, IDC_EDIT16, m_min3);
	DDX_Control(pDX, IDC_STATIC_PREVIEW, m_prelabel);
	DDX_Control(pDX, IDC_DATETIMEPICKER1, m_dtFrom);
	DDX_Control(pDX, IDC_DATETIMEPICKER2, m_dtTo);
    DDX_Control(pDX, IDC_RADIO_1, m_radio1);
    DDX_Control(pDX, IDC_RADIO_2, m_radio2);
    DDX_Control(pDX, IDC_RADIO_3, m_radio3);
}


BEGIN_MESSAGE_MAP(UsQcDlg, CDialogEx)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_CHECK15, &UsQcDlg::OnBnClickedCheck15)
	ON_BN_CLICKED(IDC_BUTTON1, &UsQcDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &UsQcDlg::OnBnClickedButton2)
    ON_BN_CLICKED(IDC_BUTTON_SEND, &UsQcDlg::OnBnClickedButtonSend)
	ON_BN_CLICKED(IDC_BUTTON_DEL, &UsQcDlg::OnBnClickedButtonDel)
	ON_BN_CLICKED(IDC_BUTTON_CONF_OK, &UsQcDlg::OnBnClickedButtonConfOk)
	ON_EN_KILLFOCUS(IDC_QC_LIST_EDIT, &UsQcDlg::OnEditKillFocus)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &UsQcDlg::OnNMDClick)
	ON_BN_CLICKED(IDC_BUTTON_CONF_OK, &UsQcDlg::OnBnClickedButtonConfOk)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &UsQcDlg::OnTcnSelchangeTab1)
	ON_WM_DESTROY()
	ON_NOTIFY(NM_CLICK, IDC_LIST1, &UsQcDlg::OnNMClickList1)
	ON_BN_CLICKED(IDC_BUTTON_CALC_CV, &UsQcDlg::OnBnClickedButtonCalcCv)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_SELPIC, &UsQcDlg::OnBnClickedButtonSelpic)
	ON_BN_CLICKED(IDC_BUTTON_RECCIRCLE, &UsQcDlg::OnBnClickedButtonReccircle)
	ON_BN_CLICKED(IDC_CHECK_QCTEST, &UsQcDlg::OnBnClickedCheckQctest)
	ON_BN_CLICKED(IDC_BUTTON_SET_ARG, &UsQcDlg::OnBnClickedButtonSetArg)
	ON_BN_CLICKED(IDC_BUTTON_RESET_ARG, &UsQcDlg::OnBnClickedButtonResetArg)
    ON_BN_CLICKED(IDC_RADIO_1, &UsQcDlg::OnBnClickedRadio)
    ON_BN_CLICKED(IDC_RADIO_2, &UsQcDlg::OnBnClickedRadio)
    ON_BN_CLICKED(IDC_RADIO_3, &UsQcDlg::OnBnClickedRadio)
    ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST1, &UsQcDlg::OnNMCustomdrawList)
END_MESSAGE_MAP()


// UsQcDlg 消息处理程序

void UsQcDlg::OnSize(UINT nType, int cx, int cy)
{
	ReControlSize();
	// TODO:  在此处添加消息处理程序代码
}

void UsQcDlg::ReControlSize()
{
	if (!m_list)
	{
		return;
	}
	CRect wndRect;

	this->GetClientRect(wndRect);
	wndRect.bottom /= 3;
	wndRect.right -= 10;
	m_list.MoveWindow(wndRect);

	CRect control(wndRect);
	control.top = wndRect.top + wndRect.Height();
	int x = 0;
	int y = 85;

    GetDlgItem(IDC_RADIO_1)->SetWindowPos(NULL, control.left + x + 20, control.top + 10, 60, 20, SWP_NOSIZE | SWP_NOACTIVATE);
    GetDlgItem(IDC_RADIO_2)->SetWindowPos(NULL, control.left + x + 100, control.top + 10, 60, 20, SWP_NOSIZE | SWP_NOACTIVATE);
    GetDlgItem(IDC_RADIO_3)->SetWindowPos(NULL, control.left + x + 180, control.top + 10, 60, 20, SWP_NOSIZE | SWP_NOACTIVATE);

    control.top += 20;

	GetDlgItem(IDC_DATETIMEPICKER1)->SetWindowPos(NULL, control.left + x, control.top + 10, 80, 20, SWP_NOSIZE | SWP_NOACTIVATE);
	GetDlgItem(IDC_STATIC)->SetWindowPos(NULL, control.left + x + 160, control.top + 12, 80, 20, SWP_NOSIZE | SWP_NOACTIVATE);
	GetDlgItem(IDC_DATETIMEPICKER2)->SetWindowPos(NULL, control.left + x + 180, control.top + 10, 80, 20, SWP_NOSIZE | SWP_NOACTIVATE);

	GetDlgItem(IDC_BUTTON1)->SetWindowPos(NULL, control.left + (x += y) + 260, control.top + 10, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
	GetDlgItem(IDC_BUTTON2)->SetWindowPos(NULL, control.left + (x += y) + 260, control.top + 10, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
    GetDlgItem(IDC_BUTTON_SEND)->SetWindowPos(NULL, control.left + (x += y) + 260, control.top + 10, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
	GetDlgItem(IDC_BUTTON_DEL)->SetWindowPos(NULL, control.left + (x += y) + 260, control.top + 10, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
	GetDlgItem(IDC_BUTTON_CALC_CV)->SetWindowPos(NULL, control.left + (x += y) + 260, control.top + 10, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
	GetDlgItem(IDC_EDIT_SHOW_CV_VALUE)->SetWindowPos(NULL, control.left + (x+2)+260, control.top + 25 + 10, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
	GetDlgItem(IDC_CHECK15)->SetWindowPos(NULL, control.left + (x += y) + 260, control.top + 10 + 3, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);

	y = 60;
	int offset = 30;
	x -= y;
	y = 60;

    GetDlgItem(IDC_BUTTON_CONF_OK)->SetWindowPos(NULL, control.left + x + 260 + 140, control.top + 10, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
    GetDlgItem(IDC_STATIC_GROUP1)->SetWindowPos(NULL, control.left + x + 260 + 50, control.top + offset + TOP_OFSET, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
	GetDlgItem(IDC_STATIC11)->SetWindowPos(NULL, control.left + x + 260 + 60, control.top + 20 + offset + TOP_OFSET, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
	GetDlgItem(IDC_EDIT11)->SetWindowPos(NULL, control.left + x + 260 + 110, control.top + 20 + offset, 0, 20, SWP_NOSIZE | SWP_NOACTIVATE);
	offset += 30;
	GetDlgItem(IDC_STATIC12)->SetWindowPos(NULL, control.left + x + 260 + 60, control.top + 20 + offset + TOP_OFSET, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
	GetDlgItem(IDC_EDIT12)->SetWindowPos(NULL, control.left + x + 260 + 110, control.top + 20 + offset, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);

    offset -= 30;
    GetDlgItem(IDC_STATIC_GROUP2)->SetWindowPos(NULL, control.left + x + 260 + 50 + 150, control.top + offset + TOP_OFSET, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
    GetDlgItem(IDC_STATIC15)->SetWindowPos(NULL, control.left + x + 260 + 60 + 150, control.top + 20 + offset + TOP_OFSET, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
    GetDlgItem(IDC_EDIT13)->SetWindowPos(NULL, control.left + x + 260 + 110 + 150, control.top + 20 + offset, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
    offset += 30;
    GetDlgItem(IDC_STATIC16)->SetWindowPos(NULL, control.left + x + 260 + 60 + 150, control.top + 20 + offset + TOP_OFSET, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
    GetDlgItem(IDC_EDIT14)->SetWindowPos(NULL, control.left + x + 260 + 110 + 150, control.top + 20 + offset, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);

    offset -= 30;
    GetDlgItem(IDC_STATIC_GROUP3)->SetWindowPos(NULL, control.left + x + 260 + 50 + 150 + 150, control.top + offset + TOP_OFSET, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
    GetDlgItem(IDC_STATIC17)->SetWindowPos(NULL, control.left + x + 260 + 60 + 150 + 150, control.top + 20 + offset + TOP_OFSET, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
    GetDlgItem(IDC_EDIT15)->SetWindowPos(NULL, control.left + x + 260 + 110 + 150 + 150, control.top + 20 + offset, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
    offset += 30;
    GetDlgItem(IDC_STATIC18)->SetWindowPos(NULL, control.left + x + 260 + 60 + 150 + 150, control.top + 20 + offset + TOP_OFSET, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
    GetDlgItem(IDC_EDIT16)->SetWindowPos(NULL, control.left + x + 260 + 110 + 150 + 150, control.top + 20 + offset, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);

	GetWindowRect(wndRect);
	m_tab.SetWindowPos(NULL, control.left, control.top + 40, x + 260 + 40, wndRect.Height() - control.bottom - 60, SWP_NOACTIVATE);
	m_tab.GetWindowRect(wndRect);
	m_paintView->MoveWindow(wndRect);

	GetDlgItem(IDC_EDIT_SHOW_CV_VALUE)->ShowWindow(SW_HIDE);

	
	GetDlgItem(IDC_CHECK_QCTEST)->SetWindowPos(NULL, wndRect.Width() + 20, wndRect.top+100, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
	GetDlgItem(IDC_GROUP_QT)->SetWindowPos(NULL, wndRect.Width() + 15, wndRect.top + 120, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
	GetDlgItem(IDC_EDIT_QCTESTPIC)->SetWindowPos(NULL, wndRect.Width() + 20, wndRect.top + 130, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
	GetDlgItem(IDC_BUTTON_SELPIC)->SetWindowPos(NULL, wndRect.Width() + 185, wndRect.top + 130, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
	GetDlgItem(IDC_BUTTON_RECCIRCLE)->SetWindowPos(NULL, wndRect.Width() + 185, wndRect.top + 160, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
	GetDlgItem(IDC_BUTTON_SET_ARG)->SetWindowPos(NULL, wndRect.Width() + 185, wndRect.top + 190, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
	GetDlgItem(IDC_BUTTON_RESET_ARG)->SetWindowPos(NULL, wndRect.Width() + 185, wndRect.top + 220, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
	GetDlgItem(IDC_STATIC_A1)->SetWindowPos(NULL, wndRect.Width() + 20, wndRect.top + 190, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
	GetDlgItem(IDC_STATIC_A2)->SetWindowPos(NULL, wndRect.Width() + 20, wndRect.top + 220, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
	GetDlgItem(IDC_STATIC_A3)->SetWindowPos(NULL, wndRect.Width() + 20, wndRect.top + 250, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
	GetDlgItem(IDC_STATIC_A4)->SetWindowPos(NULL, wndRect.Width() + 20, wndRect.top + 280, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
	GetDlgItem(IDC_STATIC_A5)->SetWindowPos(NULL, wndRect.Width() + 20, wndRect.top + 310, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
	GetDlgItem(IDC_STATIC_A6)->SetWindowPos(NULL, wndRect.Width() + 20, wndRect.top + 340, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
	GetDlgItem(IDC_EDIT_A1)->SetWindowPos(NULL, wndRect.Width() + 65, wndRect.top + 190, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE); 
	GetDlgItem(IDC_EDIT_A2)->SetWindowPos(NULL, wndRect.Width() + 65, wndRect.top + 220, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
	GetDlgItem(IDC_EDIT_A3)->SetWindowPos(NULL, wndRect.Width() + 65, wndRect.top + 250, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
	GetDlgItem(IDC_EDIT_A4)->SetWindowPos(NULL, wndRect.Width() + 65, wndRect.top + 280, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
	GetDlgItem(IDC_EDIT_A5)->SetWindowPos(NULL, wndRect.Width() + 65, wndRect.top + 310, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
	GetDlgItem(IDC_EDIT_A6)->SetWindowPos(NULL, wndRect.Width() + 65, wndRect.top + 340, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
}

BOOL UsQcDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	if (!m_Edit.Create(WS_CHILD | WS_VISIBLE
		, CRect(0, 0, 0, 0), this, IDC_QC_LIST_EDIT))
	{
		TRACE0("未能创建编辑框\n");
		return -1;      // 未能创建
	}
    static CFont editFont;
    editFont.CreateFont(16, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, _T("MS Shell Dlg"));
    m_Edit.SetFont(&editFont);
    m_list.SetFont(&editFont);

	m_paintView = new CPaintView;
	if (m_paintView)
	{
		CRect rect;
		m_tab.GetWindowRect(rect);
		m_paintView->Create(NULL, NULL, WS_CHILD | WS_VISIBLE, rect, &m_tab, AFX_IDW_PANE_LAST);
	}
	m_preView = new CPaintView;

	if (m_preView)
	{
		m_preView->Create(NULL, NULL, WS_CHILD | WS_VISIBLE, CRect(0, 0, 300, 200), &m_prelabel, NULL);
	}

	m_prelabel.ShowWindow(SW_HIDE);
	CImageList imgList;  //为ClistCtrl设置一个图像列表，以设置行高
	imgList.Create(IDB_BITMAP3, 1, 1, RGB(255, 255, 255)); // IDB_BITMAP4 是 2*16的 所以行高16像素

	m_list.SetImageList(&imgList, LVSIL_SMALL);
	m_list.SetExtendedStyle(m_list.GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_list.ModifyStyle(LVS_SHOWSELALWAYS, 0);

	int i = 0;
	m_list.InsertColumn(i++, L"序号", LVCFMT_CENTER, 60);
	m_list.InsertColumn(i++, L"状态", LVCFMT_CENTER, 60);
	m_list.InsertColumn(i++, L"质控类型", LVCFMT_CENTER, 120);

	int index = i;

	m_tab.InsertItem(index, Common::CharToWChar(theApp.us_cfg_info.par[CELL_QC_TYPE - 1].sLongName).c_str());
	m_list.InsertColumn(index++, Common::CharToWChar(theApp.us_cfg_info.par[CELL_QC_TYPE - 1].sLongName).c_str(), LVCFMT_CENTER,100);

	m_list.InsertColumn(index++, L"测试时间", LVCFMT_CENTER, 150);
	m_list.InsertColumn(index++, L"批号", LVCFMT_CENTER, 120);
	m_list.InsertColumn(index++, L"有效期", LVCFMT_CENTER, 120);

	m_confCon.SetCheck(false);
	ShowConfigControl(false);
	ReadRangeFormIni();
	UpdateStringToUI();


	char iniPath[MAX_PATH] = { 0 };
	GetModuleFileNameA(NULL, iniPath, MAX_PATH);
	PathRemoveFileSpecA(iniPath);
	PathAppendA(iniPath, "Config\\setting.ini");
	int nCircles4 = GetPrivateProfileIntA("RecCircle", "4", 1, iniPath);
	int nCircles5 = GetPrivateProfileIntA("RecCircle", "5", 20, iniPath);
	int nCircles6 = GetPrivateProfileIntA("RecCircle", "6", 10, iniPath);
	int nCircles7 = GetPrivateProfileIntA("RecCircle", "7", 60, iniPath);
	int nCircles8 = GetPrivateProfileIntA("RecCircle", "8", 15, iniPath);
	int nCircles9 = GetPrivateProfileIntA("RecCircle", "9", 40, iniPath);
	TCHAR tmp[128];
	_itot_s(nCircles4, tmp, 10);
	GetDlgItem(IDC_EDIT_A1)->SetWindowText(tmp);
	_itot_s(nCircles5, tmp, 10);
	GetDlgItem(IDC_EDIT_A2)->SetWindowText(tmp);
	_itot_s(nCircles6, tmp, 10);
	GetDlgItem(IDC_EDIT_A3)->SetWindowText(tmp);
	_itot_s(nCircles7, tmp, 10);
	GetDlgItem(IDC_EDIT_A4)->SetWindowText(tmp);
	_itot_s(nCircles8, tmp, 10);
	GetDlgItem(IDC_EDIT_A5)->SetWindowText(tmp);
	_itot_s(nCircles9, tmp, 10);
	GetDlgItem(IDC_EDIT_A6)->SetWindowText(tmp);

    m_radio1.SetCheck(1);
    //GetDlgItem(IDC_BUTTON2)->ShowWindow(SW_HIDE);

   
    m_radio1.SetWindowText(USI_QC);
    m_radio2.SetWindowText(USII_QC);
    m_radio3.SetWindowText(USIII_QC);
    SetDlgItemText(IDC_STATIC_GROUP1, USI_QC);
    SetDlgItemText(IDC_STATIC_GROUP2, USII_QC);
    SetDlgItemText(IDC_STATIC_GROUP3, USIII_QC);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void UsQcDlg::ShowConfigControl(bool isShow /*= true*/)
{
	int flag;
	if (isShow)
		flag = SW_SHOW;
	else
		flag = SW_HIDE;

    GetDlgItem(IDC_BUTTON_CONF_OK)->ShowWindow(flag);

    GetDlgItem(IDC_STATIC_GROUP1)->ShowWindow(flag);
	GetDlgItem(IDC_STATIC11)->ShowWindow(flag);
	GetDlgItem(IDC_EDIT11)->ShowWindow(flag);
	GetDlgItem(IDC_STATIC12)->ShowWindow(flag);
	GetDlgItem(IDC_EDIT12)->ShowWindow(flag);

    GetDlgItem(IDC_STATIC_GROUP2)->ShowWindow(flag);
    GetDlgItem(IDC_STATIC15)->ShowWindow(flag);
    GetDlgItem(IDC_EDIT13)->ShowWindow(flag);
    GetDlgItem(IDC_STATIC16)->ShowWindow(flag);
    GetDlgItem(IDC_EDIT14)->ShowWindow(flag);

    GetDlgItem(IDC_STATIC_GROUP3)->ShowWindow(flag);
    GetDlgItem(IDC_STATIC17)->ShowWindow(flag);
    GetDlgItem(IDC_EDIT15)->ShowWindow(flag);
    GetDlgItem(IDC_STATIC18)->ShowWindow(flag);
    GetDlgItem(IDC_EDIT16)->ShowWindow(flag);
}


void UsQcDlg::OnBnClickedCheck15()
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_confCon.GetCheck())
		ShowConfigControl();
	else
		ShowConfigControl(false);
}


void UsQcDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
		// TODO:  在此添加控件通知处理程序代码
	UpdateData();
	char lastTime[MAX_DATA_LENS] = { 0 };
    SEARCH_TASK tInfo1 = {}, tInfo2 = {}, tInfo3 = {};
	CString strValue;
	tInfo1.bIsSearch_QC_Task = true;
    tInfo1.conditions.nTestDes = UsQCBy1;

	COleDateTime dt;
	m_dtFrom.GetTime(dt);
	strcpy(tInfo1.conditions.sDate_From, WCharToChar(dt.Format(_T("%Y-%m-%d")).GetBuffer()).c_str());
	m_dtTo.GetTime(dt);
	strcpy(tInfo1.conditions.sDate_To, WCharToChar(dt.Format(_T("%Y-%m-%d")).GetBuffer()).c_str());

	days = calcTimeDiff(tInfo1.conditions.sDate_From, tInfo1.conditions.sDate_To) + 1;

	strcpy(tInfo1.conditions.sTime_From, "00:00:00");
	strcpy(tInfo1.conditions.sTime_To, "23:59:59");

    tInfo2 = tInfo1;
    tInfo2.conditions.nTestDes = UsQCBy2;
    tInfo3 = tInfo1;
    tInfo3.conditions.nTestDes = UsQCBy3;

    m_list.DeleteAllItems();

    KSTATUS rcode1 = Access_SearchRecord(&tInfo1);
    KSTATUS rcode2 = Access_SearchRecord(&tInfo2);
    KSTATUS rcode3 = Access_SearchRecord(&tInfo3);

    if (STATUS_SUCCESS != rcode1 || STATUS_SUCCESS != rcode2 || STATUS_SUCCESS != rcode3)
    {
        Access_FreeSearchRecordBuf(tInfo1);
        Access_FreeSearchRecordBuf(tInfo2);
        Access_FreeSearchRecordBuf(tInfo3);
        SendMessgeToPaint(m_tab.GetCurSel());
        return;
    }

    std::list<TASK_INFO*> tasks;
    for (int i = 0; i < tInfo1.pSearchInfoCounts; i++)
    {
        tasks.push_back(tInfo1.pSearchInfo + i);
    }
    for (int i = 0; i < tInfo2.pSearchInfoCounts; i++)
    {
        tasks.push_back(tInfo2.pSearchInfo + i);
    }
    for (int i = 0; i < tInfo3.pSearchInfoCounts; i++)
    {
        tasks.push_back(tInfo3.pSearchInfo + i);
    }
    tasks.sort(compare_id);

	int index = 0;
    for (auto itr = tasks.begin(); itr != tasks.end(); itr++)
    {
        TASK_INFO *task = *itr;
        if (task->main_info.nAttribute != UsQCBy1 && task->main_info.nAttribute != UsQCBy2 && task->main_info.nAttribute != UsQCBy3)
		{
			continue;
		}	
        StatusEnum   nUSStatus, nUDCStatus;
        theApp.ParseStatusMaskCode(task->main_info.nState, nUSStatus, nUDCStatus);
        CString stateTxt;
        switch (nUSStatus)
        {
        case NEW:     //新建
            stateTxt = _T("新建");
            break;
        case TEST:	   //检测
        case PRO:     //识别
            stateTxt = _T("测试中");
            break;
        case TESTFAIL:     //测试失败
        case FAIL:
            stateTxt = _T("测试失败");
            break;
        case FINISH:    //完成
        case CHECKED:   //已审核	
            stateTxt = _T("完成");
            break;
        default:
            break;
        }
		m_list.InsertItem(index, L"");
        strValue.Format(L"%d", task->main_info.nSN);
		m_list.SetItemText(index, 0, strValue);
        m_list.SetItemData(index, task->main_info.nID);
        m_list.SetItemText(index, 1, stateTxt);
        switch (task->main_info.nAttribute)
		{
		case UsQCBy1:strValue.Format(_T("%s"), USI_QC); break;
		case UsQCBy2:strValue.Format(_T("%s"), USII_QC); break;
        case UsQCBy3:strValue.Format(_T("%s"), USIII_QC); break;
		default:
			strValue.Format(_T("%s"), L"Normal");
			break;
		}
		m_list.SetItemText(index, 2, strValue);

        strValue.Format(task->us_info.us_node[CELL_QC_TYPE - 1].us_res>100.f ? L"%.1f" : L"%.2f", task->us_info.us_node[CELL_QC_TYPE - 1].us_res);
        if (strValue.Right(2) == _T(".0")) strValue.Delete(strValue.GetLength() - 2, 2);
        else if (strValue.Right(3) == _T(".00")) strValue.Delete(strValue.GetLength() - 3, 3);
		m_list.SetItemText(index, 3, strValue);

        strValue.Format(L"%s", CharToWChar(task->main_info.dtDate).c_str());
		m_list.SetItemText(index, 4 , strValue);
        strValue.Format(L"%s", CharToWChar(task->main_info.sDiagnosis).c_str());
		m_list.SetItemText(index, 5, strValue);
        strValue.Format(L"%s", CharToWChar(task->main_info.sSuggestion).c_str());
		m_list.SetItemText(index, 6, strValue);
		index++;
	}

	Access_FreeSearchRecordBuf(tInfo1);
    Access_FreeSearchRecordBuf(tInfo2);
    Access_FreeSearchRecordBuf(tInfo3);
	SendMessgeToPaint(m_tab.GetCurSel());
}

void UsQcDlg::OnEditKillFocus()
{
	m_Edit.ShowWindow(SW_HIDE);//隐藏编辑框

	CString str;
	m_Edit.GetWindowText(str);//取得编辑框的内容
    if (str.IsEmpty())
    {
        return;
    }
	CString strOldText = m_list.GetItemText(m_row, m_column);

	//将该内容更新到CListCtrl中
	m_list.SetItemText(m_row, m_column, str);
	if (str == strOldText)
	{
		return;
	}
	//更新数据到数据库
	TASK_INFO task_info = { 0 };
	ULONG taskId = m_list.GetItemData(m_row);

	if (Access_GetTaskInfo(taskId, USER_TYPE_INFO, &task_info) != STATUS_SUCCESS)
		return;
	if (0 == m_column)//序号
	{
		task_info.main_info.nSN = atoi(WstringToString(str.GetBuffer()).c_str());
		if (Access_UpdateTaskInfo(USER_TYPE_INFO, taskId, &task_info) != STATUS_SUCCESS)
			return;
	}
	else if (m_column == 3)
	{
		task_info.us_info.us_node[CELL_QC_TYPE - 1].us_res = atof(WCharToChar(str.GetBuffer()).c_str());
		if (Access_UpdateTaskInfo(US_TYPE_INFO, taskId, &task_info) != STATUS_SUCCESS)
			return;
	}
	else if (m_column == 4)
	{
		strcpy(task_info.main_info.dtDate, WstringToString(str.GetBuffer()).c_str());
		if (Access_UpdateTaskInfo(USER_TYPE_INFO, taskId, &task_info) != STATUS_SUCCESS)
			return;
	}
	else if (m_column == 5)
	{
		strcpy(task_info.main_info.sDiagnosis, WstringToString(str.GetBuffer()).c_str());
		if (Access_UpdateTaskInfo(USER_TYPE_INFO, taskId, &task_info) != STATUS_SUCCESS)
			return;
	}
	else if (m_column == 6)
	{
		strcpy(task_info.main_info.sSuggestion, WstringToString(str.GetBuffer()).c_str());
		if (Access_UpdateTaskInfo(USER_TYPE_INFO, taskId, &task_info) != STATUS_SUCCESS)
			return;
	}
	else
	{
		return;
	}
	SendMessgeToPaint(m_tab.GetCurSel());
}

void UsQcDlg::DeleteRecord(int nItem)
{
	ULONG       nTaskID;
	DEL_TASK    del_task;
	CMainFrame *pMainFrame;

	if (nItem == -1)
		return;

	nTaskID = m_list.GetItemData(nItem);


	del_task.bIsQC = FALSE; //不考虑是否为质控
	del_task.pDelIDArray = &nTaskID;
	del_task.nCounter = 1;
	if (Access_DelTask(&del_task) != STATUS_SUCCESS)
		return;

	m_list.DeleteItem(nItem);
}


void UsQcDlg::OnNMDClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	CRect rc;
	if (pNMListView->iItem != -1)
	{
		m_row = pNMListView->iItem;//m_row为被选中行的行序号（int类型成员变量）
		if (strncmp(Common::WCharToChar(m_list.GetItemText(m_row, 1).GetBuffer()).c_str(), "完成", 2) != 0)
		{
			return;
		}
		CMainFrame * pFrame = (CMainFrame *)theApp.GetMainWnd();
		pFrame->qcDlg->ShowWindow(SW_MINIMIZE);
		int nID = m_list.GetItemData(m_row);

		
		pFrame->ActiveTab(THUM_VIEW_TAB);

		//显示细胞列表
		THUIInterface_ShowTaskIcon(nID);
	}
}

void UsQcDlg::DoSubItemEdit(int item, int subitem)
{
	CRect rc;
	m_list.GetSubItemRect(item, subitem, LVIR_BOUNDS, rc);//取得子项的矩形
    if (subitem == 0)
    {
        CRect firstColumnRect;
        m_list.GetHeaderCtrl()->GetItemRect(0, &firstColumnRect);
        rc.right = rc.left + firstColumnRect.Width();
    }
    rc.DeflateRect(4, 1, -1, 0);
 

	CString str;
	str = m_list.GetItemText(item, subitem);//取得子项的内容
    m_Edit.SetParent(&m_list);
	m_Edit.SetWindowText(str);//将子项的内容显示到编辑框中
	m_Edit.ShowWindow(SW_SHOW);//显示编辑框
	m_Edit.MoveWindow(&rc);//将编辑框移动到子项上面，覆盖在子项上
	m_Edit.SetFocus();//使编辑框取得焦点
	m_Edit.CreateSolidCaret(1, rc.Height() - 2);//创建一个光标
	m_Edit.ShowCaret();//显示光标
	m_Edit.SetSel(-1);//使光标移到最后面
}

void UsQcDlg::SaveQCImage()
{
	char qcIni[MAX_PATH] = { 0 };
	GetModuleFileNameA(NULL, qcIni, MAX_PATH);
	PathRemoveFileSpecA(qcIni);
	sprintf(qcIni, "%s\\QcImage\\pic.bmp", qcIni);
	m_paintView->SaveBMP(qcIni);
}

void UsQcDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	// TODO: 在此添加控件通知处理程序代码
	SaveQCImage();
	CChoiceQcPri dlg;
	dlg.DoModal();
	m_priType = dlg.m_priType;
	m_prelabel.ShowWindow(SW_SHOW);
	CRect wndRect(0, 0, 5 * 108, 16 * 31);
	m_prelabel.MoveWindow(wndRect);
	m_preView->MoveWindow(wndRect);
	SendMessgeToPreview(m_tab.GetCurSel());
	m_preView->UpdateWindow();
	SaveListImage();
	m_prelabel.ShowWindow(SW_HIDE);

	char pPath[MAX_PATH] = { 0 };
	GetUitPath(pPath);
	if (TRUE != UWReport_PrintView(m_list.GetItemData(0), pPath));
	return;
}

void UsQcDlg::OnBnClickedButtonSend()
{
    int nCount = m_list.GetSelectedCount();
    if (nCount == 0)
    {
        AfxMessageBox(_T("请先选择需要发送的记录!"), MB_OK|MB_ICONINFORMATION);
        return;
    }
    POSITION pos = m_list.GetFirstSelectedItemPosition();
    for (int j = 0; j < nCount; j++)
    {
        int nItem = m_list.GetNextSelectedItem(pos);
        ULONG taskId = m_list.GetItemData(nItem);
        if (taskId > 0)
        {
            SendToLisQueue((CMainFrame*)AfxGetMainWnd(), taskId);
        }
    }
}

void UsQcDlg::GetUitPath(char* qcIni)
{
	GetModuleFileNameA(NULL, qcIni, MAX_PATH);
	PathRemoveFileSpecA(qcIni);
	sprintf(qcIni, "%s\\QcImage\\qc.uit", qcIni);
}

void UsQcDlg::SaveListImage()
{
	char qcIni[MAX_PATH] = { 0 };
	GetModuleFileNameA(NULL, qcIni, MAX_PATH);
	PathRemoveFileSpecA(qcIni);
	sprintf(qcIni, "%s\\QcImage\\list.bmp", qcIni);
	m_preView->SaveBMP(qcIni);
}

void UsQcDlg::SendMessgeToPreview(int iColumn)
{
    std::shared_ptr<PaintView> sPaint = std::make_shared<PaintView>();
    *(sPaint.get()) = {};
	int i1 = 0, i2 = 0, i3 = 0;
	CString value1, value2, value3, valueDate, valueValidDate, valueBatch;
	sPaint->x_days = days;
	
	sPaint->priType = m_priType;
	for (int iRow = 0; iRow < m_list.GetItemCount(); iRow++)
	{
		if (m_list.GetItemText(iRow, 2) == USI_QC)
		{
            if (!m_radio1.GetCheck())
            {
                continue;
            }
			value1.Append(m_list.GetItemText(iRow,3));
			value1.Append(L";");

			if (m_priType == 4)
			{
				valueDate.Append(m_list.GetItemText(iRow, 4 ));
				valueDate.Append(L";");

				valueBatch.Append(m_list.GetItemText(iRow, 5));
				valueBatch.Append(L";");

				valueValidDate.Append(m_list.GetItemText(iRow, 6));
				valueValidDate.Append(L";");
			}
			i1++;
		}
		else if (m_list.GetItemText(iRow, 2) == USII_QC)
		{
            if (!m_radio2.GetCheck())
            {
                continue;
            }
			value2.Append(m_list.GetItemText(iRow, 3));
			value2.Append(L";");
			if (m_priType == 5)
			{
				valueDate.Append(m_list.GetItemText(iRow,  4));
				valueDate.Append(L";");

				valueBatch.Append(m_list.GetItemText(iRow,5));
				valueBatch.Append(L";");

				valueValidDate.Append(m_list.GetItemText(iRow,6));
				valueValidDate.Append(L";");
			}

			i2++;
		}
        else if (m_list.GetItemText(iRow, 2) == USIII_QC)
        {
            if (!m_radio3.GetCheck())
            {
                continue;
            }
            value3.Append(m_list.GetItemText(iRow, 3));
            value3.Append(L";");
            if (m_priType == 5)
            {
                valueDate.Append(m_list.GetItemText(iRow, 4));
                valueDate.Append(L";");

                valueBatch.Append(m_list.GetItemText(iRow, 5));
                valueBatch.Append(L";");

                valueValidDate.Append(m_list.GetItemText(iRow, 6));
                valueValidDate.Append(L";");
            }

            i3++;
        }
		else
		{
			MessageBox(L"未知的沉渣质控类型", L"提示", MB_OK);
		}
	}
	if (m_list.GetItemText(0, 2) == USI_QC || m_list.GetItemText(0, 2) == USII_QC)
	{
		sPaint->itmeIndex = 40;
	}
	else
	{
		sPaint->itmeIndex = iColumn;
	}
	
	sPaint->value1 = WCharToChar(value1.GetBuffer());
	sPaint->value2 = WCharToChar(value2.GetBuffer());
	sPaint->value3 = WCharToChar(value3.GetBuffer());
	sPaint->valueBatch = WCharToChar(valueBatch.GetBuffer());
	sPaint->valueDate = WCharToChar(valueDate.GetBuffer());
	sPaint->valueValidDate = WCharToChar(valueValidDate.GetBuffer());
    if ((i1 && (i1 != days)) || (i2 && (i2 != days)) || (i3 && (i3 != days)))
	{
		if (IDNO == MessageBox(L"所选日期与已有质控记录不匹配,是否继续？", L"提示", MB_YESNO))
			return;
	}
	if (!(i1 || i2 || i3))
	{
		return;
	}
	m_preView->SendMessage(WM_PAINT_VIEW_US, (WPARAM)2, (LPARAM)sPaint.get());
}


void UsQcDlg::OnBnClickedButtonDel()
{
	// TODO: 在此添加控件通知处理程序代码
		// TODO:  在此添加控件通知处理程序代码
	int        nCounter, j;
	PLONG      pSelItem;
	POSITION   pos;

	nCounter = m_list.GetSelectedCount();

	if (nCounter == 0)
		return;

	pSelItem = new LONG[nCounter];
	if (pSelItem == NULL)
		return;

	pos = m_list.GetFirstSelectedItemPosition();

	for (j = 0; j < nCounter; j++)
		pSelItem[j] = m_list.GetNextSelectedItem(pos);

	m_list.SetRedraw(FALSE);

	for (j = nCounter - 1; j >= 0; j--)
		DeleteRecord(pSelItem[j]);

	m_list.SetRedraw(TRUE);

	delete[] pSelItem;
}


void UsQcDlg::OnBnClickedButtonConfOk()
{
	// TODO: 在此添加控件通知处理程序代码
		// TODO:  在此添加控件通知处理程序代码
	UpdateUIToString();
	WriteRangeToIni();
	SendMessgeToPaint(m_tab.GetCurSel());
}

void UsQcDlg::ReadRangeFormIni()
{
	char ConfigFile[MAX_PATH] = { 0 };
	int temp = 0;
	GetModuleFileNameA(NULL, ConfigFile, MAX_PATH);
	PathRemoveFileSpecA(ConfigFile);
	PathAppendA(ConfigFile, "Config\\qc.ini");	
	m_max = GetPrivateProfileIntA("QC", "max",100, ConfigFile);
	m_min = GetPrivateProfileIntA("QC", "min",0, ConfigFile);
    m_max2 = GetPrivateProfileIntA("QC", "max2", 100, ConfigFile);
    m_min2 = GetPrivateProfileIntA("QC", "min2", 0, ConfigFile);
    m_max3 = GetPrivateProfileIntA("QC", "max3", 100, ConfigFile);
    m_min3 = GetPrivateProfileIntA("QC", "min3", 0, ConfigFile);
}

void UsQcDlg::WriteRangeToIni()
{
	char ConfigFile[MAX_PATH] = { 0 }, temp[MAX_PATH] = { 0 };
	GetModuleFileNameA(NULL, ConfigFile, MAX_PATH);
	PathRemoveFileSpecA(ConfigFile);
	PathAppendA(ConfigFile, "Config\\qc.ini");

	WritePrivateProfileStringA("QC", "max", itoa(m_max,temp,10), ConfigFile);
	WritePrivateProfileStringA("QC", "min", itoa(m_min, temp, 10), ConfigFile);
    WritePrivateProfileStringA("QC", "max2", itoa(m_max2, temp, 10), ConfigFile);
    WritePrivateProfileStringA("QC", "min2", itoa(m_min2, temp, 10), ConfigFile);
    WritePrivateProfileStringA("QC", "max3", itoa(m_max3, temp, 10), ConfigFile);
    WritePrivateProfileStringA("QC", "min3", itoa(m_min3, temp, 10), ConfigFile);
}
void UsQcDlg::UpdateStringToUI()
{
	UpdateData(FALSE);
}

void UsQcDlg::UpdateUIToString()
{
	UpdateData();
}

void UsQcDlg::SendMessgeToPaint(int iColumn)
{
    COleDateTime dt1, dt2;
    m_dtFrom.GetTime(dt1);
    m_dtTo.GetTime(dt2);
    days = calcTimeDiff(WCharToChar(dt1.Format(_T("%Y-%m-%d")).GetBuffer()).c_str(), WCharToChar(dt2.Format(_T("%Y-%m-%d")).GetBuffer()).c_str()) + 1;

    std::shared_ptr<PaintView> sPaint = std::make_shared<PaintView>();
    *(sPaint.get()) = {};
    CString value1, value2, value3;
	sPaint->x_days = days;
	sPaint->max = m_max;
	sPaint->min = m_min;
    if (m_radio2.GetCheck())
    {
        sPaint->max = m_max2;
        sPaint->min = m_min2;
    }
    else if (m_radio3.GetCheck())
    {
        sPaint->max = m_max3;
        sPaint->min = m_min3;
    }
    std::map<CString, CString> mapDateRange1 = GetCurDateRange();
    std::map<CString, CString> mapDateRange2 = mapDateRange1, mapDateRange3 = mapDateRange1;
	for (int iRow = 0; iRow < m_list.GetItemCount(); iRow++)
	{
		CString dt = m_list.GetItemText(iRow, 4).GetLength()>=10?m_list.GetItemText(iRow, 4).Left(10):CString();
        CString strType = m_list.GetItemText(iRow, 2);
        CString strVal  = m_list.GetItemText(iRow, 3);
        std::map<CString, CString>::iterator item1, item2, item3;
        if (strType == USI_QC)
		{
            item1 = mapDateRange1.find(dt);
            if (item1 == mapDateRange1.end() || item1->second.IsEmpty())
            {
                mapDateRange1[dt] = strVal;
            }
		}
        else if (strType == USII_QC)
		{
            item2 = mapDateRange2.find(dt);
            if (item2 == mapDateRange2.end() || item2->second.IsEmpty())
            {
                mapDateRange2[dt] = strVal;
            }
		}
        else if (strType == USIII_QC)
        {
            item3 = mapDateRange3.find(dt);
            if (item3 == mapDateRange3.end() || item3->second.IsEmpty())
            {
                mapDateRange3[dt] = strVal;
            }
        }
		else
		{
			MessageBox(L"未知的沉渣质控类型", L"提示", MB_OK);
		}
		m_list.GetItemText(iRow, iColumn);
	}
    if (m_radio1.GetCheck())
    {
        mapDateRange2.clear();
        mapDateRange3.clear();
    }
    else if (m_radio2.GetCheck())
    {
        mapDateRange1.clear();
        mapDateRange3.clear();
    }
    else if (m_radio3.GetCheck())
    {
        mapDateRange1.clear();
        mapDateRange2.clear();
    }
    for (std::map<CString, CString>::const_iterator itr = mapDateRange1.cbegin(); itr != mapDateRange1.cend(); itr++)
    {
        value1.Append(itr->second);
        value1.Append(L";");
    }
    for (std::map<CString, CString>::const_iterator itr = mapDateRange2.cbegin(); itr != mapDateRange2.cend(); itr++)
    {
        value2.Append(itr->second);
        value2.Append(L";");
    }
    for (std::map<CString, CString>::const_iterator itr = mapDateRange3.cbegin(); itr != mapDateRange3.cend(); itr++)
    {
        value3.Append(itr->second);
        value3.Append(L";");
    }
	sPaint->value1 = WCharToChar(value1.GetBuffer());
	sPaint->value2 = WCharToChar(value2.GetBuffer());
    sPaint->value3 = WCharToChar(value3.GetBuffer());
    COleDateTime dtFrom;
    m_dtFrom.GetTime(dtFrom);
    sPaint->dtStart = dtFrom;
    /*if ((i1 && (i1 != days)) || (i2 && (i2 != days)) || (i3 && (i3 != days)))
	{
		if (IDNO == MessageBox(L"所选日期与已有质控记录不匹配,是否继续？", L"提示", MB_YESNO))
			return;
	}
	if (!(i1 || i2 || i3))
	{
		return;
	}*/
	m_paintView->SendMessage(WM_PAINT_VIEW_US, (WPARAM)1, (LPARAM)sPaint.get());
}

void UsQcDlg::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO:  在此添加控件通知处理程序代码
	if (!isValid)
	{
		return;
	}

	switch (m_tab.GetCurSel())
	{
	case 0:SendMessgeToPaint(m_tab.GetCurSel()); break;
	default:
		break;
	}
	*pResult = 0;
}


int UsQcDlg::calcTimeDiff(const char* from, const char* to)
{
	int nYear, nMonth, nDate;
	sscanf(from, "%d-%d-%d", &nYear, &nMonth, &nDate);
	CTime t1(nYear, nMonth, nDate, 0, 0, 0);

	sscanf(to, "%d-%d-%d", &nYear, &nMonth, &nDate);
	CTime t2(nYear, nMonth, nDate, 0, 0, 0);

	CTimeSpan ts = t2 - t1;
	ATLASSERT((t1 + ts) == t2);
	ATLASSERT((t2 - ts) == t1);
	return ts.GetDays();
}

void UsQcDlg::OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO:  在此添加控件通知处理程序代码
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	CRect rc;
	if (pNMListView->iItem != -1)
	{
		m_row = pNMListView->iItem;//m_row为被选中行的行序号（int类型成员变量）
		m_column = pNMListView->iSubItem;//m_column为被选中行的列序号（int类型成员变量）
		if (m_column != 0 && m_column < 3)
		{
			return;
		}
		DoSubItemEdit(m_row, m_column);
	}
	*pResult = 0;
}


void UsQcDlg::OnBnClickedButtonCalcCv()
{
	// TODO:  在此添加控件通知处理程序代码
	if (m_list.GetItemCount() < 2 || theApp.IsIdleStatus() == FALSE)
		return;
	CalcCvByQC();
}

void UsQcDlg::CalcCvByQC()
{
	int index = 0;
	int total[256] = { 0 };
	CString tValue;

	for (int i = 0; i < m_list.GetItemCount(); i++)
	{
		tValue = m_list.GetItemText(i, 3);
		total[index] = _ttoi(tValue);
		index++;
	}
	CString tmp;
	tmp.Format(L"%.6g%%", CalcCvByBlock(total, index) * 100);
	GetDlgItem(IDC_EDIT_SHOW_CV_VALUE)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_EDIT_SHOW_CV_VALUE)->SetWindowText(tmp);
}

double UsQcDlg::CalcCvByBlock(int *num, int index)
{
	int total = 0;
	double powValue = 0;
	double sqrtValue = 0;
	for (int i = 0; i < index; i++)
	{
		DBG_MSG("debug:计算cv值：num[%d] = %d\n", i, num[i]);
		total += num[i];
	}
	double avg = total / (double)index;
	if (avg <= 0)
		return 0;
	for (int i = 0; i < index; i++)
	{
		double k = double(num[i] - avg);
		powValue += pow(double(num[i] - avg), 2);
	}
	if (index < 1)
	{
		return (sqrt((double)((powValue) / index))) / avg;
	}
	return sqrtValue = (sqrt((double)((powValue) / (index - 1)))) / avg;

}

static CString m_qcTestPicPath;
void UsQcDlg::OnBnClickedButtonSelpic()
{
	CString strFile = _T("所有文件(*.*)|*.*||");
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, strFile);
	dlg.m_ofn.lpstrTitle = _T("原文件");
	if (dlg.DoModal() == IDOK)
	{
		m_qcTestPicPath = dlg.GetPathName();
		GetDlgItem(IDC_EDIT_QCTESTPIC)->SetWindowText(m_qcTestPicPath);
	}
}

void UsQcDlg::OnBnClickedButtonReccircle()
{
	char iniPath[MAX_PATH] = { 0 };
	GetModuleFileNameA(NULL, iniPath, MAX_PATH);
	PathRemoveFileSpecA(iniPath);
	PathAppendA(iniPath, "Config\\setting.ini");
	int nCircles4 = GetPrivateProfileIntA("RecCircle", "4", 1, iniPath);
	int nCircles5 = GetPrivateProfileIntA("RecCircle", "5", 20, iniPath);
	int nCircles6 = GetPrivateProfileIntA("RecCircle", "6", 10, iniPath);
	int nCircles7 = GetPrivateProfileIntA("RecCircle", "7", 60, iniPath);
	int nCircles8 = GetPrivateProfileIntA("RecCircle", "8", 15, iniPath);
	int nCircles9 = GetPrivateProfileIntA("RecCircle", "9", 40, iniPath);

	IplImage *srcImg = NULL, *grayImg = NULL, *grayImg2 = NULL;
	if (!PathFileExists(m_qcTestPicPath))
	{
		AfxMessageBox(_T("图片路径不存在"));
		return;
	}
	string path = WstringToString(m_qcTestPicPath.GetBuffer());
	srcImg = cvLoadImage(path.c_str());
	grayImg = cvCreateImage(cvGetSize(srcImg), IPL_DEPTH_8U, 1);
	cvCvtColor(srcImg, grayImg, CV_RGB2GRAY);
	cv::Mat blur;
	cv::medianBlur(cv::Mat(grayImg), blur, 3);
	//cv::GaussianBlur(blur, blur, cv::Size(9,9), 2, 2);
	grayImg2 = &IplImage(blur);
	/*	cvSmooth(srcImg, srcImg, CV_GAUSSIAN, 7, srcImg->nChannels);*/
	CvMemStorage* storage = cvCreateMemStorage(0);
	CvSeq* circles = 0;
	//第四个参数，double类型的dp，用来检测圆心的累加器图像的分辨率于输入图像之比的倒数，且此参数允许创建一个比输入图像分辨率低的累加器。上述文字不好理解的话，来看例子吧。例如，如果dp= 1时，累加器和输入图像具有相同的分辨率。如果dp=2，累加器便有输入图像一半那么大的宽度和高度。
	//第五个参数，double类型的minDist，为霍夫变换检测到的圆的圆心之间的最小距离，即让我们的算法能明显区分的两个不同圆之间的最小距离。这个参数如果太小的话，多个相邻的圆可能被错误地检测成了一个重合的圆。反之，这个参数设置太大的话，某些圆就不能被检测出来了。
	//第六个参数，double类型的param1，有默认值100。它是第三个参数method设置的检测方法的对应的参数。对当前唯一的方法霍夫梯度法CV_HOUGH_GRADIENT，它表示传递给canny边缘检测算子的高阈值，而低阈值为高阈值的一半。
	//第七个参数，double类型的param2，也有默认值100。它是第三个参数method设置的检测方法的对应的参数。对当前唯一的方法霍夫梯度法CV_HOUGH_GRADIENT，它表示在检测阶段圆心的累加器阈值。它越小的话，就可以检测到更多根本不存在的圆，而它越大的话，能通过检测的圆就更加接近完美的圆形了。
	//第八个参数，int类型的minRadius,有默认值0，表示圆半径的最小值。
	//第九个参数，int类型的maxRadius,也有默认值0，表示圆半径的最大值。

	circles = cvHoughCircles(grayImg2, storage, CV_HOUGH_GRADIENT,
		nCircles4,
		nCircles5, //很重要的一个参数，告诉两个圆之间的距离的最小距离，如果已知一副图像，可以先行计
		//算出符合自己需要的两个圆之间的最小距离。
		nCircles6, //canny算法的阈值上限，下限为一半（即100以上为边缘点，50以下抛弃，中间视是否相连而
		//定）
		nCircles7, //决定成圆的多寡 ，一个圆上的像素超过这个阈值，则成圆，否则丢弃
		nCircles8,//最小圆半径，这个可以通过图片确定你需要的圆的区间范围
		nCircles9 //最大圆半径
		);
	float* p = NULL;
	cv::Mat src = cv::Mat(srcImg).clone();
    int count = 0;
	for (size_t i = 0; i < circles->total; i++)
	{
		p = (float*)cvGetSeqElem(circles, i);
		if (p != NULL)
		{
			int tL = ((LONG)p[0] - (LONG)p[2]);
			int tT = ((LONG)p[1] - (LONG)p[2]);
			int tR = ((LONG)p[0] + (LONG)p[2]);
			int tB = ((LONG)p[1] + (LONG)p[2]);
			if (tL < 0) tL = 0;
			if (tT < 0) tT = 0;
			if (tR > src.cols) tR = src.cols;
			if (tB > src.rows) tB = src.rows;
			double val = 100.f;
			try
			{
				cv::Mat roi = src(cv::Rect(tL, tT, tR - tL, tB - tT));
				val = variance_of_laplacian(&IplImage(roi));
			} 
			catch (...)
			{
			}
			if (val < 10.f)
			{
				DBG_MSG("[%d][%d, %d, %d, %d] %0.3f\n", i, tL, tT, tR, tB, val);
				continue;
			}

			CvPoint pt = cvPoint(cvRound(p[0]), cvRound(p[1]));
			cvCircle(srcImg, pt, cvRound(p[2]*1.5), CV_RGB(255, 0, 0), 3);
            count++;
		}
	}
    DBG_MSG("debug:质控图片路径：%s,识别数目为%d", path.c_str(), count);

	int with = GetSystemMetrics(SM_CXFULLSCREEN);
	int heigh = GetSystemMetrics(SM_CYFULLSCREEN);
	CvSize dec_size;
	dec_size.height = heigh;
	dec_size.width = with;
	IplImage* dec = cvCreateImage(dec_size, srcImg->depth, srcImg->nChannels);
	cvResize(srcImg, dec);

	CvFont font;
	cvInitFont(&font, CV_FONT_HERSHEY_COMPLEX, 1, 1);
	char buf[128] = { 0 };
	sprintf_s(buf, "Rec result count: %d", circles->total);
	cvPutText(dec, buf, cvPoint(10, 35), &font, CV_RGB(255, 0, 0));
	cvShowImage("识别结果", dec);
    cvMoveWindow("识别结果", 0, 0);
	cv::waitKey(0);
	
	cvReleaseMemStorage(&storage);
	cvReleaseImage(&srcImg);
	cvReleaseImage(&grayImg);
	cvReleaseImage(&dec);
}

void UsQcDlg::OnBnClickedCheckQctest()
{
	int cmd = ((CButton*)GetDlgItem(IDC_CHECK_QCTEST))->GetCheck()?SW_SHOW:SW_HIDE;
	GetDlgItem(IDC_EDIT_QCTESTPIC)->ShowWindow(cmd);
	GetDlgItem(IDC_BUTTON_SELPIC)->ShowWindow(cmd);
	GetDlgItem(IDC_BUTTON_RECCIRCLE)->ShowWindow(cmd);
	GetDlgItem(IDC_BUTTON_SET_ARG)->ShowWindow(cmd);
	GetDlgItem(IDC_BUTTON_RESET_ARG)->ShowWindow(cmd);
	GetDlgItem(IDC_GROUP_QT)->ShowWindow(cmd);
	GetDlgItem(IDC_STATIC_A1)->ShowWindow(cmd);
	GetDlgItem(IDC_STATIC_A2)->ShowWindow(cmd);
	GetDlgItem(IDC_STATIC_A3)->ShowWindow(cmd);
	GetDlgItem(IDC_STATIC_A4)->ShowWindow(cmd);
	GetDlgItem(IDC_STATIC_A5)->ShowWindow(cmd);
	GetDlgItem(IDC_STATIC_A6)->ShowWindow(cmd);
	GetDlgItem(IDC_EDIT_A1)->ShowWindow(cmd);
	GetDlgItem(IDC_EDIT_A2)->ShowWindow(cmd);
	GetDlgItem(IDC_EDIT_A3)->ShowWindow(cmd);
	GetDlgItem(IDC_EDIT_A4)->ShowWindow(cmd);
	GetDlgItem(IDC_EDIT_A5)->ShowWindow(cmd);
	GetDlgItem(IDC_EDIT_A6)->ShowWindow(cmd);
}

void UsQcDlg::OnBnClickedButtonSetArg()
{
	char iniPath[MAX_PATH] = { 0 };
	GetModuleFileNameA(NULL, iniPath, MAX_PATH);
	PathRemoveFileSpecA(iniPath);
	PathAppendA(iniPath, "Config\\setting.ini");
	CString strA1, strA2, strA3, strA4, strA5, strA6;
	GetDlgItem(IDC_EDIT_A1)->GetWindowText(strA1);
	GetDlgItem(IDC_EDIT_A2)->GetWindowText(strA2);
	GetDlgItem(IDC_EDIT_A3)->GetWindowText(strA3);
	GetDlgItem(IDC_EDIT_A4)->GetWindowText(strA4);
	GetDlgItem(IDC_EDIT_A5)->GetWindowText(strA5);
	GetDlgItem(IDC_EDIT_A6)->GetWindowText(strA6);
	if (strA1.IsEmpty() || strA2.IsEmpty() || strA3.IsEmpty() || strA4.IsEmpty() || strA5.IsEmpty() || strA6.IsEmpty())
	{
		AfxMessageBox(_T("参数不能为空"));
		return;
	}
	WritePrivateProfileString(_T("RecCircle"), _T("4"), strA1, StringToWstring(iniPath).c_str());
	WritePrivateProfileString(_T("RecCircle"), _T("5"), strA2, StringToWstring(iniPath).c_str());
	WritePrivateProfileString(_T("RecCircle"), _T("6"), strA3, StringToWstring(iniPath).c_str());
	WritePrivateProfileString(_T("RecCircle"), _T("7"), strA4, StringToWstring(iniPath).c_str());
	WritePrivateProfileString(_T("RecCircle"), _T("8"), strA5, StringToWstring(iniPath).c_str());
	WritePrivateProfileString(_T("RecCircle"), _T("9"), strA6, StringToWstring(iniPath).c_str());
}

void UsQcDlg::OnBnClickedButtonResetArg()
{
	char iniPath[MAX_PATH] = { 0 };
	GetModuleFileNameA(NULL, iniPath, MAX_PATH);
	PathRemoveFileSpecA(iniPath);
	PathAppendA(iniPath, "Config\\setting.ini");
	WritePrivateProfileString(_T("RecCircle"), _T("4"), _T("1"), StringToWstring(iniPath).c_str());
	WritePrivateProfileString(_T("RecCircle"), _T("5"), _T("20"), StringToWstring(iniPath).c_str());
	WritePrivateProfileString(_T("RecCircle"), _T("6"), _T("10"), StringToWstring(iniPath).c_str());
	WritePrivateProfileString(_T("RecCircle"), _T("7"), _T("60"), StringToWstring(iniPath).c_str());
	WritePrivateProfileString(_T("RecCircle"), _T("8"), _T("15"), StringToWstring(iniPath).c_str());
	WritePrivateProfileString(_T("RecCircle"), _T("9"), _T("40"), StringToWstring(iniPath).c_str());
	GetDlgItem(IDC_EDIT_A1)->SetWindowText(_T("1"));
	GetDlgItem(IDC_EDIT_A2)->SetWindowText(_T("20"));
	GetDlgItem(IDC_EDIT_A3)->SetWindowText(_T("10"));
	GetDlgItem(IDC_EDIT_A4)->SetWindowText(_T("60"));
	GetDlgItem(IDC_EDIT_A5)->SetWindowText(_T("15"));
	GetDlgItem(IDC_EDIT_A6)->SetWindowText(_T("40"));
}


BOOL UsQcDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO:  在此添加专用代码和/或调用基类
    if (WM_KEYDOWN == pMsg->message)
    {
        if (pMsg->wParam == VK_ESCAPE)
        {
            return TRUE;
        }
        else if (pMsg->wParam == VK_RETURN)
        {
            this->SetFocus();
            return TRUE;
        }
    }
    else if (WM_SYSKEYDOWN == pMsg->message)
    {
        if (VK_F4 == pMsg->wParam)
        {
            return TRUE;
        }
    }

    return CDialogEx::PreTranslateMessage(pMsg);
}


void UsQcDlg::OnBnClickedRadio()
{
    // TODO:  在此添加控件通知处理程序代码
    //OnBnClickedButton1();
    SendMessgeToPaint(m_tab.GetCurSel());
}

std::map<CString, CString> UsQcDlg::GetCurDateRange() const
{
    std::map<CString, CString> ret;
    COleDateTime dtFrom, dtTo, dtTemp;
    m_dtFrom.GetTime(dtFrom);
    m_dtTo.GetTime(dtTo);

    if (dtFrom > dtTo)
    {
        dtTemp = dtFrom;
        dtFrom = dtTo;
        dtTo = dtTemp;
    }

    for (COleDateTime dt = dtFrom; dt <= dtTo; dt += COleDateTimeSpan(1, 0, 0, 0))
    {
        ret[dt.Format(_T("%Y-%m-%d"))] = _T("");
    }

    return ret;
}


void UsQcDlg::OnNMCustomdrawList(NMHDR *pNMHDR, LRESULT *pResult)
{
    NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>(pNMHDR);
    // TODO:  在此添加控件通知处理程序代码
    *pResult = CDRF_DODEFAULT;

    if (CDDS_PREPAINT == pLVCD->nmcd.dwDrawStage)
    {
        *pResult = CDRF_NOTIFYITEMDRAW;
    }
    else if (CDDS_ITEMPREPAINT == pLVCD->nmcd.dwDrawStage)
    {
        *pResult = CDRF_NOTIFYSUBITEMDRAW;
    }
    else if ((CDDS_ITEMPREPAINT | CDDS_SUBITEM) == pLVCD->nmcd.dwDrawStage)
    {

        COLORREF clrNewTextColor, clrNewBkColor;

        int nItem = static_cast<int>(pLVCD->nmcd.dwItemSpec);

        POSITION pos = m_list.GetFirstSelectedItemPosition();
        int index = m_list.GetNextSelectedItem(pos);

        if (index == nItem)//如果要刷新的项为当前选择的项，则将文字设为白色，背景色设为蓝色
        {
            clrNewTextColor = RGB(255, 255, 255);        //Set the text to white
            clrNewBkColor = RGB(51, 153, 255);        //Set the background color to blue
        }
        else
        {
            clrNewTextColor = RGB(0, 0, 0);        //set the text black
            clrNewBkColor = RGB(255, 255, 255);    //leave the background color white
        }

        pLVCD->clrText = clrNewTextColor;
        pLVCD->clrTextBk = clrNewBkColor;

        *pResult = CDRF_DODEFAULT;
    }
}
