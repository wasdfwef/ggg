// SetTestTypeDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "UW.h"
#include "SetTestTypeDlg.h"
#include "afxdialogex.h"
#include "DbgMsg.h"
#include <map>



// CSetTestTypeDlg 对话框

IMPLEMENT_DYNAMIC(CSetTestTypeDlg, CDialogEx)

CSetTestTypeDlg::CSetTestTypeDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSetTestTypeDlg::IDD, pParent)
{

}

CSetTestTypeDlg::~CSetTestTypeDlg()
{
}

void CSetTestTypeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


void CSetTestTypeDlg::initUI()
{
	if (theApp.g_nSettingType == 1) ((CButton*)GetDlgItem(IDC_RADIO_TYPE1))->SetCheck(1);
	else ((CButton*)GetDlgItem(IDC_RADIO_TYPE2))->SetCheck(1);
	if (theApp.g_nSettingType1_type == 1) ((CButton*)GetDlgItem(IDC_RADIO_TYPE1_1))->SetCheck(1);
	else if (theApp.g_nSettingType1_type == 2) ((CButton*)GetDlgItem(IDC_RADIO_TYPE1_2))->SetCheck(1);
	else ((CButton*)GetDlgItem(IDC_RADIO_TYPE1_3))->SetCheck(1);

	for (int i = 0; i < theApp.g_vecHoleForSet.size(); i++)
	{
		((CButton*)GetDlgItem(theApp.g_mapHoleSet[i][theApp.g_vecHoleForSet[i]]))->SetCheck(1);
	}
}

BEGIN_MESSAGE_MAP(CSetTestTypeDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_OK, &CSetTestTypeDlg::OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_RADIO_TYPE1, &CSetTestTypeDlg::OnBnClickedRadioType)
	ON_BN_CLICKED(IDC_RADIO_TYPE2, &CSetTestTypeDlg::OnBnClickedRadioType)
	ON_BN_CLICKED(IDC_RADIO_TYPE1_1, &CSetTestTypeDlg::OnBnClickedRadioType1)
	ON_BN_CLICKED(IDC_RADIO_TYPE1_2, &CSetTestTypeDlg::OnBnClickedRadioType1)
	ON_BN_CLICKED(IDC_RADIO_TYPE1_3, &CSetTestTypeDlg::OnBnClickedRadioType1)
	ON_BN_CLICKED(IDC_RADIO_TYPE2_H1_1, &CSetTestTypeDlg::OnBnClickedRadioType2H1)
	ON_BN_CLICKED(IDC_RADIO_TYPE2_H1_2, &CSetTestTypeDlg::OnBnClickedRadioType2H1)
	ON_BN_CLICKED(IDC_RADIO_TYPE2_H1_3, &CSetTestTypeDlg::OnBnClickedRadioType2H1)
	ON_BN_CLICKED(IDC_RADIO_TYPE2_H2_1, &CSetTestTypeDlg::OnBnClickedRadioType2H2)
	ON_BN_CLICKED(IDC_RADIO_TYPE2_H2_2, &CSetTestTypeDlg::OnBnClickedRadioType2H2)
	ON_BN_CLICKED(IDC_RADIO_TYPE2_H2_3, &CSetTestTypeDlg::OnBnClickedRadioType2H2)
	ON_BN_CLICKED(IDC_RADIO_TYPE2_H3_1, &CSetTestTypeDlg::OnBnClickedRadioType2H3)
	ON_BN_CLICKED(IDC_RADIO_TYPE2_H3_2, &CSetTestTypeDlg::OnBnClickedRadioType2H3)
	ON_BN_CLICKED(IDC_RADIO_TYPE2_H3_3, &CSetTestTypeDlg::OnBnClickedRadioType2H3)
	ON_BN_CLICKED(IDC_RADIO_TYPE2_H4_1, &CSetTestTypeDlg::OnBnClickedRadioType2H4)
	ON_BN_CLICKED(IDC_RADIO_TYPE2_H4_2, &CSetTestTypeDlg::OnBnClickedRadioType2H4)
	ON_BN_CLICKED(IDC_RADIO_TYPE2_H4_3, &CSetTestTypeDlg::OnBnClickedRadioType2H4)
	ON_BN_CLICKED(IDC_RADIO_TYPE2_H5_1, &CSetTestTypeDlg::OnBnClickedRadioType2H5)
	ON_BN_CLICKED(IDC_RADIO_TYPE2_H5_2, &CSetTestTypeDlg::OnBnClickedRadioType2H5)
	ON_BN_CLICKED(IDC_RADIO_TYPE2_H5_3, &CSetTestTypeDlg::OnBnClickedRadioType2H5)
	ON_BN_CLICKED(IDC_RADIO_TYPE2_H6_1, &CSetTestTypeDlg::OnBnClickedRadioType2H6)
	ON_BN_CLICKED(IDC_RADIO_TYPE2_H6_2, &CSetTestTypeDlg::OnBnClickedRadioType2H6)
	ON_BN_CLICKED(IDC_RADIO_TYPE2_H6_3, &CSetTestTypeDlg::OnBnClickedRadioType2H6)
	ON_BN_CLICKED(IDC_RADIO_TYPE2_H7_1, &CSetTestTypeDlg::OnBnClickedRadioType2H7)
	ON_BN_CLICKED(IDC_RADIO_TYPE2_H7_2, &CSetTestTypeDlg::OnBnClickedRadioType2H7)
	ON_BN_CLICKED(IDC_RADIO_TYPE2_H7_3, &CSetTestTypeDlg::OnBnClickedRadioType2H7)
	ON_BN_CLICKED(IDC_RADIO_TYPE2_H8_1, &CSetTestTypeDlg::OnBnClickedRadioType2H8)
	ON_BN_CLICKED(IDC_RADIO_TYPE2_H8_2, &CSetTestTypeDlg::OnBnClickedRadioType2H8)
	ON_BN_CLICKED(IDC_RADIO_TYPE2_H8_3, &CSetTestTypeDlg::OnBnClickedRadioType2H8)
	ON_BN_CLICKED(IDC_RADIO_TYPE2_H9_1, &CSetTestTypeDlg::OnBnClickedRadioType2H9)
	ON_BN_CLICKED(IDC_RADIO_TYPE2_H9_2, &CSetTestTypeDlg::OnBnClickedRadioType2H9)
	ON_BN_CLICKED(IDC_RADIO_TYPE2_H9_3, &CSetTestTypeDlg::OnBnClickedRadioType2H9)
	ON_BN_CLICKED(IDC_RADIO_TYPE2_H10_1, &CSetTestTypeDlg::OnBnClickedRadioType2H10)
	ON_BN_CLICKED(IDC_RADIO_TYPE2_H10_2, &CSetTestTypeDlg::OnBnClickedRadioType2H10)
	ON_BN_CLICKED(IDC_RADIO_TYPE2_H10_3, &CSetTestTypeDlg::OnBnClickedRadioType2H10)
	ON_BN_CLICKED(IDC_RADIO_TYPE2_H11_1, &CSetTestTypeDlg::OnBnClickedRadioType2H11)
	ON_BN_CLICKED(IDC_RADIO_TYPE2_H11_2, &CSetTestTypeDlg::OnBnClickedRadioType2H11)
	ON_BN_CLICKED(IDC_RADIO_TYPE2_H11_3, &CSetTestTypeDlg::OnBnClickedRadioType2H11)
END_MESSAGE_MAP()


// CSetTestTypeDlg 消息处理程序


void CSetTestTypeDlg::OnBnClickedButtonOk()
{
	if (theApp.g_nSettingType == 1 && theApp.IsIdleStatus() == FALSE)
	{
		MessageBox(L"请等到检测完成后再变更测试类型", MB_OK);
		return;
	}
	DBG_MSG("正在变更测试类型");

	// 配置文件
	char CfgFilePath[MAX_PATH] = { 0 };
	GetModuleFileNameA(NULL, CfgFilePath, MAX_PATH);
	PathRemoveFileSpecA(CfgFilePath);
	PathAppendA(CfgFilePath, "\\Config\\testmode.ini");
	char key[MAX_PATH] = { 0 };
	char value[MAX_PATH] = { 0 };


	if (theApp.g_nSettingType == 1)
	{
		GETTESTINFO_CONTEXT ctx = { 0 };
		bool bSuccess = false;
		ctx.nTestType = theApp.g_nSettingType1_type;
		ctx.nUdcTestItemCount = theApp.m_nUdcItemCount;
		theApp.setTestType = true;
		DBG_MSG("1.debug:theApp.setTestType = %d,& = %p\n", theApp.setTestType, &theApp.setTestType);
		BeginWaitCursor();
		WirteHardwareParamTestType(ctx, bSuccess);
		EndWaitCursor();
		DBG_MSG("2.debug:theApp.setTestType = %d,& = %p\n", theApp.setTestType, &theApp.setTestType);
		if (bSuccess)
		{
			MessageBox(L"设置成功", MB_OK);
			theApp.UpdateCheckType();

			sprintf_s(value, "%d", theApp.g_nSettingType);
			WritePrivateProfileStringA("testmode", "mode", value, CfgFilePath);
			sprintf_s(value, "%d", theApp.g_nSettingType1_type);
			WritePrivateProfileStringA("testmode", "mode1_type", value, CfgFilePath);

			OnOK();
		}
		else
		{
			MessageBox(L"设置失败", MB_OK);
		}
	}
	else
	{
		sprintf_s(value, "%d", theApp.g_nSettingType);
		WritePrivateProfileStringA("testmode", "mode", value, CfgFilePath);
		for (int i = 0; i < theApp.g_vecHoleForSet.size(); i++)
		{
			sprintf_s(key, "hole_%d", (i + 1));
			sprintf_s(value, "%d", theApp.g_vecHoleForSet[i]);
			WritePrivateProfileStringA("testmode", key, value, CfgFilePath);
		}
		MessageBox(L"设置成功", MB_OK);
		OnOK();
	}
}


BOOL CSetTestTypeDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	initUI();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

void CSetTestTypeDlg::OnBnClickedRadioType()
{
	if (((CButton*)GetDlgItem(IDC_RADIO_TYPE1))->GetCheck())
	{
		theApp.g_nSettingType = 1;
	}
	else if (((CButton*)GetDlgItem(IDC_RADIO_TYPE2))->GetCheck())
	{
		theApp.g_nSettingType = 2;
	}
}

void CSetTestTypeDlg::OnBnClickedRadioType1()
{
	if (((CButton*)GetDlgItem(IDC_RADIO_TYPE1_1))->GetCheck())
	{
		theApp.g_nSettingType1_type = 1;
	}
	else if (((CButton*)GetDlgItem(IDC_RADIO_TYPE1_2))->GetCheck())
	{
		theApp.g_nSettingType1_type = 2;
	}
	else
	{
		theApp.g_nSettingType1_type = 3;
	}
}

void CSetTestTypeDlg::OnBnClickedRadioType2H1()
{
	for (auto itr = theApp.g_mapHoleSet[0].begin(); itr != theApp.g_mapHoleSet[0].end(); itr++)
	{
		if (((CButton*)GetDlgItem(itr->second))->GetCheck())
		{
			theApp.g_vecHoleForSet[0] = itr->first;
			break;
		}
	}
}

void CSetTestTypeDlg::OnBnClickedRadioType2H2()
{
	for (auto itr = theApp.g_mapHoleSet[1].begin(); itr != theApp.g_mapHoleSet[1].end(); itr++)
	{
		if (((CButton*)GetDlgItem(itr->second))->GetCheck())
		{
			theApp.g_vecHoleForSet[1] = itr->first;
			break;
		}
	}
}

void CSetTestTypeDlg::OnBnClickedRadioType2H3()
{
	for (auto itr = theApp.g_mapHoleSet[2].begin(); itr != theApp.g_mapHoleSet[2].end(); itr++)
	{
		if (((CButton*)GetDlgItem(itr->second))->GetCheck())
		{
			theApp.g_vecHoleForSet[2] = itr->first;
			break;
		}
	}
}

void CSetTestTypeDlg::OnBnClickedRadioType2H4()
{
	for (auto itr = theApp.g_mapHoleSet[3].begin(); itr != theApp.g_mapHoleSet[3].end(); itr++)
	{
		if (((CButton*)GetDlgItem(itr->second))->GetCheck())
		{
			theApp.g_vecHoleForSet[3] = itr->first;
			break;
		}
	}
}

void CSetTestTypeDlg::OnBnClickedRadioType2H5()
{
	for (auto itr = theApp.g_mapHoleSet[4].begin(); itr != theApp.g_mapHoleSet[4].end(); itr++)
	{
		if (((CButton*)GetDlgItem(itr->second))->GetCheck())
		{
			theApp.g_vecHoleForSet[4] = itr->first;
			break;
		}
	}
}

void CSetTestTypeDlg::OnBnClickedRadioType2H6()
{
	for (auto itr = theApp.g_mapHoleSet[5].begin(); itr != theApp.g_mapHoleSet[5].end(); itr++)
	{
		if (((CButton*)GetDlgItem(itr->second))->GetCheck())
		{
			theApp.g_vecHoleForSet[5] = itr->first;
			break;
		}
	}
}

void CSetTestTypeDlg::OnBnClickedRadioType2H7()
{
	for (auto itr = theApp.g_mapHoleSet[6].begin(); itr != theApp.g_mapHoleSet[6].end(); itr++)
	{
		if (((CButton*)GetDlgItem(itr->second))->GetCheck())
		{
			theApp.g_vecHoleForSet[6] = itr->first;
			break;
		}
	}
}

void CSetTestTypeDlg::OnBnClickedRadioType2H8()
{
	for (auto itr = theApp.g_mapHoleSet[7].begin(); itr != theApp.g_mapHoleSet[7].end(); itr++)
	{
		if (((CButton*)GetDlgItem(itr->second))->GetCheck())
		{
			theApp.g_vecHoleForSet[7] = itr->first;
			break;
		}
	}
}

void CSetTestTypeDlg::OnBnClickedRadioType2H9()
{
	for (auto itr = theApp.g_mapHoleSet[8].begin(); itr != theApp.g_mapHoleSet[8].end(); itr++)
	{
		if (((CButton*)GetDlgItem(itr->second))->GetCheck())
		{
			theApp.g_vecHoleForSet[8] = itr->first;
			break;
		}
	}
}

void CSetTestTypeDlg::OnBnClickedRadioType2H10()
{
	for (auto itr = theApp.g_mapHoleSet[9].begin(); itr != theApp.g_mapHoleSet[9].end(); itr++)
	{
		if (((CButton*)GetDlgItem(itr->second))->GetCheck())
		{
			theApp.g_vecHoleForSet[9] = itr->first;
			break;
		}
	}
}

void CSetTestTypeDlg::OnBnClickedRadioType2H11()
{
	for (auto itr = theApp.g_mapHoleSet[10].begin(); itr != theApp.g_mapHoleSet[10].end(); itr++)
	{
		if (((CButton*)GetDlgItem(itr->second))->GetCheck())
		{
			theApp.g_vecHoleForSet[10] = itr->first;
			break;
		}
	}
}

