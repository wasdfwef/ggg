#pragma once

#import "C:\\Program Files\\Common Files\\System\\ado\\msado15.dll" rename("EOF", "adoEOF") rename("BOF", "adoBOF")
#include "..\..\inc\f_err.h"
#include <string>

using namespace std;


#define _ACCESS_EXPORTS

#ifdef _ACCESS_EXPORTS
    #define ACCESS_API extern "C" __declspec(dllexport)  
#else
	#define ACCESS_API extern "C" __declspec(dllimport)  
#endif




#define CONFIG_INI_FILENAME         "conf.ini"
#define CONFIG_INI_US               "th_us"
#define CONFIG_INI_US_COUNTS        "counts"
#define CONFIG_INI_UDC              "th_udc"
#define CONFIG_INI_UDC_COUNTS       "counts"

#define CONFIG_INI_INDEX            "nIndex"
#define CONFIG_INI_MAX_NORMAL_MALE  "nMax_normal_male"
#define CONFIG_INI_MIN_NORMAL_MALE  "nMin_normal_male"
#define CONFIG_INI_MAX_NORMAL_FEMALE "nMax_normal_female"
#define CONFIG_INI_MIN_NORMAL_FEMALE "nMin_normal_female"

#define CONFIG_INI_MAX_UNUSUAL_MALE "nMax_unusual_male"
#define CONFIG_INI_MIN_UNUSUAL_MALE "nMin_unusual_male"
#define CONFIG_INI_MAX_UNUSUAL_FEMALE "nMax_unusual_female"
#define CONFIG_INI_MIN_UNUSUAL_FEMALE "nMin_unusual_female"

#define CONFIG_INI_ISTOLIS    "bIsSendtoLIS"
#define CONFIG_INI_ISSHOWUI   "bIsShowUI"
#define CONFIG_INI_ISSENDTOREPORT "bIsSendtoReport"
#define CONFIG_INI_ISDELETE   "Delete"

#define CONFIG_INI_ARGUMENT   "fArgument"
#define CONFIG_INI_SHORTNAME  "sShortName"
#define CONFIG_INI_LONGNAME   "sLongName"
#define CONFIG_INI_UNIT       "nUnit"

#define CONFIG_INI_CELLNAMECOLOR_R "sCellNameColoer_R"
#define CONFIG_INI_CELLNAMECOLOR_G "sCellNameColoer_G"
#define CONFIG_INI_CELLNAMECOLOR_B "sCellNameColoer_B"

#define CONFIG_INI_SHOWCELLFRAME "showframe"

#define MAX_CR300_DUS_DATE_LENS   64
#define MAX_CR300_RES_COUNTS     16
#define FRAME_MAX_LENGTH          256
#define INVAILD_VALUE             -1

#define MAX_DATA_LENGTH       64
#define MAX_SQL_LENS              1024
#define MAX_USER_NAME_LENS        32
#define MAX_PASSWORD_LENS         32
#define MAX_US_COUNTS             45
#define MAX_UDC_COUNTS            16




#define MAX_BARCODE_LENS       255
#define MAX_COMPANY_LENS       255
#define MAX_DEPARTMENT_LENS    255
#define MAX_DOCTOR_LENS        255
#define MAX_LABOR_LENS         255
#define MAX_AUDITOR_LENS       255
#define MAX_REPORTER_LENS      32
#define MAX_IMAGEFOLDER_LENS   255
#define MAX_PRINTIMAGE_LENS    255
#define MAX_DIAGNOSIS_LENS     32
#define MAX_COMMENT_LENS       255
#define MAX_NAME_LENS          32
#define MAX_SEX_LENS           32
#define MAX_AGE_UNIT_LENS      32
#define MAX_SICKBED_LENS       255
#define MAX_DATA_LENS          64
#define MAX_TIME_LENS          64
#define MAX_UDC_SN_LENS        64
#define MAX_UDC_RES_LENS       64
#define MAX_US_LENS            47
#define MAX_UDC_LENS           16

#define MAX_PAT_LENS           255

#define MAX_UDC_RESULT_LENS    64

#define MAX_UDC_DATA_LENGTH 64


#pragma pack(1)

typedef enum tagOpenTypeEnum 
{
	OpenAccess2000 = 1,
	OpenAccess2007 = 2

}OpenTypeEnum;

typedef enum tagUS_UNIT
{
	nomal_ul = 1,
	hpf = 2,
	lpf = 3

}US_UNIT;



typedef struct _OPEN_ACCESS_FILE_PAR
{
	OpenTypeEnum        nOpen_Type;
	char                csAccess_file_full_path[MAX_PATH];  
	char                csAccess_file_user[MAX_PATH];
	char                csAccess_file_password[MAX_PATH];
	long                nOptions;
	char                reserved[32];

}OPEN_ACCESS_FILE_PAR, *POPEN_ACCESS_FILE_PAR;


typedef struct _OPEN_RS_PAR
{
	char     sql[MAX_SQL_LENS];
	ADODB::_RecordsetPtr	Rs;

}OPEN_RS_PAR,*POPEN_RS_PAR;


typedef struct _EXECUTE_PAR
{
	char     sql[MAX_SQL_LENS];
	LONG     nOptions;

}EXECUTE_PAR,*PEXECUTE_PAR;




//用户类型
typedef enum USERTYPE
{
	USER_TOKEN_USER = 0, 
	USER_TOKEN_ADMINISTRATOR = 4
}USERTYPE;
// 用户登录
typedef struct _USER_LOGIN
{
	char    sUser[MAX_USER_NAME_LENS];
	char    sPassword[MAX_PASSWORD_LENS];

}USER_LOGIN,*PUSER_LOGIN;

// 检测用户是否存在
typedef struct _CHECK_ACCOUNT
{
	char    sAccount[MAX_USER_NAME_LENS];

}CHECK_ACCOUNT,*PCHECK_ACCOUNT;


// 返回用户信息
typedef struct _USER_INFO
{
	char sAccount[MAX_USER_NAME_LENS];
	char sPosition[MAX_USER_NAME_LENS];
	char sName[MAX_USER_NAME_LENS];
	char sDept[MAX_PATH];
	char dtDate[MAX_DATA_LENS];
	LONG Group;

}USER_INFO,*PUSER_INFO;

typedef struct _USER_LIST_INFO1
{
	ULONG         nUserCounter;
	PUSER_INFO   pUserInfo;

}USER_LIST_INFO1, *PUSER_LIST_INFO1;

// 添加用户
typedef  struct _USER_ADD
{
	char sAccount[MAX_USER_NAME_LENS];
	char sPassword[MAX_PASSWORD_LENS];
	char sDept[MAX_PATH];
	/*char dtDate[MAX_DATA_LENS];*/
	char sPosition[MAX_USER_NAME_LENS];
	char sName[MAX_USER_NAME_LENS];

}USER_ADD,*PUSER_ADD;

// 删除用户
typedef struct _USER_DEL
{
	char    sUser[MAX_USER_NAME_LENS];
	LONG    Group;   // 0 普通用户， 4 管理员帐户

}USER_DEL,*PUSER_DEL;


//用户密码
typedef struct _USER_UPC
{
	char   UserName[128];
	char   OldPsd[64];
	char   NewPsd[64];

}USER_UPC, *PUSER_UPC;


typedef struct _PAR
{
	unsigned int  nIndex;

	int          nMax_normal_male;
	int          nMin_normal_male;
	int          nMax_normal_female;
	int          nMin_normal_female;


	int          nMax_unusual_male;
	int          nMin_unusual_male;
	int          nMax_unusual_female;
	int          nMin_unusual_female;

	bool         bIsSendtoLIS;
	bool         bIsShowUI;
	bool         bIsSendtoReport;
	bool         bIsDelete;

	float        fArgument; // ul->hpf

	char         sShortName[MAX_PATH];
	char         sLongName[MAX_PATH];

	int          nUnit;
	/*COLORREF     m_CellNameColoer;*/ 
	char          m_CellNameColoer_R[MAX_PATH];
	char          m_CellNameColoer_G[MAX_PATH];
	char          m_CellNameColoer_B[MAX_PATH];
	

}PAR,*PPAR;


typedef struct _GET_US_INFO
{
	bool         bIsShowFrame;
	unsigned int nUsCounts;
	PAR          par[MAX_US_COUNTS];

}GET_US_INFO,*PGET_US_INFO;


typedef struct _GET_UDC_INFO
{
	unsigned int nUdcCounts;
	PAR          par[MAX_UDC_COUNTS];

}GET_UDC_INFO,*PGET_UDC_INFO;


typedef struct _SET_US_INFO
{
	bool         bIsShowFrame;
	unsigned int nUsCounts;
	PAR       par[MAX_US_COUNTS];

}SET_US_INFO,*PSET_US_INFO;


typedef struct _SET_UDC_INFO
{
	unsigned int nUdcCounts;
	PAR       par[MAX_UDC_COUNTS];

}SET_UDC_INFO,*PSET_UDC_INFO;


typedef enum TestType
{
	US_TEST_TYPE = 1,  //us
	UDC_TEST_TYPE =2,  //udc
	ALL_TEST_TYPE =3   //all
}TestTypeEnum;

typedef enum USQCTYPE
{
	type_p = 1,  //阳性
	type_n =2	//阴性
}UsQcEnum;

typedef enum UDCQCTYPE
{
	type1 = 1,  //1号
	type2 = 2,	//2
	type3 = 3
}UdsQcEnum;

typedef enum QCTYPE
{
	US_QC_TYPE = 1,  //1号
	UDC_QC_TYPE = 2  //2
	
}QCTypeEnum;

typedef enum STATUSTYPE
{
	NEW       = 1,     //新建
	TEST      = 2,	   //检测
	PRO       = 3,     //识别
	TESTFAIL  = 4,     //测试失败
	FINISH    = 5,     //完成
	CHECKED   = 6,     //已审核	
	FAIL      = 7      //吸样失败


}StatusEnum;

typedef struct _TASK_ADD
{
	unsigned int nID;       // 添加成功返回有效值，识别返回-1
	StatusEnum   nStatus;   //吸样状态
	int          nRow;      //架号
	int          nPos;      //孔号
	//int          nSN;       //顺序号
	int          nNo;       //顺序号
	int          nCha;      //通道号
	TestTypeEnum nTestType; //测试类型
	bool         bIsQC;     //是否为质控任务
	QCTypeEnum   nQcType;   //质控类型
	UsQcEnum     nQcUsType; //us质控类型
	UdsQcEnum    nQcUdcType;//udc质控类型
}TASK_ADD,*PTASK_ADD;



typedef struct _SET_PIC_PATH_
{   
    unsigned int nID;
	char sImageFolder[MAX_PATH];  // 保存的图片路径
}SET_PIC_PATH,*PSET_PIC_PATH;

typedef struct _US_NODE
{
	bool bIsShowUi;
	bool bIsSendtoLIS;
	bool bIsReport;

	int  nMax_normal_male;
	int  nMin_normal_male;
	int  nMax_normal_female;
	int  nMin_normal_female;


	int  nMax_unusual_male;
	int  nMin_unusual_male;
	int  nMax_unusual_female;
	int  nMin_unusual_female;

	//int us_res;
	float us_res;

}US_NODE,*PUS_NODE;



typedef struct _US_INFO
{
	int nID;
	int bUs;
	int nVariableRatio;
	US_NODE us_node[MAX_US_LENS];

}US_INFO,*PUS_INFO;



typedef struct _UDC_NODE
{
	bool bIsShowUi;
	bool bIsSendtoLIS;
	bool bIsReport;

	char udc_res[MAX_UDC_RESULT_LENS];

}UDC_NODE,*PUDC_NODE;



typedef struct _UDC_INFO
{
	int  nID;
	int  bUdc;
	UDC_NODE udc_node[MAX_UDC_LENS];

}UDC_INFO,*PUDC_INFO;



typedef struct _MAIN_INFO
{
	int nID;
	int nAlarm;
	int nRow;
	int bMain;
	int nState;
	int nAttribute;
	int nTestType;
	int nSN;
	int nPos;
	int nCha;

	char sCode[MAX_BARCODE_LENS];
	char sCompany[MAX_COMPANY_LENS];
	char sDepartment[MAX_DEPARTMENT_LENS];
	char sDoctor[MAX_DOCTOR_LENS];
	char sDocimaster[MAX_DOCTOR_LENS];
	char sAuditor[MAX_DOCTOR_LENS];
	char sReporter[MAX_DOCTOR_LENS];
	char sColor[MAX_PATH];
	char sTransparency[MAX_PATH];
	char sFolder[MAX_IMAGEFOLDER_LENS];
	char sImage[MAX_PRINTIMAGE_LENS];
	char sDiagnosis[MAX_DIAGNOSIS_LENS];
	char sSuggestion[MAX_COMMENT_LENS];
	char dtDate[MAX_DATA_LENS];
	int  dtPrintStatus;
	int  dtLisStatus;
	int  nQcType;
	int  nQcUsType;
	int  nQcUdcType;
}MAIN_INFO,*PMAIN_INFO;


//病人信息
typedef struct _PAT_INFO
{
	int nID;
	int bPat;
	int nNo;
	char sName[MAX_PATH];
	int nSex;
	int nAge;
	int nAgeUnit;
	char sHospital[MAX_COMPANY_LENS];
	char sSickBed[MAX_SICKBED_LENS];
	char dtBirthDate[MAX_DATA_LENS];	
}PAT_INFO,*PPAT_INFO;


typedef struct _TASK_INFO
{
	MAIN_INFO main_info;
	US_INFO us_info;
	UDC_INFO udc_info;
	PAT_INFO pat_info;

}TASK_INFO,*PTASK_INFO;



typedef struct _SEARCH_CONDITIONS
{

	int nSN;
	int nID_From;
	int nID_To;
	int nStatus;

	int nQcType;
	int nUsQcType;
	int nUdcQcType;
	

	bool bIsSearch_completed;
	bool bIsSearch_uncomplete;

	char sReporter[MAX_AUDITOR_LENS];
	char sBarcode[MAX_BARCODE_LENS];
	char sName[MAX_NAME_LENS];
	char sSickBed[MAX_SICKBED_LENS];
	char sCompany[MAX_COMPANY_LENS];
	char sDepartment[MAX_DEPARTMENT_LENS];
	char sDoctor[MAX_DOCTOR_LENS];

	char sDate_From[MAX_DATA_LENS];
	char sDate_To[MAX_DATA_LENS];

	char sTime_From[MAX_TIME_LENS];
	char sTime_To[MAX_TIME_LENS];

}SEARCH_CONDITIONS,*PSEARCH_CONDITIONS;



typedef struct _SEARCH_TASK
{
	SEARCH_CONDITIONS conditions;
	TASK_INFO* pSearchInfo;
	unsigned int pSearchInfoCounts;
	bool bIsSearch_QC_Task;

}SEARCH_TASK,*PSEARCH_TASK;

typedef struct _SENDTO_LIS_ID
{
	int SendtoLisID;    // 需要传LIS的ID
	TASK_INFO LisInfo;  // 返回需要传LIS的结构数组
}SENDTO_LIS_ID,*PSENDTO_LIS_ID;

typedef enum report_mask
{
	REPORT_LIS_MASK   = 1,  //LIS
	REPORT_PRINT_MASK = 2   //Print

}Report_Mask;

typedef struct _REPORT_STATUS1
{
	ULONG   nTaskID;
	Report_Mask   report_mask;   //标志LIS或print
	BOOLEAN bPrint;
	BOOLEAN bLis;

}REPORT_STATUS1, *PREPORT_STATUS1;




typedef enum INFOTYPE
{
	USER_TYPE_INFO = 1,
	US_TYPE_INFO   = 2,
	UDC_TYPE_INFO  = 3,
	ALL_TYPE_INFO  = 4
}TYPE_INFO;

typedef struct _CONFIG_RECORD_INFO
{
	USHORT          nRecordType;   //US_TYPE_INFO,UDC_TYPE_INFO
	PVOID           pConfigConent; //GET_US_INFO ,GET_UDC_INFO
	ULONG           nConfigLength; //sizeof(GET_US_INFO),sizeof(GET_UDC_INFO)

}CONFIG_RECORD_INFO, *PCONFIG_RECORD_INFO;

typedef  struct _SAMPLING_GRAP_PATH
{
	ULONG  nTaskID;
	char   RelativePath[MAX_PATH];
	char   Reserved[32];

}SAMPLING_GRAP_PATH, *PSAMPLING_GRAP_PATH;

typedef struct _UPDATE_PRINT_PATH1
{
	ULONG   nTaskID;
	char    PrintPath[MAX_PATH];

}UPDATE_PRINT_PATH1, *PUPDATE_PRINT_PATH1;



//typedef struct _FULL_TASK_INFO1
//{
//	TYPE_INFO    TypeInfo;
//	MAIN_INFO   MainInfo1;
//	PAT_INFO    pat;
//	US_INFO     us1;
//	UDC_INFO    udc1;
//
//}FULL_TASK_INFO1, *PFULL_TASK_INFO1;


typedef struct _TOADY_TASK_INFO1
{
	ULONG               nToadyTaskCounter;
	PTASK_INFO          pFullTaskInfo;

}TOADY_TASK_INFO1, *PTOADY_TASK_INFO1;


typedef struct _QC_INFO
{
	US_INFO     us1;
	UDC_INFO    udc1;
}QC_INFO, *PQC_INFO;

typedef struct _QC_TASK_INFO
{
	ULONG             nQcTaskCounter;
	PQC_INFO          pQcTaskInfo;

}QC_TASK_INFO, *PQC_TASK_INFO;

typedef struct _RESET_RECORD_INFO
{
	ULONG               nTaskID;
	
}RESET_RECORD_INFO, *PRESET_RECORD_INFO;

//typedef struct _SEARCH_CONDITIONS_1
//{
//
//	int nSN;
//	int nID_From;
//	int nID_To;
//	int nQcType;
//	int nStatus;
//
//	bool bIsSearch_completed;
//	bool bIsSearch_uncomplete;
//
//	char sReporter[255];
//	char sBarcode[255];
//	char sName[32];
//	char sSickBed[255];
//	char sCompany[255];
//	char sDepartment[255];
//	char sDoctor[255];
//
//	char sDate_From[64];
//	char sDate_To[64];
//
//	char sTime_From[64];
//	char sTime_To[64];
//
//}SEARCH_CONDITIONS_1,*PSEARCH_CONDITIONS_1;

//typedef struct _SEARCH_TASK1
//{
//	SEARCH_CONDITIONS conditions;
//	PTASK_INFO  pSearchInfo;
//	unsigned int       pSearchInfoCounts;
//	bool bIsSearch_QC_Task;
//
//}SEARCH_TASK1,*PSEARCH_TASK1;

#define  DELETE_ALL_TASK_ITEM  0XFFFFFFFF

typedef struct _DEL_TASK
{
	PULONG    pDelIDArray;  //如果该值为空
	ULONG     nCounter;     //且该值为DELETE_ALL_TASK_ITEM,则删除所有任务
	BOOL      bIsQC;
	char      Reserved[32];

}DEL_TASK, *PDEL_TASK;



typedef struct _LIS_SET_INFO
{
	ULONG  nTaskID;
	TASK_INFO TaskInfo;

}LIS_SET_INFO, *PLIS_SET_INFO;

#pragma pack()
