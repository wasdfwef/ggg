#pragma once

#include "MyListCtrl.h"
#include <algorithm>
#include <vector>
#include <map>

#include "UW.h"
#include "CriticalSection.h"
//#include "listctrlcomboex.h"
//#include "Include\MFCCtrl\MFCCtrl.h"
//#include "Include\Common\Node.h"
// CTodayList
#include "LockListCtrl.h"
#include "PropertiesWndRet.h"


#define WM_USER_ADDTASKTOQC WM_USER + 505
class CTodayList;

typedef struct _ThreadArgv
{
	int id;
	CTodayList* pParam;
	TASK_INFO info;
	CWnd* pMain;
	std::map<int, int> RBCSize;
}ThreadArgv;

using namespace std;

#define ID_INDEX     0

class CTodayList : public CDockablePane, public IConnectListener
{
	DECLARE_DYNAMIC(CTodayList)


	BOOL  m_BillMode;
	BOOL  m_AutoSwitchTab;
	int   GetIndexByTaskID(int nID);
	int   GetItemTaskID(int nIndex);
	int   GetItemStatus(int nIndex);
	int   AddTaskToList(TASK_INFO &task_info);
	int   AddTaskToList(int  nID, TASK_INFO &task_info);
	int   CellMapUsIndex(USHORT nCellType);
	LONG  IsNoramlTestType(int &nTestType, int &nItemIndex);
	BOOL  IsSaveCurRecord();
	BOOL  QueryBillMode();
	BOOL  IsSelectedCurTask();
    void  UpdateTaskToList(int nID);
	
	void  DelRecGrap(ULONG nTaskID);
	void  SetBillMode(BOOL bCheckMode);
	void  SetSaveCurRecord(BOOL  bSave);
	void  AnalyzePostivePic(int nGrapIndex, PCELL_FULL_INFO  cell_info, map<int, map<int, int>> &mapCells);
	void  CountCellResult(PCELL_FULL_INFO  cell_info, TASK_INFO & task_info, std::map<int, int> & RBCSize);
	void  CountCellResult(PCELL_FULL_INFO  cell_info, TASK_INFO & task_info, int nGrapIndex, vector<int> &v, map<int, USHORT> mapCellGrap);
	void  UpdateUSReslut(int nID, float cf,bool bSuccess = true);
	static DWORD PaintDRBCImageThread(ThreadArgv* pParam);
	void PaintDRBCImage(CDC* pDC,int id);
	void SaveDRBCImage(CString strPath);
	KSTATUS GetRecGrapPath(PVOID pCodePath, LPWSTR pRecGrapPath);
	void  UpdateUDCResult(PUDC_RESULT_CONTEXT  udc_ctx);
	void  GetGradeResult(int Index, int Value, TASK_INFO &taskInfo);
	void  ResetItemMaskCode(int nIndex, int nID, int nStatus);
	void  ShowStatus(int nIndex, int nSubItem, StatusEnum nStatus);
	void  ShowAttribute(int nIndex, int nAttribute);
	void  ShowSex(int nIndex, int nSex);
	void  SetListWndHook();
	void  DeleteRecord();
	void TestRecCircle();
	void  DeleteAllRecord();
	void  DeleteRecord(int nItem);
	void  PrintReport(BOOL bPrint);
	void  ChangePriTemp(int,char*);//1 udc 2 US 3 udc+us;
	void  SendLis();
	void  SendLis(int nItem);
	void  SendLis(int nItem,int sendType);
	void SendLisLisShuiXian(int nItem, int sendType);
	void  CmdResetType(int nResetType);
	void  ShowPrintStatus(int Index, int Status);
	void  ShowLisStatus(int Index, int Status);
	bool  SignalUDCisPostive(char*);
	void  AutoUpdateCheckStatus(BOOL bAuto = TRUE);
	void GetLogFileName(char *LogFilePath);
	void GetUDCLogData(PUDC_RESULT_CONTEXT udc_ctx, CStringA &LogData);
	void LogUDCResultToFile(PUDC_RESULT_CONTEXT udc_ctx);
	void UpdateNewTaskInfoUIByBarcode(int,TASK_INFO);
	void SetDefaultPrintGrap(ULONG nID, vector<int> &v, map<int, USHORT> mapCellGrap, int nGrapSize = 40);

	void DoSubItemEdit(int item, int subitem);

	LONG                m_CurSelIndex, m_SaveCurRecord;
	TASK_INFO           m_CurTaskInfo;
	CRITICAL_SECTION    m_TaskInfo_CS;
	bool				flagLock;
	

public:
	CTodayList();
	virtual ~CTodayList();
	int totalId = 0;//最新建id
	int todayFirstID = 0;//第一个新建id

	int curUSTaskId = 0;//已完成的镜检id
	int curUDCTaskId = 0;//已完成的干化id
	CEdit m_BarcodeEdit;
	LONG  GetOnlyOneSelected();
	LONG  GetCurSelIndex();
	BOOL  IsCurSelSameAppointID(LONG nID);
	void  UpdateBaseInfo(int taskId, std::map<string, string>& baseInfo);
	void  UpdateBarCode(PBARCODE_REQ_CONTEXT  barcode);
	void  ShowUSSampleProgress(int nID, int nPos);
	void  ShowUSRecognitionProgress(int nID, int nPos);
	void  UpdateFinishStatus(int nID, BOOL bUSTestType, BOOL bSuccess, PVOID pContext);
	void  UpdateFocusStatus(int nID, PVOID pContext);
	void  UpdateCurTaskInfo(TASK_INFO &task_info, TYPE_INFO type_info);
	void  UpdateUSValueOfCurTaskInfo(USHORT nCellType,float u_res);
	void  UpdateStatusToCache(ULONG nStatus);
	void  GetCurTaskInfo(TASK_INFO &task_info);
	void  AutoSaveCurRecord();
	void  UpdateCellList();
	void  UpdateRecordList();

	KSTATUS QueryQCInfo(char* pPath, PCELL_FULL_INFO pCellInfo);
	void UpdatePrintStatusOnUI(int TaskID);
	void UpdateLisStatusOnUI(int TaskID);

	
	void  UpdatePhysicsResult(PPHYSICS_RESULT_CONTEXT ctx);

	void UpdateCurTaskPrintImages(ULONG nTaskId, char* PrintPath);
public:
//	Common::Node            *m_pConfig;
	//MFCCtrl::CListCtrlEx     m_List;
	CFont                    m_Font;
	//CListCtrl                m_List;
	CLockListCtrl              m_List;
	int						curSelectID;
	int						m_nSelItem;
	int                      m_nlistNum;
	int						m_row;
	int						m_column;
	BOOL					m_bEscEdit;
	int						m_autoChangePrit;

protected:
	afx_msg void OnNMClick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRClick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDClick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEditKillFocus();
	void CountRBCSize(CRect rt, std::map<int, int> & RBCSize);
	afx_msg void OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCustomList(NMHDR* pNMHDR, LRESULT* pResult);
	void OnContextMenu(CWnd* /*pWnd*/, CPoint point);
	virtual BOOL IsAutoHideMode();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

    int  Test(int nPos, int nCha, int nTestType, BOOL bTaskResult, TASK_INFO &task_info);
    void SetStatusToFailed(int nID, StatusEnum nState);
    void FillList(void);

protected:
	// 初始化列表
	void InitList(void);
	void SimulationCell(void);
    
	// 新建记录
	//int New();

	// 删除记录
	void Delete(void);

	CString        m_strTestResult[UDC_MAXITEM];                 // 检测结果
	CString        m_strUnit[UDC_MAXITEM];                       // 物质含量
	bool           m_bAbnormity[UDC_MAXITEM];                    // 异常标记
	vector<int> PROId;//蛋白质为阴性的ID（东南医院蛋白质）

	afx_msg void OnUpdateOperate(CCmdUI *pCmdUI);
	afx_msg void OnOperate(UINT nID);


	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	afx_msg LRESULT OnNextBarcode(WPARAM w, LPARAM l);

private:
	bool IsSelectTaskNotTodayChecked();
	int GetRandomLEUValue(int UDCItemIndex);
	int MakeRandomUDCItemValue(int UDCItemIndex, int Level);
	void GetLevel(TCHAR *LevelDes, int UDCItemIndex, int &Level);
	bool IsNeedModifyTask(int TaskID);
	bool IsNormalTask(int TaskID);

	int GetModifiedLEUValue();
	int GetModifiedNITValue();
	int GetModifiedUROValue();
	int GetModifiedPROValue();
	int GetModifiedPHValue();
	int GetModifiedBLDValue();
	int GetModifiedSGValue();
	int GetModifiedKETValue();
	int GetModifiedBILValue();
	int GetModifiedGLUValue();
	int GetModifiedVCValue();
	int GetRandomUDCItemValue(int UDCItemIndex, TCHAR *LevelDes);
	int GetModifiedUDCValue(int UDCItemIndex);
	void ModifyUDCValue(USHORT *UDCValueList);

	bool GetGradeValue(int Index, TCHAR *Grade, int &MinValue, int &MaxValue);
	bool IsTheGrade(int Index, int Value, TCHAR *GradeDes);
	bool IsPositive1(int Index, int Value);
	bool IsPositive2(int Index, int Value);
	bool IsPositive3(int Index, int Value);
	bool IsPositive4(int Index, int Value);
	bool IsWeakPositive(int Index, int Value);
	bool IsNegative(int Index, int Value);

	// add by sy.hu at 2022.6.21
	bool GetGradeValue(int projIndex, int grade, int &MinValue, int &MaxValue); // 获取干化项目的对应等级的上下限值
	bool IsGreatGrade(int projIndex, int Value, int grade); // 判断干化项目是否大于该等级

	bool IsPositiveResultTask(TASK_INFO &TaskInfo); // 是否是阳性
	bool IsPositiveResultTask(int nIndex); // 是否是阳性

	void GetPositiveCellTypes(const TASK_INFO &TaskInfo, vector<int> &vec); //获取阳性细胞类型集合

	void JudgeUSResult(TASK_INFO);
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	virtual void OnSuccess(unsigned int s, unsigned long taskID, char *data, unsigned short len, void *userdata);
	virtual void OnFail(unsigned long taskID, char *data, unsigned short len, int nOperationType, void *userdata);
	afx_msg void OnOperateGetinfoFromLis();
	afx_msg void OnOperate();
};



