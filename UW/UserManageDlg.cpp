// UserManageDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "UW.h"
#include "UserManageDlg.h"
#include "UserInfoDlg.h"
#include "Include/Common/String.h"
#include "UserPasswordDlg.h"

// CUserManageDlg 对话框

extern 
std::wstring 
GetIdsString(const std::wstring &sMain,const std::wstring &sId);

IMPLEMENT_DYNAMIC(CUserManageDlg, CDialog)

using namespace Common;

CUserManageDlg::CUserManageDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUserManageDlg::IDD, pParent)
	, m_nDept(0)
{

}

CUserManageDlg::~CUserManageDlg()
{
}

void CUserManageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_USERDEP_LIST, m_depList);
	DDX_Control(pDX, IDC_USERNAME_LIST, m_nameList);
	DDX_Control(pDX, IDC_USERMANAGE_NEW, m_addBtn);
	DDX_Control(pDX, IDC_USERMANAGE_DEL, m_delBtn);
	DDX_Control(pDX, IDC_USERMANAGE_MODIFY, m_editBtn);
	DDX_Control(pDX, IDC_BUTTON_PWD, m_modifyPwdBtn);
}


BEGIN_MESSAGE_MAP(CUserManageDlg, CDialog)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_USERDEP_LIST, &CUserManageDlg::OnLvnItemchangedUserdepList)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_USERNAME_LIST, &CUserManageDlg::OnLvnItemchangedUsernameList)
	ON_BN_CLICKED(IDC_USERMANAGE_NEW, &CUserManageDlg::OnBnClickedUsermanageNew)
	ON_BN_CLICKED(IDC_USERMANAGE_DEL, &CUserManageDlg::OnBnClickedUsermanageDel)
	ON_BN_CLICKED(IDC_USERMANAGE_MODIFY, &CUserManageDlg::OnBnClickedUsermanageModify)
	ON_BN_CLICKED(IDC_USERMANAGE_FINISH, &CUserManageDlg::OnBnClickedUsermanageFinish)
	ON_BN_CLICKED(IDC_BUTTON_PWD, &CUserManageDlg::OnBnClickedButtonPwd)
END_MESSAGE_MAP()


// CUserManageDlg 消息处理程序

BOOL CUserManageDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowText(GetIdsString(_T("usermanage"),_T("title")).c_str());

	Access_GetDeptAndPosInfo(&dep,&pos);
	initDepList();

	initNameList();
	// TODO:  在此添加额外的初始化
	bool hasAdminRight = (theApp.loginGroup == 111 || theApp.loginGroup == 4);
	if (!hasAdminRight) m_addBtn.EnableWindow(FALSE);
	m_delBtn.EnableWindow(FALSE);
	m_editBtn.EnableWindow(FALSE);
	m_modifyPwdBtn.EnableWindow(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CUserManageDlg::initDepList()
{
	// 部门列表
	//m_depList.CreateSmallImage(1, 18, ILC_COLOR24, 0, 1);

	
	m_depList.InsertColumn(0,GetIdsString(_T("usermanage"),_T("dep")).c_str(),0,100);
	for (int i=0;i<dep.nCount;++i)
	{
		
		wstring sdep = Common::CharToWChar(dep.pDept[i].name);
	     m_depList.InsertItem(i,sdep.c_str());
	}
	m_depList.ModifyStyle(LVS_TYPEMASK, LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS | LVS_NOSORTHEADER);
	m_depList.SetExtendedStyle(LVS_EX_HEADERDRAGDROP | LVS_EX_FULLROWSELECT);
	m_depList.SetExtendedStyle(m_depList.GetExtendedStyle()| LVS_EX_GRIDLINES);
}

void CUserManageDlg::initNameList()
{
	// 用户列表
	//m_ListUser.CreateSmallImage(1, 18, ILC_COLOR24, 0, 1);
	
	m_nameList.InsertColumn(0,GetIdsString(_T("usermanage"),_T("name")).c_str(),0,100);
	m_nameList.InsertColumn(1,GetIdsString(_T("usermanage"),_T("position")).c_str(),0,100);
	m_nameList.InsertColumn(2,_T("account"),0,0);
	m_nameList.InsertColumn(3,_T("group"),0,0);
	m_nameList.ModifyStyle(LVS_TYPEMASK, LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS | LVS_NOSORTHEADER);
	m_nameList.SetExtendedStyle(LVS_EX_HEADERDRAGDROP | LVS_EX_FULLROWSELECT);
	m_nameList.SetExtendedStyle(m_nameList.GetExtendedStyle()| LVS_EX_GRIDLINES);

}

void CUserManageDlg::OnLvnItemchangedUserdepList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	bool hasAdminRight = (theApp.loginGroup == 111 || theApp.loginGroup == 4);
	
	// TODO: 在此添加控件通知处理程序代码
	if (pNMLV->uChanged & LVIF_STATE)
	{
		if ((pNMLV->uOldState & LVIS_SELECTED) == 0 && (pNMLV->uNewState & LVIS_SELECTED))
		{
			m_nDept = pNMLV->iItem;
			m_nUser = 0;
			CString dep=m_depList.GetItemText(m_nDept,0);
			if (dep==_T(""))
			{
				//AfxMessageBox((const TCHAR*) m_ListUser.m_Error);
			}
			else
			{
				// 填充用户
				 FillUser(dep.GetBuffer());
				// m_nUser= theApp.m_userlist.nUserCounter;
				 // 按钮状态
				 // 权限控制，普通用户只允许编辑自己的资料和修改自己的密码
				 m_delBtn.EnableWindow(FALSE);
				 m_editBtn.EnableWindow(FALSE);
				 m_modifyPwdBtn.EnableWindow(FALSE);
			}
		}
		else if ((pNMLV->uOldState & LVIS_SELECTED) && (pNMLV->uNewState & LVIS_SELECTED) == 0)
		{
			m_nDept = -1;
			m_nUser = -1;
			// 清空列表
			m_nameList.DeleteAllItems();
			// 按钮状态
			m_delBtn.EnableWindow(FALSE);
			m_editBtn.EnableWindow(FALSE);
			m_modifyPwdBtn.EnableWindow(FALSE);
		}
	}
	*pResult = 0;
}

void CUserManageDlg::OnLvnItemchangedUsernameList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码

	bool hasAdminRight = (theApp.loginGroup == 111 || theApp.loginGroup == 4);

	if (pNMLV->uChanged & LVIF_STATE)
	{
		if ((pNMLV->uOldState & LVIS_SELECTED) == 0 && (pNMLV->uNewState & LVIS_SELECTED))
		{
			m_nUser = pNMLV->iItem;
			//// 移动指针
			////if (!m_ListUser.Move(m_nUser))
			////{
			////	AfxMessageBox((const TCHAR*) m_ListUser.m_Error);
			////}
			// 按钮状态
			// 权限控制，普通用户只允许编辑自己的资料和修改自己的密码
			m_delBtn.EnableWindow(hasAdminRight);
			m_editBtn.EnableWindow(hasAdminRight);
			m_modifyPwdBtn.EnableWindow(hasAdminRight);
			if (!hasAdminRight && m_nameList.GetItemText(m_nUser, 2) == Common::CharToWChar(theApp.loginUser.sAccount).c_str())
			{
				m_delBtn.EnableWindow(FALSE);
				m_editBtn.EnableWindow(TRUE);
				m_modifyPwdBtn.EnableWindow(TRUE);
			}
		}
		else if ((pNMLV->uOldState & LVIS_SELECTED) && (pNMLV->uNewState & LVIS_SELECTED) == 0)
		{
			m_nUser = -1;
			// 按钮状态
			// 权限控制，普通用户只允许编辑自己的资料和修改自己的密码
			m_delBtn.EnableWindow(FALSE);
			m_editBtn.EnableWindow(FALSE);
			m_modifyPwdBtn.EnableWindow(FALSE);
		}
	}
	*pResult = 0;
}

void CUserManageDlg::FillUser(TCHAR *dep)
{
	CString strGroup;
    for (int i = 0;i<theApp.m_nUserNum;++i)
    {
		int j = 0;
		if (dep == Common::CharToWChar(theApp.m_userlist.pUserInfo[i].sDept))
		{
            
			m_nameList.InsertItem(j,_T(""));
			m_nameList.SetItemText(j,0,Common::CharToWChar(theApp.m_userlist.pUserInfo[i].sName).c_str());
			m_nameList.SetItemText(j,1,Common::CharToWChar(theApp.m_userlist.pUserInfo[i].sPosition).c_str());
			m_nameList.SetItemText(j,2,Common::CharToWChar(theApp.m_userlist.pUserInfo[i].sAccount).c_str());
			strGroup.Format(_T("%d"),theApp.m_userlist.pUserInfo[i].Group);
			m_nameList.SetItemText(j,3,strGroup);
            j++;
		}
    }
}

void CUserManageDlg::FillDept( int nIndex)
{
	
	// 禁止刷新
	m_depList.SetRedraw(FALSE);
	// 清空数据
	m_depList.DeleteAllItems();
 
	int nDept = 0;

	for (int i=0;i<dep.nCount;++i)
	{
		wstring sdep = Common::CharToWChar(dep.pDept[i].name);
		m_depList.InsertItem(i,sdep.c_str());
	}
		nDept = m_depList.GetItemCount();
		
		// 选择行
		if (nIndex >= 0 && nIndex < nDept)
		{
			m_depList.SetItemState(nIndex, LVIS_SELECTED, LVIS_SELECTED);
		}

	// 允许刷新
	m_depList.SetRedraw(TRUE);
}

void CUserManageDlg::OnBnClickedUsermanageNew()
{
	// TODO: 在此添加控件通知处理程序代码
	CUserInfoDlg UserInfoDlg;
	
	// 默认部门
	if (m_nDept != -1)
	{
		UserInfoDlg.m_sDep = m_depList.GetItemText(m_nDept, 0);
	}
	// 默认组 "用户"
	//UserInfoDlg.m_nGroup = 1; //bug
	 UserInfoDlg.m_nGroup = 0;

	// 新增用户
	UserInfoDlg.m_bNew = TRUE;
	if (UserInfoDlg.DoModal() == IDOK)
	{
		//// 编辑用户
		//if (!UserEdit(&UserEditDlg))
		//{
		//	AfxMessageBox((const TCHAR*) m_Error);
		//}
  //      switch(UserInfoDlg.AddUser())
		//{
		//case STATUS_SUCCESS:
		//	AfxMessageBox(GetIdsString(_T("userinfo"),_T("message4")).c_str());
		//	break;
		//case STATUS_LOGIN_USER_EXIST:
		//	AfxMessageBox(GetIdsString(_T("userinfo"),_T("message5")).c_str());
		//	break;
		//}
		
		Access_FreeGetUserListBuf(theApp.m_userlist);
        Access_GetUserList(&theApp.m_userlist);
		theApp.m_nUserNum = theApp.m_userlist.nUserCounter;
		Access_FreeGetDeptAndPosInfoBuf(dep, pos);
		Access_GetDeptAndPosInfo(&dep,&pos);
		// 清空部门
		m_depList.DeleteAllItems();
		// 清空用户
		m_nameList.DeleteAllItems();
		// 填充部门
		FillDept(m_nDept);
		m_depList.Invalidate();
		m_nameList.Invalidate();
	}
}


#define ADM_GROUP 4

void CUserManageDlg::OnBnClickedUsermanageDel()
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_nameList.GetSelectedCount() == 0)
	{
		MessageBox(_T("请选择需要删除的用户"), _T("提示"), MB_OK);
		return;
	}
	// 确认
	if (MessageBox(GetIdsString(_T("usermanage"),_T("message")).c_str(), NULL, MB_YESNO | MB_ICONQUESTION) == IDYES)
	{
		CString accountName = m_nameList.GetItemText(m_nUser,2);
		CString strGroup = m_nameList.GetItemText(m_nUser,3);

		USER_DEL  userinfo ={0};
		strcpy(userinfo.sUser,Common::WCharToChar(accountName.GetBuffer()).c_str());
		userinfo.Group = _ttoi(strGroup);
		if (accountName == _T("Adm"))
		{
			AfxMessageBox(GetIdsString(_T("usermanage"),_T("message4")).c_str());
			return;
		}

		USER_LIST_INFO1 UserInfoList;
		KSTATUS Result = Access_GetUserList(&UserInfoList);
		if (Result != STATUS_SUCCESS)
		{
			AfxMessageBox(GetIdsString(_T("usermanage"),_T("message1")).c_str());
			return;
		}

		for (int i = 0; i < UserInfoList.nUserCounter; ++i)
		{
			if ( wcsicmp(CString(UserInfoList.pUserInfo[i].sAccount), accountName) != 0)
				continue;

			if (UserInfoList.pUserInfo[i].Group == ADM_GROUP)    
			{
				AfxMessageBox(GetIdsString(_T("usermanage"),_T("cannotdeladm")).c_str());
				return;
			}
			else
			{
				break;
			}
		}
		if (Result == STATUS_SUCCESS)
		{
			Access_FreeGetUserListBuf(UserInfoList);
		}

		Result = Access_DelUser(&userinfo);
		switch (Result)
		{
		case STATUS_SUCCESS:
			Access_FreeGetUserListBuf(theApp.m_userlist);
			Access_GetUserList(&theApp.m_userlist);
			theApp.m_nUserNum = theApp.m_userlist.nUserCounter;
			Access_FreeGetDeptAndPosInfoBuf(dep, pos);
			Access_GetDeptAndPosInfo(&dep,&pos);
			m_depList.DeleteAllItems();
			m_nameList.DeleteAllItems();
			FillDept(m_nDept);
			break;
		case STATUS_INVALID_PARAMETERS:
			AfxMessageBox(GetIdsString(_T("usermanage"),_T("message1")).c_str());
			break;
		}

	}
}

void CUserManageDlg::OnBnClickedUsermanageModify()
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_nameList.GetSelectedCount() == 0)
	{
		MessageBox(_T("请选择需要编辑的用户"), _T("提示"), MB_OK);
		return;
	}

	if (m_nUser != -1)
	{
		CUserInfoDlg UserInfoDlg;
		UserInfoDlg.m_bNew = FALSE;
		UserInfoDlg.strAccount=m_nameList.GetItemText(m_nUser,2);

		if( wcsicmp(UserInfoDlg.strAccount.GetBuffer(), _T("Adm")) == 0 )
		{
			AfxMessageBox(GetIdsString(_T("usermanage"),_T("message5")).c_str());
			return;
		}

		if (UserInfoDlg.DoModal() == IDOK)
		{
			Access_FreeGetUserListBuf(theApp.m_userlist);
			Access_GetUserList(&theApp.m_userlist);
			theApp.m_nUserNum = theApp.m_userlist.nUserCounter;
			Access_FreeGetDeptAndPosInfoBuf(dep, pos);
			Access_GetDeptAndPosInfo(&dep,&pos);
			// 清空部门
			m_depList.DeleteAllItems();
			// 清空用户
			m_nameList.DeleteAllItems();
			// 填充部门
			FillDept(m_nDept);
		}
	}
	
}

void CUserManageDlg::OnBnClickedUsermanageFinish()
{
	// TODO: 在此添加控件通知处理程序代码
	OnOK();
}

void CUserManageDlg::OnBnClickedButtonPwd()
{
	// TODO:  在此添加控件通知处理程序代码
	if (m_nUser != -1)
	{
		CUserPasswordDlg pdlg;
		pdlg.m_sName = m_nameList.GetItemText(m_nUser, 0);
		pdlg.m_sAccount = m_nameList.GetItemText(m_nUser, 2);
		if (wcsicmp(pdlg.m_csAccount.GetBuffer(), _T("Adm")) == 0)
		{
			AfxMessageBox(GetIdsString(_T("usermanage"), _T("message5")).c_str());
			return;
		}
		pdlg.DoModal();
	}

}
