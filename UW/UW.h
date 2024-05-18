
// UW.h : UW 应用程序的主头文件
//
#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif
//#include ".\inc\THDBUW_Access.h"

#include "..\..\..\inc\THInterface.h"
#include "..\..\..\inc\UW2000DB.h"
#include "..\..\..\inc\HospitalInfo.h"
#include "..\..\..\inc\DoctorSetInfo.h"
#include "..\..\..\inc\CmdHelper.h"
#include "..\..\..\inc\CSetConfig.h"
#include "..\..\..\inc\CSetUdcConfig.h"
#include "..\..\..\inc\CUWSetCollectPicRange.h"
#include "..\..\..\inc\UWReportAPI.h"
#include "..\..\..\inc\CPrintModuleSet.h"
#include "..\..\..\inc\CCameraSet.h"

#pragma comment(lib, "..\\..\\..\\lib\\CCameraSet.lib")
#pragma comment(lib, "..\\..\\..\\lib\\CPrintModuleSet.lib")
#pragma comment(lib, "..\\..\\..\\lib\\UWReportAPI.lib")
#pragma comment(lib, "..\\..\\..\\lib\\THInterface.lib")
#pragma comment(lib, "..\\..\\..\\lib\\UW2000DB.lib")
#pragma comment(lib, "..\\..\\..\\lib\\HospitalInfo.lib")
#pragma comment(lib, "..\\..\\..\\lib\\DoctorSetInfo.lib")
#pragma comment(lib, "..\\..\\..\\lib\\CSetConfig.lib")
#pragma comment(lib, "..\\..\\..\\lib\\CSetUdcConfig.lib")
#pragma comment(lib, "..\\..\\..\\lib\\CmdHelper.lib")
#pragma comment(lib, "..\\..\\..\\lib\\CUWSetCollectPicRange.lib")

#include "resource.h"       // 主符号
#include <xstring>
//#include "Include/Common/Node.h"
#include "TaskStatus.h"
#include "TestType.h"
#include "UserDlg.h"
#include "SelfCheckdlg.h"

#include "THMessageDispatcher.h"
#include "CLiushuixianOpt.h"

#include "websocket/websoket-interface.h"

#include <atomic>
#include <mutex>

// CUWApp:
// 有关此类的实现，请参阅 UW.cpp
//
#define MAX_SN 10000
#define MAX_ROW 10000
#define INDEX_BUF_LEN              16
#define CELLTYPE_BUF_LEN           32

#define US_SHOW_MASK  0x1
#define US_PRINT_MASK 0x2
#define US_SEND_MASK  0x4
#define UDC_MAXITEM          16
#define UDC_MAXGRADE         7

#define VIDEO_CY_VIEW_TAB  0
//#define VIDEO_CY_VIEW2_TAB 1
#define THUM_VIEW_TAB      1
#define THUM_UDC_TAB       2

#ifdef NDEBUG
#define CONTRALCLEAN   1 //耗材是否控制0是否1是开
#else
#define CONTRALCLEAN   0 
#endif

#define CLASS_INI_NAME             _T("Config\\debug.ini")
#define CLASS_SECTION_NAME         _T("ClassConfig")

#define WM_FINDRECORD_LOOKINFO     WM_USER+120  //查询记录视图 查看详细信息
#define WM_FINDRECORD_DELINFO      WM_USER+121  //查询记录视图 删除数据
#define WM_TODAYLIST_LOOKINFO      WM_USER+122  //当天记录视图 查看详细信息
#define WM_TODAYLIST_DELINFO       WM_USER+123  //当天记录视图 删除数据
#define WM_TODAYLIST_ADDINFO       WM_USER+124  //当天记录视图 添加数据
#define WM_WNDPAT_UPDATEDATA       WM_USER+125  //病人信息窗口 更新数据

#define WM_TODAYLIST_UPDATELISTPOS WM_USER+126  //当天记录视图 更新列表控件滚动到最新位置

#define WM_EQUIPMENT_MSG           WM_USER+130  // 下位机消息
#define WM_UDCTEST_MSG             WM_USER+131  // UDC检测模块消息
#define WM_UDCTEST_DEBUG_MSG       WM_USER+132  
#define WM_SELECTRECORDBYUDC       WM_USER+133  

#define WM_AUTO_FOCUS			  (WM_USER + 150)

#define WM_HAOCAI				  (WM_USER + 160)	
#define WM_UDCSETPAPERTYPE		  (WM_USER + 161)
#define WM_SETCLEANTIMER2		  (WM_USER + 162)

#define WM_UPDATE_TASKUI		  (WM_USER + 170)

#define WM_WS_QC_CTRL	          (WM_USER + 370)

#define UDC_SETTING_INI			"udc.ini"
#define UDC_CHECK_SETTING		"udcsetting"
#define UDC_CHECK_ITEM_COUNT	"udccount"

#define UW2000_ISRUNNING_MUTEX "THME_UW2000_ISRUNNING_MUTEX"

#define REC_PATH  "RecGrapReslut\\RecPic"
#define REC_PATH2 "RecGrapReslut\\RecPic2"


typedef struct udcinfo
{
	TCHAR m_ctsItemCode[INDEX_BUF_LEN];
	TCHAR m_ctsItemName[INDEX_BUF_LEN];
	BOOL  m_bGradeEnable[UDC_MAXGRADE];
	TCHAR m_ctsGradeName[UDC_MAXGRADE][INDEX_BUF_LEN];
	int   m_nGradeThreshold[UDC_MAXGRADE];
	TCHAR m_ctsGradeValue[UDC_MAXGRADE][INDEX_BUF_LEN];
	TCHAR m_ctsGradeUnit[UDC_MAXGRADE][INDEX_BUF_LEN];
}UW2000_UDC_INFO;

typedef struct UW2000_VISIO_INFO
{
	int nChannel;
	int nIndex;
	unsigned int nSelect;
} UW2000_VISIO_INFO;

typedef struct UW2000_VISIO_LINE_COLUMN
{
	int nChannel;
	unsigned int nLines;
	unsigned int nColumn;

}UW2000_VISIO_LINE_COLUMN;

#define HOST_LEN (64)
typedef struct
{
	char host[HOST_LEN];
	unsigned short port;
	unsigned short port2;
	char imagePath[MAX_PATH];
}HOSTINFO;
typedef struct {
	HOSTINFO local;
	HOSTINFO remote;
}PIPELINE_INFO, *PPIPELINE_INFO;

typedef struct {
    std::mutex lock;
    unsigned long idleTimeSecond;
    int countSamples;
}STRONG_CLEAN_SCHRDULE;

typedef struct _QC_TYPE_NODE{
    TestDes td;
    _QC_TYPE_NODE *nextTypeNode;
}QC_TYPE_NODE;

typedef struct {
    QC_TYPE_NODE *headerNode;
    QC_TYPE_NODE *curNode;
}QC_CIRCULAR_LIST;


#define _CRT_SECURE_NO_DEPRECATE
class CUWApp : public CWinAppEx, public CTaskSTatus, public CTestType, public FlowlineClientInterface
{

	BOOL    GetCellConfig(BOOL  bUS);
	void    GetBD();
	void    GetConsumableControl();
	void    GetLiushuixianCfg();
	
public:
	CUWApp();


// 重写
public:
	virtual BOOL InitInstance();

	// 实现
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	//oid menumodify();
	DECLARE_MESSAGE_MAP()

	// 配置文件路径
	//std::string GetFilePath(void);
public:
	BOOL               StartUW2KFramework();
	bool			   ShowOtherType();

	float              m_b;
	BOOL			   m_isVariableRatioSpecialDeal; // 系数是否特别处理
	GET_US_INFO        us_cfg_info;
	GET_UDC_INFO       udc_cfg_info;
    char			   m_UDCIni[MAX_PATH];

	TCHAR m_chClassIniPath[MAX_PATH];     // 细胞类型配置文件路径
	TCHAR m_szExeFolder[MAX_PATH];        // exe文件路径
	char  m_szExeFolderA[MAX_PATH];       // exe文件路径(char型字符串)
	UW2000_UDC_INFO   m_UdcitemInfo[UDC_MAXITEM];

	CUserDlg userdlg;
	USER_LIST_INFO1      m_userlist;     //所有用户列表	                      
	USER_INFO            loginUser;      //当前登录用户
	SEARCH_TASK          searchtask;      //查询记录

	HOSPITAL_INFO_LIST   hospitalInfo;    //医院信息
	LPCOLOR_INFO         colorInfo;       //颜色
	int                  colorNum;
	LPCLEAR_INFO         clearInfo;       //透明度
	int                  clearNum;
	LPDIAGNOS_INFO       diagnosInfo;     //诊断信息
	int                  diagnosNum;
	int                  m_LastClearTimes;
	bool				continueResetZ;
	TestDes              nAttribute;      // 任务类型
	TestDes              nAttributeBeforeQC;      // 质控之前任务类型
	int              testTypeBeforeQC;      // 质控之前测试类型
	bool				 m_IsAutoModifyLEU;
	int isQC;//尿沉渣质控
	int m_nUserNum;                       //用户数
	int loginGroup;
	int m_nUdcItem;
	int m_nUsNum;
	int m_nID;                           //ID号

	int m_nTestQcType;                   // 测试质控类型(unused)
	int m_nSN;                           // 顺序号
	int m_SaveCount;					 // 保存的任务数
	LONG m_nTubeShelfNum;                // 架号
	
	bool m_startDev;
	bool m_IsFirst;
	bool m_bNeesAutoFocus;
	bool m_bCanAutoFocus;
	bool m_bAutoCheckFail;
	bool m_bInDebugMode;
	bool m_bIsInAutoFocus;
	bool m_bSaveDate;

	GET_US_INFO  m_usInfo;               //有效的沉渣粒子信息
	GET_UDC_INFO m_udcInfo;              //有效的干化粒子信息

	SYSTEMTIME m_CurrentDate;

	bool m_bIsControlConsumable;   // 是否控制耗材
	bool setTestType;
	char udcOrder[64];
	int m_sendLisType;
	int m_sendNum;
	int m_sendLisTimeInterval;
	CTHMessageDispatcher m_clsMsgDipatcher; // 下位机消息分发器
	std::atomic<bool> m_bLCResetSuccess; // 下位机复位是否成功
	std::atomic<int> m_nUdcItemCount;
	bool GetSetTestType();
	bool m_bIsLiushuixain;  // 流水线模式
	int m_nDeviceNo;        // 流水线设备编号
	PIPELINE_INFO m_stDevPipelineInfo;
	CCLiushuixianOpt m_lsx;

	BOOL m_bIsDoubleRec; // 是否是双识别

	int g_nSettingType = 1; // 测试类型设置类型 。1统一设置的测试类型，2每个孔位独立设置的测试类型
	int g_nSettingType1_type = 3; // 测试设置类型1时的测试类型，默认为3都做
	std::vector<int> g_vecHoleForSet; // 每个孔位设置的测试类型
	std::map<int, std::map<int, int>> g_mapHoleSet; // 每个孔位对应的测试类型对应的控件ID
	BOOL g_isForceUseSoftwareSettingTesttype = FALSE;

	STRONG_CLEAN_SCHRDULE g_schrduleStrongClean = { };

    QC_CIRCULAR_LIST m_qcTypeList;
	
public:
	void   GetStatusAtNew(StatusEnum & nUSStatus, StatusEnum & nUDCStatus, int nTestType, BOOL bTaskResult); //新建时获取us,udc初始状态
	void   ParseStatusMaskCode(ULONG nStatusMaskCode, StatusEnum & nUSStatus, StatusEnum & nUDCStatus);//解析掩码值
	ULONG  MakeStatusMaskCode(StatusEnum  nUSStatus, StatusEnum  nUDCStatus); //生成掩码值

	void   SubCleanCount(USHORT nCleanCount,int &RemainTimes,BOOL bShowTip);//耗材控制
	bool   SubCleanCount(USHORT nCleanCount);//耗材控制
	bool   QueryConsumables(USHORT nCleanCount, int &count, bool &isExpired, HWND hWnd);//耗材控制
	bool   CopyDBToBackup(bool isError = false);

	bool CopyBackupToDB(bool isReset = false);
	//	afx_msg void OnMenuSet();
	virtual int ExitInstance();
	void WriteSNNums(int nNums);
	int ReadSNNums();
	void WriteTubeShelfNums(int nNums);
	int ReadTubeShelfNums();
	void WriteSaveNums(int nNums);
	int ReadSaveNums(void);
	void WriteSendLisSign(int IsAutoSendToLis);
	int ReadSendLisSign();
	void GetConfigVisioLineAndColumn(UW2000_VISIO_LINE_COLUMN* pUw2000visiolinecolumn);
	void WriteConfigVisioVaule(UW2000_VISIO_INFO info);
	void GetUDCOrder();
	void GetLisType();
	void GetConfigVisioVaule(UW2000_VISIO_INFO* pUw2000visioinfo);
	void WriteConfigVisioLineAndColumn(UW2000_VISIO_LINE_COLUMN info);
	void GetUDCSettingIniPath();
	void UpdateSNByDate();
	UINT GetFocusStep();
	int  GetModeIndex();
	BOOL GetRecInitResult();
	BOOL LoadRecProcess();// 启动NY_DetectService.exe  进程版识别
	void CleanTestImages();
	void GetTestTypeHoleInfo();
	void Poweroff(int sec); // 关机
    void EnterDebugMode(bool on);
    bool IsInDebugMode() const;
    void SetQcType(BOOL arr[6]);
    TestDes GetNextQcType();

	// -------------implement FlowlineClientInterface-----------
	virtual void OnWsConnectEstablished(CWebSocket *session);
	virtual void OnWsPoweroffRequest();
    virtual void OnWsConsumablesRequest();
    virtual void OnWsTestStateRequest();
    virtual void OnWsChangeQcModelRequest(bool qc, int model, int item);

private:
    void CheckDbbackupAndcleanup();
    bool DoDbbackupAndcleanup();
};

extern CUWApp theApp;
