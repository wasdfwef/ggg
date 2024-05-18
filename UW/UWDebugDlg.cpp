// UWDebugDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "UW.h"
#include "UWDebugDlg.h"
#include "Include/Common/String.h"
#include "DbgMsg.h"
#include "MainFrm.h"

// CUWDebugDlg 对话框
extern 
std::wstring 
GetIdsString(const std::wstring &sMain,const std::wstring &sId);

IMPLEMENT_DYNAMIC(CUWDebugDlg, CPropertyPage)

CUWDebugDlg::CUWDebugDlg()
	: CPropertyPage(CUWDebugDlg::IDD)
	,m_nLight(0)
	, m_nMicLight(0)
    , m_nUdcLight(0)
{

}

CUWDebugDlg::~CUWDebugDlg()
{
}

void CUWDebugDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTN_PUSHIN, m_btnPushIn);
	DDX_Control(pDX, IDC_BTN_PUSHSTEP, m_btnPushStep);
	DDX_Control(pDX, IDC_BTN_PUSHSTEP_RESET, m_btnPushStep_Reset);
	DDX_Control(pDX, IDC_BTN_PUSHOUT, m_btnPushOut);
	DDX_Control(pDX, IDC_BTN_BARCODE, m_btnBarcode);
	DDX_Control(pDX, IDC_BTN_NEEDLE_DOWN, m_btnNeedleDown);
	DDX_Control(pDX, IDC_BTN_NEEDLE_UP, m_btnNeedleUp);
	DDX_Control(pDX, IDC_BTN_NEEDLE_RESET, m_btnNeedleReset);
	DDX_Control(pDX, IDC_BTN_NEEDLE_SAMPLE, m_btnNeedleSample);
	DDX_Control(pDX, IDC_BTN_NEEDLE_CLEAN, m_btnNeedleClean);
	DDX_Control(pDX, IDC_BTN_NEEDLE_QC, m_btnNeedleQc);
	DDX_Control(pDX, IDC_BTN_ROLLER_RESET, m_btnRollerReset);
	DDX_Control(pDX, IDC_BTN_ROLLER_DEBUG, m_btnRollerDebug);
	DDX_Control(pDX, IDC_BTN_SELECT_1PAPER, m_btnSelect1Paper);
	DDX_Control(pDX, IDC_BTN_SELECT_100PAPER, m_btnSelect100Paper);
	DDX_Control(pDX, IDC_BTN_TURN_FORE, m_btnTurnFore);
	DDX_Control(pDX, IDC_BTN_TURN_BACK, m_btnTurnBack);
	DDX_Control(pDX, IDC_BTN_PAPERFEED, m_btnPaperFeed);
	DDX_Control(pDX, IDC_BTN_X_RESET, m_btn_X_Reset);
	DDX_Control(pDX, IDC_BTN_Y_RESET, m_btn_Y_Reset);
	DDX_Control(pDX, IDC_BTN_Z_RESET, m_btn_Z_Reset);
	DDX_Control(pDX, IDC_BTN_Z_POS, m_btn_Z_Pos);
	DDX_Control(pDX, IDC_BTN_GOTO10LENS, m_btnGoTo10Lens);
	DDX_Control(pDX, IDC_BTN_GOTO40LENS, m_btnGoTo40Lens);
	DDX_Control(pDX, IDC_BTN_MIC_LIGHT_DARK, m_btnMicLight);
	DDX_Control(pDX, IDC_BTN_DROPPUMP_TEST, m_btnDropPump);
	DDX_Control(pDX, IDC_BTN_CLEANPUMP_TEST, m_btnCleanPump);
	DDX_Control(pDX, IDC_BTN_UDC_TEST, m_btnUdcTest);
	DDX_Control(pDX, IDC_BTN_PAPER_TEST, m_btnPaperTest);
	DDX_Control(pDX, IDC_BTN_ROLLER_LIGHT_DARK, m_btnRollerLight);
	DDX_Control(pDX, IDC_BTN_UDC_LIGHT, m_btnUdcLight);
	DDX_Control(pDX, IDC_INFO_STATIC, m_strInfo);
	DDX_Control(pDX, IDC_CBO_PAPER, m_cboPaper);
	DDX_Control(pDX, IDC_BTN_SETPAPERTYPE, m_btnSetPaperType);	
	DDX_Control(pDX, IDC_BTN_GREY_PAPER_TEST, m_btnGreyPaperTest);
}


BEGIN_MESSAGE_MAP(CUWDebugDlg, CPropertyPage)
	ON_BN_CLICKED(IDC_BTN_PUSHIN, &CUWDebugDlg::OnBnClickedBtnPushin)
	ON_BN_CLICKED(IDC_BTN_PUSHSTEP, &CUWDebugDlg::OnBnClickedBtnPushstep)
	ON_BN_CLICKED(IDC_BTN_PUSHSTEP_RESET, &CUWDebugDlg::OnBnClickedBtnPushstepReset)
	ON_BN_CLICKED(IDC_BTN_PUSHOUT, &CUWDebugDlg::OnBnClickedBtnPushout)
	ON_BN_CLICKED(IDC_BTN_BARCODE, &CUWDebugDlg::OnBnClickedBtnBarcode)
	ON_BN_CLICKED(IDC_BTN_NEEDLE_DOWN, &CUWDebugDlg::OnBnClickedBtnNeedleDown)
	ON_BN_CLICKED(IDC_BTN_NEEDLE_UP, &CUWDebugDlg::OnBnClickedBtnNeedleUp)
	ON_BN_CLICKED(IDC_BTN_NEEDLE_RESET, &CUWDebugDlg::OnBnClickedBtnNeedleReset)
	ON_BN_CLICKED(IDC_BTN_NEEDLE_SAMPLE, &CUWDebugDlg::OnBnClickedBtnNeedleSample)
	ON_BN_CLICKED(IDC_BTN_NEEDLE_CLEAN, &CUWDebugDlg::OnBnClickedBtnNeedleClean)
	ON_BN_CLICKED(IDC_BTN_NEEDLE_QC, &CUWDebugDlg::OnBnClickedBtnNeedleQc)
	ON_BN_CLICKED(IDC_BTN_ROLLER_RESET, &CUWDebugDlg::OnBnClickedBtnRollerReset)
	ON_BN_CLICKED(IDC_BTN_ROLLER_DEBUG, &CUWDebugDlg::OnBnClickedBtnRollerDebug)
	ON_BN_CLICKED(IDC_BTN_SELECT_1PAPER, &CUWDebugDlg::OnBnClickedBtnSelect1paper)
	ON_BN_CLICKED(IDC_BTN_SELECT_100PAPER, &CUWDebugDlg::OnBnClickedBtnSelect100paper)
	ON_BN_CLICKED(IDC_BTN_TURN_FORE, &CUWDebugDlg::OnBnClickedBtnTurnFore)
	ON_BN_CLICKED(IDC_BTN_TURN_BACK, &CUWDebugDlg::OnBnClickedBtnTurnBack)
	ON_BN_CLICKED(IDC_BTN_PAPERFEED, &CUWDebugDlg::OnBnClickedBtnPaperfeed)
	ON_BN_CLICKED(IDC_BTN_X_RESET, &CUWDebugDlg::OnBnClickedBtnXReset)
	ON_BN_CLICKED(IDC_BTN_Y_RESET, &CUWDebugDlg::OnBnClickedBtnYReset)
	ON_BN_CLICKED(IDC_BTN_Z_RESET, &CUWDebugDlg::OnBnClickedBtnZReset)
	ON_BN_CLICKED(IDC_BTN_Z_POS, &CUWDebugDlg::OnBnClickedBtnZPos)
	ON_BN_CLICKED(IDC_BTN_GOTO10LENS, &CUWDebugDlg::OnBnClickedBtnGoto10lens)
	ON_BN_CLICKED(IDC_BTN_GOTO40LENS, &CUWDebugDlg::OnBnClickedBtnGoto40lens)
	ON_BN_CLICKED(IDC_BTN_MIC_LIGHT_DARK, &CUWDebugDlg::OnBnClickedBtnMicLightDark)
	ON_BN_CLICKED(IDC_BTN_DROPPUMP_TEST, &CUWDebugDlg::OnBnClickedBtnDroppumpTest)
	ON_BN_CLICKED(IDC_BTN_CLEANPUMP_TEST, &CUWDebugDlg::OnBnClickedBtnCleanpumpTest)
	ON_BN_CLICKED(IDC_BTN_UDC_TEST, &CUWDebugDlg::OnBnClickedBtnUdcTest)
	ON_BN_CLICKED(IDC_BTN_PAPER_TEST, &CUWDebugDlg::OnBnClickedBtnPaperTest)
	ON_BN_CLICKED(IDC_BTN_ROLLER_LIGHT_DARK, &CUWDebugDlg::OnBnClickedBtnRollerLightDark)
	ON_BN_CLICKED(IDC_BTN_UDC_LIGHT, &CUWDebugDlg::OnBnClickedBtnUdcLight)
	ON_BN_CLICKED(IDC_BTN_SETPAPERTYPE, &CUWDebugDlg::OnBnClickedBtnSetpapertype)
	ON_BN_CLICKED(IDC_BTN_GREY_PAPER_TEST, &CUWDebugDlg::OnBnClickedBtnGreyPaperTest)
	ON_BN_CLICKED(IDC_BUTTON_SOLENOID1, &CUWDebugDlg::OnBnClickedButtonSolenoid1)
	ON_BN_CLICKED(IDC_BUTTON_SOLENOID2, &CUWDebugDlg::OnBnClickedButtonSolenoid2)
	ON_BN_CLICKED(IDC_BUTTON_SOLENOID3, &CUWDebugDlg::OnBnClickedButtonSolenoid3)
	ON_BN_CLICKED(IDC_BUTTON_SOLENOID4, &CUWDebugDlg::OnBnClickedButtonSolenoid4)
	ON_BN_CLICKED(IDC_BUTTON_SOLENOID5, &CUWDebugDlg::OnBnClickedButtonSolenoid5)
	ON_BN_CLICKED(IDC_BUTTON_SOLENOID6, &CUWDebugDlg::OnBnClickedButtonSolenoid6)
	ON_BN_CLICKED(IDC_BUTTON_SOLENOID7, &CUWDebugDlg::OnBnClickedButtonSolenoid7)
	ON_BN_CLICKED(IDC_BUTTON_SOLENOID8, &CUWDebugDlg::OnBnClickedButtonSolenoid8)
END_MESSAGE_MAP()


// 推进

void CUWDebugDlg::OnBnClickedBtnPushin()
{
	// TODO: 在此添加控件通知处理程序代码
	//TestHardware(Pipeline,PipeIn);
	PipeBeginAutoRun();
}

//单步
void CUWDebugDlg::OnBnClickedBtnPushstep()
{
	// TODO: 在此添加控件通知处理程序代码
	TestHardware(Pipeline,SingleStep);

}

//单步复位
void CUWDebugDlg::OnBnClickedBtnPushstepReset()
{
	// TODO: 在此添加控件通知处理程序代码
	TestHardware(Pipeline,SingleStepReset);
}

//推出
void CUWDebugDlg::OnBnClickedBtnPushout()
{
	// TODO: 在此添加控件通知处理程序代码
	TestHardware(Pipeline,PipeOut);

}

//读条码
void CUWDebugDlg::OnBnClickedBtnBarcode()
{
	// TODO: 在此添加控件通知处理程序代码
	TestHardware(Pipeline,ScanBarcode);
}




//升降向下

void CUWDebugDlg::OnBnClickedBtnNeedleDown()
{
	// TODO: 在此添加控件通知处理程序代码
	TestHardware(Channel,LiftDown);
}

//升降复位
void CUWDebugDlg::OnBnClickedBtnNeedleUp()
{
	// TODO: 在此添加控件通知处理程序代码
	TestHardware(Channel,LiftReset);
}

//移位复位
void CUWDebugDlg::OnBnClickedBtnNeedleReset()
{
	// TODO: 在此添加控件通知处理程序代码
	TestHardware(Channel,MoveReset);
}


//到吸样位
void CUWDebugDlg::OnBnClickedBtnNeedleSample()
{
	// TODO: 在此添加控件通知处理程序代码
	TestHardware(Channel,SamplePos);

}

//到清洗位
void CUWDebugDlg::OnBnClickedBtnNeedleClean()
{
	// TODO: 在此添加控件通知处理程序代码
	TestHardware(Channel,CleanPos);
}

//到质控位
void CUWDebugDlg::OnBnClickedBtnNeedleQc()
{
	// TODO: 在此添加控件通知处理程序代码
	TestHardware(Channel,QCPos);
}

//点样泵测试
void CUWDebugDlg::OnBnClickedBtnDroppumpTest()
{
	TestHardware(Channel,SamplePumpTest);
}


// 清洗泵测试
void CUWDebugDlg::OnBnClickedBtnCleanpumpTest()
{
	TestHardware(Channel,CleanPumpTest);
}




//选纸复位
void CUWDebugDlg::OnBnClickedBtnRollerReset()
{
	// TODO: 在此添加控件通知处理程序代码
	TestHardware(Roller,SelectPaperReset);
}


//选纸调试
void CUWDebugDlg::OnBnClickedBtnRollerDebug()
{
	// TODO: 在此添加控件通知处理程序代码
	TestHardware(Roller,SelectPaperDebug);
}

//单个选纸
void CUWDebugDlg::OnBnClickedBtnSelect1paper()
{
	// TODO: 在此添加控件通知处理程序代码
	TestHardware(Roller,SingleSelectPaper);
}

//连续选纸
void CUWDebugDlg::OnBnClickedBtnSelect100paper()
{
	// TODO: 在此添加控件通知处理程序代码
	TestHardware(Roller,ContinueSelectPaper);
}

//正向翻转
void CUWDebugDlg::OnBnClickedBtnTurnFore()
{
	// TODO: 在此添加控件通知处理程序代码
	TestHardware(Roller,ForwardTurn);
}


//反向翻转
void CUWDebugDlg::OnBnClickedBtnTurnBack()
{
	// TODO: 在此添加控件通知处理程序代码
	TestHardware(Roller,BackTurn);
}

//送纸
void CUWDebugDlg::OnBnClickedBtnPaperfeed()
{
	// TODO: 在此添加控件通知处理程序代码
	TestHardware(Roller,SendPaper);
}


//UDC灯
void CUWDebugDlg::OnBnClickedBtnRollerLightDark()
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_nLight == 0)
	{
		TestHardware(Pipeline,RollerTurnOnLight);
		m_nLight = 1;
	}
	if (m_nLight == 1)
	{
		TestHardware(Pipeline,RollerTurnOffLight);
		m_nLight = 0;
	}
	
}






//X轴复位
void CUWDebugDlg::OnBnClickedBtnXReset()
{
	// TODO: 在此添加控件通知处理程序代码
	XAxisReset();
}


//Y轴复位
void CUWDebugDlg::OnBnClickedBtnYReset()
{
	// TODO: 在此添加控件通知处理程序代码
	YAxisReset();
}

//Z轴复位
void CUWDebugDlg::OnBnClickedBtnZReset()
{
	// TODO: 在此添加控件通知处理程序代码
	ZAxisReset();
}


//聚焦步数
void CUWDebugDlg::OnBnClickedBtnZPos()
{
	// TODO: 在此添加控件通知处理程序代码
	CZPosType postype;
	WORD wcount;
	wchar_t Msg[MAX_PATH] = {0};

	if( !QueryZPos(postype,wcount) )
	{
		swprintf(Msg, sizeof(wchar_t)*sizeof(Msg), _T("%S"), GetIdsString(_T("warning"),_T("getzposfail")).c_str());
	}
	else
	{
		switch (postype)
		{
		case AChannel40:
			swprintf(Msg, sizeof(wchar_t)*sizeof(Msg), L"%s%d",L"A通道40倍镜: ", wcount);
			break;
		case AChannel10:
			swprintf(Msg, sizeof(wchar_t)*sizeof(Msg), L"%s%d", L"A通道10倍镜: ", wcount);
			break;
		case BChannel40:
			swprintf(Msg, sizeof(wchar_t)*sizeof(Msg), L"%s%d", L"B通道40倍镜: ", wcount);
			break;
		case BChannel10:
			swprintf(Msg, sizeof(wchar_t)*sizeof(Msg), L"%s%d", L"B通道10倍镜: ", wcount);
			break;
		default:
			break;
		}
	}

	m_strInfo.SetWindowText(Msg);
}


//10倍换镜
void CUWDebugDlg::OnBnClickedBtnGoto10lens()
{
	// TODO: 在此添加控件通知处理程序代码
	ConvertTo10Microscope();
}


//40倍换镜
void CUWDebugDlg::OnBnClickedBtnGoto40lens()
{
	// TODO: 在此添加控件通知处理程序代码
	ConvertTo40Microscope();
}


//显微镜灯
void CUWDebugDlg::OnBnClickedBtnMicLightDark()
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_nMicLight == 0)
	{
		TurnOnLight();
		m_nMicLight = 1;
	}
	else
	{
		TurnOffLight();
		m_nMicLight = 0;
	}
	
}


BOOL CUWDebugDlg::GetTransUDCReslut( int nItemIndex, int nMirror, CStringA &strReslut )
{
	for( USHORT j = 0 ; j < UDC_MAXGRADE ; j ++ )
	{
		if( nMirror >= theApp.m_UdcitemInfo[nItemIndex].m_nGradeThreshold[j] )
		{
			strReslut.AppendFormat("%s    反射量:%d      含量: %s %s\r\n", 
				Common::WCharToChar(theApp.m_UdcitemInfo[nItemIndex].m_ctsItemName).c_str(), 
				nMirror,
				Common::WCharToChar(theApp.m_UdcitemInfo[nItemIndex].m_ctsGradeValue[j]).c_str(),
				Common::WCharToChar(theApp.m_UdcitemInfo[nItemIndex].m_ctsGradeUnit[j]).c_str()
				) ;

			return TRUE;
		}
	}
	return FALSE;
}


#define TEST_PAPER_RESULT_ITEM_COUNT  12			//只记录12项
struct CTempTestPaperResult
{
	WORD ItemResult[TEST_PAPER_RESULT_ITEM_COUNT];
};


void CUWDebugDlg::GetUDCLogData(const CUDCTestPaperResult &paperResut, CStringA &LogData)
{
	CStringA Data;
	int nIndex = 0;

	SYSTEMTIME CurrentTime = {0};
	GetLocalTime(&CurrentTime);

	CTempTestPaperResult *TempTestPaperResult = (CTempTestPaperResult *)&paperResut;

	DBG_MSG("检测尿干化结果 VC: %d,葡萄糖GLU:%d,胆红素BIL:%d,酮体KET:%d,比重SG:%d,隐血BLD:%d,酸碱度PH:%d,蛋白质PRO:%d,尿胆原URO:%d,亚硝酸NIT:%d,白细胞LEU:%d,MCA:%d\n",paperResut.VC,paperResut.GLU,paperResut.BIL,paperResut.KET,paperResut.SG,paperResut.BLD,paperResut.PH,paperResut.PRO,paperResut.URO,paperResut.NIT,paperResut.LEU,paperResut.MCA);
	for ( int Index = 0; Index < TEST_PAPER_RESULT_ITEM_COUNT; ++Index )
	{
		if( Index != 11 )
			nIndex = TEST_PAPER_RESULT_ITEM_COUNT - 1 - Index - 1;
		else
			nIndex = Index;
		GetTransUDCReslut(nIndex, TempTestPaperResult->ItemResult[Index], Data);
	}

	LogData.AppendFormat("\r\n\r\n%d:%d:%d\r\n%s\r\n", CurrentTime.wHour, CurrentTime.wMinute, CurrentTime.wSecond, Data);

	return;
}



void CUWDebugDlg::GetLogFileName(char *LogFilePath)
{
	SYSTEMTIME CurrentTime = {0};
	CStringA LogFileName;

	GetLocalTime(&CurrentTime);

	GetModuleFileNameA(NULL, LogFilePath, MAX_PATH);
	PathRemoveFileSpecA(LogFilePath);
	PathAppendA(LogFilePath, "Log");
	LogFileName.Format("%d_%d_%ddebug.txt", CurrentTime.wYear, CurrentTime.wMonth, CurrentTime.wDay);
	PathAppendA( LogFilePath, LogFileName.GetBuffer() );
}


void CUWDebugDlg::LogToFile(const CUDCTestPaperResult &paperResut)
{
	HANDLE LogFileHandle = INVALID_HANDLE_VALUE ;

	CStringA LogData;
	char LogFilePath[MAX_PATH] = {0};
	
	DWORD NumberOfBytesWritten = 0;
	
	GetLogFileName(LogFilePath);
	LogFileHandle = CreateFileA(LogFilePath, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if ( LogFileHandle == INVALID_HANDLE_VALUE )
	{
		goto end;
	}

	GetUDCLogData(paperResut, LogData);

	
	SetFilePointer(LogFileHandle, 0, 0, FILE_END);
	WriteFile(LogFileHandle, LogData.GetBuffer(), LogData.GetLength(), &NumberOfBytesWritten, NULL);

end:
	if (LogFileHandle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(LogFileHandle);
	}
}



//检测尿干化
void CUWDebugDlg::OnBnClickedBtnUdcTest()
{
	// TODO: 在此添加控件通知处理程序代码
    CUCDTestErrorCode errCode;
	CUDCTestPaperResult paperResut = {0};
	CString strInfo;
	COleDateTime tmNow = COleDateTime::GetCurrentTime();
	CString timeNow = tmNow.Format(_T("%Y-%m-%d %H:%M:%S :"));
	DBG_MSG("发送测试字条命令\n");
	UDCTestPaper(errCode,paperResut);
	if (errCode == ERROR_OPERATE_SUCCESS)
	{
		LogToFile(paperResut);
		strInfo = GetIdsString(_T("debug"),_T("message3")).c_str();
	}
	else if (errCode == ERROR_POS_NOT_CORRECT)
	{
		strInfo = GetIdsString(_T("debug"),_T("message4")).c_str();
	}
	else
	{
		strInfo = GetIdsString(_T("debug"),_T("message7")).c_str();
	}

	m_strInfo.SetWindowText(timeNow + strInfo);
}

//灰条干标定
void CUWDebugDlg::OnBnClickedBtnGreyPaperTest()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strInfo;
	COleDateTime tmNow = COleDateTime::GetCurrentTime();
	CString timeNow = tmNow.Format(_T("%Y-%m-%d %H:%M:%S :"));
	bool isOk;
	UDCCorrectModuleEx(isOk);
	if (isOk)
	{
		strInfo = GetIdsString(_T("debug"),_T("message5")).c_str();
	}
	else
	{
		strInfo = GetIdsString(_T("debug"),_T("message6")).c_str();
	}
	m_strInfo.SetWindowText(timeNow + strInfo);
}



//白条干标定
void CUWDebugDlg::OnBnClickedBtnPaperTest()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strInfo;
	COleDateTime tmNow = COleDateTime::GetCurrentTime();
	CString timeNow = tmNow.Format(_T("%Y-%m-%d %H:%M:%S :"));
	bool isOk;
	UDCCorrectModule(isOk);
	if (isOk)
	{
		strInfo = GetIdsString(_T("debug"),_T("message5")).c_str();
	}
	else
	{
		strInfo = GetIdsString(_T("debug"),_T("message6")).c_str();
	}
	m_strInfo.SetWindowText(timeNow + strInfo);

}



void CUWDebugDlg::OnBnClickedBtnUdcLight()
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_nUdcLight == 0)
	{
		UDCTurnOnLight();
		m_nUdcLight = 1;
	}
	else
	{	
		UDCTurnOffLight();
		m_nUdcLight = 0;
	}

}




BOOL CUWDebugDlg::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	SetWindowText(GetIdsString(_T("debug"),_T("title")).c_str());

	// TODO:  在此添加额外的初始化
	 m_btnPushIn.SetWindowText(GetIdsString(_T("debug"),_T("1")).c_str());
	 m_btnPushStep.SetWindowText(GetIdsString(_T("debug"),_T("2")).c_str());
	 m_btnPushStep_Reset.SetWindowText(GetIdsString(_T("debug"),_T("3")).c_str());
	 m_btnPushOut.SetWindowText(GetIdsString(_T("debug"),_T("4")).c_str());
	 m_btnBarcode.SetWindowText(GetIdsString(_T("debug"),_T("5")).c_str());
	 m_btnNeedleDown.SetWindowText(GetIdsString(_T("debug"),_T("6")).c_str());
	 m_btnNeedleUp.SetWindowText(GetIdsString(_T("debug"),_T("7")).c_str());
	 m_btnNeedleReset.SetWindowText(GetIdsString(_T("debug"),_T("8")).c_str());
	 m_btnNeedleSample.SetWindowText(GetIdsString(_T("debug"),_T("9")).c_str());
	 m_btnNeedleClean.SetWindowText(GetIdsString(_T("debug"),_T("10")).c_str());
	 m_btnNeedleQc.SetWindowText(GetIdsString(_T("debug"),_T("11")).c_str());

	 m_btnRollerReset.SetWindowText(GetIdsString(_T("debug"),_T("12")).c_str());
	 m_btnRollerDebug.SetWindowText(GetIdsString(_T("debug"),_T("13")).c_str());
	 m_btnSelect1Paper.SetWindowText(GetIdsString(_T("debug"),_T("14")).c_str());
	 m_btnSelect100Paper.SetWindowText(GetIdsString(_T("debug"),_T("15")).c_str());

	 m_btnTurnFore.SetWindowText(GetIdsString(_T("debug"),_T("16")).c_str());
	 m_btnTurnBack.SetWindowText(GetIdsString(_T("debug"),_T("17")).c_str());
	 m_btnPaperFeed.SetWindowText(GetIdsString(_T("debug"),_T("18")).c_str());

	 m_btn_X_Reset.SetWindowText(GetIdsString(_T("debug"),_T("19")).c_str());
	 m_btn_Y_Reset.SetWindowText(GetIdsString(_T("debug"),_T("20")).c_str());
	 m_btn_Z_Reset.SetWindowText(GetIdsString(_T("debug"),_T("21")).c_str());
	 m_btn_Z_Pos.SetWindowText(GetIdsString(_T("debug"),_T("22")).c_str());
	 m_btnGoTo10Lens.SetWindowText(GetIdsString(_T("debug"),_T("23")).c_str());
	 m_btnGoTo40Lens.SetWindowText(GetIdsString(_T("debug"),_T("24")).c_str());

	 m_btnMicLight.SetWindowText(GetIdsString(_T("debug"),_T("25")).c_str());
	 m_btnDropPump.SetWindowText(GetIdsString(_T("debug"),_T("26")).c_str());
	 m_btnCleanPump.SetWindowText(GetIdsString(_T("debug"),_T("27")).c_str());

	 m_btnUdcTest.SetWindowText(GetIdsString(_T("debug"),_T("28")).c_str());

	 m_btnPaperTest.SetWindowText(GetIdsString(_T("debug"),_T("29")).c_str());
	 m_btnGreyPaperTest.SetWindowText(GetIdsString(_T("debug"),_T("34")).c_str());

	 m_btnRollerLight.SetWindowText(GetIdsString(_T("debug"),_T("30")).c_str());
	 m_btnUdcLight.SetWindowText(GetIdsString(_T("debug"),_T("31")).c_str());
	 m_btnSetPaperType.SetWindowText(GetIdsString(_T("debug"),_T("32")).c_str());

	 

	 GetUDCSettingIniPath();

	 CString strNum;
	 for (int i = 0,j = 11; j < 15; ++i,++j)
	 {
		 strNum.Format(GetIdsString(_T("debug"),_T("item")).c_str(),j);
		 m_cboPaper.InsertString(i,strNum);
	 }
	 int num = 0;
	 switch( GetPrivateProfileIntA(UDC_CHECK_SETTING, UDC_CHECK_ITEM_COUNT, 11, m_UDCIni) )
	 {
	 case 11:
		 num = 0;
		 break;
	 case 12:
		 num = 1;
		 break;
	 case 13:
		 num = 2;
		 break;
	 case 14:
		 num = 3;
		 break;
	 }
	 m_cboPaper.SetCurSel(num);


	 
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}



void CUWDebugDlg::GetUDCSettingIniPath()
{
	GetModuleFileNameA( NULL, m_UDCIni, sizeof(m_UDCIni) );
	PathRemoveFileSpecA(m_UDCIni);
	PathAppendA(m_UDCIni, "Config");
	PathAppendA(m_UDCIni, UDC_SETTING_INI);
}



void CUWDebugDlg::OnBnClickedBtnSetpapertype()
{
	// TODO: 在此添加控件通知处理程序代码
	char Value[10] = {0};
	int nselect = m_cboPaper.GetCurSel();
	bool bsuccess = false;
	switch (nselect)
	{
	case 0:
		sprintf(Value, "%d", 11);
		UDCSpecifyPaperType(PAPER_TYPE_11,bsuccess);
		break;
	case 1:
		sprintf(Value, "%d", 12);
		UDCSpecifyPaperType(PAPER_TYPE_12,bsuccess);
		break;
	case 2:
		break;
	case 3:
		break;
	}
	//if (bsuccess) // 不用判断结果
	{
		WritePrivateProfileStringA(UDC_CHECK_SETTING, UDC_CHECK_ITEM_COUNT, Value, m_UDCIni);
		m_strInfo.SetWindowText(GetIdsString(_T("debug"),_T("message1")).c_str());

		CWnd *pUdcView = ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetUWView()->GetTabWnd(THUM_UDC_TAB);
		if (pUdcView)
		{
			pUdcView->PostMessage(WM_UDCSETPAPERTYPE, (WPARAM)atoi(Value));
		}
	}
	/*else
	{
		m_strInfo.SetWindowText(GetIdsString(_T("debug"),_T("message2")).c_str());
	}*/
	
}



void CUWDebugDlg::OnBnClickedButtonSolenoid1()
{
	// TODO: 在此添加控件通知处理程序代码
	TestHardware(Solenoid,Solenoid1);
}

void CUWDebugDlg::OnBnClickedButtonSolenoid2()
{
	// TODO: 在此添加控件通知处理程序代码
	TestHardware(Solenoid,Solenoid2);
}

void CUWDebugDlg::OnBnClickedButtonSolenoid3()
{
	// TODO: 在此添加控件通知处理程序代码
	TestHardware(Solenoid,Solenoid3);
}

void CUWDebugDlg::OnBnClickedButtonSolenoid4()
{
	// TODO: 在此添加控件通知处理程序代码
	TestHardware(Solenoid,Solenoid4);
}

void CUWDebugDlg::OnBnClickedButtonSolenoid5()
{
	// TODO: 在此添加控件通知处理程序代码
	TestHardware(Solenoid,Solenoid5);
}

void CUWDebugDlg::OnBnClickedButtonSolenoid6()
{
	// TODO: 在此添加控件通知处理程序代码
	TestHardware(Solenoid,Solenoid6);
}

void CUWDebugDlg::OnBnClickedButtonSolenoid7()
{
	// TODO: 在此添加控件通知处理程序代码
	TestHardware(Solenoid,Solenoid7);
}

void CUWDebugDlg::OnBnClickedButtonSolenoid8()
{
	// TODO: 在此添加控件通知处理程序代码
	TestHardware(Solenoid,Solenoid8);
}
