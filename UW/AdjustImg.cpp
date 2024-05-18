// AdjustImg.cpp : 实现文件
//

#include "stdafx.h"
#include "UW.h"
#include "AdjustImg.h"


// CAdjustImg 对话框

IMPLEMENT_DYNAMIC(CAdjustImg, CDialog)

CAdjustImg::CAdjustImg(CWnd* pParent /*=NULL*/)
	: CDialog(CAdjustImg::IDD, pParent)
	//, m_pDevice(NULL)
	, m_nCha(0)
	, m_n10xIndex(1)
	, m_n40xIndex(1)
	, m_nNum(20)
{

}

CAdjustImg::~CAdjustImg()
{
}

void CAdjustImg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_ADJUST_NUM, m_nNum);
}


BEGIN_MESSAGE_MAP(CAdjustImg, CDialog)
	ON_BN_CLICKED(IDC_ADJUST_10x_1, &CAdjustImg::OnBnClickedAdjust10x1)
	ON_BN_CLICKED(IDC_ADJUST_10x_2, &CAdjustImg::OnBnClickedAdjust10x2)
	ON_BN_CLICKED(IDC_ADJUST_10x_3, &CAdjustImg::OnBnClickedAdjust10x3)
	ON_BN_CLICKED(IDC_ADJUST_10x_4, &CAdjustImg::OnBnClickedAdjust10x4)
	ON_BN_CLICKED(IDC_ADJUST_10x_5, &CAdjustImg::OnBnClickedAdjust10x5)
	ON_BN_CLICKED(IDC_ADJUST_10x_6, &CAdjustImg::OnBnClickedAdjust10x6)
	ON_BN_CLICKED(IDC_ADJUST_10x_7, &CAdjustImg::OnBnClickedAdjust10x7)
	ON_BN_CLICKED(IDC_ADJUST_10x_8, &CAdjustImg::OnBnClickedAdjust10x8)
	ON_BN_CLICKED(IDC_ADJUST_10x_9, &CAdjustImg::OnBnClickedAdjust10x9)
	ON_BN_CLICKED(IDC_ADJUST_10x_10, &CAdjustImg::OnBnClickedAdjust10x10)
	ON_BN_CLICKED(IDC_ADJUST_40x_1, &CAdjustImg::OnBnClickedAdjust40x1)
	ON_BN_CLICKED(IDC_ADJUST_40x_2, &CAdjustImg::OnBnClickedAdjust40x2)
	ON_BN_CLICKED(IDC_ADJUST_40x_3, &CAdjustImg::OnBnClickedAdjust40x3)
	ON_BN_CLICKED(IDC_ADJUST_40x_4, &CAdjustImg::OnBnClickedAdjust40x4)
	ON_BN_CLICKED(IDC_ADJUST_40x_5, &CAdjustImg::OnBnClickedAdjust40x5)
	ON_BN_CLICKED(IDC_ADJUST_40x_6, &CAdjustImg::OnBnClickedAdjust40x6)
	ON_BN_CLICKED(IDC_ADJUST_40x_7, &CAdjustImg::OnBnClickedAdjust40x7)
	ON_BN_CLICKED(IDC_ADJUST_40x_8, &CAdjustImg::OnBnClickedAdjust40x8)
	ON_BN_CLICKED(IDC_ADJUST_40x_9, &CAdjustImg::OnBnClickedAdjust40x9)
	ON_BN_CLICKED(IDC_ADJUST_40x_10, &CAdjustImg::OnBnClickedAdjust40x10)
	ON_BN_CLICKED(IDC_ADJUST_40x_11, &CAdjustImg::OnBnClickedAdjust40x11)
	ON_BN_CLICKED(IDC_ADJUST_40x_12, &CAdjustImg::OnBnClickedAdjust40x12)
	ON_BN_CLICKED(IDC_ADJUST_40x_13, &CAdjustImg::OnBnClickedAdjust40x13)
	ON_BN_CLICKED(IDC_ADJUST_40x_14, &CAdjustImg::OnBnClickedAdjust40x14)
	ON_BN_CLICKED(IDC_ADJUST_40x_15, &CAdjustImg::OnBnClickedAdjust40x15)
	ON_BN_CLICKED(IDC_ADJUST_40x_16, &CAdjustImg::OnBnClickedAdjust40x16)
	ON_BN_CLICKED(IDC_MOVE_LEFT, &CAdjustImg::OnBnClickedMoveLeft)
	ON_BN_CLICKED(IDC_MOVE_RIGHT, &CAdjustImg::OnBnClickedMoveRight)
	ON_BN_CLICKED(IDC_MOVE_FRONT, &CAdjustImg::OnBnClickedMoveFront)
	ON_BN_CLICKED(IDC_MOVE_BACK, &CAdjustImg::OnBnClickedMoveBack)
	ON_BN_CLICKED(IDC_AJUSTIMG_A, &CAdjustImg::OnBnClickedAjustimgA)
	ON_BN_CLICKED(IDC_AJUSTIMG_B, &CAdjustImg::OnBnClickedAjustimgB)
	ON_BN_CLICKED(IDOK, &CAdjustImg::OnBnClickedOk)
END_MESSAGE_MAP()


// CAdjustImg 消息处理程序

void CAdjustImg::OnBnClickedAdjust10x1()
{
	// TODO: 在此添加控件通知处理程序代码
	m_n10xIndex = 0;
	Pic10x();
}

void CAdjustImg::OnBnClickedAdjust10x2()
{
	// TODO: 在此添加控件通知处理程序代码
	m_n10xIndex = 1;
	Pic10x();
}

void CAdjustImg::OnBnClickedAdjust10x3()
{
	// TODO: 在此添加控件通知处理程序代码
	m_n10xIndex = 2;
	Pic10x();
}

void CAdjustImg::OnBnClickedAdjust10x4()
{
	// TODO: 在此添加控件通知处理程序代码
	m_n10xIndex = 3;
	Pic10x();
}

void CAdjustImg::OnBnClickedAdjust10x5()
{
	// TODO: 在此添加控件通知处理程序代码
	m_n10xIndex = 4;
	Pic10x();
}

void CAdjustImg::OnBnClickedAdjust10x6()
{
	// TODO: 在此添加控件通知处理程序代码
	m_n10xIndex = 5;
	Pic10x();
}

void CAdjustImg::OnBnClickedAdjust10x7()
{
	// TODO: 在此添加控件通知处理程序代码
	m_n10xIndex = 6;
	Pic10x();
}

void CAdjustImg::OnBnClickedAdjust10x8()
{
	// TODO: 在此添加控件通知处理程序代码
	m_n10xIndex = 7;
	Pic10x();
}

void CAdjustImg::OnBnClickedAdjust10x9()
{
	// TODO: 在此添加控件通知处理程序代码
	m_n10xIndex = 8;
	Pic10x();
}

void CAdjustImg::OnBnClickedAdjust10x10()
{
	// TODO: 在此添加控件通知处理程序代码
	m_n10xIndex = 9;
	Pic10x();
}

void CAdjustImg::OnBnClickedAdjust40x1()
{
	// TODO: 在此添加控件通知处理程序代码
	m_n40xIndex = 0;
	Pic40x();
}

void CAdjustImg::OnBnClickedAdjust40x2()
{
	// TODO: 在此添加控件通知处理程序代码
	m_n40xIndex = 1;
	Pic40x();
}

void CAdjustImg::OnBnClickedAdjust40x3()
{
	// TODO: 在此添加控件通知处理程序代码
	m_n40xIndex = 2;
	Pic40x();
}

void CAdjustImg::OnBnClickedAdjust40x4()
{
	// TODO: 在此添加控件通知处理程序代码
	m_n40xIndex = 3;
	Pic40x();
}

void CAdjustImg::OnBnClickedAdjust40x5()
{
	// TODO: 在此添加控件通知处理程序代码
	m_n40xIndex = 4;
	Pic40x();
}

void CAdjustImg::OnBnClickedAdjust40x6()
{
	// TODO: 在此添加控件通知处理程序代码
	m_n40xIndex = 5;
	Pic40x();
}

void CAdjustImg::OnBnClickedAdjust40x7()
{
	// TODO: 在此添加控件通知处理程序代码
	m_n40xIndex = 6;
	Pic40x();
}

void CAdjustImg::OnBnClickedAdjust40x8()
{
	// TODO: 在此添加控件通知处理程序代码
	m_n40xIndex = 7;
	Pic40x();
}

void CAdjustImg::OnBnClickedAdjust40x9()
{
	// TODO: 在此添加控件通知处理程序代码
	m_n40xIndex = 8;
	Pic40x();
}

void CAdjustImg::OnBnClickedAdjust40x10()
{
	// TODO: 在此添加控件通知处理程序代码
	m_n40xIndex = 9;
	Pic40x();
}

void CAdjustImg::OnBnClickedAdjust40x11()
{
	// TODO: 在此添加控件通知处理程序代码
	m_n40xIndex = 10;
	Pic40x();
}

void CAdjustImg::OnBnClickedAdjust40x12()
{
	// TODO: 在此添加控件通知处理程序代码
	m_n40xIndex = 11;
	Pic40x();
}

void CAdjustImg::OnBnClickedAdjust40x13()
{
	// TODO: 在此添加控件通知处理程序代码
	m_n40xIndex = 12;
	Pic40x();
}

void CAdjustImg::OnBnClickedAdjust40x14()
{
	// TODO: 在此添加控件通知处理程序代码
	m_n40xIndex = 13;
	Pic40x();
}

void CAdjustImg::OnBnClickedAdjust40x15()
{
	// TODO: 在此添加控件通知处理程序代码
	m_n40xIndex = 14;
	Pic40x();
}

void CAdjustImg::OnBnClickedAdjust40x16()
{
	// TODO: 在此添加控件通知处理程序代码
	m_n40xIndex = 15;
	Pic40x();
}

void CAdjustImg::Pic10x()
{
	//unsigned char chPara[8] = {0};
	//chPara[0] = m_n10xIndex;
	//chPara[1] = 0;
	//chPara[2] = m_nCha;
	//chPara[3] = 1;
	//theApp.m_pDevice->SendCommand(CCMD_10LOCATION, chPara, 4);
	CLocate10MicroscopeInfo mic10;
	mic10.XPicNum = m_n10xIndex;
	mic10.YPicNum = 0;
	mic10.ChannelNum= m_nCha;
	mic10.IsReturnTakePicCommand =1;
	Locate10Microscope(mic10);
}

void CAdjustImg::Pic40x()
{
	/*unsigned char chPara[8] = {0};
	chPara[0] = m_n10xIndex;
	chPara[1] = m_n40xIndex;
	chPara[2] = m_nCha;
	chPara[3] = 1;
	theApp.m_pDevice->SendCommand(CCMD_40LOCATION, chPara, 4);*/


	//CLocate40MicroscopeInfo mic40;
	//mic40 = m_n10xIndex;
	//mic40.YPicNum = m_n40xIndex;
	//mic40.ChannelNum= m_nCha;
	//mic40.IsReturnTakePicCommand =1;
	//	Locate40Microscope(mic40);

}

void CAdjustImg::OnBnClickedMoveLeft()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	//unsigned char chPara[8] = {0};
	//chPara[0] = 5;
	//chPara[1] = 1;
	//chPara[2] = m_nNum;
	//theApp.m_pDevice->SendCommand(CCMD_MICRO_MOVE, chPara, 4);
	MoveScanPlatform(XMotorID,Foreward,m_nNum);
	
}

void CAdjustImg::OnBnClickedMoveRight()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	/*unsigned char chPara[8] = {0};
	chPara[0] = 5;
	chPara[1] = 0;
	chPara[2] = m_nNum;
	theApp.m_pDevice->SendCommand(CCMD_MICRO_MOVE, chPara, 4);*/
	MoveScanPlatform(XMotorID,Backward,m_nNum);
}

void CAdjustImg::OnBnClickedMoveFront()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	//unsigned char chPara[8] = {0};
	//chPara[0] = 9;
	//chPara[1] = 1;
	//chPara[2] = m_nNum;
	//theApp.m_pDevice->SendCommand(CCMD_MICRO_MOVE, chPara, 4);
	MoveScanPlatform(YMotorID,Foreward,m_nNum);
}

void CAdjustImg::OnBnClickedMoveBack()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	//unsigned char chPara[8] = {0};
	//chPara[0] = 9;
	//chPara[1] = 0;
	//chPara[2] = m_nNum;
	//theApp.m_pDevice->SendCommand(CCMD_MICRO_MOVE, chPara, 4);
	MoveScanPlatform(YMotorID,Backward,m_nNum);
}

void CAdjustImg::OnBnClickedAjustimgA()
{
	// TODO: 在此添加控件通知处理程序代码
	ConvertToAChannel();
}

void CAdjustImg::OnBnClickedAjustimgB()
{
	// TODO: 在此添加控件通知处理程序代码
	ConvertToBChannel();
}

void CAdjustImg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	//OnOK();
}
