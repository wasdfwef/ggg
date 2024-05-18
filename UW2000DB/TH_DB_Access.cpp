#include "StdAfx.h"
#include "TH_DB_Access.h"
#include "IniParser.h"
#include <Shlwapi.h>
#include "strsafe.h"
TH_DB_Access::TH_DB_Access(void)
{
	

	/*if(bInitialize)
		CoInitializeEx(NULL ,COINIT_MULTITHREADED);*/

	bInitialize = TRUE;
	m_Connection = NULL;

	memset(m_nConfig_IniFile,0,sizeof(m_nConfig_IniFile));
	GetConfigIniFilePath();
}

TH_DB_Access::~TH_DB_Access(void)
{
	Close_Connect();
	CoUninitialize();
}

void TH_DB_Access::Close_Connect( void )
{
	if(m_Connection == NULL)
	{
		return ;
	}

	try
	{
		if (m_Connection->State & ADODB::adStateOpen)
		{
			m_Connection->Close();
			m_Connection.Release();
			//m_Connection = NULL;
		}
	}
	catch(_com_error e)
	{
		DBG_MSG("Close _com_error ,errMessage = %s", (const char *)(LPCTSTR)e.Description());
	}

}

KSTATUS TH_DB_Access::Open_Connect(OPEN_ACCESS_FILE_PAR open_access_file_par)
{

	DBG_MSG("bInitialize = %d\n",bInitialize);
	if(bInitialize)
		CoInitializeEx(NULL ,COINIT_MULTITHREADED);

	KSTATUS status = STATUS_SUCCESS;
	HRESULT hr = S_OK;
	char szConnectingString[MAX_PATH]={0};

	if(strlen(open_access_file_par.csAccess_file_full_path) <=0)
	{
		status = STATUS_INVALID_PARAMETERS;
		return status;
	}

	if(open_access_file_par.nOpen_Type == OpenAccess2000)
	{
		//DBG_MSG("");
		sprintf(szConnectingString, "%s", OPEN_ACCESS2000_CONNECTING);
		strcat(szConnectingString,
			open_access_file_par.csAccess_file_full_path);
		if(strlen(open_access_file_par.csAccess_file_password)>0)
		{
			strcat(szConnectingString
				,OPEN_ACCESS_BY_PASSWORD);
			strcat(szConnectingString,
				open_access_file_par.csAccess_file_password);
			strcat(szConnectingString,
				OPEN_ACCESS_BY_SECURITY);
		}
	}
	else if(open_access_file_par.nOpen_Type == OpenAccess2007)
	{
		sprintf(szConnectingString, "%s", OPEN_ACCESS2007_CONNECTING);
		strcat(szConnectingString,
			open_access_file_par.csAccess_file_full_path);
		if(strlen(open_access_file_par.csAccess_file_password)>0)
		{
			strcat(szConnectingString,
				OPEN_ACCESS_BY_PASSWORD);
			strcat(szConnectingString,
				open_access_file_par.csAccess_file_password);
			strcat(szConnectingString,
				OPEN_ACCESS_BY_SECURITY);
		}
	}
	else
	{
		status = STATUS_INVALID_PARAMETERS;
		return status;
	}

	// 关闭以前的链接
	Close_Connect();

	try
	{
		_bstr_t Source(szConnectingString);

		hr = m_Connection.CreateInstance(__uuidof(ADODB::Connection));

		/*hr = m_Connection.CreateInstance("ADODB.Connection");*/
		if(SUCCEEDED(hr))
		{
			m_Connection->Open(Source,
				_bstr_t(open_access_file_par.csAccess_file_user),
				_bstr_t(open_access_file_par.csAccess_file_password),
				open_access_file_par.nOptions);
		}
		else
		{
			DBG_MSG("LLL");
			status = STATUS_UNKNOW_ERROR;
		}

		char szBuf[MAX_PATH]={0};
		sprintf(szBuf,"Open_Connect:Connection0x%x\n",m_Connection);
		OutputDebugString(szBuf);

	}
	catch(_com_error e)
	{
		status = STATUS_EXCEPTION_ERROR;
		DBG_MSG("Open _com_error ,errMessage = %s" ,
			(const char *)(LPCTSTR)e.Description());
	}
	catch(...)
	{
		status = STATUS_EXCEPTION_ERROR;
		DBG_MSG("Open other exception...");
	}

	return status;

}

KSTATUS TH_DB_Access::Excute( EXECUTE_PAR excute_par )
{
	KSTATUS status = STATUS_SUCCESS;

	// 条件限制
	if (m_Connection)
	{
		try
		{
			_variant_t vNULL;
			m_Connection->Execute(_bstr_t(excute_par.sql),
				&vNULL,
				excute_par.nOptions);	
		}
		catch (_com_error e)
		{
			status = STATUS_EXCEPTION_ERROR;
			DBG_MSG("Open _com_error ,errMessage = %s" ,
				(const char *)(LPCTSTR)e.Description());
		}
	}
	else
	{
		status = STATUS_INVALID_PARAMETERS;
	}

	return status;

}

_variant_t TH_DB_Access::GetInterfacePtrEx(void)
{
	_variant_t ActiveConnection;

	try
	{
		ActiveConnection = _variant_t(m_Connection.GetInterfacePtr());
	}
	catch (_com_error e)
	{
		DBG_MSG("Open _com_error ,errMessage = %s" ,
			(const char *)(LPCTSTR)e.Description());
	}
	return ActiveConnection;
}

KSTATUS TH_DB_Access::Open_Rs( OPEN_RS_PAR* open_rs_par )
{
	KSTATUS status = STATUS_SUCCESS;
	HRESULT hr = S_OK;

	hr = open_rs_par->Rs.CreateInstance(__uuidof(ADODB::Recordset));

	//OutputDebugString("Open_Rs CP1\n");

	if(SUCCEEDED(hr))
	{
		//OutputDebugString("Open_Rs CP2\n");
		try
		{
			open_rs_par->Rs->Open(_variant_t(open_rs_par->sql),
				GetInterfacePtrEx(),			
				ADODB::adOpenDynamic,
				ADODB::adLockOptimistic,
				ADODB::adCmdText);
		}
		catch (_com_error e)
		{
			status = STATUS_EXCEPTION_ERROR;

			DBG_MSG("Open_Rs _com_error,errMessage = %s" ,
				(const char *)(LPCTSTR)e.Description());
		}
	}
	else
	{
		//OutputDebugString("Open_Rs CP3\n");
		status = STATUS_INVALID_PARAMETERS;
	}

	return status;

}

void TH_DB_Access::Close_Rs( ADODB::_RecordsetPtr Rs )
{
	if(Rs == NULL)
	{
		return ;
	}

	try
	{
		if (Rs->State & ADODB::adStateOpen)
		{
			Rs->Close();
			Rs = NULL;
		}
	}
	catch(_com_error e)
	{
		DBG_MSG("Close_Rs _com_error,errMessage = %s" ,
			(const char *)(LPCTSTR)e.Description());
	}

	return;

}

KSTATUS TH_DB_Access::Login( PUSER_LOGIN login_par,int * UserType )
{
	KSTATUS status;
	OPEN_RS_PAR rs_par;
	memset(&rs_par,0,sizeof(rs_par));

	if(m_Connection == NULL)
	{
		status = STATUS_INVALID_PARAMETERS;
		return status;
	}

	sprintf(rs_par.sql,"SELECT TOP 1 * FROM tUser WHERE sAccount = '%s'",login_par->sAccount);
	try
	{
		status = Open_Rs(&rs_par);

		if(status != STATUS_SUCCESS)
		{
			status = STATUS_INVALID_PARAMETERS;
			return status;
		}

		if (!IsEOF(rs_par.Rs))
		{
			_variant_t varPassword = GetLoginPassword(rs_par.Rs);
			std::string sbuf;

			if(varPassword.vt == VT_NULL)
				sbuf = "";
			else
				sbuf = _bstr_t(varPassword);

			if(strcmp(sbuf.c_str(),login_par->sPassword) == 0)
			{
				status = STATUS_SUCCESS;
				*UserType = (LONG)GetLoginGroup(rs_par.Rs);				
				
				//AfxMessageBox("登录成功");
			}	
			else
			{
				//AfxMessageBox("密码错误");
				status = STATUS_LOGIN_PASSWORD_ERR;
			}
		}
		else
		{
			status = STATUS_LOGIN_USER_ERR;
		}	
	}
	catch (_com_error e)
	{
		status = STATUS_EXCEPTION_ERROR;
		DBG_MSG("Login _com_error,errMessage = %s" ,
			(const char *)(LPCTSTR)e.Description());
	}

	// 关闭
	Close_Rs(rs_par.Rs);

	return status;

}

KSTATUS TH_DB_Access::CheckAccount( CHECK_ACCOUNT check_account )
{
	KSTATUS status = STATUS_SUCCESS;
	OPEN_RS_PAR rs_par;

	if(m_Connection == NULL)
	{
		status = STATUS_INVALID_PARAMETERS;
		return status;
	}

	if(strlen(check_account.sAccount) <= 0)
	{
		status = STATUS_INVALID_PARAMETERS;
		return status;
	}

	sprintf(rs_par.sql,
		"SELECT TOP 1 * FROM tUser WHERE sAccount = '%s'",
		check_account.sAccount);


	try
	{
		status = Open_Rs(&rs_par);

		if(status != STATUS_SUCCESS)
		{
			status = STATUS_INVALID_PARAMETERS;
			return status;
		}

		if (!IsEOF(rs_par.Rs))
		{
			status = STATUS_LOGIN_USER_EXIST;
		}
		else
		{
			status = STATUS_LOGIN_USER_NOEXIST;
		}	
	}
	catch (_com_error e)
	{
		status = STATUS_EXCEPTION_ERROR;
		DBG_MSG("CheckAccount _com_error,errMessage = %s" ,
			(const char *)(LPCTSTR)e.Description());
	}

	// 关闭
	Close_Rs(rs_par.Rs);

	return status;


}

KSTATUS TH_DB_Access::GetUserInfo( USER_INFO* pUserInfo,unsigned int* pnSize )
{
	KSTATUS  status = STATUS_SUCCESS;
	OPEN_RS_PAR rs_par;

	if(m_Connection == NULL)
	{
		status = STATUS_INVALID_PARAMETERS;
		return status;
	}


	// 返回所有用户列表
	sprintf(rs_par.sql,
		"SELECT * FROM tUser");

	try
	{
		status = Open_Rs(&rs_par);

		if(status != STATUS_SUCCESS)
		{
			status = STATUS_INVALID_PARAMETERS;
			return status;
		}

		if (!IsBOF(rs_par.Rs))
		{
			rs_par.Rs->MoveFirst();
		}

		int nCounts = 0;
		for (int i = 0; !IsEOF(rs_par.Rs); rs_par.Rs->MoveNext(),i++)
		{
			++nCounts;

			if(pUserInfo == NULL)
			{
				*pnSize = nCounts;
				status = STATUS_LOGIN_MEMORY_NOTENOUGH;
			}
			else
			{
				_variant_t Value = rs_par.Rs->GetCollect(_variant_t("sAccount"));
				if(Value.vt != VT_NULL)
					strcpy(pUserInfo[i].sAccount,_bstr_t(Value));

				/*strcpy(pUserInfo[i].sAccount,_bstr_t(rs_par.Rs->GetCollect(_variant_t("sAccount"))));*/

				 Value = rs_par.Rs->GetCollect(_variant_t("sPosition"));

				if(Value.vt != VT_NULL)
				{
					strcpy(pUserInfo[i].sPosition,
						_bstr_t(Value));
				}


				Value = rs_par.Rs->GetCollect(_variant_t("sName"));

				if(Value.vt != VT_NULL)
				{
					strcpy(pUserInfo[i].sName,
						_bstr_t(Value));
				}

				Value = rs_par.Rs->GetCollect(_variant_t("sDept"));

				if(Value.vt != VT_NULL)
				{
					strcpy(pUserInfo[i].sDept,
						_bstr_t(Value));
				}

				Value = rs_par.Rs->GetCollect(_variant_t("dtDate"));

				if(Value.vt != VT_NULL)
				{
					//strcpy(pUserInfo[i].dtDate,
					//	_bstr_t(Value));
					COleDateTime odt(Value.date);
					strcpy(pUserInfo[i].dtDate, odt.Format("%Y-%m-%d %H:%M:%S").GetBuffer(0));
				}

				Value = rs_par.Rs->GetCollect(_variant_t("sPassword"));

				if(Value.vt != VT_NULL)
				{
					strcpy(pUserInfo[i].sPassword,_bstr_t(Value));
				}
				else
					strcpy(pUserInfo[i].sPassword,"");
					


				Value = rs_par.Rs->GetCollect(_variant_t("nGroup"));
				if(Value.vt != VT_NULL)
					pUserInfo[i].Group = Value;


				/*pUserInfo[i].Group = rs_par.Rs->GetCollect(_variant_t("nGroup"));*/


				status = STATUS_SUCCESS;
			}

		} // end for
	}
	catch (_com_error e)
	{
		status = STATUS_EXCEPTION_ERROR;
		DBG_MSG("GetUserInfo _com_error,errMessage = %s" ,
			(const char *)(LPCTSTR)e.Description());
	}

	// 关闭
	Close_Rs(rs_par.Rs);

	return status;

}

KSTATUS TH_DB_Access::GetPwd( const string &UserName, string &Pwd )
{
	KSTATUS  status = STATUS_SUCCESS;
	OPEN_RS_PAR rs_par;

	if(m_Connection == NULL)
	{
		status = STATUS_INVALID_PARAMETERS;
		return status;
	}

	sprintf( rs_par.sql, "SELECT * FROM tUser where sAccount='%s'", UserName.c_str() );

	try
	{
		status = Open_Rs(&rs_par);

		if(status != STATUS_SUCCESS)
		{
			status = STATUS_INVALID_PARAMETERS;
			return status;
		}

		_variant_t Value = rs_par.Rs->GetCollect(_variant_t("sPassword"));
		if(Value.vt != VT_NULL)
		{
			Pwd = _bstr_t(Value);
			status = STATUS_SUCCESS;
		}
		else
		{
			Pwd = "";
			status = STATUS_SUCCESS;
			//status = STATUS_UNKNOW_ERROR;
		}


	}
	catch (_com_error e)
	{
		status = STATUS_EXCEPTION_ERROR;
		DBG_MSG("GetPwd _com_error,errMessage = %s" ,
			(const char *)(LPCTSTR)e.Description());
	}

	Close_Rs(rs_par.Rs);
	return status;

}

KSTATUS TH_DB_Access::ModifyPwdImp( const string &UserName, const string &NewPwd )
{
	KSTATUS  status = STATUS_SUCCESS;
	OPEN_RS_PAR rs_par;


	if(m_Connection == NULL)
	{
		status = STATUS_INVALID_PARAMETERS;
		return status;
	}

	sprintf( rs_par.sql, "SELECT * FROM tUser WHERE sAccount = '%s'", UserName.c_str() );

	try
	{
		status = Open_Rs(&rs_par);

		if(status != STATUS_SUCCESS)
		{
			status = STATUS_INVALID_PARAMETERS;
			return status;
		}

		rs_par.Rs->PutCollect(_variant_t("sPassword"), _variant_t( NewPwd.c_str() ));
		rs_par.Rs->Update();
		status = STATUS_SUCCESS;
	}
	catch (_com_error e)
	{
		status = STATUS_EXCEPTION_ERROR;
		DBG_MSG("ModifyPwdImp _com_error,errMessage = %s" ,
			(const char *)(LPCTSTR)e.Description());
	}

	Close_Rs(rs_par.Rs);
	return status;

}


KSTATUS TH_DB_Access::ModifyPwd( const string &UserName, const string &OldPwd, const string &NewPwd )
{
	string Pwd;

	if( STATUS_SUCCESS != GetPwd(UserName, Pwd) )
		return STATUS_MODIFY_PWD_FAILED;

	if(Pwd != OldPwd)
		return STATUS_PWD_ERROR;

	if( STATUS_SUCCESS != ModifyPwdImp(UserName, NewPwd) )
		return STATUS_MODIFY_PWD_FAILED;

	return STATUS_SUCCESS;

}

KSTATUS TH_DB_Access::ModifyUserInfo(PUSER_INFO pUser_Info)
{
	KSTATUS  status = STATUS_SUCCESS;
	OPEN_RS_PAR rs_par;
	if(m_Connection == NULL)
	{
		status = STATUS_INVALID_PARAMETERS;
		return status;
	}
	if(strlen(pUser_Info->sAccount)<=0)
	{
		status = STATUS_INVALID_PARAMETERS;
		return status;
	}
	sprintf(rs_par.sql,
		"SELECT TOP 1 * FROM tUser WHERE sAccount = '%s'",
		pUser_Info->sAccount);

	try
	{
		status = Open_Rs(&rs_par);

		if(status != STATUS_SUCCESS)
		{
			status = STATUS_INVALID_PARAMETERS;
			return status;
		}

		if (!IsEOF(rs_par.Rs))
		{
			if(strlen(pUser_Info->sAccount)>0)
			{
				rs_par.Rs->PutCollect(_variant_t("sAccount"),
					_variant_t(pUser_Info->sAccount));

			}
			if(strlen(pUser_Info->sDept)>0)
			{
				rs_par.Rs->PutCollect(_variant_t("sDept"),
					_variant_t(pUser_Info->sDept));
			}
			if(strlen(pUser_Info->sName)>0)
			{
				rs_par.Rs->PutCollect(_variant_t("sName"),
					_variant_t(pUser_Info->sName));
			}
			if(strlen(pUser_Info->sPosition)>0)
			{
				rs_par.Rs->PutCollect(_variant_t("sPosition"),
					_variant_t(pUser_Info->sPosition));
			}
			if(pUser_Info->Group>=0)
			{
				rs_par.Rs->PutCollect(_variant_t("nGroup"),
					_variant_t(pUser_Info->Group));
			}

			rs_par.Rs->Update();
			status = STATUS_SUCCESS;
		}
		
	}
	catch (_com_error e)
	{
		status = STATUS_EXCEPTION_ERROR;
		DBG_MSG("AddUser _com_error,errMessage = %s" ,
			(const char *)(LPCTSTR)e.Description());
	}

	// 关闭
	Close_Rs(rs_par.Rs);

	return status;

}

KSTATUS TH_DB_Access::AddUser( USER_ADD user_add )
{
	KSTATUS  status = STATUS_SUCCESS;
	OPEN_RS_PAR rs_par;


	if(m_Connection == NULL)
	{
		status = STATUS_INVALID_PARAMETERS;
		return status;
	}

	if(strlen(user_add.sAccount)<=0)
	{
		status = STATUS_INVALID_PARAMETERS;
		return status;
	}
	if( user_add.nGroupe < 0)
	{
		status = STATUS_INVALID_PARAMETERS;
		return status;
	}

	if( strlen(user_add.sDept) <= 0)
	{
		status = STATUS_INVALID_PARAMETERS;
		return status;
	}

	/*if(strlen(user_add.sPosition) <= 0)
	{
		status = STATUS_INVALID_PARAMETERS;
		return status;
	}*/
	if(strlen(user_add.sName) <= 0)
	{
		status = STATUS_INVALID_PARAMETERS;
		return status;
	}

	sprintf(rs_par.sql,
		"SELECT TOP 1 * FROM tUser WHERE sAccount = '%s'",
		user_add.sAccount);

	try
	{
		status = Open_Rs(&rs_par);

		if(status != STATUS_SUCCESS)
		{
			status = STATUS_INVALID_PARAMETERS;
			return status;
		}

		if (rs_par.Rs->adoEOF)
		{
			rs_par.Rs->AddNew();
		}

		rs_par.Rs->PutCollect(_variant_t("sAccount"),
			_variant_t(user_add.sAccount));


		if(strlen(user_add.sPassword) > 0)
			rs_par.Rs->PutCollect(_variant_t("sPassword"),_variant_t(user_add.sPassword));
		else
			rs_par.Rs->PutCollect(_variant_t("sPassword"),_variant_t(""));

		
		/*rs_par.Rs->PutCollect(_variant_t("nGroup"),
			_variant_t(USER_TOKEN_USER));*/
		rs_par.Rs->PutCollect(_variant_t("nGroup"),	_variant_t(user_add.nGroupe));

	
		rs_par.Rs->PutCollect(_variant_t("sDept"),_variant_t(user_add.sDept));

		
		rs_par.Rs->PutCollect(_variant_t("sPosition"),_variant_t(user_add.sPosition));

		
		rs_par.Rs->PutCollect(_variant_t("sName"),_variant_t(user_add.sName));

	/*	rs_par.Rs->PutCollect(_variant_t("dtDate"),
			_variant_t(user_add.dtDate));*/

		rs_par.Rs->Update();

	}
	catch (_com_error e)
	{
		status = STATUS_EXCEPTION_ERROR;
		DBG_MSG("AddUser _com_error,errMessage = %s" ,
			(const char *)(LPCTSTR)e.Description());
	}

	// 关闭
	Close_Rs(rs_par.Rs);

	return status;

}

KSTATUS TH_DB_Access::DelUser( USER_DEL user_del )
{
	KSTATUS  status = STATUS_SUCCESS;
	OPEN_RS_PAR rs_par;

	if(m_Connection == NULL)
	{
		status = STATUS_INVALID_PARAMETERS;
		return status;
	}

	if(strlen(user_del.sUser) <=0)
	{
		status = STATUS_INVALID_PARAMETERS;
		return status;
	}

	if(user_del.Group > 0 && strcmp(user_del.sUser,"adm") == 0)
	{
		
		status = STATUS_INVALID_PARAMETERS;
		return status;
	}

	sprintf(rs_par.sql,
		"SELECT TOP 1 * FROM tUser WHERE sAccount = '%s'",
		user_del.sUser);


	try
	{
		status = Open_Rs(&rs_par);

		if(status != STATUS_SUCCESS)
		{
			status = STATUS_INVALID_PARAMETERS;
			return status;
		}

		if (!IsBOF(rs_par.Rs))
		{
			rs_par.Rs->MoveFirst();
			for (; !IsEOF(rs_par.Rs); rs_par.Rs->MoveNext())
			{
				rs_par.Rs->Delete(ADODB::adAffectCurrent);
				rs_par.Rs->Update();
			}
		}
	}
	catch (_com_error e)
	{
		status = STATUS_EXCEPTION_ERROR;
		DBG_MSG("DelUser _com_error,errMessage = %s" ,
			(const char *)(LPCTSTR)e.Description());
	}

	// 关闭
	Close_Rs(rs_par.Rs);


	return status;

}




#define   SIGN_STATUS_NEW          999

KSTATUS TH_DB_Access::CheckNewStatus( unsigned int* pID,bool bIsQC,ULONG nStatus)
{
	KSTATUS status = STATUS_SUCCESS;
	OPEN_RS_PAR rs_par;
	*pID = -1;

	if(m_Connection == NULL)
	{
		status = STATUS_INVALID_PARAMETERS;
		return status;
	}


	strcpy(rs_par.sql, "SELECT * FROM (((tMain LEFT OUTER JOIN tUs ON tMain.nID = tUs.nID) \
					   LEFT OUTER JOIN tUdc ON tMain.nID = tUdc.nID) \
					   LEFT OUTER JOIN tPat ON tMain.nID = tPat.nID) \
					   WHERE DateDiff('D', dtDate, NOW()) = 0 ORDER BY tMain.nID");


	try
	{
		status = Open_Rs(&rs_par);

		if(status != STATUS_SUCCESS)
		{
			status = STATUS_INVALID_PARAMETERS;
			return status;
		}

		if(!IsBOF(rs_par.Rs))
		{
			rs_par.Rs->MoveFirst();
		}

		for (; !IsEOF(rs_par.Rs); rs_par.Rs->MoveNext())
		{
			unsigned int nStatus = rs_par.Rs->GetCollect(_variant_t("nState"));
			bool bGetIsQC = rs_par.Rs->GetCollect(_variant_t("nAttribute"));

			if(nStatus == SIGN_STATUS_NEW && bIsQC == bGetIsQC)
			{
				*pID = rs_par.Rs->GetCollect(_variant_t("tMain.nID"));
				// 设置为新任务				
				rs_par.Rs->PutCollect(_variant_t("nState"),_variant_t(nStatus));
				rs_par.Rs->Update();
				break;
			}// end if

		} // end for
	}
	catch (_com_error e)
	{
		status = STATUS_EXCEPTION_ERROR;
		DBG_MSG("CheckNewStatus _com_error,errMessage = %s" ,
			(const char *)(LPCTSTR)e.Description());
	}

	// 关闭
	Close_Rs(rs_par.Rs);

	return status;

}

KSTATUS TH_DB_Access::AddTask( TASK_ADD* ptask_add )
{
	KSTATUS  status = STATUS_SUCCESS;
	OPEN_RS_PAR rs_par;

	char ModePath[MAX_PATH] = {0},temp[MAX_PATH] = {0};



	if(m_Connection == NULL)
	{
		status = STATUS_INVALID_PARAMETERS;
		return status;
	}

	if(ptask_add == NULL)
	{
		status = STATUS_INVALID_PARAMETERS;
		return status;
	}

	// 判断保存是否限制任务总数
	if (ptask_add->bLimitTaskTotalCount && ptask_add->nLimitTaskTotalCount > 0)
	{
		OPEN_RS_PAR orp1, orp2;
		int nTotalCount = 0;
		do
		{
			// 查询记录总数
			strcpy(orp1.sql, "SELECT COUNT(*) as TotalCount FROM tMain");
			try
			{
				status = Open_Rs(&orp1);
				if (status != STATUS_SUCCESS)
				{
					break;
				}
				if (!IsBOF(orp1.Rs))
				{
					orp1.Rs->MoveFirst();
				}
				_variant_t Value = orp1.Rs->GetCollect(_variant_t("TotalCount"));
				if (Value.vt != VT_NULL) nTotalCount = Value;
				Close_Rs(orp1.Rs);
			}
			catch (_com_error e)
			{
				DBG_MSG("AddTask Limit total count _com_error,errMessage = %s", (const char *)(LPCTSTR)e.Description());
				Close_Rs(orp1.Rs);
				break;
			}
			
			// 计算需要删除的纪录数量
			int nDeleteCount = nTotalCount - ptask_add->nLimitTaskTotalCount + 1;
			if (nDeleteCount > 0)
			{
				// 查询需要删除
				sprintf_s(orp2.sql, "SELECT TOP %d * FROM (((tMain LEFT OUTER JOIN tUs ON tMain.nID = tUs.nID) \
													LEFT OUTER JOIN tUdc ON tMain.nID = tUdc.nID) \
													LEFT OUTER JOIN tPat ON tMain.nID = tPat.nID) \
													ORDER BY tMain.nID", nDeleteCount);
				try
				{
					status = Open_Rs(&orp2);
					if (status != STATUS_SUCCESS)
					{
						break;
					}
					if (!IsBOF(orp2.Rs))
					{
						orp2.Rs->MoveFirst();
					}
					for (; !IsEOF(orp2.Rs); orp2.Rs->MoveNext())
					{
						orp2.Rs->Delete(ADODB::adAffectCurrent);
						orp2.Rs->Update();
					}
				}
				catch (_com_error e)
				{
					DBG_MSG("AddTask Limit total count delete _com_error,errMessage = %s", (const char *)(LPCTSTR)e.Description());
				}
				Close_Rs(orp2.Rs);
			}
		} while (0);
	}

	// 检测是否有可以用的任务
/*	status = CheckNewStatus(&ptask_add->nID,ptask_add->bIsQC,ptask_add->nStatus);

	if(status != STATUS_SUCCESS)
	{
		return status;
	}

	if(ptask_add->nID != -1)
	{
		return STATUS_SUCCESS;
	}*/
	strcpy(rs_par.sql, "SELECT * FROM (((tMain LEFT OUTER JOIN tUs ON tMain.nID = tUs.nID) \
					   LEFT OUTER JOIN tUdc ON tMain.nID = tUdc.nID) \
					   LEFT OUTER JOIN tPat ON tMain.nID = tPat.nID) \
					   WHERE DateDiff('D', dtDate, NOW()) = 0 ORDER BY tMain.nID");
	try
	{
		status = Open_Rs(&rs_par);

		if(status != STATUS_SUCCESS)
		{
			status = STATUS_INVALID_PARAMETERS;
			return status;
		}

		int nSN = 0;
		int nNo = 0;


		if(!IsBOF(rs_par.Rs))
		{
			rs_par.Rs->MoveFirst();
		}
		
		/*for (; !IsEOF(rs_par.Rs); rs_par.Rs->MoveNext())
		{
			int  ntype = rs_par.Rs->GetCollect(_variant_t("nAttribute"));

			if(ptask_add->nTestDes == ntype)
			{
				nSN = rs_par.Rs->GetCollect(_variant_t("nSN"));
				nNo = rs_par.Rs->GetCollect(_variant_t("nNo"));
			}	
		}
		nSN++;
		nNo++;*/

		rs_par.Rs->AddNew();		

		rs_par.Rs->PutCollect(_variant_t("nState"),_variant_t(ptask_add->nStatus));

	
		// 返回ID
		ptask_add->nID = rs_par.Rs->GetCollect(_variant_t("tMain.nID"));
		
		//if(ptask_add->nNo <= 0)			
		//	//rs_par.Rs->PutCollect(_variant_t("nNo"), _variant_t(nNo));
		//	rs_par.Rs->PutCollect(_variant_t("nSN"), _variant_t(nSN));
		//else
			//rs_par.Rs->PutCollect(_variant_t("nNo"),_variant_t(ptask_add->nNo));
			rs_par.Rs->PutCollect(_variant_t("nSN"),_variant_t(ptask_add->nNo));

		if(ptask_add->nCha >= 0)
			rs_par.Rs->PutCollect(_variant_t("nCha"),_variant_t(ptask_add->nCha));

		/*if(ptask_add->bIsQC == false)
			rs_par.Rs->PutCollect(_variant_t("nTestType"),_variant_t(ptask_add->nTestType));*/

		rs_par.Rs->PutCollect(_variant_t("nAttribute"),_variant_t(ptask_add->nTestDes));

		//if(ptask_add->nTestDes == TestNormal || ptask_add->nTestDes == TestEP)
			rs_par.Rs->PutCollect(_variant_t("nTestType"),_variant_t(ptask_add->nTestType));
		//else
		//	rs_par.Rs->PutCollect(_variant_t("nTestType"), _variant_t(0));


	//	if(ptask_add->nTestDes == TestNormal)
	//		rs_par.Rs->PutCollect(_variant_t("nTestType"),_variant_t(ptask_add->nTestType));

	//	else if(ptask_add->nTestDes == TestEP)
	//	{
	//		rs_par.Rs->PutCollect(_variant_t("bMain"),_variant_t(true));
	//		rs_par.Rs->PutCollect(_variant_t("nTestType"),_variant_t(ptask_add->nTestType));
	//	}
	//		
	///*	else
	//		rs_par.Rs->PutCollect(_variant_t("bMain"),_variant_t(false));*/

	//	else
	//	{
	//		rs_par.Rs->PutCollect(_variant_t("nAttribute"),_variant_t(-1));
	//		if(ptask_add->nTestDes == UsQCByP || ptask_add->nTestDes == UsQCByN)
	//		{
	//			rs_par.Rs->PutCollect(_variant_t("nQcType"),_variant_t(1));
	//			if(ptask_add->nTestDes == UsQCByP)
	//				rs_par.Rs->PutCollect(_variant_t("nQcUsType"),_variant_t(2));
	//			else
	//				rs_par.Rs->PutCollect(_variant_t("nQcUsType"),_variant_t(1));

	//		}
	//		else
	//		{
	//			rs_par.Rs->PutCollect(_variant_t("nQcType"),_variant_t(2));
	//			if(ptask_add->nTestDes == UdcQCBy1)
	//				rs_par.Rs->PutCollect(_variant_t("nQcUdcType"),_variant_t(1));
	//			else if(ptask_add->nTestDes == UdcQCBy2)
	//				rs_par.Rs->PutCollect(_variant_t("nQcUdcType"),_variant_t(2));
	//			else
	//				rs_par.Rs->PutCollect(_variant_t("nQcUdcType"),_variant_t(3));

	//		}

	//	}
		

	/*	if(ptask_add->bIsQC == true)
		{
			rs_par.Rs->PutCollect(_variant_t("nQcType"),_variant_t(ptask_add->nQcType));

			if(ptask_add->nQcType == US_QC_TYPE)
				rs_par.Rs->PutCollect(_variant_t("nQcUsType"),_variant_t(ptask_add->nQcUsType));
			else if(ptask_add->nQcType == UDC_QC_TYPE)
				rs_par.Rs->PutCollect(_variant_t("nQcUdcType"),_variant_t(ptask_add->nQcUdcType));
		}	*/

		if(ptask_add->nRow >= 0)
			rs_par.Rs->PutCollect(_variant_t("nRow"),_variant_t(ptask_add->nRow));
		if(ptask_add->nPos >= 0)
			rs_par.Rs->PutCollect(_variant_t("nPos"),_variant_t(ptask_add->nPos));
		
		rs_par.Rs->PutCollect(_variant_t("tUdc.nID"), _variant_t(ptask_add->nID));

		rs_par.Rs->PutCollect(_variant_t("tUs.nID"),_variant_t(ptask_add->nID));

		rs_par.Rs->PutCollect(_variant_t("tPat.nID"),_variant_t(ptask_add->nID));	


		GetModuleFileName(NULL,ModePath,MAX_PATH);
		PathRemoveFileSpec(ModePath);
		PathAppend(ModePath,"RecGrapReslut");

		sprintf(temp,"%d",ptask_add->nID);
		PathAppend(ModePath,temp);

		DBG_MSG("PicPath:%s\n",ModePath);

		rs_par.Rs->PutCollect(_variant_t("sFolder"),_variant_t(ModePath));

		rs_par.Rs->Update();
	}
	catch (_com_error e)
	{
		status = STATUS_EXCEPTION_ERROR;
		DBG_MSG("AddTask _com_error,errMessage = %s" ,
			(const char *)(LPCTSTR)e.Description());
	}

	// 关闭
	Close_Rs(rs_par.Rs);



	return status;

}

KSTATUS TH_DB_Access::AddTaskMust( TASK_ADD* ptask_add )
{
	KSTATUS  status = STATUS_SUCCESS;
	OPEN_RS_PAR rs_par;



	//if(m_Connection == NULL)
	//{
	//	status = STATUS_INVALID_PARAMETERS;
	//	return status;
	//}

	//if(ptask_add == NULL)
	//{
	//	status = STATUS_INVALID_PARAMETERS;
	//	return status;
	//}

	//strcpy(rs_par.sql, "SELECT * FROM (((tMain LEFT OUTER JOIN tUs ON tMain.nID = tUs.nID) \
	//				   LEFT OUTER JOIN tUdc ON tMain.nID = tUdc.nID) \
	//				   LEFT OUTER JOIN tPat ON tMain.nID = tPat.nID) \
	//				   WHERE DateDiff('D', dtDate, NOW()) = 0 ORDER BY tMain.nID");

	//try
	//{
	//	status = Open_Rs(&rs_par);

	//	if(status != STATUS_SUCCESS)
	//	{
	//		status = STATUS_INVALID_PARAMETERS;
	//		return status;
	//	}


	//	int nSN = 0;
	//	int nNo = 0;

	//	if(!IsBOF(rs_par.Rs))
	//	{
	//		rs_par.Rs->MoveFirst();
	//	}

	//	for (; !IsEOF(rs_par.Rs); rs_par.Rs->MoveNext())
	//	{
	//		bool bIsQC = rs_par.Rs->GetCollect(_variant_t("nAttribute"));
	//		if(ptask_add->bIsQC == bIsQC)
	//		{
	//			nSN = rs_par.Rs->GetCollect(_variant_t("nSN"));
	//			nNo = rs_par.Rs->GetCollect(_variant_t("nNo"));
	//		}	
	//	}
	//	nSN++;
	//	nNo++;


	//	rs_par.Rs->AddNew();

	//	rs_par.Rs->PutCollect(_variant_t("nState"),_variant_t(999));

	//	rs_par.Rs->PutCollect(_variant_t("nSN"),_variant_t(nSN));

	//	if(ptask_add->bIsEP == true)
	//		rs_par.Rs->PutCollect(_variant_t("bMain"),_variant_t(true));
	//	else
	//		rs_par.Rs->PutCollect(_variant_t("bMain"),_variant_t(false));

	//	// 返回ID
	//	ptask_add->nID = rs_par.Rs->GetCollect(_variant_t("tMain.nID"));

	//	if(ptask_add->nNo <= 0)
	//		//rs_par.Rs->PutCollect(_variant_t("nNo"), _variant_t(ptask_add->nID));
	//		rs_par.Rs->PutCollect(_variant_t("nNo"), _variant_t(nNo));
	//	else
	//		rs_par.Rs->PutCollect(_variant_t("nNo"),_variant_t(ptask_add->nNo));

	//	if(ptask_add->nCha >= 0)
	//		rs_par.Rs->PutCollect(_variant_t("nCha"),_variant_t(ptask_add->nCha));

	//	if(ptask_add->bIsQC == false)
	//		rs_par.Rs->PutCollect(_variant_t("nTestType"),_variant_t(ptask_add->nTestType));

	//	if(ptask_add->nRow >= 0)
	//		rs_par.Rs->PutCollect(_variant_t("nRow"),_variant_t(ptask_add->nRow));

	//	if(ptask_add->nPos >= 0)
	//		rs_par.Rs->PutCollect(_variant_t("nPos"),_variant_t(ptask_add->nPos));

	//	rs_par.Rs->PutCollect(_variant_t("tUdc.nID"), _variant_t(ptask_add->nID));

	//	rs_par.Rs->PutCollect(_variant_t("tUs.nID"),_variant_t(ptask_add->nID));

	//	rs_par.Rs->PutCollect(_variant_t("tPat.nID"),_variant_t(ptask_add->nID));

	//	DBG_MSG("ptask_add->bIsQC=%d,ptask_add->nQcType=%d\n",ptask_add->bIsQC,ptask_add->nQcType);

	//	rs_par.Rs->PutCollect(_variant_t("nAttribute"),_variant_t(ptask_add->bIsQC));

	//	if(ptask_add->bIsQC == true)
	//	{
	//		rs_par.Rs->PutCollect(_variant_t("nQcType"),_variant_t(ptask_add->nQcType));

	//		if(ptask_add->nQcType == US_QC_TYPE)
	//			rs_par.Rs->PutCollect(_variant_t("nQcUsType"),_variant_t(ptask_add->nQcUsType));
	//		else if(ptask_add->nQcType == UDC_QC_TYPE)
	//			rs_par.Rs->PutCollect(_variant_t("nQcUdcType"),_variant_t(ptask_add->nQcUdcType));
	//	}	

	//	rs_par.Rs->Update();


	//}
	//catch (_com_error e)
	//{
	//	status = STATUS_EXCEPTION_ERROR;
	//	DBG_MSG("AddTask _com_error,errMessage = %s" ,
	//		(const char *)(LPCTSTR)e.Description());
	//}

	//// 关闭
	//Close_Rs(rs_par.Rs);

	return status;

}

KSTATUS TH_DB_Access::DelTask( int* pnDelID,int nCounts,bool bIsqc )
{
	KSTATUS  status = STATUS_SUCCESS;
	OPEN_RS_PAR rs_par;


	if(m_Connection == NULL)
	{
		status = STATUS_INVALID_PARAMETERS;
		return status;
	}

	if(pnDelID == NULL)
	{
		status = STATUS_INVALID_PARAMETERS;
		return status;
	}

	if(nCounts <= 0)
	{
		status = STATUS_INVALID_PARAMETERS;
		return status;
	}

	try
	{
		for(int i = 0 ; i < nCounts ; i ++)
		{

			wsprintf(rs_par.sql,
				"SELECT * FROM ((tMain left outer join tUs on tMain.nID=tUs.nID) left outer join tUdc on tMain.nID=tUdc.nID) left outer join tPat on tMain.nID=tPat.nID where tMain.nID=%d",
				pnDelID[i]);

			DBG_MSG("%d\n",pnDelID[i]);

			status = Open_Rs(&rs_par);

			if(status != STATUS_SUCCESS)
			{
				status = STATUS_INVALID_PARAMETERS;
				return status;
			}

			char szBuf[MAX_PATH]={0};

			if(!IsBOF(rs_par.Rs))
			{
				rs_par.Rs->MoveFirst();
			}

			for (; !IsEOF(rs_par.Rs); rs_par.Rs->MoveNext())
			{
				
				/*bool bIsQC = rs_par.Rs->GetCollect(_variant_t("nAttribute"));

				DBG_MSG("nAttribute = %d,bIsqc = %d\n",bIsQC,bIsqc);

				if(bIsqc == bIsQC)*/
				{
					rs_par.Rs->Delete(ADODB::adAffectCurrent);
					rs_par.Rs->Update();
				}
			}


			// 关闭
			Close_Rs(rs_par.Rs);

		}// end for

	}
	catch (_com_error e)
	{
		status = STATUS_EXCEPTION_ERROR;
		DBG_MSG("DelTask _com_error,errMessage = %s",
			(const char *)(LPCTSTR)e.Description());
	}



	return status;

}

KSTATUS TH_DB_Access::DelAllTask(bool isAll)
{
	KSTATUS status = STATUS_SUCCESS;
	OPEN_RS_PAR rs_par;


	if(m_Connection == NULL)
	{
		status = STATUS_INVALID_PARAMETERS;
		return status;
	}
	if (isAll)
		strcpy(rs_par.sql, "SELECT * FROM (((tMain LEFT OUTER JOIN tUs ON tMain.nID = tUs.nID) \
					   LEFT OUTER JOIN tUdc ON tMain.nID = tUdc.nID) \
					   LEFT OUTER JOIN tPat ON tMain.nID = tPat.nID)\
					   WHERE 1=1 AND nAttribute = 0 OR nAttribute = 6 ORDER BY tMain.nID");
	else
		strcpy(rs_par.sql, "SELECT * FROM (((tMain LEFT OUTER JOIN tUs ON tMain.nID = tUs.nID) \
					   LEFT OUTER JOIN tUdc ON tMain.nID = tUdc.nID) \
					   LEFT OUTER JOIN tPat ON tMain.nID = tPat.nID)\
					   WHERE DateDiff('D', dtDate, NOW()) = 0 AND nAttribute = 0 OR nAttribute = 6 ORDER BY tMain.nID");


	try
	{
		status = Open_Rs(&rs_par);

		if(status != STATUS_SUCCESS)
		{			
			status = STATUS_INVALID_PARAMETERS;
			return status;
		}
		DBG_MSG("IsEOF = %d\n",IsEOF(rs_par.Rs));
		for (; !IsEOF(rs_par.Rs); rs_par.Rs->MoveNext())
		{
			if(!IsBOF(rs_par.Rs))
			{
				rs_par.Rs->MoveFirst();
			}
			rs_par.Rs->Move(0);
			
			rs_par.Rs->Delete(ADODB::adAffectCurrent);
			rs_par.Rs->Update();
		}


	}
	catch (_com_error e)
	{
		status = STATUS_EXCEPTION_ERROR;
		DBG_MSG("DelTask _com_error,errMessage = %s" ,
			(const char *)(LPCTSTR)e.Description());
	}

	// 关闭
	Close_Rs(rs_par.Rs);


	return status;

}

KSTATUS TH_DB_Access::UpdateAlarm( ULONG TaskID,int nAlarm )
{
	KSTATUS status = STATUS_SUCCESS;
	OPEN_RS_PAR rs_par;

	if(m_Connection == NULL)
	{
		status = STATUS_INVALID_PARAMETERS;
		return status;
	}

	sprintf(rs_par.sql,"SELECT * FROM (((tMain left outer join tUs on tMain.nID=tUs.nID) \
					   left outer join tUdc on tMain.nID=tUdc.nID )\
					   left outer join tPat on tMain.nID=tPat.nID where tMain.nID=%d",TaskID);

	try
	{
		status = Open_Rs(&rs_par);

		if(status != STATUS_SUCCESS)
		{
			status = STATUS_INVALID_PARAMETERS;
			return status;
		}

		for (; !IsEOF(rs_par.Rs); rs_par.Rs->MoveNext())
		{
			if(!IsBOF(rs_par.Rs))
			{
				rs_par.Rs->MoveFirst();
			}
			rs_par.Rs->put_Collect(_variant_t("tMain.nAlarm"),_variant_t(nAlarm));
		}


	}
	catch (_com_error e)
	{
		status = STATUS_EXCEPTION_ERROR;
		DBG_MSG("DelTask _com_error,errMessage = %s" ,
			(const char *)(LPCTSTR)e.Description());
	}

	// 关闭
	Close_Rs(rs_par.Rs);


	return status;

}

KSTATUS TH_DB_Access::UpdateTaskStatus( ULONG TaskID,KSTATUS state )
{
	KSTATUS status = STATUS_SUCCESS;
	OPEN_RS_PAR rs_par;

	if(m_Connection == NULL)
	{
		status = STATUS_INVALID_PARAMETERS;
		return status;
	}

	sprintf(rs_par.sql,"SELECT * FROM ((tMain left outer join tUs on tMain.nID=tUs.nID) \
					   left outer join tUdc on tMain.nID=tUdc.nID )\
					   left outer join tPat on tMain.nID=tPat.nID where tMain.nID=%d",TaskID);

	try
	{
		status = Open_Rs(&rs_par);

		if(status != STATUS_SUCCESS)
		{
			status = STATUS_INVALID_PARAMETERS;
			return status;
		}

		for (; !IsEOF(rs_par.Rs); rs_par.Rs->MoveNext())
		{
			if(!IsBOF(rs_par.Rs))
			{
				rs_par.Rs->MoveFirst();
			}
			rs_par.Rs->put_Collect(_variant_t("nState"),_variant_t(state));
		}
	}
	catch (_com_error e)
	{
		status = STATUS_EXCEPTION_ERROR;
		DBG_MSG("DelTask _com_error,errMessage = %s" ,
			(const char *)(LPCTSTR)e.Description());
	}

	// 关闭
	Close_Rs(rs_par.Rs);


	return status;


}


KSTATUS TH_DB_Access::GetTaskInfo( bool bIsSelectById,int nID,TASK_INFO* pTaskInfo,unsigned int* pnSize)
{
	KSTATUS status = STATUS_SUCCESS;
	OPEN_RS_PAR rs_par;

	if(m_Connection == NULL)
	{
		status = STATUS_INVALID_PARAMETERS;
		return status;
	}

	//*pnSize = 0;

	if(bIsSelectById == false)
	{
		strcpy(rs_par.sql, "SELECT * FROM (((tMain LEFT OUTER JOIN tUs ON tMain.nID = tUs.nID) \
						   LEFT OUTER JOIN tUdc ON tMain.nID = tUdc.nID) \
						   LEFT OUTER JOIN tPat ON tMain.nID = tPat.nID)\
						   WHERE DateDiff('D', dtDate, NOW()) = 0 ORDER BY tMain.nID");
	}
	else
	{

		sprintf(rs_par.sql,"SELECT * FROM ((tMain left outer join tUs on tMain.nID=tUs.nID) \
						   left outer join tUdc on tMain.nID=tUdc.nID )\
						   left outer join tPat on tMain.nID=tPat.nID where tMain.nID=%d",nID);
	}


	try
	{
		status = Open_Rs(&rs_par);

		if(status != STATUS_SUCCESS)
		{
			status = STATUS_INVALID_PARAMETERS;
			return status;
		}


		if (!IsBOF(rs_par.Rs))
		{
			rs_par.Rs->MoveFirst();
		}

		int nCounts = 0;
		int bIsQc = 0;
		int i = 0;

		for (; !IsEOF(rs_par.Rs); rs_par.Rs->MoveNext())
		{		
			//bIsQc =(int)rs_par.Rs->GetCollect(_variant_t("nAttribute"));
			////DBG_MSG("bIsQc = %d\n",bIsQc);
			//
			//if(bIsQc == TestNormal || bIsQc == TestEP)
			//	++nCounts;
			//else
			//	continue;			 

			++nCounts;

			if(pTaskInfo == NULL)
			{

				*pnSize = nCounts;
				status = STATUS_LOGIN_MEMORY_NOTENOUGH;
			}
			else
			{

				// ID	
				_variant_t Value = rs_par.Rs->GetCollect(_variant_t("tMain.nID"));
				if(Value.vt != VT_NULL)
					pTaskInfo[i].main_info.nID = Value;						
				
				//Alarm				
				Value= rs_par.Rs->GetCollect(_variant_t("nAlarm"));
				if(Value.vt != VT_NULL)
					pTaskInfo[i].main_info.nAlarm  = Value;

				//Row	
				Value= rs_par.Rs->GetCollect(_variant_t("nRow"));
				if(Value.vt != VT_NULL)
					pTaskInfo[i].main_info.nRow  = Value;

				//bMain				
				Value= rs_par.Rs->GetCollect(_variant_t("bMain"));
				if(Value.vt != VT_NULL)
					pTaskInfo[i].main_info.bMain  = Value;

		        // nStatus
				Value= rs_par.Rs->GetCollect(_variant_t("nState"));
				if(Value.vt != VT_NULL)
					pTaskInfo[i].main_info.nState  = Value;

				// nSN
				Value= rs_par.Rs->GetCollect(_variant_t("nSN"));
				if(Value.vt != VT_NULL)
					pTaskInfo[i].main_info.nSN  = Value;

				// bIsQC
				Value= rs_par.Rs->GetCollect(_variant_t("nAttribute"));
				if(Value.vt != VT_NULL)
					pTaskInfo[i].main_info.nAttribute  = Value;
				
				//TestType					
					Value= rs_par.Rs->GetCollect(_variant_t("nTestType"));
					if(Value.vt != VT_NULL)
						pTaskInfo[i].main_info.nTestType  = Value;					

				//LisStatus
				Value= rs_par.Rs->GetCollect(_variant_t("dtLisStatus"));
				if(Value.vt != VT_NULL)
					pTaskInfo[i].main_info.dtLisStatus  = Value;

				//PrintStatus
				Value= rs_par.Rs->GetCollect(_variant_t("dtPrintStatus"));
				if(Value.vt != VT_NULL)
					pTaskInfo[i].main_info.dtPrintStatus  = Value;

				//Pos				
				Value= rs_par.Rs->GetCollect(_variant_t("nPos"));
				if(Value.vt != VT_NULL)
					pTaskInfo[i].main_info.nPos  = Value;

				//Cha				
				Value= rs_par.Rs->GetCollect(_variant_t("nCha"));
				if(Value.vt != VT_NULL)
					pTaskInfo[i].main_info.nCha  = Value;

				//Code
				Value = rs_par.Rs->GetCollect(_variant_t("sCode"));				
				if(Value.vt != VT_NULL)
				{					
					strcpy(pTaskInfo[i].main_info.sCode,_bstr_t(Value));
				}

				//Company
				Value = rs_par.Rs->GetCollect(_variant_t("sCompany"));
				if(Value.vt != VT_NULL)
				{					
					strcpy(pTaskInfo[i].main_info.sCompany,_bstr_t(Value));
				}

				//Department
				Value = rs_par.Rs->GetCollect(_variant_t("sDepartment"));
				if(Value.vt != VT_NULL)
				{					
					strcpy(pTaskInfo[i].main_info.sDepartment,_bstr_t(Value));
				}

				//Doctor
				Value = rs_par.Rs->GetCollect(_variant_t("sDoctor"));
				if(Value.vt != VT_NULL)
				{					
					strcpy(pTaskInfo[i].main_info.sDoctor,_bstr_t(Value));
				}

				//sDocimaster
				Value = rs_par.Rs->GetCollect(_variant_t("sDocimaster"));
				if(Value.vt != VT_NULL)
				{					
					strcpy(pTaskInfo[i].main_info.sDocimaster,_bstr_t(Value));
				}

				//sAuditor
				Value = rs_par.Rs->GetCollect(_variant_t("sAuditor"));
				if(Value.vt != VT_NULL)
				{					
					strcpy(pTaskInfo[i].main_info.sAuditor,_bstr_t(Value));
				}

				//sReporter
				Value = rs_par.Rs->GetCollect(_variant_t("sReporter"));
				if(Value.vt != VT_NULL)
				{					
					strcpy(pTaskInfo[i].main_info.sReporter,_bstr_t(Value));
				}

				//sColor
				Value = rs_par.Rs->GetCollect(_variant_t("sColor"));
				if(Value.vt != VT_NULL)
				{					
					strcpy(pTaskInfo[i].main_info.sColor,_bstr_t(Value));
				}

				//sTransparency
				Value = rs_par.Rs->GetCollect(_variant_t("sTransparency"));
				if(Value.vt != VT_NULL)
				{					
					strcpy(pTaskInfo[i].main_info.sTransparency,_bstr_t(Value));
				}

				//sFolder
				Value = rs_par.Rs->GetCollect(_variant_t("sFolder"));
				if(Value.vt != VT_NULL)
				{					
					strcpy(pTaskInfo[i].main_info.sFolder,_bstr_t(Value));
				}

				//sImage
				Value = rs_par.Rs->GetCollect(_variant_t("sImage"));
				if(Value.vt != VT_NULL)
				{					
					strcpy(pTaskInfo[i].main_info.sImage,_bstr_t(Value));
				}

				//sDiagnosis
				Value = rs_par.Rs->GetCollect(_variant_t("sDiagnosis"));
				if(Value.vt != VT_NULL)
				{					
					strcpy(pTaskInfo[i].main_info.sDiagnosis,_bstr_t(Value));
				}

				//sSuggestion
				Value = rs_par.Rs->GetCollect(_variant_t("sSuggestion"));
				if(Value.vt != VT_NULL)
				{					
					strcpy(pTaskInfo[i].main_info.sSuggestion,_bstr_t(Value));
				}

				//dtDate
				Value = rs_par.Rs->GetCollect(_variant_t("dtDate"));
				if(Value.vt != VT_NULL)
				{	
					COleDateTime odt(Value.date); 
					strcpy(pTaskInfo[i].main_info.dtDate, odt.Format("%Y-%m-%d %H:%M:%S").GetBuffer(0));
				}


				//dtPrintStatus
				Value = rs_par.Rs->GetCollect(_variant_t("dtPrintStatus"));
				if(Value.vt != VT_NULL)
					pTaskInfo[i].main_info.dtPrintStatus  = Value;


				//dtLisStatus
				Value = rs_par.Rs->GetCollect(_variant_t("dtLisStatus"));
				if(Value.vt != VT_NULL)
					pTaskInfo[i].main_info.dtLisStatus  = Value;

				//pat_ID
				Value= rs_par.Rs->GetCollect(_variant_t("tPat.nID"));
				if(Value.vt != VT_NULL)
					pTaskInfo[i].pat_info.nID  = Value;

				//bPat
				Value= rs_par.Rs->GetCollect(_variant_t("bPat"));
				if(Value.vt != VT_NULL)
					pTaskInfo[i].pat_info.bPat  = Value;
	
				//nNo		
				Value= rs_par.Rs->GetCollect(_variant_t("nNo"));
				if(Value.vt != VT_NULL)
					pTaskInfo[i].pat_info.nNo  = Value;

				//sName
				Value = rs_par.Rs->GetCollect(_variant_t("sName"));
				if(Value.vt != VT_NULL)
				{					
					strcpy(pTaskInfo[i].pat_info.sName,_bstr_t(Value));
				}
				
				//nSex				
				Value= rs_par.Rs->GetCollect(_variant_t("nSex"));
				if(Value.vt != VT_NULL)
					pTaskInfo[i].pat_info.nSex  = Value;


				//nAge				
				Value= rs_par.Rs->GetCollect(_variant_t("nAge"));
				if(Value.vt != VT_NULL)
					pTaskInfo[i].pat_info.nAge  = Value;

				
				//nAgeUnit				
				Value=  rs_par.Rs->GetCollect(_variant_t("nAgeUnit"));
				if(Value.vt != VT_NULL)
					pTaskInfo[i].pat_info.nAgeUnit  = Value;

				

				//sHospital
				Value = rs_par.Rs->GetCollect(_variant_t("sHospital"));
				if(Value.vt != VT_NULL)
				{					
					strcpy(pTaskInfo[i].pat_info.sHospital,_bstr_t(Value));
				}
				//sSickBed

				
				Value = rs_par.Rs->GetCollect(_variant_t("sSickBed"));
				if(Value.vt != VT_NULL)
				{					
					strcpy(pTaskInfo[i].pat_info.sSickBed,_bstr_t(Value));
				}
				//dtBirthDate
				Value = rs_par.Rs->GetCollect(_variant_t("dtBirthDate"));
				if(Value.vt != VT_NULL)
				{					
					strcpy(pTaskInfo[i].pat_info.dtBirthDate,_bstr_t(Value));
				}
				

				// us_id
				Value= rs_par.Rs->GetCollect(_variant_t("tUs.nID"));
				if(Value.vt != VT_NULL)
					pTaskInfo[i].us_info.nID  = Value;
				// us_bUs			
				Value= rs_par.Rs->GetCollect(_variant_t("bUs"));
				if(Value.vt != VT_NULL)
					pTaskInfo[i].us_info.bUs  = Value;
				//nVariableRatio
				Value= rs_par.Rs->GetCollect(_variant_t("nVariableRatio"));
				if(Value.vt != VT_NULL)
					pTaskInfo[i].us_info.nVariableRatio  = Value;

				
				for(unsigned int j =1 ; j <= MAX_US_LENS ; j++)
				{
					
					char szBuf[MAX_PATH]={0};
					sprintf(szBuf,"us_res%d",j);					
					/*pTaskInfo[i].us_info.us_node[j].us_res = (float)rs_par.Rs->GetCollect(_variant_t(szBuf));*/

					Value= (float)rs_par.Rs->GetCollect(_variant_t(szBuf));
					if(Value.vt != VT_NULL)
						pTaskInfo[i].us_info.us_node[j - 1].us_res  = Value;

				}			

				// nID				
				Value= (int)rs_par.Rs->GetCollect(_variant_t("tUdc.nID"));
				if(Value.vt != VT_NULL)
					pTaskInfo[i].udc_info.nID  = Value;
				//bUdc
				Value= (int)rs_par.Rs->GetCollect(_variant_t("bUdc"));
				if(Value.vt != VT_NULL)
					pTaskInfo[i].udc_info.bUdc  = Value;
							
				for(unsigned int x = 1 ; x <= MAX_UDC_LENS ; x ++)
				{
					char szBuf[MAX_PATH] = {0} ;
					sprintf(szBuf,"udc_sRes%d",x);					

					Value = rs_par.Rs->GetCollect(_variant_t(szBuf));

					if(Value.vt != VT_NULL)
					{
						strcpy(pTaskInfo[i].udc_info.udc_node[x - 1].udc_res,_bstr_t(Value));
					}

					sprintf(szBuf,"sValue%d",x);
					Value = rs_par.Rs->GetCollect(_variant_t(szBuf));

					if(Value.vt != VT_NULL)
					{
						strcpy(pTaskInfo[i].udc_info.udc_node[x - 1].udc_value,_bstr_t(Value));
					}

					sprintf(szBuf,"bAbn%d",x);
					Value = (bool)rs_par.Rs->GetCollect(_variant_t(szBuf));
					if(Value.vt != VT_NULL)
						pTaskInfo[i].udc_info.udc_node[x - 1].udc_abn  = Value;

					sprintf(szBuf,"nColor%d",x);
					Value = (int)rs_par.Rs->GetCollect(_variant_t(szBuf));
					if(Value.vt != VT_NULL)
						pTaskInfo[i].udc_info.udc_node[x - 1].udc_col  = Value;
				}

				i++;

				status = STATUS_SUCCESS;
			}

		} // end for
	}
	catch (_com_error e)
	{
		status = STATUS_EXCEPTION_ERROR;
		DBG_MSG("GetTodayTaskInfo _com_error,errMessage = %s" ,
			(const char *)(LPCTSTR)e.Description());
	}

	// 关闭
	Close_Rs(rs_par.Rs);


	return status;

}

KSTATUS TH_DB_Access::ReSetTask( int nID,TestTypeEnum nTestType,ULONG nStatus )
{
	KSTATUS status = STATUS_SUCCESS;
	OPEN_RS_PAR rs_par;

	if(m_Connection == NULL)
	{
		status = STATUS_INVALID_PARAMETERS;
		return status;
	}

	sprintf(rs_par.sql,
	"SELECT * FROM ((tMain left outer join tUs on tMain.nID=tUs.nID) left outer join tUdc on tMain.nID=tUdc.nID) left outer join tPat on tMain.nID=tPat.nID where tMain.nID=%d",
	nID);

	try
	{
		status = Open_Rs(&rs_par);		

		if(status != STATUS_SUCCESS)
		{
			status = STATUS_INVALID_PARAMETERS;
			return status;
		}

		for (; !IsEOF(rs_par.Rs); rs_par.Rs->MoveNext())
		{
			if(!IsBOF(rs_par.Rs))
			{
				rs_par.Rs->MoveFirst();
			}

			rs_par.Rs->PutCollect(_variant_t("nState"),_variant_t(nStatus));			

			int bIsQC = rs_par.Rs->GetCollect(_variant_t("nAttribute"));			

			//DBG_MSG("%d\n",nTestType);

			//if(bIsQC)
			//{	
			//	rs_par.Rs->PutCollect(_variant_t("nQcType"),_variant_t(10));
			//	rs_par.Rs->PutCollect(_variant_t("nMaxvalue"),_variant_t(0));
			//	rs_par.Rs->PutCollect(_variant_t("nMinvalue"),_variant_t(0));
			//	rs_par.Rs->PutCollect(_variant_t("nGerValue"),_variant_t(0));
			//	rs_par.Rs->PutCollect(_variant_t("fArcgment"),_variant_t(0));
			//	// 有效时间未修改
			//}



			if(nTestType == 2)
			{
				for(int i = 1 ; i <= MAX_US_LENS ; i ++)
				{
					char szbuf[MAX_PATH]={0};

					sprintf(szbuf,"us_res%d",i);
					rs_par.Rs->PutCollect(_variant_t(szbuf),_variant_t(0));
				}

				rs_par.Rs->PutCollect(_variant_t("nVariableRatio"),_variant_t(0));

			}
			else if(nTestType == 1)
			{
				for(int i = 1 ; i <= MAX_UDC_LENS ; i ++)
				{
					char szbuf[MAX_PATH]={0};

					sprintf(szbuf,"udc_sRes%d",i);
					rs_par.Rs->PutCollect(_variant_t(szbuf),_variant_t(""));

					sprintf(szbuf,"bAbn%d",i);
					rs_par.Rs->PutCollect(_variant_t(szbuf),_variant_t(false));

					sprintf(szbuf,"nColor%d",i);
					rs_par.Rs->PutCollect(_variant_t(szbuf),_variant_t(0));

				}
			}
			else
			{
				for(int i = 1 ; i <= MAX_US_LENS ; i ++)
				{
					char szbuf[MAX_PATH]={0};

					sprintf(szbuf,"us_res%d",i);

					rs_par.Rs->PutCollect(_variant_t(szbuf),_variant_t(0));
				}
				rs_par.Rs->PutCollect(_variant_t("nVariableRatio"),_variant_t(0));

				for(int i = 1 ; i <= MAX_UDC_LENS ; i ++)
				{
					char szbuf[MAX_PATH]={0};

					sprintf(szbuf,"udc_sRes%d",i);
					rs_par.Rs->PutCollect(_variant_t(szbuf),_variant_t(""));

					sprintf(szbuf,"bAbn%d",i);
					rs_par.Rs->PutCollect(_variant_t(szbuf),_variant_t(false));

					sprintf(szbuf,"nColor%d",i);
					rs_par.Rs->PutCollect(_variant_t(szbuf),_variant_t(0));
				}
			}
			
		

			rs_par.Rs->Update();
		}
	}
	catch (_com_error e)
	{
		status = STATUS_EXCEPTION_ERROR;
		DBG_MSG("ReSetTask _com_error,errMessage = %s" ,
			(const char *)(LPCTSTR)e.Description());
	}

	// 关闭
	Close_Rs(rs_par.Rs);

	return status;

}



KSTATUS TH_DB_Access::GetInfo( TASK_INFO* pTaskInfo,ULONG nID,TYPE_INFO type )
{
	KSTATUS status = STATUS_SUCCESS;
	OPEN_RS_PAR rs_par;

	if(pTaskInfo == NULL)
	{
		status = STATUS_INVALID_PARAMETERS;
		return status;
	}

	if(m_Connection == NULL)
	{
		status = STATUS_INVALID_PARAMETERS;
		return status;
	}
	
		sprintf(rs_par.sql,
		"SELECT * FROM ((tMain left outer join tUs on tMain.nID=tUs.nID)\
		left outer join tUdc on tMain.nID=tUdc.nID) \
		left outer join tPat on tMain.nID=tPat.nID \
		where tMain.nID=%d",
		nID);

	try
	{
		status = Open_Rs(&rs_par);

		if(status != STATUS_SUCCESS)
		{
			status = STATUS_INVALID_PARAMETERS;
			return status;
		}

		if(IsEOF(rs_par.Rs) == TRUE)
		{
			status = STATUS_NOT_FOUND_VALUE;
			return status;
		}

		for (; !IsEOF(rs_par.Rs); rs_par.Rs->MoveNext())
		{
			if(!IsBOF(rs_par.Rs))
			{
				rs_par.Rs->MoveFirst();
			}

			_variant_t Value;

			if(type == USER_TYPE_INFO)
			{
				// ID
				Value = rs_par.Rs->GetCollect(_variant_t("tMain.nID"));
				if(Value.vt != VT_NULL)
					pTaskInfo->main_info.nID = Value;

				//Alarm
				Value = (int)rs_par.Rs->GetCollect(_variant_t("nAlarm"));
				if(Value.vt != VT_NULL)
					pTaskInfo->main_info.nAlarm = Value;

				//Row
				Value = (int)rs_par.Rs->GetCollect(_variant_t("nRow"));
				if(Value.vt != VT_NULL)
					pTaskInfo->main_info.nRow  = Value;

				//bMain				
				Value = rs_par.Rs->GetCollect(_variant_t("bMain"));
				if(Value.vt != VT_NULL)
					pTaskInfo->main_info.bMain  = Value;

				// nStatus			
				Value = rs_par.Rs->GetCollect(_variant_t("nState"));
				if(Value.vt != VT_NULL)
					pTaskInfo->main_info.nState  = Value;

				// nSN				
				Value = rs_par.Rs->GetCollect(_variant_t("nSN"));
				if(Value.vt != VT_NULL)
					pTaskInfo->main_info.nSN  = Value;

				// bIsQC					
				Value = (int)rs_par.Rs->GetCollect(_variant_t("nAttribute"));
				if(Value.vt != VT_NULL)
					pTaskInfo->main_info.nAttribute  = Value;

				//DBG_MSG("pTaskInfo->main_info.nAttribute = %d\n",pTaskInfo->main_info.nAttribute);

				//TestType				
				Value = (int)rs_par.Rs->GetCollect(_variant_t("nTestType"));
				if(Value.vt != VT_NULL)
					pTaskInfo->main_info.nTestType  = Value;

				//Pos
				Value = (int)rs_par.Rs->GetCollect(_variant_t("nPos"));
				if(Value.vt != VT_NULL)
					pTaskInfo->main_info.nPos  = Value;

				//Cha				
				Value = (int)rs_par.Rs->GetCollect(_variant_t("nCha"));
				if(Value.vt != VT_NULL)
					pTaskInfo->main_info.nCha  = Value;

				//Code
				Value = rs_par.Rs->GetCollect(_variant_t("sCode"));				
				if(Value.vt != VT_NULL)
				{					
					//strcpy(pTaskInfo->main_info.sCode,_bstr_t(Value));
					StringCbCopy(pTaskInfo->main_info.sCode,MAX_BARCODE_LENS,_bstr_t(Value));
				}
				//Company
				Value = rs_par.Rs->GetCollect(_variant_t("sCompany"));
				if(Value.vt != VT_NULL)
				{					
					//strcpy(pTaskInfo->main_info.sCompany,_bstr_t(Value));
					StringCbCopy(pTaskInfo->main_info.sCompany,MAX_COMPANY_LENS,_bstr_t(Value));
				}
				//Department
				Value = rs_par.Rs->GetCollect(_variant_t("sDepartment"));
				if(Value.vt != VT_NULL)
				{					
					//strcpy(pTaskInfo->main_info.sDepartment,_bstr_t(Value));
					StringCbCopy(pTaskInfo->main_info.sDepartment,MAX_DEPARTMENT_LENS,_bstr_t(Value));
				}
				//Doctor
				Value = rs_par.Rs->GetCollect(_variant_t("sDoctor"));
				if(Value.vt != VT_NULL)
				{					
					//strcpy(pTaskInfo->main_info.sDoctor,_bstr_t(Value));
					StringCbCopy(pTaskInfo->main_info.sDoctor,MAX_DOCTOR_LENS,_bstr_t(Value));
				}
				//sDocimaster
				Value = rs_par.Rs->GetCollect(_variant_t("sDocimaster"));
				if(Value.vt != VT_NULL)
				{					
					strcpy(pTaskInfo->main_info.sDocimaster,_bstr_t(Value));

				}
				//sAuditor
				Value = rs_par.Rs->GetCollect(_variant_t("sAuditor"));
				if(Value.vt != VT_NULL)
				{					
					strcpy(pTaskInfo->main_info.sAuditor,_bstr_t(Value));
				}
				//sReporter
				Value = rs_par.Rs->GetCollect(_variant_t("sReporter"));
				if(Value.vt != VT_NULL)
				{					
					strcpy(pTaskInfo->main_info.sReporter,_bstr_t(Value));
				}
				//sColor
				Value = rs_par.Rs->GetCollect(_variant_t("sColor"));
				if(Value.vt != VT_NULL)
				{					
					strcpy(pTaskInfo->main_info.sColor,_bstr_t(Value));
				}
				//sTransparency
				Value = rs_par.Rs->GetCollect(_variant_t("sTransparency"));
				if(Value.vt != VT_NULL)
				{					
					strcpy(pTaskInfo->main_info.sTransparency,_bstr_t(Value));
				}
				//sFolder
				Value = rs_par.Rs->GetCollect(_variant_t("sFolder"));
				if(Value.vt != VT_NULL)
				{					
					strcpy(pTaskInfo->main_info.sFolder,_bstr_t(Value));
				}
				//sImage
				Value = rs_par.Rs->GetCollect(_variant_t("sImage"));
				if(Value.vt != VT_NULL)
				{					
					strcpy(pTaskInfo->main_info.sImage,_bstr_t(Value));
				}
				//sDiagnosis
				Value = rs_par.Rs->GetCollect(_variant_t("sDiagnosis"));
				if(Value.vt != VT_NULL)
				{					
					strcpy(pTaskInfo->main_info.sDiagnosis,_bstr_t(Value));
				}
				//sSuggestion
				Value = rs_par.Rs->GetCollect(_variant_t("sSuggestion"));
				if(Value.vt != VT_NULL)
				{					
					strcpy(pTaskInfo->main_info.sSuggestion,_bstr_t(Value));
				}
				//dtDate
				Value = rs_par.Rs->GetCollect(_variant_t("dtDate"));
				if(Value.vt != VT_NULL)
				{					
					COleDateTime odt(Value.date);
					strcpy(pTaskInfo->main_info.dtDate, odt.Format("%Y-%m-%d %H:%M:%S").GetBuffer(0));
				}

				//dtPrintStatus
				Value = rs_par.Rs->GetCollect(_variant_t("dtPrintStatus"));
				if(Value.vt != VT_NULL)
					pTaskInfo->main_info.dtPrintStatus  = Value;
				

				//dtLisStatus
				Value = rs_par.Rs->GetCollect(_variant_t("dtLisStatus"));
				if(Value.vt != VT_NULL)
					pTaskInfo->main_info.dtLisStatus  = Value;

				//pat_ID				
				Value = rs_par.Rs->GetCollect(_variant_t("tPat.nID"));
				if(Value.vt != VT_NULL)
					pTaskInfo->pat_info.nID  = Value;

				//bPat
				
				Value = rs_par.Rs->GetCollect(_variant_t("bPat"));
				if(Value.vt != VT_NULL)
					pTaskInfo->pat_info.bPat  = Value;

				//nNo				
				Value = rs_par.Rs->GetCollect(_variant_t("nNo"));
				if(Value.vt != VT_NULL)
					pTaskInfo->pat_info.nNo  = Value;

				//sName
				Value = rs_par.Rs->GetCollect(_variant_t("sName"));
				if(Value.vt != VT_NULL)
				{					
					strcpy(pTaskInfo->pat_info.sName,_bstr_t(Value));
					//DBG_MSG("DB pTaskInfo->pat_info.sName = %s\n",pTaskInfo->pat_info.sName);
				}

				//nSex
				Value = rs_par.Rs->GetCollect(_variant_t("nSex"));
				if(Value.vt != VT_NULL)
					pTaskInfo->pat_info.nSex  = Value;

				//nAge				
				Value = rs_par.Rs->GetCollect(_variant_t("nAge"));
				if(Value.vt != VT_NULL)
					pTaskInfo->pat_info.nAge  = Value;

				//nAgeUnit				
				Value = rs_par.Rs->GetCollect(_variant_t("nAgeUnit"));
				if(Value.vt != VT_NULL)
					pTaskInfo->pat_info.nAgeUnit  = Value;

				//sHospital
				Value = rs_par.Rs->GetCollect(_variant_t("sHospital"));
				if(Value.vt != VT_NULL)
				{					
					strcpy(pTaskInfo->pat_info.sHospital,_bstr_t(Value));
				}
				//sSickBed
				Value = rs_par.Rs->GetCollect(_variant_t("sSickBed"));
				if(Value.vt != VT_NULL)
				{					
					strcpy(pTaskInfo->pat_info.sSickBed,_bstr_t(Value));
				}
				//dtBirthDate
				Value = rs_par.Rs->GetCollect(_variant_t("dtBirthDate"));
				if(Value.vt != VT_NULL)
				{					
					//strcpy(pTaskInfo->pat_info.dtBirthDate,_bstr_t(Value));
					StringCbCopy(pTaskInfo->pat_info.dtBirthDate,MAX_DATA_LENGTH,_bstr_t(Value)); 
				}

			}
			else if(type == US_TYPE_INFO)
			{
				//ID				
				Value = rs_par.Rs->GetCollect(_variant_t("tUs.nID"));
				if(Value.vt != VT_NULL)
					pTaskInfo->us_info.nID  = Value;

				//bUs				
				Value = rs_par.Rs->GetCollect(_variant_t("bUs"));
				if(Value.vt != VT_NULL)
					pTaskInfo->us_info.bUs  = Value;

				//nVariableRatio				
				Value = rs_par.Rs->GetCollect(_variant_t("nVariableRatio"));
				if(Value.vt != VT_NULL)
					pTaskInfo->us_info.nVariableRatio  = Value;

				/*if(pTaskInfo->us_info.bUs)
				{*/
					int nUsIndex = 1;
					for(unsigned int j =0 ; j < MAX_US_LENS ; j++)
					{
						char szBuf[MAX_PATH]={0};
						sprintf(szBuf,"us_res%d",nUsIndex);
						nUsIndex++;

						/*pTaskInfo->us_info.us_node[j].us_res = (float)rs_par.Rs->GetCollect(_variant_t(szBuf));*/

						Value = (float)rs_par.Rs->GetCollect(_variant_t(szBuf));
						if(Value.vt != VT_NULL)
							pTaskInfo->us_info.us_node[j].us_res  = Value;
					}
				/*}*/
			}
			else if(type == UDC_TYPE_INFO)
			{
				// nID				
				Value = (int)rs_par.Rs->GetCollect(_variant_t("tUdc.nID"));
				if(Value.vt != VT_NULL)
					pTaskInfo->udc_info.nID  = Value;

				//bUdc					
				Value = (int)rs_par.Rs->GetCollect(_variant_t("bUdc"));
				if(Value.vt != VT_NULL)
					pTaskInfo->udc_info.bUdc  = Value;

				/*if(pTaskInfo->udc_info.bUdc)
				{*/
					int nUdcIndex = 1;
					for(unsigned int x = 0 ; x < MAX_UDC_LENS ; x ++)
					{
						char szBuf[MAX_PATH] = {0} ;
						sprintf(szBuf,"udc_sRes%d",nUdcIndex);
						

						Value = rs_par.Rs->GetCollect(_variant_t(szBuf));

						if(Value.vt != VT_NULL)
						{
							strcpy(pTaskInfo->udc_info.udc_node[x].udc_res,_bstr_t(Value));
						}

						sprintf(szBuf,"sValue%d",nUdcIndex);
						Value = rs_par.Rs->GetCollect(_variant_t(szBuf));

						if(Value.vt != VT_NULL)
						{
							strcpy(pTaskInfo->udc_info.udc_node[x].udc_value,_bstr_t(Value));
						}

						sprintf(szBuf,"bAbn%d",nUdcIndex);
						Value = (bool)rs_par.Rs->GetCollect(_variant_t(szBuf));
						if(Value.vt != VT_NULL)
							pTaskInfo->udc_info.udc_node[x].udc_abn  = Value;

						sprintf(szBuf,"nColor%d",nUdcIndex);
						Value = (int)rs_par.Rs->GetCollect(_variant_t(szBuf));
						if(Value.vt != VT_NULL)
							pTaskInfo->udc_info.udc_node[x].udc_col  = Value;

						nUdcIndex++;
					}
				/*}*/
			}
			else
			{
			}

		}
	}
	catch (_com_error e)
	{
		status = STATUS_EXCEPTION_ERROR;
		DBG_MSG("GetTask _com_error,errMessage = %s" ,
			(const char *)(LPCTSTR)e.Description());
	}

	// 关闭
	Close_Rs(rs_par.Rs);


	return status;

}

KSTATUS TH_DB_Access::SetInfo( TASK_INFO* pTaskInfo,ULONG nID,TYPE_INFO type )
{
	KSTATUS status = STATUS_SUCCESS;
	OPEN_RS_PAR rs_par;


	if(m_Connection == NULL)
	{
		status = STATUS_INVALID_PARAMETERS;
		return status;
	}


	sprintf(rs_par.sql,
		"SELECT * FROM ((tMain left outer join tUs on tMain.nID=tUs.nID)\
		left outer join tUdc on tMain.nID=tUdc.nID) \
		left outer join tPat on tMain.nID=tPat.nID \
		where tMain.nID=%d",
		nID);

	try
	{
		status = Open_Rs(&rs_par);

		if(status != STATUS_SUCCESS)
		{
			status = STATUS_INVALID_PARAMETERS;
			return status;
		}

		for (; !IsEOF(rs_par.Rs); rs_par.Rs->MoveNext())
		{
			if(!IsBOF(rs_par.Rs))
			{
				rs_par.Rs->MoveFirst();
			}

			if(type == USER_TYPE_INFO)
			{

DBG_MSG("USER_INFO\n");
				
				// nAlarm
				if(pTaskInfo->main_info.nAlarm >= 0)
					rs_par.Rs->PutCollect(_variant_t("nAlarm"),_variant_t(pTaskInfo->main_info.nAlarm));

				// nRow
				if(pTaskInfo->main_info.nRow >= 0)
					rs_par.Rs->PutCollect(_variant_t("nRow"),_variant_t(pTaskInfo->main_info.nRow));

				// bMain
				rs_par.Rs->PutCollect(_variant_t("bMain"),_variant_t(pTaskInfo->main_info.bMain));

				// nState
				if(pTaskInfo->main_info.nState >= 0)
					rs_par.Rs->PutCollect(_variant_t("nState"),_variant_t(pTaskInfo->main_info.nState));

				// nAttribute
				if(pTaskInfo->main_info.nAttribute >= 0)
					rs_par.Rs->PutCollect(_variant_t("nAttribute"),_variant_t(pTaskInfo->main_info.nAttribute));

				// nTestType
				if(pTaskInfo->main_info.nTestType >= 0)
					rs_par.Rs->PutCollect(_variant_t("nTestType"),_variant_t(pTaskInfo->main_info.nTestType));

				// nSN
				if(pTaskInfo->main_info.nSN >= 0)
					rs_par.Rs->PutCollect(_variant_t("nSN"),_variant_t(pTaskInfo->main_info.nSN));

				// nPos
				if(pTaskInfo->main_info.nPos >= 0)
					rs_par.Rs->PutCollect(_variant_t("nPos"),_variant_t(pTaskInfo->main_info.nPos));

				// nCha
				if(pTaskInfo->main_info.nCha >= 0)
					rs_par.Rs->PutCollect(_variant_t("nCha"),_variant_t(pTaskInfo->main_info.nCha));

				// sCode
				if(strlen(pTaskInfo->main_info.sCode) > 0 && strlen(pTaskInfo->main_info.sCode) <= MAX_BARCODE_LENS)
					rs_par.Rs->PutCollect(_variant_t("sCode"),_variant_t(pTaskInfo->main_info.sCode));

				// sCompany
				if(strlen(pTaskInfo->main_info.sCompany) > 0 && strlen(pTaskInfo->main_info.sCompany) <= MAX_COMPANY_LENS)
					rs_par.Rs->PutCollect(_variant_t("sCompany"),_variant_t(pTaskInfo->main_info.sCompany));

				// sDepartment
				if(strlen(pTaskInfo->main_info.sDepartment) > 0 && strlen(pTaskInfo->main_info.sDepartment) <= MAX_DEPARTMENT_LENS)
					rs_par.Rs->PutCollect(_variant_t("sDepartment"),_variant_t(pTaskInfo->main_info.sDepartment));

				// sDoctor
				if(strlen(pTaskInfo->main_info.sDoctor) > 0 && strlen(pTaskInfo->main_info.sDoctor) <= MAX_DOCTOR_LENS)
					rs_par.Rs->PutCollect(_variant_t("sDoctor"),_variant_t(pTaskInfo->main_info.sDoctor));
				
				//sDocimaster
				if(strlen(pTaskInfo->main_info.sDocimaster) > 0 && strlen(pTaskInfo->main_info.sDocimaster) <= MAX_DOCTOR_LENS)
					rs_par.Rs->PutCollect(_variant_t("sDocimaster"),_variant_t(pTaskInfo->main_info.sDocimaster));

				// sAuditor
				if(strlen(pTaskInfo->main_info.sAuditor) > 0 && strlen(pTaskInfo->main_info.sAuditor)  <= MAX_DOCTOR_LENS)
					rs_par.Rs->PutCollect(_variant_t("sAuditor"),_variant_t(pTaskInfo->main_info.sAuditor));

				// sReporter
				if(strlen(pTaskInfo->main_info.sReporter) > 0 && strlen(pTaskInfo->main_info.sReporter) <= MAX_DOCTOR_LENS)
					rs_par.Rs->PutCollect(_variant_t("sReporter"),_variant_t(pTaskInfo->main_info.sReporter));

				// sColor
				if(strlen(pTaskInfo->main_info.sColor) > 0 && strlen(pTaskInfo->main_info.sColor) <= 32)
					rs_par.Rs->PutCollect(_variant_t("sColor"),_variant_t(pTaskInfo->main_info.sColor));

				// sTransparency
				if(strlen(pTaskInfo->main_info.sTransparency) > 0 && strlen(pTaskInfo->main_info.sTransparency) <= 32)
					rs_par.Rs->PutCollect(_variant_t("sTransparency"),_variant_t(pTaskInfo->main_info.sTransparency));

				// sFolder
				if(strlen(pTaskInfo->main_info.sFolder) > 0 && strlen(pTaskInfo->main_info.sFolder) <= MAX_IMAGEFOLDER_LENS)
					rs_par.Rs->PutCollect(_variant_t("sFolder"),_variant_t(pTaskInfo->main_info.sFolder));

				// sImage
				if(strlen(pTaskInfo->main_info.sImage) > 0 && strlen(pTaskInfo->main_info.sImage) <= MAX_PRINTIMAGE_LENS)
					rs_par.Rs->PutCollect(_variant_t("sImage"),_variant_t(pTaskInfo->main_info.sImage));

				// sDiagnosis
				if(strlen(pTaskInfo->main_info.sDiagnosis) > 0 && strlen(pTaskInfo->main_info.sDiagnosis) <= MAX_DIAGNOSIS_LENS)
					rs_par.Rs->PutCollect(_variant_t("sDiagnosis"),_variant_t(pTaskInfo->main_info.sDiagnosis));

				// sSuggestion
				if(strlen(pTaskInfo->main_info.sSuggestion) > 0 && strlen(pTaskInfo->main_info.sSuggestion) <= MAX_COMMENT_LENS)
					rs_par.Rs->PutCollect(_variant_t("sSuggestion"),_variant_t(pTaskInfo->main_info.sSuggestion));

				// dtDate

				if(strlen(pTaskInfo->main_info.dtDate) > 0  && strlen(pTaskInfo->main_info.dtDate) <= MAX_DATA_LENS)
					rs_par.Rs->PutCollect(_variant_t("dtDate"),_variant_t(pTaskInfo->main_info.dtDate));
			
		DBG_MSG("cc1");

				//dtPrintStatus
				rs_par.Rs->PutCollect(_variant_t("dtPrintStatus"),_variant_t(pTaskInfo->main_info.dtPrintStatus));

				//dtLisStatus
				rs_par.Rs->PutCollect(_variant_t("dtLisStatus"),_variant_t(pTaskInfo->main_info.dtLisStatus));

					DBG_MSG("cc2");
				// bPat
				rs_par.Rs->PutCollect(_variant_t("bPat"),_variant_t(pTaskInfo->pat_info.bPat));
				// nNo
				if(pTaskInfo->pat_info.nNo >= 0)
					rs_par.Rs->PutCollect(_variant_t("nNo"),_variant_t(pTaskInfo->pat_info.nNo));
				
				// sName
				if(strlen(pTaskInfo->pat_info.sName) > 0 && strlen(pTaskInfo->pat_info.sName) <= MAX_USER_NAME_LENS)
					rs_par.Rs->PutCollect(_variant_t("sName"),_variant_t(pTaskInfo->pat_info.sName));
				
				// nSex
				if(pTaskInfo->pat_info.nSex >= 0)
					rs_par.Rs->PutCollect(_variant_t("nSex"),_variant_t(pTaskInfo->pat_info.nSex));
				// nAge
				if(pTaskInfo->pat_info.nAge >= 0)
					rs_par.Rs->PutCollect(_variant_t("nAge"),_variant_t(pTaskInfo->pat_info.nAge));
				// nAgeUnit
				if(pTaskInfo->pat_info.nAgeUnit >= 0)
					rs_par.Rs->PutCollect(_variant_t("nAgeUnit"),_variant_t(pTaskInfo->pat_info.nAgeUnit));
				// sHospital
				if(strlen(pTaskInfo->pat_info.sHospital) > 0 && strlen(pTaskInfo->pat_info.sHospital) <= MAX_COMPANY_LENS)
					rs_par.Rs->PutCollect(_variant_t("sHospital"),_variant_t(pTaskInfo->pat_info.sHospital));
				// sSickBed
				if(strlen(pTaskInfo->pat_info.sSickBed) > 0 && strlen(pTaskInfo->pat_info.sSickBed) <= MAX_SICKBED_LENS)
					rs_par.Rs->PutCollect(_variant_t("sSickBed"),_variant_t(pTaskInfo->pat_info.sSickBed));
		//DBG_MSG("dtBirthDate\n");
				// dtBirthDate	
				if(strlen(pTaskInfo->pat_info.dtBirthDate) > 0 && strlen(pTaskInfo->pat_info.dtBirthDate) <= MAX_DATA_LENS)
					rs_par.Rs->PutCollect(_variant_t("dtBirthDate"),_variant_t(pTaskInfo->pat_info.dtBirthDate));			
			

				rs_par.Rs->Update();

				//DBG_MSG("USER_INFO_end\n");

			}
			else if(type == US_TYPE_INFO)
			{
				DBG_MSG("US_INFO\n");

				rs_par.Rs->PutCollect(_variant_t("bUs"),_variant_t(pTaskInfo->us_info.bUs));

				if(pTaskInfo->us_info.nVariableRatio >= 0)
					rs_par.Rs->PutCollect(_variant_t("nVariableRatio"),_variant_t(pTaskInfo->us_info.nVariableRatio));
						
				int nUsIndex = 1;
				for(unsigned int j =0 ; j < MAX_US_LENS ; j++)
				{
					char szBuf[MAX_PATH]={0};
					char usRes[MAX_PATH]={0}; 
					sprintf(szBuf,"us_res%d",nUsIndex);
					nUsIndex++;	

					//DBG_MSG("pTaskInfo->us_info.us_node[%d].us_res=%f\n",j,pTaskInfo->us_info.us_node[j].us_res);
					if(pTaskInfo->us_info.us_node[j].us_res >= 0)
					{						
						sprintf(usRes,"%.2f",pTaskInfo->us_info.us_node[j].us_res );
						rs_par.Rs->PutCollect(_variant_t(szBuf),_variant_t(usRes));
						//rs_par.Rs->PutCollect(_variant_t(szBuf),_variant_t(pTaskInfo->us_info.us_node[j].us_res));
					}
						
				}

				rs_par.Rs->Update();
				//DBG_MSG("US_INFO_END\n");
			}
			else if(type == UDC_TYPE_INFO)
			{
				DBG_MSG("UDC_INFO\n");
				
				if(pTaskInfo->udc_info.bUdc >= 0)
					rs_par.Rs->PutCollect(_variant_t("bUdc"),_variant_t(pTaskInfo->udc_info.bUdc));
				int nUdcIndex = 1;
				for(unsigned int x = 0 ; x < MAX_UDC_LENS ; x ++)
				{
					char szBuf[MAX_PATH] = {0} ;
					sprintf(szBuf,"udc_sRes%d",nUdcIndex);
					

					if(strlen(pTaskInfo->udc_info.udc_node[x].udc_res) > 0 && strlen(pTaskInfo->udc_info.udc_node[x].udc_res) <= MAX_UDC_RESULT_LENS)
						rs_par.Rs->PutCollect(_variant_t(szBuf),_variant_t(pTaskInfo->udc_info.udc_node[x].udc_res));

					sprintf(szBuf,"sValue%d",nUdcIndex);
					if(strlen(pTaskInfo->udc_info.udc_node[x].udc_value) > 0 && strlen(pTaskInfo->udc_info.udc_node[x].udc_value) <= MAX_UDC_RESULT_LENS)
						rs_par.Rs->PutCollect(_variant_t(szBuf),_variant_t(pTaskInfo->udc_info.udc_node[x].udc_value));
					
					sprintf(szBuf,"bAbn%d",nUdcIndex);
					rs_par.Rs->PutCollect(_variant_t(szBuf),_variant_t(pTaskInfo->udc_info.udc_node[x].udc_abn));

					sprintf(szBuf,"nColor%d",nUdcIndex);
					rs_par.Rs->PutCollect(_variant_t(szBuf),_variant_t(pTaskInfo->udc_info.udc_node[x].udc_col));
				
					
					nUdcIndex++;
				} // end for

				rs_par.Rs->Update();

				//DBG_MSG("UDC_INFO_END\n");
			}
			else
			{
			}

		}
	}
	catch (_com_error e)
	{
		status = STATUS_EXCEPTION_ERROR;
		DBG_MSG("SetTaskInfo _com_error,errMessage = %s" ,
			(const char *)(LPCTSTR)e.Description());
	}

	// 关闭
	Close_Rs(rs_par.Rs);



	return status;

}

KSTATUS TH_DB_Access::GetUsInfo( GET_US_INFO* pGetUsInfo )
{
	KSTATUS status = STATUS_SUCCESS;
	pGetUsInfo->nUsCounts = 0;


	CIniParser IniObj;
	IniObj.Initialize(m_nConfig_IniFile);

	READ_PAR read_par;
	memset(&read_par,0,sizeof(read_par));


	// 读取US总条数
	read_par.bIsString = false;
	read_par.bIsfloat  = false;
	strcpy(read_par.sAppName,CONFIG_INI_US);
	strcpy(read_par.sKeyName,CONFIG_INI_US_COUNTS);

	status = IniObj.Read(&read_par);
	if(status != STATUS_SUCCESS)
	{
		status = STATUS_INVALID_PARAMETERS;
		return status;
	}
	if(read_par.nValue == 0)
	{
		status = STATUS_INVALID_PARAMETERS;
		return status;
	}
	else
	{
		pGetUsInfo->nUsCounts = read_par.nValue;		

		int nIndex = 1;
		for(unsigned int i = 0 ; i < pGetUsInfo->nUsCounts ; i ++)
		{
			char szBuf[MAX_PATH]={0};
			memset(&read_par,0,sizeof(read_par));
			read_par.bIsString = false;
			read_par.bIsfloat  = false;

			// INDEX
			sprintf(szBuf,"us_particle%d",nIndex);
			strcpy(read_par.sAppName,szBuf);
			strcpy(read_par.sKeyName,CONFIG_INI_INDEX);

			IniObj.Read(&read_par);
			pGetUsInfo->par[i].nIndex = read_par.nValue;

			
			// male_normal_MAX
			memset(&read_par,0,sizeof(read_par));
			read_par.bIsString = false;
			read_par.bIsfloat  = false;
            strcpy(read_par.sAppName, szBuf);
			strcpy(read_par.sKeyName,CONFIG_INI_MAX_NORMAL_MALE);

			IniObj.Read(&read_par);
			pGetUsInfo->par[i].nMax_normal_male = read_par.nValue;

			// male_normal_MIN
			memset(&read_par,0,sizeof(read_par));
			read_par.bIsString = false;
			read_par.bIsfloat  = false;
            strcpy(read_par.sAppName, szBuf);
			strcpy(read_par.sKeyName,CONFIG_INI_MIN_NORMAL_MALE);

			IniObj.Read(&read_par);
			pGetUsInfo->par[i].nMin_normal_male = read_par.nValue;

			// nMax_normal_female
			memset(&read_par,0,sizeof(read_par));
			read_par.bIsString = false;
			read_par.bIsfloat  = false;
            strcpy(read_par.sAppName, szBuf);
			strcpy(read_par.sKeyName,CONFIG_INI_MAX_NORMAL_FEMALE);

			IniObj.Read(&read_par);
			pGetUsInfo->par[i].nMax_normal_female = read_par.nValue;

			// nMin_normal_female
			memset(&read_par,0,sizeof(read_par));
			read_par.bIsString = false;
			read_par.bIsfloat  = false;
            strcpy(read_par.sAppName, szBuf);
			strcpy(read_par.sKeyName,CONFIG_INI_MIN_NORMAL_FEMALE);

			IniObj.Read(&read_par);
			pGetUsInfo->par[i].nMin_normal_female = read_par.nValue;

			// nMax_unusual_male
			memset(&read_par,0,sizeof(read_par));
			read_par.bIsString = false;
			read_par.bIsfloat  = false;
            strcpy(read_par.sAppName, szBuf);
			strcpy(read_par.sKeyName,CONFIG_INI_MAX_UNUSUAL_MALE);

			IniObj.Read(&read_par);
			pGetUsInfo->par[i].nMax_unusual_male = read_par.nValue;

			//nMin_unusual_male
			memset(&read_par,0,sizeof(read_par));
			read_par.bIsString = false;
			read_par.bIsfloat  = false;
            strcpy(read_par.sAppName, szBuf);
			strcpy(read_par.sKeyName,CONFIG_INI_MIN_UNUSUAL_MALE);

			IniObj.Read(&read_par);
			pGetUsInfo->par[i].nMin_unusual_male = read_par.nValue;

			//nMax_unusual_female
			memset(&read_par,0,sizeof(read_par));
			read_par.bIsString = false;
			read_par.bIsfloat  = false;
            strcpy(read_par.sAppName, szBuf);
			strcpy(read_par.sKeyName,CONFIG_INI_MAX_UNUSUAL_FEMALE);

			IniObj.Read(&read_par);
			pGetUsInfo->par[i].nMax_unusual_female = read_par.nValue;

			//nMin_unusual_female
			memset(&read_par,0,sizeof(read_par));
			read_par.bIsString = false;
			read_par.bIsfloat  = false;
            strcpy(read_par.sAppName, szBuf);
			strcpy(read_par.sKeyName,CONFIG_INI_MIN_UNUSUAL_FEMALE);

			IniObj.Read(&read_par);
			pGetUsInfo->par[i].nMin_unusual_female = read_par.nValue;

			//bIsSendtoLIS
			memset(&read_par,0,sizeof(read_par));
			read_par.bIsString = false;
			read_par.bIsfloat  = false;
            strcpy(read_par.sAppName, szBuf);
			strcpy(read_par.sKeyName,CONFIG_INI_ISTOLIS);

			IniObj.Read(&read_par);

			if( read_par.nValue >=1)
			{
				pGetUsInfo->par[i].bIsSendtoLIS =true;
			}
			else
			{
				pGetUsInfo->par[i].bIsSendtoLIS =false;
			}

			//bIsShowUI
			memset(&read_par,0,sizeof(read_par));
			read_par.bIsString = false;
			read_par.bIsfloat  = false;
            strcpy(read_par.sAppName, szBuf);
			strcpy(read_par.sKeyName,CONFIG_INI_ISSHOWUI);

			IniObj.Read(&read_par);

			if(read_par.nValue >= 1)
			{
				pGetUsInfo->par[i].bIsShowUI = true;
			}
			else
			{
				pGetUsInfo->par[i].bIsShowUI = false;
			}

			// bIsSendtoReport
			memset(&read_par,0,sizeof(read_par));
			read_par.bIsString = false;
			read_par.bIsfloat  = false;
            strcpy(read_par.sAppName, szBuf);
			strcpy(read_par.sKeyName,CONFIG_INI_ISSENDTOREPORT);

			IniObj.Read(&read_par);

			if(read_par.nValue >= 1)
			{
				pGetUsInfo->par[i].bIsSendtoReport = true;
			}
			else
			{
				pGetUsInfo->par[i].bIsSendtoReport = false;
			}


			// bIsDelete
			memset(&read_par,0,sizeof(read_par));
			read_par.bIsString = false;
			read_par.bIsfloat  = false;
            strcpy(read_par.sAppName, szBuf);
			strcpy(read_par.sKeyName,CONFIG_INI_ISDELETE);

			IniObj.Read(&read_par);

			if(read_par.nValue >= 1)
			{
				pGetUsInfo->par[i].bIsDelete = true;
			}
			else
			{
				pGetUsInfo->par[i].bIsDelete = false;
			}


			// fArgument
			memset(&read_par,0,sizeof(read_par));
			read_par.bIsString = false;
			read_par.bIsfloat  = true;
            strcpy(read_par.sAppName, szBuf);
			strcpy(read_par.sKeyName,CONFIG_INI_ARGUMENT);

			IniObj.Read(&read_par);
			pGetUsInfo->par[i].fArgument = read_par.fValue;

			// nUnit
			memset(&read_par,0,sizeof(read_par));
			read_par.bIsString = false;
			read_par.bIsfloat  = false;
            strcpy(read_par.sAppName, szBuf);
			strcpy(read_par.sKeyName,CONFIG_INI_UNIT);

			IniObj.Read(&read_par);
			pGetUsInfo->par[i].nUnit = read_par.nValue;

			// sShortName
			memset(&read_par,0,sizeof(read_par));
			read_par.bIsString = true;
			read_par.bIsfloat  = false;
            strcpy(read_par.sAppName, szBuf);
			strcpy(read_par.sKeyName,CONFIG_INI_SHORTNAME);

			IniObj.Read(&read_par);
			strcpy(pGetUsInfo->par[i].sShortName, read_par.sString);

			// sLongName
			memset(&read_par,0,sizeof(read_par));
			read_par.bIsString = true;
			read_par.bIsfloat  = false;
            strcpy(read_par.sAppName, szBuf);
			strcpy(read_par.sKeyName,CONFIG_INI_LONGNAME);

			IniObj.Read(&read_par);
			strcpy(pGetUsInfo->par[i].sLongName, read_par.sString);

			//m_CellNameColoer_R
			memset(&read_par,0,sizeof(read_par));
			read_par.bIsString = true;
			read_par.bIsfloat  = false;
            strcpy(read_par.sAppName, szBuf);
			strcpy(read_par.sKeyName,CONFIG_INI_CELLNAMECOLOR_R);

			IniObj.Read(&read_par);
			strcpy(pGetUsInfo->par[i].m_CellNameColoer_R, read_par.sString);
			/*pGetUsInfo->par[i].m_CellNameColoer = read_par.fValue;*/
			//DBG_MSG("TH_DB_Access::pGetUsInfo->par[%d].m_CellNameColoer_R = %s\n",i,pGetUsInfo->par[i].m_CellNameColoer_R);


			//m_CellNameColoer_G
			memset(&read_par,0,sizeof(read_par));
			read_par.bIsString = true;
			read_par.bIsfloat  = false;
            strcpy(read_par.sAppName, szBuf);
			strcpy(read_par.sKeyName,CONFIG_INI_CELLNAMECOLOR_G);

			IniObj.Read(&read_par);
			strcpy(pGetUsInfo->par[i].m_CellNameColoer_G, read_par.sString);
			/*pGetUsInfo->par[i].m_CellNameColoer = read_par.fValue;*/
			//DBG_MSG("TH_DB_Access::pGetUsInfo->par[%d].m_CellNameColoer_G = %s\n",i,pGetUsInfo->par[i].m_CellNameColoer_G);
			
			//m_CellNameColoer_B
			memset(&read_par,0,sizeof(read_par));
			read_par.bIsString = true;
			read_par.bIsfloat  = false;
            strcpy(read_par.sAppName, szBuf);
			strcpy(read_par.sKeyName,CONFIG_INI_CELLNAMECOLOR_B);

			IniObj.Read(&read_par);
			strcpy(pGetUsInfo->par[i].m_CellNameColoer_B, read_par.sString);
			/*pGetUsInfo->par[i].m_CellNameColoer = read_par.fValue;*/
			//DBG_MSG("TH_DB_Access::pGetUsInfo->par[%d].m_CellNameColoer_B = %s\n",i,pGetUsInfo->par[i].m_CellNameColoer_B);

            //m_reliability 
            memset(&read_par, 0, sizeof(read_par));
            read_par.bIsString = false;
            read_par.bIsfloat = false;
            strcpy(read_par.sAppName, szBuf);
            strcpy(read_par.sKeyName, CONFIG_INI_RELIABILITY);

            IniObj.Read(&read_par);
            pGetUsInfo->par[i].m_reliability = read_par.nValue;
            if (pGetUsInfo->par[i].m_reliability < 0)
            {
                pGetUsInfo->par[i].m_reliability = 0;
            }
            else if (pGetUsInfo->par[i].m_reliability > 100)
            {
                pGetUsInfo->par[i].m_reliability = 100;
            }
			
			nIndex ++;
		}// end for
	}

	//showframe
	memset(&read_par,0,sizeof(read_par));
	read_par.bIsString = false;
	read_par.bIsfloat  = false;	
	strcpy(read_par.sAppName,CONFIG_INI_US);
	strcpy(read_par.sKeyName,CONFIG_INI_SHOWCELLFRAME);
	IniObj.Read(&read_par);
	if(read_par.nValue >= 1)
		pGetUsInfo->bIsShowFrame = true;
	else
		pGetUsInfo->bIsShowFrame = false;
	//DBG_MSG("TH_DB_Access::pGetUsInfo->bIsShowFrame = %d\n",pGetUsInfo->bIsShowFrame);


	return status;

}


KSTATUS TH_DB_Access::SetUsInfo( SET_US_INFO set_us_info )
{
	KSTATUS status = STATUS_SUCCESS;

	if(set_us_info.nUsCounts <= 0)
	{
		status = STATUS_INVALID_PARAMETERS;
		return status;
	}

	CIniParser IniObj;
	IniObj.Initialize(m_nConfig_IniFile);

	READ_PAR read_par;
	memset(&read_par,0,sizeof(read_par));

	// 读取US总条数
	read_par.bIsString = false;
	strcpy(read_par.sAppName,CONFIG_INI_US);
	strcpy(read_par.sKeyName,CONFIG_INI_US_COUNTS);

	IniObj.Read(&read_par);

	int nIndex = 1;
	for(int i = 0 ; i < read_par.nValue ; i ++)
	{
		char szBuf[MAX_PATH]={0};
		sprintf(szBuf,"us_particle%d",nIndex);
		nIndex++;

		// 清空
		IniObj.EmptyByAppName(szBuf);
	}

	// 写入counts新项
	WRITE_PAR write_par;
	strcpy(write_par.sAppName,CONFIG_INI_US);
	strcpy(write_par.sKeyName,CONFIG_INI_US_COUNTS);
	sprintf(write_par.sString,"%d",set_us_info.nUsCounts);

	IniObj.Write(write_par);


	//bIsShowFrame
	strcpy(write_par.sAppName,CONFIG_INI_US);
	strcpy(write_par.sKeyName,CONFIG_INI_SHOWCELLFRAME);
	sprintf(write_par.sString,"%d",set_us_info.bIsShowFrame);
	//DBG_MSG("TH_DB_Access::write_par.sString.bIsShowFrame = %s\n",write_par.sString);
	IniObj.Write(write_par);

	nIndex = 1;
	for(unsigned int i = 0 ; i < set_us_info.nUsCounts ; i ++)
	{
		memset(&write_par,0,sizeof(WRITE_PAR));
		char szBuf[MAX_PATH]={0};

		// 序号
		sprintf(szBuf,"us_particle%d",nIndex);
		strcpy(write_par.sAppName,szBuf);
		strcpy(write_par.sKeyName,CONFIG_INI_INDEX);
		sprintf(write_par.sString,"%d",set_us_info.par[i].nIndex);
		IniObj.Write(write_par);

		// MAX_NORMAL_MALE
		strcpy(write_par.sKeyName,CONFIG_INI_MAX_NORMAL_MALE);
		sprintf(write_par.sString,"%d",set_us_info.par[i].nMax_normal_male);
		IniObj.Write(write_par);

		// CONFIG_INI_MIN_NORMAL_MALE
		strcpy(write_par.sKeyName,CONFIG_INI_MIN_NORMAL_MALE);
		sprintf(write_par.sString,"%d",set_us_info.par[i].nMin_normal_male);
		IniObj.Write(write_par);

		// CONFIG_INI_MAX_NORMAL_FEMALE
		strcpy(write_par.sKeyName,CONFIG_INI_MAX_NORMAL_FEMALE);
		sprintf(write_par.sString,"%d",set_us_info.par[i].nMax_normal_female);
		IniObj.Write(write_par);

		// CONFIG_INI_MIN_NORMAL_FEMALE
		strcpy(write_par.sKeyName,CONFIG_INI_MIN_NORMAL_FEMALE);
		sprintf(write_par.sString,"%d",set_us_info.par[i].nMin_normal_female);
		IniObj.Write(write_par);

		// CONFIG_INI_MAX_UNUSUAL_MALE
		strcpy(write_par.sKeyName,CONFIG_INI_MAX_UNUSUAL_MALE);
		sprintf(write_par.sString,"%d",set_us_info.par[i].nMax_unusual_male);
		IniObj.Write(write_par);

		// CONFIG_INI_MIN_UNUSUAL_MALE
		strcpy(write_par.sKeyName,CONFIG_INI_MIN_UNUSUAL_MALE);
		sprintf(write_par.sString,"%d",set_us_info.par[i].nMin_unusual_male);
		IniObj.Write(write_par);

		// CONFIG_INI_MAX_UNUSUAL_FEMALE
		strcpy(write_par.sKeyName,CONFIG_INI_MAX_UNUSUAL_FEMALE);
		sprintf(write_par.sString,"%d",set_us_info.par[i].nMax_unusual_female);
		IniObj.Write(write_par);

		//CONFIG_INI_MIN_UNUSUAL_FEMALE
		strcpy(write_par.sKeyName,CONFIG_INI_MIN_UNUSUAL_FEMALE);
		sprintf(write_par.sString,"%d",set_us_info.par[i].nMin_unusual_female);
		IniObj.Write(write_par);

		// CONFIG_INI_ISTOLIS
		strcpy(write_par.sKeyName,CONFIG_INI_ISTOLIS);
		sprintf(write_par.sString,"%d",set_us_info.par[i].bIsSendtoLIS);
		IniObj.Write(write_par);

		// CONFIG_INI_ISSHOWUI
		strcpy(write_par.sKeyName,CONFIG_INI_ISSHOWUI);
		sprintf(write_par.sString,"%d",set_us_info.par[i].bIsShowUI);
		IniObj.Write(write_par);

		// CONFIG_INI_ISSENDTOREPORT
		strcpy(write_par.sKeyName,CONFIG_INI_ISSENDTOREPORT);
		sprintf(write_par.sString,"%d",set_us_info.par[i].bIsSendtoReport);
		IniObj.Write(write_par);

		// 写DELETE
		strcpy(write_par.sKeyName,CONFIG_INI_ISDELETE);
		sprintf(write_par.sString,"%d",set_us_info.par[i].bIsDelete);
		IniObj.Write(write_par);

		// CONFIG_INI_ARGUMENT
		strcpy(write_par.sKeyName,CONFIG_INI_ARGUMENT);
		sprintf(write_par.sString,"%f",set_us_info.par[i].fArgument);
		IniObj.Write(write_par);

		// CONFIG_INI_SHORTNAME
		strcpy(write_par.sKeyName,CONFIG_INI_SHORTNAME);
		strcpy(write_par.sString,set_us_info.par[i].sShortName);
		IniObj.Write(write_par);

		// CONFIG_INI_LONGNAME
		strcpy(write_par.sKeyName,CONFIG_INI_LONGNAME);
		strcpy(write_par.sString,set_us_info.par[i].sLongName);
		IniObj.Write(write_par);

		// CONFIG_INI_UNIT
		strcpy(write_par.sKeyName,CONFIG_INI_UNIT);
		sprintf(write_par.sString,"%d",set_us_info.par[i].nUnit);
		IniObj.Write(write_par);

		// CONFIG_INI_CELLNAMECOLOR_R
		strcpy(write_par.sKeyName,CONFIG_INI_CELLNAMECOLOR_R);
		sprintf(write_par.sString,"%s",set_us_info.par[i].m_CellNameColoer_R);
		//DBG_MSG("TH_DB_Access::%d.write_par.sString.m_CellNameColoer_R = %s\n",nIndex,write_par.sString);
		IniObj.Write(write_par);

		// CONFIG_INI_CELLNAMECOLOR_G
		strcpy(write_par.sKeyName,CONFIG_INI_CELLNAMECOLOR_G);
		sprintf(write_par.sString,"%s",set_us_info.par[i].m_CellNameColoer_G);
		//DBG_MSG("TH_DB_Access::%d.write_par.sString.m_CellNameColoer_G = %s\n",nIndex,write_par.sString);
		IniObj.Write(write_par);

		// CONFIG_INI_CELLNAMECOLOR_B
		strcpy(write_par.sKeyName,CONFIG_INI_CELLNAMECOLOR_B);
		sprintf(write_par.sString,"%s",set_us_info.par[i].m_CellNameColoer_B);
		//DBG_MSG("TH_DB_Access::%d.write_par.sString.m_CellNameColoer_B = %s\n",nIndex,write_par.sString);
		IniObj.Write(write_par);

        //CONFIG_INI_RELIABILITY 
        strcpy(write_par.sKeyName, CONFIG_INI_RELIABILITY);
        sprintf(write_par.sString, "%d", set_us_info.par[i].m_reliability);
        IniObj.Write(write_par);

		nIndex ++;
	} // end for


    strcpy(write_par.sAppName, "us_particle_changed");
    strcpy(write_par.sKeyName, "0");
    strcpy(write_par.sString, "1");
    IniObj.Write(write_par);

	return status;

}

KSTATUS TH_DB_Access::GetUdcInfo( GET_UDC_INFO* pGetUdcInfo )
{
	KSTATUS status = STATUS_SUCCESS;

	if(pGetUdcInfo == NULL)
	{
		status = STATUS_INVALID_PARAMETERS;
		return status;
	}

	pGetUdcInfo->nUdcCounts = 0;

	CIniParser IniObj;
	IniObj.Initialize(m_nConfig_IniFile);

	READ_PAR read_par;
	memset(&read_par,0,sizeof(read_par));


	// 读取US总条数
	read_par.bIsString = false;
	read_par.bIsfloat  = false;
	strcpy(read_par.sAppName,CONFIG_INI_UDC);
	strcpy(read_par.sKeyName,CONFIG_INI_UDC_COUNTS);

	status = IniObj.Read(&read_par);
	if(status != STATUS_SUCCESS)
	{
		pGetUdcInfo->nUdcCounts = 0;
		status = STATUS_INVALID_PARAMETERS;
		return status;
	}

	if(read_par.nValue == 0)
	{
		pGetUdcInfo->nUdcCounts = 0;
		status = STATUS_INVALID_PARAMETERS;
		return status;
	}
	else
	{
		pGetUdcInfo->nUdcCounts = read_par.nValue;

		int nIndex = 1;
		for(unsigned int i = 0 ; i < pGetUdcInfo->nUdcCounts ; i ++)
		{
			char szBuf[MAX_PATH]={0};
			memset(&read_par,0,sizeof(read_par));
			read_par.bIsString = false;
			read_par.bIsfloat  = false;

			// INDEX
			sprintf(szBuf,"udc_particle%d",nIndex);
			strcpy(read_par.sAppName,szBuf);
			strcpy(read_par.sKeyName,CONFIG_INI_INDEX);

			IniObj.Read(&read_par);
			pGetUdcInfo->par[i].nIndex = read_par.nValue;

			// male_normal_MAX
			memset(&read_par,0,sizeof(read_par));
			read_par.bIsString = false;
			read_par.bIsfloat  = false;
			sprintf(szBuf,"udc_particle%d",nIndex);
			strcpy(read_par.sAppName,szBuf);
			strcpy(read_par.sKeyName,CONFIG_INI_MAX_NORMAL_MALE);

			IniObj.Read(&read_par);
			pGetUdcInfo->par[i].nMax_normal_male = read_par.nValue;

			// male_normal_MIN
			memset(&read_par,0,sizeof(read_par));
			read_par.bIsString = false;
			read_par.bIsfloat  = false;
			sprintf(szBuf,"udc_particle%d",nIndex);
			strcpy(read_par.sAppName,szBuf);
			strcpy(read_par.sKeyName,CONFIG_INI_MIN_NORMAL_MALE);

			IniObj.Read(&read_par);
			pGetUdcInfo->par[i].nMin_normal_male = read_par.nValue;

			// nMax_normal_female
			memset(&read_par,0,sizeof(read_par));
			read_par.bIsString = false;
			read_par.bIsfloat  = false;
			sprintf(szBuf,"udc_particle%d",nIndex);
			strcpy(read_par.sAppName,szBuf);
			strcpy(read_par.sKeyName,CONFIG_INI_MAX_NORMAL_FEMALE);

			IniObj.Read(&read_par);
			pGetUdcInfo->par[i].nMax_normal_female = read_par.nValue;

			// nMin_normal_female
			memset(&read_par,0,sizeof(read_par));
			read_par.bIsString = false;
			read_par.bIsfloat  = false;
			sprintf(szBuf,"udc_particle%d",nIndex);
			strcpy(read_par.sAppName,szBuf);
			strcpy(read_par.sKeyName,CONFIG_INI_MIN_NORMAL_FEMALE);

			IniObj.Read(&read_par);
			pGetUdcInfo->par[i].nMin_normal_female = read_par.nValue;

			// nMax_unusual_male
			memset(&read_par,0,sizeof(read_par));
			read_par.bIsString = false;
			read_par.bIsfloat  = false;
			sprintf(szBuf,"udc_particle%d",nIndex);
			strcpy(read_par.sAppName,szBuf);
			strcpy(read_par.sKeyName,CONFIG_INI_MAX_UNUSUAL_MALE);

			IniObj.Read(&read_par);
			pGetUdcInfo->par[i].nMax_unusual_male = read_par.nValue;

			//nMin_unusual_male
			memset(&read_par,0,sizeof(read_par));
			read_par.bIsString = false;
			read_par.bIsfloat  = false;
			sprintf(szBuf,"udc_particle%d",nIndex);
			strcpy(read_par.sAppName,szBuf);
			strcpy(read_par.sKeyName,CONFIG_INI_MIN_UNUSUAL_MALE);

			IniObj.Read(&read_par);
			pGetUdcInfo->par[i].nMin_unusual_male = read_par.nValue;

			//nMax_unusual_female
			memset(&read_par,0,sizeof(read_par));
			read_par.bIsString = false;
			read_par.bIsfloat  = false;
			sprintf(szBuf,"udc_particle%d",nIndex);
			strcpy(read_par.sAppName,szBuf);
			strcpy(read_par.sKeyName,CONFIG_INI_MAX_UNUSUAL_FEMALE);

			IniObj.Read(&read_par);
			pGetUdcInfo->par[i].nMax_unusual_female = read_par.nValue;

			//nMin_unusual_female
			memset(&read_par,0,sizeof(read_par));
			read_par.bIsString = false;
			read_par.bIsfloat  = false;
			sprintf(szBuf,"udc_particle%d",nIndex);
			strcpy(read_par.sAppName,szBuf);
			strcpy(read_par.sKeyName,CONFIG_INI_MIN_UNUSUAL_FEMALE);

			IniObj.Read(&read_par);
			pGetUdcInfo->par[i].nMin_unusual_female = read_par.nValue;

			//bIsSendtoLIS
			memset(&read_par,0,sizeof(read_par));
			read_par.bIsString = false;
			read_par.bIsfloat  = false;
			sprintf(szBuf,"udc_particle%d",nIndex);
			strcpy(read_par.sAppName,szBuf);
			strcpy(read_par.sKeyName,CONFIG_INI_ISTOLIS);

			IniObj.Read(&read_par);

			if( read_par.nValue >=1)
			{
				pGetUdcInfo->par[i].bIsSendtoLIS =true;
			}
			else
			{
				pGetUdcInfo->par[i].bIsSendtoLIS =false;
			}

			//bIsShowUI
			memset(&read_par,0,sizeof(read_par));
			read_par.bIsString = false;
			read_par.bIsfloat  = false;
			sprintf(szBuf,"udc_particle%d",nIndex);
			strcpy(read_par.sAppName,szBuf);
			strcpy(read_par.sKeyName,CONFIG_INI_ISSHOWUI);

			IniObj.Read(&read_par);

			if(read_par.nValue >= 1)
			{
				pGetUdcInfo->par[i].bIsShowUI = true;
			}
			else
			{
				pGetUdcInfo->par[i].bIsShowUI = false;
			}

			// bIsSendtoReport
			memset(&read_par,0,sizeof(read_par));
			read_par.bIsString = false;
			read_par.bIsfloat  = false;
			sprintf(szBuf,"udc_particle%d",nIndex);
			strcpy(read_par.sAppName,szBuf);
			strcpy(read_par.sKeyName,CONFIG_INI_ISSENDTOREPORT);

			IniObj.Read(&read_par);

			if(read_par.nValue >= 1)
			{
				pGetUdcInfo->par[i].bIsSendtoReport = true;
			}
			else
			{
				pGetUdcInfo->par[i].bIsSendtoReport = false;
			}

			// delete
			memset(&read_par,0,sizeof(read_par));
			read_par.bIsString = false;
			read_par.bIsfloat = false;

			sprintf(szBuf,"udc_particle%d",nIndex);
			strcpy(read_par.sAppName,szBuf);
			strcpy(read_par.sKeyName,CONFIG_INI_ISDELETE);

			IniObj.Read(&read_par);

			if(read_par.nValue >= 1)
			{
				pGetUdcInfo->par[i].bIsDelete = true;
			}
			else
			{
				pGetUdcInfo->par[i].bIsDelete = false;
			}


			// fArgument
			memset(&read_par,0,sizeof(read_par));
			read_par.bIsString = false;
			read_par.bIsfloat  = true;
			sprintf(szBuf,"udc_particle%d",nIndex);
			strcpy(read_par.sAppName,szBuf);
			strcpy(read_par.sKeyName,CONFIG_INI_ARGUMENT);

			IniObj.Read(&read_par);
			pGetUdcInfo->par[i].fArgument = read_par.fValue;

			// nUnit
			memset(&read_par,0,sizeof(read_par));
			read_par.bIsString = false;
			read_par.bIsfloat  = false;
			sprintf(szBuf,"udc_particle%d",nIndex);
			strcpy(read_par.sAppName,szBuf);
			strcpy(read_par.sKeyName,CONFIG_INI_UNIT);

			IniObj.Read(&read_par);
			pGetUdcInfo->par[i].nUnit = read_par.nValue;

			// sShortName
			memset(&read_par,0,sizeof(read_par));
			read_par.bIsString = true;
			read_par.bIsfloat  = false;
			sprintf(szBuf,"udc_particle%d",nIndex);
			strcpy(read_par.sAppName,szBuf);
			strcpy(read_par.sKeyName,CONFIG_INI_SHORTNAME);

			IniObj.Read(&read_par);
			strcpy(pGetUdcInfo->par[i].sShortName, read_par.sString);

			// sLongName
			memset(&read_par,0,sizeof(read_par));
			read_par.bIsString = true;
			read_par.bIsfloat  = false;
			sprintf(szBuf,"udc_particle%d",nIndex);
			strcpy(read_par.sAppName,szBuf);
			strcpy(read_par.sKeyName,CONFIG_INI_LONGNAME);

			IniObj.Read(&read_par);
			strcpy(pGetUdcInfo->par[i].sLongName, read_par.sString);


			nIndex ++;
		}// end for
	}


	return status;

}

KSTATUS TH_DB_Access::SetUdcInfo( SET_UDC_INFO set_udc_info )
{
	KSTATUS status = STATUS_SUCCESS;

	if(set_udc_info.nUdcCounts <= 0)
	{
		status = STATUS_INVALID_PARAMETERS;
		return status;
	}

	CIniParser IniObj;
	IniObj.Initialize(m_nConfig_IniFile);

	READ_PAR read_par;
	memset(&read_par,0,sizeof(read_par));

	// 读取UDC总条数
	read_par.bIsString = false;
	strcpy(read_par.sAppName,CONFIG_INI_UDC);
	strcpy(read_par.sKeyName,CONFIG_INI_UDC_COUNTS);

	IniObj.Read(&read_par);

	int Index = 1;
	for(int i = 0 ; i < read_par.nValue ; i ++)
	{
		char szBuf[MAX_PATH]={0};
		sprintf(szBuf,"udc_particle%d",Index);
		Index++;

		// 清空
		IniObj.EmptyByAppName(szBuf);
	}

	// 写入counts新项
	WRITE_PAR write_par;
	strcpy(write_par.sAppName,CONFIG_INI_UDC);
	strcpy(write_par.sKeyName,CONFIG_INI_UDC_COUNTS);
	sprintf(write_par.sString,"%d",set_udc_info.nUdcCounts);

	IniObj.Write(write_par);

	int nIndex = 1;
	for(unsigned int i = 0 ; i < set_udc_info.nUdcCounts ; i ++)
	{
		memset(&write_par,0,sizeof(WRITE_PAR));
		char szBuf[MAX_PATH]={0};

		// 序号
		sprintf(szBuf,"udc_particle%d",nIndex);
		strcpy(write_par.sAppName,szBuf);
		strcpy(write_par.sKeyName,CONFIG_INI_INDEX);
		sprintf(write_par.sString,"%d",set_udc_info.par[i].nIndex);
		IniObj.Write(write_par);

		// MAX_NORMAL_MALE
		strcpy(write_par.sKeyName,CONFIG_INI_MAX_NORMAL_MALE);
		sprintf(write_par.sString,"%d",set_udc_info.par[i].nMax_normal_male);
		IniObj.Write(write_par);

		// CONFIG_INI_MIN_NORMAL_MALE
		strcpy(write_par.sKeyName,CONFIG_INI_MIN_NORMAL_MALE);
		sprintf(write_par.sString,"%d",set_udc_info.par[i].nMin_normal_male);
		IniObj.Write(write_par);

		// CONFIG_INI_MAX_NORMAL_FEMALE
		strcpy(write_par.sKeyName,CONFIG_INI_MAX_NORMAL_FEMALE);
		sprintf(write_par.sString,"%d",set_udc_info.par[i].nMax_normal_female);
		IniObj.Write(write_par);

		// CONFIG_INI_MIN_NORMAL_FEMALE
		strcpy(write_par.sKeyName,CONFIG_INI_MIN_NORMAL_FEMALE);
		sprintf(write_par.sString,"%d",set_udc_info.par[i].nMin_normal_female);
		IniObj.Write(write_par);

		// CONFIG_INI_MAX_UNUSUAL_MALE
		strcpy(write_par.sKeyName,CONFIG_INI_MAX_UNUSUAL_MALE);
		sprintf(write_par.sString,"%d",set_udc_info.par[i].nMax_unusual_male);
		IniObj.Write(write_par);

		// CONFIG_INI_MIN_UNUSUAL_MALE
		strcpy(write_par.sKeyName,CONFIG_INI_MIN_UNUSUAL_MALE);
		sprintf(write_par.sString,"%d",set_udc_info.par[i].nMin_unusual_male);
		IniObj.Write(write_par);

		// CONFIG_INI_MAX_UNUSUAL_FEMALE
		strcpy(write_par.sKeyName,CONFIG_INI_MAX_UNUSUAL_FEMALE);
		sprintf(write_par.sString,"%d",set_udc_info.par[i].nMax_unusual_female);
		IniObj.Write(write_par);

		//CONFIG_INI_MIN_UNUSUAL_FEMALE
		strcpy(write_par.sKeyName,CONFIG_INI_MIN_UNUSUAL_FEMALE);
		sprintf(write_par.sString,"%d",set_udc_info.par[i].nMin_unusual_female);
		IniObj.Write(write_par);

		// CONFIG_INI_ISTOLIS
		strcpy(write_par.sKeyName,CONFIG_INI_ISTOLIS);
		sprintf(write_par.sString,"%d",set_udc_info.par[i].bIsSendtoLIS);
		IniObj.Write(write_par);

		// CONFIG_INI_ISSHOWUI
		strcpy(write_par.sKeyName,CONFIG_INI_ISSHOWUI);
		sprintf(write_par.sString,"%d",set_udc_info.par[i].bIsShowUI);
		IniObj.Write(write_par);

		// CONFIG_INI_ISSENDTOREPORT
		strcpy(write_par.sKeyName,CONFIG_INI_ISSENDTOREPORT);
		sprintf(write_par.sString,"%d",set_udc_info.par[i].bIsSendtoReport);
		IniObj.Write(write_par);

		// 写DELETE
		strcpy(write_par.sKeyName,CONFIG_INI_ISDELETE);
		sprintf(write_par.sString,"%d",set_udc_info.par[i].bIsDelete);
		IniObj.Write(write_par);

		// CONFIG_INI_ARGUMENT
		strcpy(write_par.sKeyName,CONFIG_INI_ARGUMENT);
		sprintf(write_par.sString,"%f",set_udc_info.par[i].fArgument);
		IniObj.Write(write_par);

		// CONFIG_INI_SHORTNAME
		strcpy(write_par.sKeyName,CONFIG_INI_SHORTNAME);
		strcpy(write_par.sString,set_udc_info.par[i].sShortName);
		IniObj.Write(write_par);

		// CONFIG_INI_LONGNAME
		strcpy(write_par.sKeyName,CONFIG_INI_LONGNAME);
		strcpy(write_par.sString,set_udc_info.par[i].sLongName);
		IniObj.Write(write_par);

		// CONFIG_INI_UNIT
		strcpy(write_par.sKeyName,CONFIG_INI_UNIT);
		sprintf(write_par.sString,"%d",set_udc_info.par[i].nUnit);
		IniObj.Write(write_par);


		nIndex++;
	} // end for


	return status;


}

//KSTATUS TH_DB_Access::SearchTask( SEARCH_TASK* pSearch_normal_task )
//{
//	KSTATUS status = STATUS_SUCCESS;
//	OPEN_RS_PAR rs_par;
//
//	if(pSearch_normal_task == NULL)
//	{
//		status = STATUS_INVALID_PARAMETERS;
//		return status;
//	}
//
//	if(m_Connection == NULL)
//	{
//		status = STATUS_INVALID_PARAMETERS;
//		return status;
//	}
//
//	strcpy(rs_par.sql,"SELECT * FROM ((tMain left outer join tUs on tMain.nID=tUs.nID)\
//					  left outer join tUdc on tMain.nID=tUdc.nID)\
//					  left outer join tPat on tMain.nID=tPat.nID where 1=1");
//
//
//	char szBuf[MAX_SQL_LENS] = {0};
//	pSearch_normal_task->conditions.bIsSearch_completed = false;
//	if(pSearch_normal_task->bIsSearch_QC_Task == true)
//	{
//		memset(&szBuf,0,sizeof(szBuf));
//		sprintf(szBuf," AND (nAttribute= %d OR nAttribute= %d OR nAttribute= %d OR nAttribute= %d OR nAttribute= %d)",UsQCByP,UsQCByN,UdcQCBy1,UdcQCBy2,UdcQCBy3);
//		strcat(rs_par.sql,szBuf);
//
//	}
//	else
//	{
//		memset(&szBuf,0,sizeof(szBuf));
//		sprintf(szBuf," AND (nAttribute= %d OR nAttribute= %d)",TestNormal,TestEP);
//		strcat(rs_par.sql,szBuf);
//
//	}
//
//	if(pSearch_normal_task->conditions.nTestDes >= UsQCByP && pSearch_normal_task->conditions.nTestDes <= TestEP)
//	{
//		memset(&szBuf,0,sizeof(szBuf));
//		sprintf(szBuf," AND nAttribute= %d",pSearch_normal_task->conditions.nTestDes);
//		strcat(rs_par.sql,szBuf);
//	}
//	/*else 
//	{
//		memset(&szBuf,0,sizeof(szBuf));
//		sprintf(szBuf," AND (nAttribute= %d OR nAttribute= %d)",TestNormal,TestEP);
//		strcat(rs_par.sql,szBuf);
//	}
//	*/
//	if(pSearch_normal_task->conditions.nID_From > 0
//		&& pSearch_normal_task->conditions.nID_To >0)
//	{
//		memset(&szBuf,0,sizeof(szBuf));
//		sprintf(szBuf," AND tMain.nID between %d and %d",pSearch_normal_task->conditions.nID_From,
//			pSearch_normal_task->conditions.nID_To);
//		strcat(rs_par.sql,szBuf);
//
//		pSearch_normal_task->conditions.bIsSearch_completed = true;
//	}
//	else if(pSearch_normal_task->conditions.nID_From >0
//		&& pSearch_normal_task->conditions.nID_To <=0)
//	{
//		memset(&szBuf,0,sizeof(szBuf));
//		sprintf(szBuf," AND tMain.nID=%d",pSearch_normal_task->conditions.nID_From);
//		strcat(rs_par.sql,szBuf);
//
//		pSearch_normal_task->conditions.bIsSearch_completed = true;
//	}
//	else if(pSearch_normal_task->conditions.nID_From <= 0 && pSearch_normal_task->conditions.nID_To >0)
//	{
//		status = STATUS_INVALID_PARAMETERS;
//		return status;
//	}
//	else
//	{
//
//	}
//	
//
//	if(pSearch_normal_task->conditions.nSN > 0)
//	{
//		memset(szBuf, 0, sizeof(szBuf));
//		sprintf(szBuf," AND nSN = %d",pSearch_normal_task->conditions.nSN);
//		strcat(rs_par.sql,szBuf);
//
//		pSearch_normal_task->conditions.bIsSearch_completed = true;
//	}
//
//	/*if(pSearch_normal_task->conditions.nStatus >= 0)
//	{
//		memset(szBuf, 0, sizeof(szBuf));
//		sprintf(szBuf," AND nState = %d",pSearch_normal_task->conditions.nStatus);
//		strcat(rs_par.sql,szBuf);
//	}*/
//
//	
//
//	/*if(pSearch_normal_task->conditions.bIsEP == TRUE)
//	{
//		memset(&szBuf,0,sizeof(szBuf));
//		sprintf(szBuf," AND bMain=true");
//		strcat(rs_par.sql,szBuf);
//	}
//	if(pSearch_normal_task->bIsSearch_QC_Task == true)
//	{
//		memset(szBuf, 0, sizeof(szBuf));
//		sprintf(szBuf," AND nAttribute=-1");
//		strcat(rs_par.sql,szBuf);
//
//		if(pSearch_normal_task->conditions.nQcType == US_QC_TYPE)
//		{
//			memset(szBuf, 0, sizeof(szBuf));
//			sprintf(szBuf," AND nQcType = %d ",pSearch_normal_task->conditions.nQcType);
//			strcat(rs_par.sql,szBuf);
//			if(pSearch_normal_task->conditions.nUsQcType > 0)
//			{
//				memset(szBuf, 0, sizeof(szBuf));
//				sprintf(szBuf," AND nQcUsType = %d",pSearch_normal_task->conditions.nUsQcType);
//				strcat(rs_par.sql,szBuf);
//
//			}			
//			else
//			{
//
//			}
//			
//		}
//		else if( pSearch_normal_task->conditions.nQcType == UDC_QC_TYPE)
//		{
//			memset(szBuf, 0, sizeof(szBuf));
//			sprintf(szBuf,"AND nQcType = %d ",pSearch_normal_task->conditions.nQcType);
//			strcat(rs_par.sql,szBuf);
//			if(pSearch_normal_task->conditions.nUdcQcType > 0)
//			{
//				memset(szBuf, 0, sizeof(szBuf));
//				sprintf(szBuf,"AND nQcUdcType = %d",pSearch_normal_task->conditions.nUdcQcType);
//				strcat(rs_par.sql,szBuf);
//			}			
//			else
//			{
//
//			}
//		}
//		else
//		{
//
//		}
//	}
//
//	else
//	{
//		memset(szBuf, 0, sizeof(szBuf));
//		sprintf(szBuf," AND nAttribute=0");
//		strcat(rs_par.sql,szBuf);
//	}*/
//
//	if (strlen(pSearch_normal_task->conditions.sName)>0)
//	{
//		memset(szBuf, 0, sizeof(szBuf));
//		sprintf(szBuf," AND sName LIKE \'%%%s%%\'",pSearch_normal_task->conditions.sName);
//		strcat(rs_par.sql,szBuf);
//		pSearch_normal_task->conditions.bIsSearch_completed = true;
//	}
//
//	if (strlen(pSearch_normal_task->conditions.sSickBed)>0)
//	{
//		memset(szBuf, 0, sizeof(szBuf));
//		sprintf(szBuf," AND sSickBed=\'%s\'",pSearch_normal_task->conditions.sSickBed);
//		strcat(rs_par.sql,szBuf);
//
//		pSearch_normal_task->conditions.bIsSearch_completed = true;
//	}
//
//	if (strlen(pSearch_normal_task->conditions.sCompany)>0)
//	{
//		memset(szBuf, 0, sizeof(szBuf));
//		sprintf(szBuf," AND sCompany=\'%s\'",pSearch_normal_task->conditions.sCompany);
//		strcat(rs_par.sql,szBuf);
//
//		pSearch_normal_task->conditions.bIsSearch_completed = true;
//	}
//
//
//	if (strlen(pSearch_normal_task->conditions.sHospital)>0)
//	{
//		memset(szBuf, 0, sizeof(szBuf));
//		sprintf(szBuf," AND sHospital=\'%s\'",pSearch_normal_task->conditions.sHospital);
//		strcat(rs_par.sql,szBuf);
//
//		pSearch_normal_task->conditions.bIsSearch_completed = true;
//	}
//
//	if (strlen(pSearch_normal_task->conditions.sDepartment)>0)
//	{
//		memset(szBuf, 0, sizeof(szBuf));
//		sprintf(szBuf," AND sDepartment=\'%s\'",pSearch_normal_task->conditions.sDepartment);
//		strcat(rs_par.sql,szBuf);
//
//		pSearch_normal_task->conditions.bIsSearch_completed = true;
//	}
//
//	if (strlen(pSearch_normal_task->conditions.sDoctor)>0)
//	{
//		memset(szBuf, 0, sizeof(szBuf));
//		sprintf(szBuf," AND sDoctor=\'%s\'",pSearch_normal_task->conditions.sDoctor);
//		strcat(rs_par.sql,szBuf);
//
//		pSearch_normal_task->conditions.bIsSearch_completed = true;
//	}
//
//	if (strlen(pSearch_normal_task->conditions.sReporter)>0)
//	{
//		memset(szBuf, 0, sizeof(szBuf));
//		sprintf(szBuf," AND sReporter=\'%s\'",pSearch_normal_task->conditions.sReporter);
//		strcat(rs_par.sql,szBuf);
//
//		pSearch_normal_task->conditions.bIsSearch_completed = true;
//	}
//
//	if (strlen(pSearch_normal_task->conditions.sAuditor)>0)
//	{
//		memset(szBuf, 0, sizeof(szBuf));
//		sprintf(szBuf," AND sAuditor=\'%s\'",pSearch_normal_task->conditions.sAuditor);
//		strcat(rs_par.sql,szBuf);
//
//		pSearch_normal_task->conditions.bIsSearch_completed = true;
//	}
//
//	if (strlen(pSearch_normal_task->conditions.sBarcode)>0)
//	{
//		memset(szBuf, 0, sizeof(szBuf));
//		sprintf(szBuf," AND sCode=\'%s\'",pSearch_normal_task->conditions.sBarcode);
//		strcat(rs_par.sql,szBuf);
//
//		pSearch_normal_task->conditions.bIsSearch_completed = true;
//	}
//
//	if (strlen(pSearch_normal_task->conditions.sDocimaster)>0)
//	{
//		memset(szBuf, 0, sizeof(szBuf));
//		sprintf(szBuf," AND sDocimaster=\'%s\'",pSearch_normal_task->conditions.sDocimaster);
//		strcat(rs_par.sql,szBuf);
//
//		pSearch_normal_task->conditions.bIsSearch_completed = true;
//	}
//
//
//
//	if(strlen(pSearch_normal_task->conditions.sDate_From) > 0 && strlen(pSearch_normal_task->conditions.sDate_To) > 0)
//	{
//		memset(&szBuf,0,sizeof(szBuf));
//		/*sprintf(szBuf," AND (dtDate between \'%s\' and \'%s\')",pSearch_normal_task->conditions.sDate_From,
//			pSearch_normal_task->conditions.sDate_To);*/
//		/*sprintf(szBuf," AND DateDiff('D', dtDate, '%s') >= 0 AND DateDiff('D', dtDate, '%s')<= 0",pSearch_normal_task->conditions.sDate_From,
//			pSearch_normal_task->conditions.sDate_To);*/
//
//
//		if(strlen(pSearch_normal_task->conditions.sTime_From) > 0
//			&& strlen(pSearch_normal_task->conditions.sTime_To) > 0)
//		{
//			sprintf(szBuf," AND (dtDate >=#%s %s# and dtDate<= #%s %s#)",pSearch_normal_task->conditions.sDate_From,pSearch_normal_task->conditions.sTime_From,
//				pSearch_normal_task->conditions.sDate_To,pSearch_normal_task->conditions.sTime_To);
//			
//			pSearch_normal_task->conditions.bIsSearch_completed = true;
//		}
//		else if(strlen(pSearch_normal_task->conditions.sTime_From) >0
//			&& strlen(pSearch_normal_task->conditions.sTime_To) <=0)
//		{
//			sprintf(szBuf," AND (dtDate >=#%s %s# and dtDate<= #%s 23:59:59#)",pSearch_normal_task->conditions.sDate_From,pSearch_normal_task->conditions.sTime_From,
//				pSearch_normal_task->conditions.sDate_To);
//
//			pSearch_normal_task->conditions.bIsSearch_completed = true;
//		}
//		else
//		{
//			sprintf(szBuf," AND (dtDate >=#%s# and dtDate<= #%s 23:59:59#)",pSearch_normal_task->conditions.sDate_From,
//				pSearch_normal_task->conditions.sDate_To);
//
//			pSearch_normal_task->conditions.bIsSearch_completed = true;
//		}		
//
//		strcat(rs_par.sql,szBuf);
//	}
//	else if( strlen(pSearch_normal_task->conditions.sDate_From) >0
//		&& strlen(pSearch_normal_task->conditions.sDate_To) <=0)
//	{
//
//		memset(&szBuf,0,sizeof(szBuf));
//
//		if(strlen(pSearch_normal_task->conditions.sTime_From) > 0
//			&& strlen(pSearch_normal_task->conditions.sTime_To) > 0)
//		{
//			sprintf(szBuf," AND (dtDate >=#%s %s# and dtDate<= #%s %s#)",pSearch_normal_task->conditions.sDate_From,pSearch_normal_task->conditions.sTime_From,
//				pSearch_normal_task->conditions.sDate_From,pSearch_normal_task->conditions.sTime_To);
//
//			pSearch_normal_task->conditions.bIsSearch_completed = true;
//
//		}
//		else if(strlen(pSearch_normal_task->conditions.sTime_From) >0
//			&& strlen(pSearch_normal_task->conditions.sTime_To) <=0)
//		{
//			sprintf(szBuf," AND (dtDate >=#%s %s# and dtDate<= #%s 23:59:59#)",pSearch_normal_task->conditions.sDate_From,pSearch_normal_task->conditions.sTime_From,pSearch_normal_task->conditions.sDate_From);
//
//			pSearch_normal_task->conditions.bIsSearch_completed = true;
//		}
//		else
//		{
//			
//			sprintf(szBuf," AND DateDiff('D', dtDate, '%s') = 0",pSearch_normal_task->conditions.sDate_From);
//		
//			pSearch_normal_task->conditions.bIsSearch_completed = true;
//		
//		}
//		
//		strcat(rs_par.sql,szBuf);
//	}
//	else if( strlen(pSearch_normal_task->conditions.sDate_From) <=0	&& strlen(pSearch_normal_task->conditions.sDate_To) >0)
//	{
//		status = STATUS_INVALID_PARAMETERS;
//		return status;
//	}
//	else if( strlen(pSearch_normal_task->conditions.sDate_From) <=0	&& strlen(pSearch_normal_task->conditions.sDate_To) <=0)
//	{
//		char buf[MAX_PATH]={0};
//		CTime tm;
//		tm=CTime::GetCurrentTime();
//
//		sprintf(buf,"%04d-%02d-%02d",tm.GetYear(),tm.GetMonth(),tm.GetDay());
//
//		if(strlen(pSearch_normal_task->conditions.sTime_From) > 0 && strlen(pSearch_normal_task->conditions.sTime_To) > 0)
//		{
//			memset(&szBuf,0,sizeof(szBuf));
//			sprintf(szBuf," AND (dtDate >= #%s %s# and dtDate<= #%s %s#)",buf,pSearch_normal_task->conditions.sTime_From,buf,pSearch_normal_task->conditions.sTime_To);
//			strcat(rs_par.sql,szBuf);
//
//			pSearch_normal_task->conditions.bIsSearch_completed = true;
//
//		}
//		else if(strlen(pSearch_normal_task->conditions.sTime_From) >0 && strlen(pSearch_normal_task->conditions.sTime_To) <=0)
//		{
//			memset(&szBuf,0,sizeof(szBuf));
//			sprintf(szBuf," AND (dtDate >= #%s %s# and dtDate<= #%s 23:59:59#)",buf,pSearch_normal_task->conditions.sTime_From,buf);
//			strcat(rs_par.sql,szBuf);
//
//			pSearch_normal_task->conditions.bIsSearch_completed = true;
//		}
//		else if(strlen(pSearch_normal_task->conditions.sTime_From) <= 0 && strlen(pSearch_normal_task->conditions.sTime_To) > 0)
//		{
//			status = STATUS_INVALID_PARAMETERS;
//			return status;
//		}
//		
//	}
//
//	if(pSearch_normal_task->conditions.bIsSearch_completed == false  && pSearch_normal_task->bIsSearch_QC_Task == false)
//	{
//		memset(&szBuf,0,sizeof(szBuf));
//		sprintf(szBuf," AND DateDiff('D', dtDate, NOW()) = 0");
//		strcat(rs_par.sql,szBuf);
//	}
//	
//
//	strcat(rs_par.sql," ORDER BY tMain.nID");	
//
//DBG_MSG("%s\n",rs_par.sql);
//	try
//	{
//		status = Open_Rs(&rs_par);
//
//		if(status != STATUS_SUCCESS)
//		{
//			status = STATUS_INVALID_PARAMETERS;
//			return status;
//		}
//
//
//		if(!IsBOF(rs_par.Rs))
//		{
//			rs_par.Rs->MoveFirst();
//		}
//
//		unsigned int nCounts = 0;
//		unsigned int nIndex = 0;
//
//		//DBG_MSG("IsEOF=%d\n",IsEOF(rs_par.Rs));
//
//		_variant_t Value;
//
//		for (int i = 0; !IsEOF(rs_par.Rs); rs_par.Rs->MoveNext(),i++)
//		{
//
//
//			int  nType = rs_par.Rs->GetCollect(_variant_t("nAttribute"));
//			
//			//if(nType == pSearch_normal_task->conditions.nTestDes)
//
//			if(nType >= TestNormal && nType <= TestEP)
//			{
//				++nCounts;
//			}
//			else			
//			{
//				continue;
//			}
//
//
//			if(pSearch_normal_task->pSearchInfo == NULL)
//			{
//				pSearch_normal_task->pSearchInfoCounts = nCounts;
//				status = STATUS_LOGIN_MEMORY_NOTENOUGH;
//				
//			}
//			else
//			{
//
//				
//				// ID
//				pSearch_normal_task->pSearchInfo[nIndex].main_info.nID = rs_par.Rs->GetCollect(_variant_t("tMain.nID"));
//
//				// nAlarm
//				pSearch_normal_task->pSearchInfo[nIndex].main_info.nAlarm = rs_par.Rs->GetCollect(_variant_t("nAlarm"));
//
//				// nRow
//				pSearch_normal_task->pSearchInfo[nIndex].main_info.nRow = rs_par.Rs->GetCollect(_variant_t("nRow"));
//
//				// bMain
//				pSearch_normal_task->pSearchInfo[nIndex].main_info.bMain = (bool)rs_par.Rs->GetCollect(_variant_t("bMain"));
//
//				// nState
//				pSearch_normal_task->pSearchInfo[nIndex].main_info.nState = (int)rs_par.Rs->GetCollect(_variant_t("nState"));
//
//				// nAttribute
//				pSearch_normal_task->pSearchInfo[nIndex].main_info.nAttribute = (int)rs_par.Rs->GetCollect(_variant_t("nAttribute"));
//
//				// nTestType
//				pSearch_normal_task->pSearchInfo[nIndex].main_info.nTestType = (int)rs_par.Rs->GetCollect(_variant_t("nTestType"));
//
//				// nSN
//				pSearch_normal_task->pSearchInfo[nIndex].main_info.nSN = (int)rs_par.Rs->GetCollect(_variant_t("nSN"));
//
//				// nPos
//				pSearch_normal_task->pSearchInfo[nIndex].main_info.nPos = (float)rs_par.Rs->GetCollect(_variant_t("nPos"));
//
//
//				// nCha
//				pSearch_normal_task->pSearchInfo[nIndex].main_info.nCha = (int)rs_par.Rs->GetCollect(_variant_t("nCha"));
//
//				// sCode
//				Value = rs_par.Rs->GetCollect(_variant_t("sCode"));
//				if(Value.vt != VT_NULL)
//				{					
//					strcpy(pSearch_normal_task->pSearchInfo[nIndex].main_info.sCode,_bstr_t(Value));
//				}
//
//				//sCompany
//				Value = rs_par.Rs->GetCollect(_variant_t("sCompany"));
//
//				if(Value.vt != VT_NULL)
//				{					
//					strcpy(pSearch_normal_task->pSearchInfo[nIndex].main_info.sCompany,_bstr_t(Value));
//				}
//
//				//sDepartment
//				Value = rs_par.Rs->GetCollect(_variant_t("sDepartment"));
//
//				if(Value.vt != VT_NULL)
//				{					
//					strcpy(pSearch_normal_task->pSearchInfo[nIndex].main_info.sDepartment,_bstr_t(Value));
//				}
//
//				//sDoctor
//				Value = rs_par.Rs->GetCollect(_variant_t("sDoctor"));
//				if(Value.vt != VT_NULL)
//				{					
//					strcpy(pSearch_normal_task->pSearchInfo[nIndex].main_info.sDoctor,_bstr_t(Value));
//				}
//
//				//sDocimaster
//				Value = rs_par.Rs->GetCollect(_variant_t("sDocimaster"));
//				if(Value.vt != VT_NULL)
//				{					
//					strcpy(pSearch_normal_task->pSearchInfo[nIndex].main_info.sDocimaster,_bstr_t(Value));
//				}
//
//				//sAuditor
//				Value = rs_par.Rs->GetCollect(_variant_t("sAuditor"));
//				if(Value.vt != VT_NULL)
//				{
//					strcpy(pSearch_normal_task->pSearchInfo[nIndex].main_info.sAuditor,_bstr_t(Value));
//				}
//
//				//sReporter
//				Value = rs_par.Rs->GetCollect(_variant_t("sReporter"));
//				if(Value.vt != VT_NULL)
//				{
//					strcpy(pSearch_normal_task->pSearchInfo[nIndex].main_info.sReporter,_bstr_t(Value));
//				}
//
//				//sColor
//				Value = rs_par.Rs->GetCollect(_variant_t("sColor"));
//				if(Value.vt != VT_NULL)
//				{
//					strcpy(pSearch_normal_task->pSearchInfo[nIndex].main_info.sColor,_bstr_t(Value));
//				}
//
//				//sTransparency
//				Value = rs_par.Rs->GetCollect(_variant_t("sTransparency"));
//				if(Value.vt != VT_NULL)
//				{
//					strcpy(pSearch_normal_task->pSearchInfo[nIndex].main_info.sTransparency,_bstr_t(Value));
//				}
//
//				//sFolder
//				Value = rs_par.Rs->GetCollect(_variant_t("sFolder"));
//
//				if(Value.vt != VT_NULL)
//				{
//					strcpy(pSearch_normal_task->pSearchInfo[nIndex].main_info.sFolder,_bstr_t(Value));
//				}
//
//				//sImage
//				Value = rs_par.Rs->GetCollect(_variant_t("sImage"));
//
//				if(Value.vt != VT_NULL)
//				{
//					strcpy(pSearch_normal_task->pSearchInfo[nIndex].main_info.sImage,_bstr_t(Value));
//				}
//
//				// sDiagnosis
//				Value = rs_par.Rs->GetCollect(_variant_t("sDiagnosis"));
//				if(Value.vt != VT_NULL)
//				{
//					strcpy(pSearch_normal_task->pSearchInfo[nIndex].main_info.sDiagnosis,_bstr_t(Value));
//				}
//
//				// sSuggestion
//				Value = rs_par.Rs->GetCollect(_variant_t("sSuggestion"));
//				if(Value.vt != VT_NULL)
//				{
//					strcpy(pSearch_normal_task->pSearchInfo[nIndex].main_info.sSuggestion,_bstr_t(Value));
//				}
//				// dtDate
//				Value = rs_par.Rs->GetCollect(_variant_t("dtDate"));
//				if(Value.vt != VT_NULL)
//				{
//					strcpy(pSearch_normal_task->pSearchInfo[nIndex].main_info.dtDate,_bstr_t(Value));
//				}
//
//
//				//dtPrintStatus
//				Value = rs_par.Rs->GetCollect(_variant_t("dtPrintStatus"));
//				if(Value.vt != VT_NULL)
//					pSearch_normal_task->pSearchInfo[nIndex].main_info.dtPrintStatus  = Value;
//
//
//				//dtLisStatus
//				Value = rs_par.Rs->GetCollect(_variant_t("dtLisStatus"));
//				if(Value.vt != VT_NULL)
//					pSearch_normal_task->pSearchInfo[nIndex].main_info.dtLisStatus  = Value;
//
//
//				// tPat_nID
//				pSearch_normal_task->pSearchInfo[nIndex].pat_info.nID = rs_par.Rs->GetCollect(_variant_t("tPat.nID"));	
//				// bPat
//				pSearch_normal_task->pSearchInfo[nIndex].pat_info.bPat = rs_par.Rs->GetCollect(_variant_t("bPat"));
//				// nNo
//				pSearch_normal_task->pSearchInfo[nIndex].pat_info.nNo = rs_par.Rs->GetCollect(_variant_t("nNo"));
//
//				// nSex
//				pSearch_normal_task->pSearchInfo[nIndex].pat_info.nSex = rs_par.Rs->GetCollect(_variant_t("nSex"));
//				// nAge
//				pSearch_normal_task->pSearchInfo[nIndex].pat_info.nAge = rs_par.Rs->GetCollect(_variant_t("nAge"));
//				// nAgeUnit
//				pSearch_normal_task->pSearchInfo[nIndex].pat_info.nAgeUnit = rs_par.Rs->GetCollect(_variant_t("nAgeUnit"));
//
//				// sName
//				Value = rs_par.Rs->GetCollect(_variant_t("sName"));
//				if(Value.vt != VT_NULL)
//				{					
//					strcpy(pSearch_normal_task->pSearchInfo[nIndex].pat_info.sName,_bstr_t(Value));
//				}
//
//				// sHospital
//				Value = rs_par.Rs->GetCollect(_variant_t("sHospital"));
//				if(Value.vt != VT_NULL)
//				{					
//					strcpy(pSearch_normal_task->pSearchInfo[nIndex].pat_info.sHospital,_bstr_t(Value));
//				}
//
//				// sSickBed
//				Value = rs_par.Rs->GetCollect(_variant_t("sSickBed"));
//				if(Value.vt != VT_NULL)
//				{					
//					strcpy(pSearch_normal_task->pSearchInfo[nIndex].pat_info.sSickBed,_bstr_t(Value));
//				}
//
//				// dtBirthDate
//				Value = rs_par.Rs->GetCollect(_variant_t("dtBirthDate"));
//				if(Value.vt != VT_NULL)
//				{					
//					strcpy(pSearch_normal_task->pSearchInfo[nIndex].pat_info.dtBirthDate,_bstr_t(Value));
//				}
//
//
//				
//
//				// us_id
//				pSearch_normal_task->pSearchInfo[nIndex].us_info.nID = rs_par.Rs->GetCollect(_variant_t("tUs.nID"));
//				// bUs
//				pSearch_normal_task->pSearchInfo[nIndex].us_info.bUs = rs_par.Rs->GetCollect(_variant_t("bUs"));
//				//nVariableRatio
//				pSearch_normal_task->pSearchInfo[nIndex].us_info.nVariableRatio = rs_par.Rs->GetCollect(_variant_t("nVariableRatio"));
//
//
//				int nUsIndex = 1;
//				for(unsigned int j =0 ; j < MAX_US_LENS ; j++)
//				{
//					char szBuf[MAX_PATH]={0};
//					sprintf(szBuf,"us_res%d",nUsIndex);
//					nUsIndex++;
//					pSearch_normal_task->pSearchInfo[nIndex].us_info.us_node[j].us_res = (float)rs_par.Rs->GetCollect(_variant_t(szBuf));
//					/*Value = rs_par.Rs->GetCollect(_variant_t(szBuf));
//					DBG_MSG("szBuf = %s,Value = %d\n",szBuf,Value);
//					if(Value.vt != VT_NULL)
//						pSearch_normal_task->pSearchInfo[nIndex].us_info.us_node[j].us_res = Value;*/
//					//DBG_MSG("pSearch_normal_task->pSearchInfo[%d].us_info.us_node[%d].us_res = %f\n",nIndex,j,pSearch_normal_task->pSearchInfo[nIndex].us_info.us_node[j].us_res);			
//				}
//	
//
//
//
//
//
//				// tUdc_nID
//				pSearch_normal_task->pSearchInfo[nIndex].udc_info.nID = (int)rs_par.Rs->GetCollect(_variant_t("tUdc.nID"));	
//				// bUdc
//				pSearch_normal_task->pSearchInfo[nIndex].udc_info.bUdc = (int)rs_par.Rs->GetCollect(_variant_t("bUdc"));
//
//				int nUdcIndex = 1;
//				for(unsigned int x = 0 ; x < MAX_UDC_LENS ; x ++)
//				{
//					char szBuf[MAX_PATH] = {0} ;
//					sprintf(szBuf,"udc_sRes%d",nUdcIndex);
//					Value = rs_par.Rs->GetCollect(_variant_t(szBuf));
//					if(Value.vt != VT_NULL)
//					{
//						strcpy(pSearch_normal_task->pSearchInfo[nIndex].udc_info.udc_node[x].udc_res,
//							_bstr_t(Value));
//					}
//					//DBG_MSG("pSearch_normal_task->pSearchInfo[%d].udc_info.udc_node[%d].udc_res = %s\n",nIndex,x,pSearch_normal_task->pSearchInfo[nIndex].udc_info.udc_node[x].udc_res);
//				
//					sprintf(szBuf,"sValue%d",nUdcIndex);
//					Value = rs_par.Rs->GetCollect(_variant_t(szBuf));
//
//					if(Value.vt != VT_NULL)
//					{
//						strcpy(pSearch_normal_task->pSearchInfo[nIndex].udc_info.udc_node[x].udc_value,_bstr_t(Value));
//					}
//
//					sprintf(szBuf,"bAbn%d",nUdcIndex);
//					Value = (bool)rs_par.Rs->GetCollect(_variant_t(szBuf));
//					if(Value.vt != VT_NULL)
//						pSearch_normal_task->pSearchInfo[nIndex].udc_info.udc_node[x].udc_abn  = Value;
//
//					sprintf(szBuf,"nColor%d",nUdcIndex);
//					Value = (int)rs_par.Rs->GetCollect(_variant_t(szBuf));
//					if(Value.vt != VT_NULL)
//						pSearch_normal_task->pSearchInfo[nIndex].udc_info.udc_node[x].udc_col  = Value;			
//					nUdcIndex++;
//
//				}
//				
//				
//				nIndex ++;
//				status = STATUS_SUCCESS;
//			}
//
//		} // end for
//		
//	} 
//	catch (_com_error e)
//	{
//		status = STATUS_EXCEPTION_ERROR;
//		DBG_MSG("SearchTaskInfo _com_error,errMessage = %s" ,
//			(const char *)(LPCTSTR)e.Description());
//	}
//
//	// 关闭
//	Close_Rs(rs_par.Rs);
//
//	return status;
//
//}

KSTATUS TH_DB_Access::SearchTask( SEARCH_TASK* pSearch_normal_task )
{
	KSTATUS status = STATUS_SUCCESS;
	OPEN_RS_PAR rs_par;

	if( pSearch_normal_task == NULL )
	{
		status = STATUS_INVALID_PARAMETERS;
		return status;
	}

	if( m_Connection == NULL )
	{
		status = STATUS_INVALID_PARAMETERS;
		return status;
	}

	strcpy(rs_par.sql,"SELECT * FROM ((tMain left outer join tUs on tMain.nID=tUs.nID)\
					  left outer join tUdc on tMain.nID=tUdc.nID)\
					  left outer join tPat on tMain.nID=tPat.nID where 1=1");


	char szBuf[MAX_SQL_LENS] = {0};

	pSearch_normal_task->conditions.bIsCondition = false;

	if( pSearch_normal_task->bIsSearch_QC_Task == true )
	{
		if( pSearch_normal_task->conditions.nTestDes >= UsQCBy1 && pSearch_normal_task->conditions.nTestDes <= UdcQCByN )
		{
			memset(&szBuf,0,sizeof(szBuf));
			sprintf(szBuf," AND nAttribute= %d",pSearch_normal_task->conditions.nTestDes);
			strcat(rs_par.sql,szBuf);
		}
		else
		{
			memset(&szBuf,0,sizeof(szBuf));
            sprintf(szBuf, " AND (nAttribute= %d OR nAttribute= %d OR nAttribute= %d OR nAttribute= %d OR nAttribute= %d)", UsQCBy1, UsQCBy2, UsQCBy3, UdcQCByP, UdcQCByN);
			strcat(rs_par.sql,szBuf);
		}
		
	}
	else
	{
		if( pSearch_normal_task->conditions.nTestDes == ALLTestDes )
		{
			memset(&szBuf,0,sizeof(szBuf));
            sprintf(szBuf, "AND (nAttribute= %d OR nAttribute= %d OR nAttribute= %d OR nAttribute= %d OR nAttribute= %d OR nAttribute= %d OR nAttribute= %d)", TestNormal, UsQCBy1, UsQCBy2, UsQCBy3, UdcQCByP, UdcQCByN, TestEP);
			strcat(rs_par.sql,szBuf);
		}	
		else if( pSearch_normal_task->conditions.nTestDes == TestEP )
		{
			memset(&szBuf,0,sizeof(szBuf));
			sprintf(szBuf," AND nAttribute= %d ",TestEP);
			strcat(rs_par.sql,szBuf);
		}
		else
		{
			memset(&szBuf,0,sizeof(szBuf));
			sprintf(szBuf," AND (nAttribute= %d OR nAttribute= %d)",TestNormal,TestEP);
			strcat(rs_par.sql,szBuf);
		}	
	}

	if(pSearch_normal_task->conditions.nID_From > 0	&& pSearch_normal_task->conditions.nID_To >0)
	{
		memset(&szBuf,0,sizeof(szBuf));
		/*sprintf(szBuf," AND tMain.nID between %d and %d",pSearch_normal_task->conditions.nID_From,
			pSearch_normal_task->conditions.nID_To);*/
		sprintf(szBuf," AND ( tMain.nID >= %d and tMain.nID <= %d )",pSearch_normal_task->conditions.nID_From,
			pSearch_normal_task->conditions.nID_To);

		strcat(rs_par.sql,szBuf);

		pSearch_normal_task->conditions.bIsCondition = true;
	}
	else if(pSearch_normal_task->conditions.nID_From >0	&& pSearch_normal_task->conditions.nID_To <=0)
	{
		memset(&szBuf,0,sizeof(szBuf));
		sprintf(szBuf," AND tMain.nID=%d",pSearch_normal_task->conditions.nID_From);
		strcat(rs_par.sql,szBuf);

		pSearch_normal_task->conditions.bIsCondition = true;
	}
	else if(pSearch_normal_task->conditions.nID_From <= 0 && pSearch_normal_task->conditions.nID_To >0)
	{
		status = STATUS_INVALID_PARAMETERS;
		return status;
	}
	else
	{

	}

	if(pSearch_normal_task->conditions.nSN > 0)
	{
		memset(szBuf, 0, sizeof(szBuf));
		sprintf(szBuf," AND nSN = %d",pSearch_normal_task->conditions.nSN);
		strcat(rs_par.sql,szBuf);	
	}

	/*if(pSearch_normal_task->conditions.nStatus >= 0)
	{
	memset(szBuf, 0, sizeof(szBuf));
	sprintf(szBuf," AND nState = %d",pSearch_normal_task->conditions.nStatus);
	strcat(rs_par.sql,szBuf);
	}*/

	if ( strlen(pSearch_normal_task->conditions.sName) > 0 )
	{
		memset(szBuf, 0, sizeof(szBuf));
		sprintf(szBuf," AND sName LIKE \'%%%s%%\'",pSearch_normal_task->conditions.sName);
		strcat(rs_par.sql,szBuf);		
	}

	if ( strlen(pSearch_normal_task->conditions.sSickBed) > 0 )
	{
		memset(szBuf, 0, sizeof(szBuf));
		sprintf(szBuf," AND sSickBed=\'%s\'",pSearch_normal_task->conditions.sSickBed);
		strcat(rs_par.sql,szBuf);		
	}

	if ( strlen(pSearch_normal_task->conditions.sCompany) > 0 )
	{
		memset(szBuf, 0, sizeof(szBuf));
		sprintf(szBuf," AND sCompany=\'%s\'",pSearch_normal_task->conditions.sCompany);
		strcat(rs_par.sql,szBuf);	
	}


	if ( strlen(pSearch_normal_task->conditions.sHospital) > 0 )
	{
		memset(szBuf, 0, sizeof(szBuf));
		sprintf(szBuf," AND sHospital=\'%s\'",pSearch_normal_task->conditions.sHospital);
		strcat(rs_par.sql,szBuf);		
	}

	if ( strlen(pSearch_normal_task->conditions.sDepartment) > 0 )
	{
		memset(szBuf, 0, sizeof(szBuf));
		sprintf(szBuf," AND sDepartment=\'%s\'",pSearch_normal_task->conditions.sDepartment);
		strcat(rs_par.sql,szBuf);
	}

	if ( strlen(pSearch_normal_task->conditions.sDoctor) > 0 )
	{
		memset(szBuf, 0, sizeof(szBuf));
		sprintf(szBuf," AND sDoctor=\'%s\'",pSearch_normal_task->conditions.sDoctor);
		strcat(rs_par.sql,szBuf);		
	}

	if ( strlen(pSearch_normal_task->conditions.sReporter) > 0 )
	{
		memset(szBuf, 0, sizeof(szBuf));
		sprintf(szBuf," AND sReporter=\'%s\'",pSearch_normal_task->conditions.sReporter);
		strcat(rs_par.sql,szBuf);
	}

	if ( strlen(pSearch_normal_task->conditions.sAuditor) > 0 )
	{
		memset(szBuf, 0, sizeof(szBuf));
		sprintf(szBuf," AND sAuditor=\'%s\'",pSearch_normal_task->conditions.sAuditor);
		strcat(rs_par.sql,szBuf);	
	}

	if ( strlen(pSearch_normal_task->conditions.sBarcode) > 0 )
	{
		memset(szBuf, 0, sizeof(szBuf));
		sprintf(szBuf," AND sCode=\'%s\'",pSearch_normal_task->conditions.sBarcode);
		strcat(rs_par.sql,szBuf);		
	}

	if ( strlen(pSearch_normal_task->conditions.sDocimaster) > 0 )
	{
		memset(szBuf, 0, sizeof(szBuf));
		sprintf(szBuf," AND sDocimaster=\'%s\'",pSearch_normal_task->conditions.sDocimaster);
		strcat(rs_par.sql,szBuf);		
	}

	if( strlen(pSearch_normal_task->conditions.sDate_From) > 0 && strlen(pSearch_normal_task->conditions.sDate_To) > 0 )
	{
		memset(&szBuf,0,sizeof(szBuf));
		/*sprintf(szBuf," AND (dtDate between \'%s\' and \'%s\')",pSearch_normal_task->conditions.sDate_From,
		pSearch_normal_task->conditions.sDate_To);*/
		/*sprintf(szBuf," AND DateDiff('D', dtDate, '%s') >= 0 AND DateDiff('D', dtDate, '%s')<= 0",pSearch_normal_task->conditions.sDate_From,
		pSearch_normal_task->conditions.sDate_To);*/

		if( strlen(pSearch_normal_task->conditions.sTime_From) > 0 && strlen(pSearch_normal_task->conditions.sTime_To) > 0 )
		{
			sprintf(szBuf," AND (dtDate >=# %s %s# and dtDate<= # %s %s#)",pSearch_normal_task->conditions.sDate_From,pSearch_normal_task->conditions.sTime_From,
				pSearch_normal_task->conditions.sDate_To,pSearch_normal_task->conditions.sTime_To);			
		}
		else if( strlen(pSearch_normal_task->conditions.sTime_From) > 0 && strlen(pSearch_normal_task->conditions.sTime_To) <=0 )
		{
			sprintf(szBuf," AND (dtDate >=#%s %s# and dtDate<= #%s 23:59:59#)",pSearch_normal_task->conditions.sDate_From,pSearch_normal_task->conditions.sTime_From,
				pSearch_normal_task->conditions.sDate_To);			
		}
		else
		{
			sprintf(szBuf," AND (dtDate >=# %s # and dtDate<= #%s 23:59:59#)",pSearch_normal_task->conditions.sDate_From,
				pSearch_normal_task->conditions.sDate_To);			
		}

		pSearch_normal_task->conditions.bIsCondition = true;
		strcat(rs_par.sql,szBuf);
	}
	else if( strlen(pSearch_normal_task->conditions.sDate_From) > 0	&& strlen(pSearch_normal_task->conditions.sDate_To) <= 0 )
	{
		memset(&szBuf,0,sizeof(szBuf));

		if( strlen(pSearch_normal_task->conditions.sTime_From) > 0 && strlen(pSearch_normal_task->conditions.sTime_To) > 0 )
		{
			sprintf(szBuf," AND (dtDate >=#%s %s# and dtDate<= #%s %s#)",pSearch_normal_task->conditions.sDate_From,pSearch_normal_task->conditions.sTime_From,
				pSearch_normal_task->conditions.sDate_From,pSearch_normal_task->conditions.sTime_To);
		}
		else if( strlen(pSearch_normal_task->conditions.sTime_From) > 0	&& strlen(pSearch_normal_task->conditions.sTime_To) <= 0 )
		{
			sprintf(szBuf," AND (dtDate >=#%s %s# and dtDate<= #%s 23:59:59#)",pSearch_normal_task->conditions.sDate_From,pSearch_normal_task->conditions.sTime_From,pSearch_normal_task->conditions.sDate_From);
		}
		else
		{
			sprintf(szBuf," AND DateDiff('D', dtDate, '%s') = 0",pSearch_normal_task->conditions.sDate_From);
		}

		pSearch_normal_task->conditions.bIsCondition = true;
		strcat(rs_par.sql,szBuf);
	}
	else if( strlen(pSearch_normal_task->conditions.sDate_From) <= 0 && strlen(pSearch_normal_task->conditions.sDate_To) > 0 )
	{
		status = STATUS_INVALID_PARAMETERS;
		return status;
	}
	else if( strlen(pSearch_normal_task->conditions.sDate_From) <= 0 && strlen(pSearch_normal_task->conditions.sDate_To) <= 0 )
	{
		char buf[MAX_PATH]={0};
		CTime tm;
		tm=CTime::GetCurrentTime();

		sprintf(buf,"%04d-%02d-%02d",tm.GetYear(),tm.GetMonth(),tm.GetDay());

		if( strlen(pSearch_normal_task->conditions.sTime_From) > 0 && strlen(pSearch_normal_task->conditions.sTime_To) > 0 )
		{
			memset(&szBuf,0,sizeof(szBuf));
			sprintf(szBuf," AND (dtDate >= #%s %s# and dtDate<= #%s %s#)",buf,pSearch_normal_task->conditions.sTime_From,buf,pSearch_normal_task->conditions.sTime_To);
			strcat(rs_par.sql,szBuf);

			pSearch_normal_task->conditions.bIsCondition = true;

		}
		else if( strlen(pSearch_normal_task->conditions.sTime_From) > 0 && strlen(pSearch_normal_task->conditions.sTime_To) <= 0 )
		{
			memset(&szBuf,0,sizeof(szBuf));
			sprintf(szBuf," AND (dtDate >= #%s %s# and dtDate<= #%s 23:59:59#)",buf,pSearch_normal_task->conditions.sTime_From,buf);
			strcat(rs_par.sql,szBuf);

			pSearch_normal_task->conditions.bIsCondition = true;
		}
		else if( strlen(pSearch_normal_task->conditions.sTime_From) <= 0 && strlen(pSearch_normal_task->conditions.sTime_To) > 0 )
		{
			status = STATUS_INVALID_PARAMETERS;
			return status;
		}
	}



	//if( pSearch_normal_task->conditions.bIsCondition == false && pSearch_normal_task->bIsShowTodayTasks == true )//未指定起止日期或ID且显示当天任务
	//{
	//	memset(&szBuf,0,sizeof(szBuf));

	//	if( pSearch_normal_task->bIsSearch_QC_Task == true )//质控任务
	//	{
	//		sprintf(szBuf," AND ( DateDiff('D', dtDate, NOW()) = 0 AND (nAttribute= %d OR nAttribute= %d OR nAttribute= %d OR nAttribute= %d OR nAttribute= %d))",UsQCByP,UsQCByN,UdcQCBy1,UdcQCBy2,UdcQCBy3);
	//	}
	//	else if( pSearch_normal_task->bIsSearch_QC_Task == false )
	//	{
	//		if( pSearch_normal_task->conditions.nTestDes == ALLTestDes )//所有任务
	//		{				
	//			sprintf(szBuf," AND DateDiff('D', dtDate, NOW()) = 0");
	//		}
	//		else//普通任务
	//		{
	//			sprintf(szBuf," AND ( DateDiff('D', dtDate, NOW()) = 0 AND (nAttribute= %d OR nAttribute= %d) )",TestNormal,TestEP);
	//		}
	//	}

	//	strcat(rs_par.sql,szBuf);
	//}
	//else if( pSearch_normal_task->conditions.bIsCondition == true && pSearch_normal_task->bIsShowTodayTasks == true )//指定起止日期或ID且显示当天任务
	//{
	//	memset(&szBuf,0,sizeof(szBuf));

	//	if( pSearch_normal_task->bIsSearch_QC_Task == true )//质控任务
	//	{
	//		sprintf(szBuf," OR ( DateDiff('D', dtDate, NOW()) = 0 AND (nAttribute= %d OR nAttribute= %d OR nAttribute= %d OR nAttribute= %d OR nAttribute= %d))",UsQCByP,UsQCByN,UdcQCBy1,UdcQCBy2,UdcQCBy3);
	//	}
	//	else if( pSearch_normal_task->bIsSearch_QC_Task == false )
	//	{
	//		if( pSearch_normal_task->conditions.nTestDes == ALLTestDes )//所有任务
	//		{				
	//			sprintf(szBuf," OR DateDiff('D', dtDate, NOW()) = 0");
	//		}
	//		else//普通任务
	//		{
	//			sprintf(szBuf," OR ( DateDiff('D', dtDate, NOW()) = 0 AND (nAttribute= %d OR nAttribute= %d) )",TestNormal,TestEP);
	//		}
	//	}

	//	strcat(rs_par.sql,szBuf);
	//}
	//else if( pSearch_normal_task->conditions.bIsCondition == true && pSearch_normal_task->bIsShowTodayTasks == false )
	//{

	//}
	//else if( pSearch_normal_task->conditions.bIsCondition == false && pSearch_normal_task->bIsShowTodayTasks == false )
	//{

	//}






	//if( pSearch_normal_task->conditions.bIsCondition == false )
	//{
	//	memset(&szBuf,0,sizeof(szBuf));
	//	
	//	if( pSearch_normal_task->bIsShowTodayTasks == TRUE )		
	//	{
	//		if( pSearch_normal_task->bIsSearch_QC_Task == true )//质控任务
	//		{
	//			sprintf(szBuf," AND ( DateDiff('D', dtDate, NOW()) = 0 AND (nAttribute= %d OR nAttribute= %d OR nAttribute= %d OR nAttribute= %d OR nAttribute= %d))",UsQCByP,UsQCByN,UdcQCBy1,UdcQCBy2,UdcQCBy3);
	//		}
	//		else if( pSearch_normal_task->bIsSearch_QC_Task == false )
	//		{
	//			if( pSearch_normal_task->conditions.nTestDes == ALLTestDes )//所有任务
	//			{				
	//				sprintf(szBuf," AND DateDiff('D', dtDate, NOW()) = 0");
	//			}
	//			else//普通任务
	//			{
	//				sprintf(szBuf," AND ( DateDiff('D', dtDate, NOW()) = 0 AND (nAttribute= %d OR nAttribute= %d) )",TestNormal,TestEP);
	//			}
	//		}

	//		strcat(rs_par.sql,szBuf);
	//	}	
	//	else
	//	{

	//	}
	//}

	if( pSearch_normal_task->bIsShowTodayTasks == true ) 
	{
		memset(&szBuf,0,sizeof(szBuf));
		/*sprintf(szBuf," OR DateDiff('D', dtDate, NOW()) = 0");
		strcat(rs_par.sql,szBuf);*/

		if( pSearch_normal_task->conditions.bIsCondition == false )
		{
			if( pSearch_normal_task->bIsSearch_QC_Task == true )//质控任务
			{
                sprintf(szBuf, " AND ( DateDiff('D', dtDate, NOW()) = 0 AND (nAttribute= %d OR nAttribute= %d OR nAttribute= %d OR nAttribute= %d OR nAttribute= %d))", UsQCBy1, UsQCBy2, UsQCBy3, UdcQCByP, UdcQCByN);
			}
			else if( pSearch_normal_task->bIsSearch_QC_Task == false )
			{
				if( pSearch_normal_task->conditions.nTestDes == ALLTestDes )//所有任务
				{				
					sprintf(szBuf," AND DateDiff('D', dtDate, NOW()) = 0");
				}
				else//普通任务
				{
					sprintf(szBuf," AND ( DateDiff('D', dtDate, NOW()) = 0 AND (nAttribute= %d OR nAttribute= %d) )",TestNormal,TestEP);
				}
			}

		}
		else
		{
			if( pSearch_normal_task->bIsSearch_QC_Task == true )//质控任务
			{
                sprintf(szBuf, " OR ( DateDiff('D', dtDate, NOW()) = 0 AND (nAttribute= %d OR nAttribute= %d OR nAttribute= %d OR nAttribute= %d OR nAttribute= %d))", UsQCBy1, UsQCBy2, UsQCBy3, UdcQCByP, UdcQCByN);
			}
			else if( pSearch_normal_task->bIsSearch_QC_Task == false )
			{
				if( pSearch_normal_task->conditions.nTestDes == ALLTestDes )//所有任务
				{				
					sprintf(szBuf," OR DateDiff('D', dtDate, NOW()) = 0");
				}
				else//普通任务
				{
					sprintf(szBuf," OR ( DateDiff('D', dtDate, NOW()) = 0 AND (nAttribute= %d OR nAttribute= %d) )",TestNormal,TestEP);
				}
			}
		}		

		strcat(rs_par.sql,szBuf);
	}
	else
	{
		if( pSearch_normal_task->conditions.bIsCondition == false )
		{
			memset(&szBuf,0,sizeof(szBuf));
			sprintf(szBuf," AND DateDiff('D', dtDate, NOW()) = 0");
			strcat(rs_par.sql,szBuf);
		}
	}
	if (pSearch_normal_task->bIsSearch_QC_Task == true)
	{
		strcat(rs_par.sql, " ORDER BY tMain.dtDate");
	}
	else
	{
		strcat(rs_par.sql, " ORDER BY tMain.nID");
	}

	DBG_MSG("%s\n",rs_par.sql);
	try
	{
		status = Open_Rs(&rs_par);

		if(status != STATUS_SUCCESS)
		{
			status = STATUS_INVALID_PARAMETERS;
			return status;
		}


		if(!IsBOF(rs_par.Rs))
		{
			rs_par.Rs->MoveFirst();
		}

		unsigned int nCounts = 0;
		unsigned int nIndex = 0;

		//DBG_MSG("IsEOF=%d\n",IsEOF(rs_par.Rs));

		_variant_t Value;

		for (int i = 0; !IsEOF(rs_par.Rs); rs_par.Rs->MoveNext(),i++)
		{
			int  nType = rs_par.Rs->GetCollect(_variant_t("nAttribute"));			

			if( nType >= TestNormal && nType <= TestEP )
			{
				++nCounts;
			}
			else			
			{
				continue;
			}


			if(pSearch_normal_task->pSearchInfo == NULL)
			{
				pSearch_normal_task->pSearchInfoCounts = nCounts;
				status = STATUS_LOGIN_MEMORY_NOTENOUGH;

			}
			else
			{


				// ID
				pSearch_normal_task->pSearchInfo[nIndex].main_info.nID = rs_par.Rs->GetCollect(_variant_t("tMain.nID"));

				// nAlarm
				pSearch_normal_task->pSearchInfo[nIndex].main_info.nAlarm = rs_par.Rs->GetCollect(_variant_t("nAlarm"));

				// nRow
				pSearch_normal_task->pSearchInfo[nIndex].main_info.nRow = rs_par.Rs->GetCollect(_variant_t("nRow"));

				// bMain
				pSearch_normal_task->pSearchInfo[nIndex].main_info.bMain = (bool)rs_par.Rs->GetCollect(_variant_t("bMain"));

				// nState
				pSearch_normal_task->pSearchInfo[nIndex].main_info.nState = (int)rs_par.Rs->GetCollect(_variant_t("nState"));

				// nAttribute
				pSearch_normal_task->pSearchInfo[nIndex].main_info.nAttribute = (int)rs_par.Rs->GetCollect(_variant_t("nAttribute"));

				// nTestType
				pSearch_normal_task->pSearchInfo[nIndex].main_info.nTestType = (int)rs_par.Rs->GetCollect(_variant_t("nTestType"));

				// nSN
				pSearch_normal_task->pSearchInfo[nIndex].main_info.nSN = (int)rs_par.Rs->GetCollect(_variant_t("nSN"));

				// nPos
				pSearch_normal_task->pSearchInfo[nIndex].main_info.nPos = (float)rs_par.Rs->GetCollect(_variant_t("nPos"));


				// nCha
				pSearch_normal_task->pSearchInfo[nIndex].main_info.nCha = (int)rs_par.Rs->GetCollect(_variant_t("nCha"));

				// sCode
				Value = rs_par.Rs->GetCollect(_variant_t("sCode"));
				if(Value.vt != VT_NULL)
				{					
					strcpy(pSearch_normal_task->pSearchInfo[nIndex].main_info.sCode,_bstr_t(Value));
				}

				//sCompany
				Value = rs_par.Rs->GetCollect(_variant_t("sCompany"));

				if(Value.vt != VT_NULL)
				{					
					strcpy(pSearch_normal_task->pSearchInfo[nIndex].main_info.sCompany,_bstr_t(Value));
				}

				//sDepartment
				Value = rs_par.Rs->GetCollect(_variant_t("sDepartment"));

				if(Value.vt != VT_NULL)
				{					
					strcpy(pSearch_normal_task->pSearchInfo[nIndex].main_info.sDepartment,_bstr_t(Value));
				}

				//sDoctor
				Value = rs_par.Rs->GetCollect(_variant_t("sDoctor"));
				if(Value.vt != VT_NULL)
				{					
					strcpy(pSearch_normal_task->pSearchInfo[nIndex].main_info.sDoctor,_bstr_t(Value));
				}

				//sDocimaster
				Value = rs_par.Rs->GetCollect(_variant_t("sDocimaster"));
				if(Value.vt != VT_NULL)
				{					
					strcpy(pSearch_normal_task->pSearchInfo[nIndex].main_info.sDocimaster,_bstr_t(Value));
				}

				//sAuditor
				Value = rs_par.Rs->GetCollect(_variant_t("sAuditor"));
				if(Value.vt != VT_NULL)
				{
					strcpy(pSearch_normal_task->pSearchInfo[nIndex].main_info.sAuditor,_bstr_t(Value));
				}

				//sReporter
				Value = rs_par.Rs->GetCollect(_variant_t("sReporter"));
				if(Value.vt != VT_NULL)
				{
					strcpy(pSearch_normal_task->pSearchInfo[nIndex].main_info.sReporter,_bstr_t(Value));
				}

				//sColor
				Value = rs_par.Rs->GetCollect(_variant_t("sColor"));
				if(Value.vt != VT_NULL)
				{
					strcpy(pSearch_normal_task->pSearchInfo[nIndex].main_info.sColor,_bstr_t(Value));
				}

				//sTransparency
				Value = rs_par.Rs->GetCollect(_variant_t("sTransparency"));
				if(Value.vt != VT_NULL)
				{
					strcpy(pSearch_normal_task->pSearchInfo[nIndex].main_info.sTransparency,_bstr_t(Value));
				}

				//sFolder
				Value = rs_par.Rs->GetCollect(_variant_t("sFolder"));

				if(Value.vt != VT_NULL)
				{
					strcpy(pSearch_normal_task->pSearchInfo[nIndex].main_info.sFolder,_bstr_t(Value));
				}

				//sImage
				Value = rs_par.Rs->GetCollect(_variant_t("sImage"));

				if(Value.vt != VT_NULL)
				{
					strcpy(pSearch_normal_task->pSearchInfo[nIndex].main_info.sImage,_bstr_t(Value));
				}

				// sDiagnosis
				Value = rs_par.Rs->GetCollect(_variant_t("sDiagnosis"));
				if(Value.vt != VT_NULL)
				{
					strcpy(pSearch_normal_task->pSearchInfo[nIndex].main_info.sDiagnosis,_bstr_t(Value));
				}

				// sSuggestion
				Value = rs_par.Rs->GetCollect(_variant_t("sSuggestion"));
				if(Value.vt != VT_NULL)
				{
					strcpy(pSearch_normal_task->pSearchInfo[nIndex].main_info.sSuggestion,_bstr_t(Value));
				}
				// dtDate
				Value = rs_par.Rs->GetCollect(_variant_t("dtDate"));
				if(Value.vt != VT_NULL)
				{
					COleDateTime odt(Value.date);
					strcpy(pSearch_normal_task->pSearchInfo[nIndex].main_info.dtDate, odt.Format("%Y-%m-%d %H:%M:%S").GetBuffer(0));
				}


				//dtPrintStatus
				Value = rs_par.Rs->GetCollect(_variant_t("dtPrintStatus"));
				if(Value.vt != VT_NULL)
					pSearch_normal_task->pSearchInfo[nIndex].main_info.dtPrintStatus  = Value;


				//dtLisStatus
				Value = rs_par.Rs->GetCollect(_variant_t("dtLisStatus"));
				if(Value.vt != VT_NULL)
					pSearch_normal_task->pSearchInfo[nIndex].main_info.dtLisStatus  = Value;


				// tPat_nID
				pSearch_normal_task->pSearchInfo[nIndex].pat_info.nID = rs_par.Rs->GetCollect(_variant_t("tPat.nID"));	
				// bPat
				pSearch_normal_task->pSearchInfo[nIndex].pat_info.bPat = rs_par.Rs->GetCollect(_variant_t("bPat"));
				// nNo
				pSearch_normal_task->pSearchInfo[nIndex].pat_info.nNo = rs_par.Rs->GetCollect(_variant_t("nNo"));

				// nSex
				pSearch_normal_task->pSearchInfo[nIndex].pat_info.nSex = rs_par.Rs->GetCollect(_variant_t("nSex"));
				// nAge
				pSearch_normal_task->pSearchInfo[nIndex].pat_info.nAge = rs_par.Rs->GetCollect(_variant_t("nAge"));
				// nAgeUnit
				pSearch_normal_task->pSearchInfo[nIndex].pat_info.nAgeUnit = rs_par.Rs->GetCollect(_variant_t("nAgeUnit"));

				// sName
				Value = rs_par.Rs->GetCollect(_variant_t("sName"));
				if(Value.vt != VT_NULL)
				{					
					strcpy(pSearch_normal_task->pSearchInfo[nIndex].pat_info.sName,_bstr_t(Value));
				}

				// sHospital
				Value = rs_par.Rs->GetCollect(_variant_t("sHospital"));
				if(Value.vt != VT_NULL)
				{					
					strcpy(pSearch_normal_task->pSearchInfo[nIndex].pat_info.sHospital,_bstr_t(Value));
				}

				// sSickBed
				Value = rs_par.Rs->GetCollect(_variant_t("sSickBed"));
				if(Value.vt != VT_NULL)
				{					
					strcpy(pSearch_normal_task->pSearchInfo[nIndex].pat_info.sSickBed,_bstr_t(Value));
				}

				// dtBirthDate
				Value = rs_par.Rs->GetCollect(_variant_t("dtBirthDate"));
				if(Value.vt != VT_NULL)
				{					
					strcpy(pSearch_normal_task->pSearchInfo[nIndex].pat_info.dtBirthDate,_bstr_t(Value));
				}




				// us_id
				pSearch_normal_task->pSearchInfo[nIndex].us_info.nID = rs_par.Rs->GetCollect(_variant_t("tUs.nID"));
				// bUs
				pSearch_normal_task->pSearchInfo[nIndex].us_info.bUs = rs_par.Rs->GetCollect(_variant_t("bUs"));
				//nVariableRatio
				pSearch_normal_task->pSearchInfo[nIndex].us_info.nVariableRatio = rs_par.Rs->GetCollect(_variant_t("nVariableRatio"));


				int nUsIndex = 1;
 				for(unsigned int j =0 ; j < MAX_US_LENS ; j++)
				{
					char szBuf[MAX_PATH]={0};
					sprintf(szBuf,"us_res%d",nUsIndex);
					nUsIndex++;
					pSearch_normal_task->pSearchInfo[nIndex].us_info.us_node[j].us_res = (float)rs_par.Rs->GetCollect(_variant_t(szBuf));
					/*Value = rs_par.Rs->GetCollect(_variant_t(szBuf));
					DBG_MSG("szBuf = %s,Value = %d\n",szBuf,Value);
					if(Value.vt != VT_NULL)
					pSearch_normal_task->pSearchInfo[nIndex].us_info.us_node[j].us_res = Value;*/
					//DBG_MSG("pSearch_normal_task->pSearchInfo[%d].us_info.us_node[%d].us_res = %f\n",nIndex,j,pSearch_normal_task->pSearchInfo[nIndex].us_info.us_node[j].us_res);			
				}






				// tUdc_nID
				pSearch_normal_task->pSearchInfo[nIndex].udc_info.nID = (int)rs_par.Rs->GetCollect(_variant_t("tUdc.nID"));	
				// bUdc
				pSearch_normal_task->pSearchInfo[nIndex].udc_info.bUdc = (int)rs_par.Rs->GetCollect(_variant_t("bUdc"));

				int nUdcIndex = 1;
				for(unsigned int x = 0 ; x < MAX_UDC_LENS ; x ++)
				{
					char szBuf[MAX_PATH] = {0} ;
					sprintf(szBuf,"udc_sRes%d",nUdcIndex);
					Value = rs_par.Rs->GetCollect(_variant_t(szBuf));
					if(Value.vt != VT_NULL)
					{
						strcpy(pSearch_normal_task->pSearchInfo[nIndex].udc_info.udc_node[x].udc_res,
							_bstr_t(Value));
					}
					//DBG_MSG("pSearch_normal_task->pSearchInfo[%d].udc_info.udc_node[%d].udc_res = %s\n",nIndex,x,pSearch_normal_task->pSearchInfo[nIndex].udc_info.udc_node[x].udc_res);

					sprintf(szBuf,"sValue%d",nUdcIndex);
					Value = rs_par.Rs->GetCollect(_variant_t(szBuf));

					if(Value.vt != VT_NULL)
					{
						strcpy(pSearch_normal_task->pSearchInfo[nIndex].udc_info.udc_node[x].udc_value,_bstr_t(Value));
					}

					sprintf(szBuf,"bAbn%d",nUdcIndex);
					Value = (bool)rs_par.Rs->GetCollect(_variant_t(szBuf));
					if(Value.vt != VT_NULL)
						pSearch_normal_task->pSearchInfo[nIndex].udc_info.udc_node[x].udc_abn  = Value;

					sprintf(szBuf,"nColor%d",nUdcIndex);
					Value = (int)rs_par.Rs->GetCollect(_variant_t(szBuf));
					if(Value.vt != VT_NULL)
						pSearch_normal_task->pSearchInfo[nIndex].udc_info.udc_node[x].udc_col  = Value;			
					nUdcIndex++;

				}

			                                                                                                                                                                                                                                                                                                                          
				nIndex ++;
				status = STATUS_SUCCESS;
			}

		} // end for

	} 
	catch (_com_error e)
	{
		status = STATUS_EXCEPTION_ERROR;
		DBG_MSG("SearchTaskInfo _com_error,errMessage = %s" ,
			(const char *)(LPCTSTR)e.Description());
	}

	// 关闭
	Close_Rs(rs_par.Rs);

	return status;

}

KSTATUS TH_DB_Access::GetLisStatus( int nID,unsigned int* pStatus )
{
	KSTATUS status = STATUS_SUCCESS;
	OPEN_RS_PAR rs_par;

	if(m_Connection == NULL)
	{
		status = STATUS_INVALID_PARAMETERS;
		return status;
	}

	sprintf(rs_par.sql,"SELECT * FROM ((tMain left outer join tUs on tMain.nID=tUs.nID) \
					   left outer join tUdc on tMain.nID=tUdc.nID )\
					   left outer join tPat on tMain.nID=tPat.nID \
					   where tMain.nID=%d",nID);


	try
	{
		status = Open_Rs(&rs_par);

		if(status != STATUS_SUCCESS)
		{
			status = STATUS_INVALID_PARAMETERS;
			return status;
		}

		if (!IsBOF(rs_par.Rs))
		{
			rs_par.Rs->MoveFirst();
		}

		for (int i = 0; !IsEOF(rs_par.Rs); rs_par.Rs->MoveNext(),i++)
		{
			*pStatus = (int)rs_par.Rs->GetCollect(_variant_t("dtLisStatus"));
			break;

		} // end for
	}
	catch (_com_error e)
	{
		status = STATUS_EXCEPTION_ERROR;
		DBG_MSG("GetLisStatus _com_error,errMessage = %s" ,
			(const char *)(LPCTSTR)e.Description());
	}

	// 关闭
	Close_Rs(rs_par.Rs);
	return status;

}

KSTATUS TH_DB_Access::SetLisStatus( int nID,unsigned int nStatus )
{
	KSTATUS status = STATUS_SUCCESS;
	OPEN_RS_PAR rs_par;

	if(m_Connection == NULL)
	{
		status = STATUS_INVALID_PARAMETERS;
		return status;
	}

	sprintf(rs_par.sql,"SELECT * FROM ((tMain left outer join tUs on tMain.nID=tUs.nID) \
					   left outer join tUdc on tMain.nID=tUdc.nID )\
					   left outer join tPat on tMain.nID=tPat.nID \
					   where tMain.nID=%d",nID);


	try
	{
		status = Open_Rs(&rs_par);

		if(status != STATUS_SUCCESS)
		{
			status = STATUS_INVALID_PARAMETERS;
			return status;
		}

		if (!IsBOF(rs_par.Rs))
		{
			rs_par.Rs->MoveFirst();
		}

		for (int i = 0; !IsEOF(rs_par.Rs); rs_par.Rs->MoveNext(),i++)
		{
			rs_par.Rs->PutCollect(_variant_t("dtLisStatus"),_variant_t(nStatus));
			rs_par.Rs->Update();

		} // end for
	}
	catch (_com_error e)
	{
		status = STATUS_EXCEPTION_ERROR;
		DBG_MSG("SetLisStatus _com_error,errMessage = %s" ,
			(const char *)(LPCTSTR)e.Description());
	}

	// 关闭
	Close_Rs(rs_par.Rs);

	return status;


}

KSTATUS TH_DB_Access::GetPrintStatus( int nID,unsigned int* pStatus )
{
	KSTATUS status = STATUS_SUCCESS;
	OPEN_RS_PAR rs_par;

	if(m_Connection == NULL)
	{
		status = STATUS_INVALID_PARAMETERS;
		return status;
	}

	sprintf(rs_par.sql,"SELECT * FROM ((tMain left outer join tUs on tMain.nID=tUs.nID) \
					   left outer join tUdc on tMain.nID=tUdc.nID ) \
					   left outer join tPat on tMain.nID=tPat.nID \
					   where tMain.nID=%d",nID);
	try
	{
		status = Open_Rs(&rs_par);

		if(status != STATUS_SUCCESS)
		{
			status = STATUS_INVALID_PARAMETERS;
			return status;
		}

		if (!IsBOF(rs_par.Rs))
		{
			rs_par.Rs->MoveFirst();
		}

		for (int i = 0; !IsEOF(rs_par.Rs); rs_par.Rs->MoveNext(),i++)
		{
			*pStatus = (int)rs_par.Rs->GetCollect(_variant_t("dtPrintStatus"));
			break;

		} // end for
	}
	catch (_com_error e)
	{
		status = STATUS_EXCEPTION_ERROR;
		DBG_MSG("GetPrintStatus _com_error,errMessage = %s" ,
			(const char *)(LPCTSTR)e.Description());
	}

	// 关闭
	Close_Rs(rs_par.Rs);

	return status;

}

KSTATUS TH_DB_Access::SetPrintStatus( int nID,unsigned int nStatus )
{
	KSTATUS status = STATUS_SUCCESS;
	OPEN_RS_PAR rs_par;

	if(m_Connection == NULL)
	{
		status = STATUS_INVALID_PARAMETERS;
		return status;
	}

	sprintf(rs_par.sql,"SELECT * FROM ((tMain left outer join tUs on tMain.nID=tUs.nID) \
					   left outer join tUdc on tMain.nID=tUdc.nID ) \
					   left outer join tPat on tMain.nID=tPat.nID \
					   where tMain.nID=%d",nID);


	try
	{
		status = Open_Rs(&rs_par);

		if(status != STATUS_SUCCESS)
		{
			status = STATUS_INVALID_PARAMETERS;
			return status;
		}

		if (!IsBOF(rs_par.Rs))
		{
			rs_par.Rs->MoveFirst();
		}

		for (int i = 0; !IsEOF(rs_par.Rs); rs_par.Rs->MoveNext(),i++)
		{
			rs_par.Rs->PutCollect(_variant_t("dtPrintStatus"),_variant_t(nStatus));
			rs_par.Rs->Update();

		} // end for
	}
	catch (_com_error e)
	{
		status = STATUS_EXCEPTION_ERROR;
		DBG_MSG("SetPrintStatus _com_error,errMessage = %s" ,
			(const char *)(LPCTSTR)e.Description());
	}

	// 关闭
	Close_Rs(rs_par.Rs);

	return status;

}

KSTATUS TH_DB_Access::GetPrintPicPath( int nID,char* pPrintPicPath )
{
	KSTATUS status = STATUS_SUCCESS;
	OPEN_RS_PAR rs_par;

	if(m_Connection == NULL)
	{
		status = STATUS_INVALID_PARAMETERS;
		return status;
	}

	sprintf(rs_par.sql,"SELECT * FROM ((tMain left outer join tUs on tMain.nID=tUs.nID) \
					   left outer join tUdc on tMain.nID=tUdc.nID ) \
					   left outer join tPat on tMain.nID=tPat.nID \
					   where tMain.nID=%d",nID);


	try
	{
		status = Open_Rs(&rs_par);

		if(status != STATUS_SUCCESS)
		{
			status = STATUS_INVALID_PARAMETERS;
			return status;
		}

		if (!IsBOF(rs_par.Rs))
		{
			rs_par.Rs->MoveFirst();
		}

		for (int i = 0; !IsEOF(rs_par.Rs); rs_par.Rs->MoveNext(),i++)
		{
			
			_variant_t Value = rs_par.Rs->GetCollect(_variant_t("sImage"));

			if(Value.vt != VT_NULL)
			{
				strcpy(pPrintPicPath,_bstr_t(Value));
			}

		} // end for
	}
	catch (_com_error e)
	{
		status = STATUS_EXCEPTION_ERROR;
		DBG_MSG("GetPrintPicPath _com_error,errMessage = %s" ,
			(const char *)(LPCTSTR)e.Description());
	}

	// 关闭
	Close_Rs(rs_par.Rs);

	return status;
}

KSTATUS TH_DB_Access::SetPrintPicPath( int nID,char* pPrintPicPath )
{
	KSTATUS status = STATUS_SUCCESS;
	OPEN_RS_PAR rs_par;

	if(m_Connection == NULL)
	{
		status = STATUS_INVALID_PARAMETERS;
		return status;
	}

	sprintf(rs_par.sql,"SELECT * FROM ((tMain left outer join tUs on tMain.nID=tUs.nID) \
					   left outer join tUdc on tMain.nID=tUdc.nID ) \
					   left outer join tPat on tMain.nID=tPat.nID \
					   where tMain.nID=%d",nID);


	try
	{
		status = Open_Rs(&rs_par);

		if(status != STATUS_SUCCESS)
		{
			status = STATUS_INVALID_PARAMETERS;
			return status;
		}

		if (!IsBOF(rs_par.Rs))
		{
			rs_par.Rs->MoveFirst();
		}

		for (int i = 0; !IsEOF(rs_par.Rs); rs_par.Rs->MoveNext(),i++)
		{			
			rs_par.Rs->PutCollect(_variant_t("sImage"),_variant_t(pPrintPicPath));
			rs_par.Rs->Update();

		} // end for
	}
	catch (_com_error e)
	{
		status = STATUS_EXCEPTION_ERROR;
		DBG_MSG("SetPrintPicPath _com_error,errMessage = %s" ,
			(const char *)(LPCTSTR)e.Description());
	}

	// 关闭
	Close_Rs(rs_par.Rs);

	return status;
}

KSTATUS TH_DB_Access::GetLISList( SENDTO_LIS_ID* pSendto_lis_id )
{
	KSTATUS status = STATUS_SUCCESS;

	if(m_Connection == NULL)
	{
		status = STATUS_INVALID_PARAMETERS;
		return status;
	}


	unsigned int nLisInfoCounts = 1;

	status = GetTaskInfo(true,pSendto_lis_id->SendtoLisID,&pSendto_lis_id->LisInfo,
		&nLisInfoCounts);

	if(status != STATUS_SUCCESS)
	{
		return status;
	}

	GET_US_INFO get_us_info;
	memset(&get_us_info,0,sizeof(get_us_info));

	status = GetUsInfo(&get_us_info);

	if(status != STATUS_SUCCESS)
	{
		return status;
	}

	for(unsigned int i = 0 ; i < MAX_US_LENS ; i ++)
	{

		pSendto_lis_id->LisInfo.us_info.us_node[i].bIsSendtoLIS = get_us_info.par[i].bIsSendtoLIS;
		pSendto_lis_id->LisInfo.us_info.us_node[i].bIsShowUi = get_us_info.par[i].bIsShowUI;
		pSendto_lis_id->LisInfo.us_info.us_node[i].bIsReport = get_us_info.par[i].bIsSendtoReport;

		pSendto_lis_id->LisInfo.us_info.us_node[i].nMax_normal_female = get_us_info.par[i].nMax_normal_female;
		pSendto_lis_id->LisInfo.us_info.us_node[i].nMax_normal_male   = get_us_info.par[i].nMax_normal_male;

		pSendto_lis_id->LisInfo.us_info.us_node[i].nMax_unusual_female = get_us_info.par[i].nMax_unusual_female;
		pSendto_lis_id->LisInfo.us_info.us_node[i].nMax_unusual_male = get_us_info.par[i].nMax_unusual_male;

		pSendto_lis_id->LisInfo.us_info.us_node[i].nMin_normal_female = get_us_info.par[i].nMin_normal_female;
		pSendto_lis_id->LisInfo.us_info.us_node[i].nMin_normal_male = get_us_info.par[i].nMin_normal_male;

		pSendto_lis_id->LisInfo.us_info.us_node[i].nMin_unusual_female = get_us_info.par[i].nMin_unusual_female;
		pSendto_lis_id->LisInfo.us_info.us_node[i].nMin_unusual_male = get_us_info.par[i].nMin_unusual_male;
	}

	GET_UDC_INFO get_udc_info;
	memset(&get_udc_info,0,sizeof(get_udc_info));

	status = GetUdcInfo(&get_udc_info);

	if(status != STATUS_SUCCESS)
	{
		return status;
	}
	for(unsigned int i = 0 ; i < MAX_UDC_LENS ; i ++)
	{
		pSendto_lis_id->LisInfo.udc_info.udc_node[i].bIsSendtoLIS = get_udc_info.par[i].bIsSendtoLIS;
		pSendto_lis_id->LisInfo.udc_info.udc_node[i].bIsShowUi = get_udc_info.par[i].bIsShowUI;
		pSendto_lis_id->LisInfo.udc_info.udc_node[i].bIsReport = get_udc_info.par[i].bIsSendtoReport;
	}

	return status;

}

KSTATUS TH_DB_Access::SetBarcode(int nID, char* pBarcode)
{
	KSTATUS status = STATUS_SUCCESS;
	OPEN_RS_PAR rs_par;

	if (m_Connection == NULL)
	{
		status = STATUS_INVALID_PARAMETERS;
		return status;
	}

	sprintf(rs_par.sql, "SELECT * FROM ((tMain left outer join tUs on tMain.nID=tUs.nID) \
						left outer join tUdc on tMain.nID=tUdc.nID ) \
						left outer join tPat on tMain.nID=tPat.nID \
						where tMain.nID=%d", nID);
	try
	{
		status = Open_Rs(&rs_par);

		if (status != STATUS_SUCCESS)
		{
			status = STATUS_INVALID_PARAMETERS;
			return status;
		}

		if (!IsBOF(rs_par.Rs))
		{
			rs_par.Rs->MoveFirst();
		}

		for (int i = 0; !IsEOF(rs_par.Rs); rs_par.Rs->MoveNext(), i++)
		{
			rs_par.Rs->PutCollect(_variant_t("sCode"), _variant_t(pBarcode));
			rs_par.Rs->Update();

		} // end for
	}
	catch (_com_error e)
	{
		status = STATUS_EXCEPTION_ERROR;
		DBG_MSG("SetBarcode _com_error,errMessage = %s",
			(const char *)(LPCTSTR)e.Description());
	}

	// 关闭
	Close_Rs(rs_par.Rs);

	return status;
}

KSTATUS TH_DB_Access::GetBarcode(int nID, char* pBarcode)
{
	KSTATUS status = STATUS_SUCCESS;
	OPEN_RS_PAR rs_par;

	if (m_Connection == NULL)
	{
		status = STATUS_INVALID_PARAMETERS;
		return status;
	}

	sprintf(rs_par.sql, "SELECT * FROM ((tMain left outer join tUs on tMain.nID=tUs.nID) \
						left outer join tUdc on tMain.nID=tUdc.nID ) \
						left outer join tPat on tMain.nID=tPat.nID \
						where tMain.nID=%d", nID);
	try
	{
		status = Open_Rs(&rs_par);

		if (status != STATUS_SUCCESS)
		{
			status = STATUS_INVALID_PARAMETERS;
			return status;
		}

		if (!IsBOF(rs_par.Rs))
		{
			rs_par.Rs->MoveFirst();
		}

		for (int i = 0; !IsEOF(rs_par.Rs); rs_par.Rs->MoveNext(), i++)
		{

			_variant_t Value = rs_par.Rs->GetCollect(_variant_t("sCode"));

			if (Value.vt != VT_NULL)
			{
				strcpy_s(pBarcode, MAX_BARCODE_LENS-1, _bstr_t(Value));
			}

		} // end for
	}
	catch (_com_error e)
	{
		status = STATUS_EXCEPTION_ERROR;
		DBG_MSG("GetBarcode _com_error,errMessage = %s",
			(const char *)(LPCTSTR)e.Description());
	}

	// 关闭
	Close_Rs(rs_par.Rs);

	return status;
}

KSTATUS TH_DB_Access::GetLastTaskId(ULONG *nID)
{
    KSTATUS status = STATUS_SUCCESS;
    OPEN_RS_PAR rs_par;

    if (m_Connection == NULL || nID == NULL)
    {
        status = STATUS_INVALID_PARAMETERS;
        return status;
    }

    strcpy(rs_par.sql, "select top 1 nID from tMain order by nID desc");

    try
    {
        status = Open_Rs(&rs_par);

        if (status != STATUS_SUCCESS)
        {
            return status;
        }

        if (!IsBOF(rs_par.Rs))
        {
            rs_par.Rs->MoveFirst();
        }

        _variant_t Value = rs_par.Rs->GetCollect(_variant_t("nID"));
        if (Value.vt != VT_NULL) *nID = Value;

        status = STATUS_SUCCESS;
    }
    catch (_com_error e)
    {
        status = STATUS_EXCEPTION_ERROR;
        DBG_MSG("GetLastTaskId _com_error,errMessage = %s",
            (const char *)(LPCTSTR)e.Description());
    }

    // 关闭
    Close_Rs(rs_par.Rs);


    return status;
}

KSTATUS TH_DB_Access::SetPicPath( SET_PIC_PATH set_pic_path )
{
	KSTATUS status = STATUS_SUCCESS;
	OPEN_RS_PAR rs_par;

	if(m_Connection == NULL)
	{
		status = STATUS_INVALID_PARAMETERS;
		return status;
	}

	sprintf(rs_par.sql,"SELECT * FROM ((tMain left outer join tUs on tMain.nID=tUs.nID) \
					   left outer join tUdc on tMain.nID=tUdc.nID ) \
					   left outer join tPat on tMain.nID=tPat.nID \
					   where tMain.nID=%d",set_pic_path.nID);


	try
	{
		status = Open_Rs(&rs_par);

		if(status != STATUS_SUCCESS)
		{
			status = STATUS_INVALID_PARAMETERS;
			return status;
		}

		if (!IsBOF(rs_par.Rs))
		{
			rs_par.Rs->MoveFirst();
		}

		for (int i = 0; !IsEOF(rs_par.Rs); rs_par.Rs->MoveNext(),i++)
		{
			rs_par.Rs->PutCollect(_variant_t("sFolder"),_variant_t(set_pic_path.sImageFolder));
			rs_par.Rs->Update();

		} // end for
	}
	catch (_com_error e)
	{
		status = STATUS_EXCEPTION_ERROR;
		DBG_MSG("SetPicPath _com_error,errMessage = %s" ,
			(const char *)(LPCTSTR)e.Description());
	}

	// 关闭
	Close_Rs(rs_par.Rs);

	return status;

}


bool TH_DB_Access::IsEOF( ADODB::_RecordsetPtr Rs )
{
	return Rs->adoEOF ? true : false;
}

bool TH_DB_Access::IsBOF( ADODB::_RecordsetPtr Rs )
{
	return Rs->adoBOF ? true : false;
}

_variant_t TH_DB_Access::GetLoginPassword( ADODB::_RecordsetPtr Rs )
{
	return Rs->GetCollect(_variant_t("sPassword"));
}

_variant_t TH_DB_Access::GetLoginGroup( ADODB::_RecordsetPtr Rs )
{
	return Rs->GetCollect(_variant_t("nGroup"));
}

//KSTATUS TH_DB_Access::AutoImportUdcData( UDC_IMPORT_INFO* pAutoImportUdcInfo,bool* pbIsNew,unsigned int* pID )
//{
//	KSTATUS status = STATUS_SUCCESS;
//
//	if(m_Connection == NULL)
//	{
//		status = STATUS_INVALID_PARAMETERS;
//		return status;
//	}
//
//	// 合并
//	status = AutoImport(pAutoImportUdcInfo,pbIsNew,pID);
//
//	if(status  != STATUS_SUCCESS)
//	{
//		status = STATUS_INVALID_PARAMETERS;
//		return status;
//	}
//
//	return status;
//
//}

//KSTATUS TH_DB_Access::AutoImport( UDC_IMPORT_INFO* pUdcImportInfo,bool* pbIsNew,unsigned int* pID )
//{
//	KSTATUS status = STATUS_SUCCESS;
//	OPEN_RS_PAR rs_par;
//
//	*pID = 0;
//	*pbIsNew = FALSE;
//
//
//	char szBuf[MAX_PATH*8]={0};
//	OutputDebugString("CTH_Db_Access::AutoImport CP1\n");
//
//	if(m_Connection == NULL)
//	{
//		OutputDebugString("CTH_Db_Access::AutoImport CP2\n");
//
//		status = STATUS_INVALID_PARAMETERS;
//		return status;
//	}
//
//	OutputDebugString("CTH_Db_Access::AutoImport CP3\n");
//
//	// 判断通过ID或者barcode合并
//	int nType = GetImportType();
//
//
//	// barcode
//	if(nType == 1)
//	{
//
//		sprintf(rs_par.sql, "SELECT * FROM tUdc WHERE nID = \
//							(SELECT  TOP 1  nID FROM tUdc WHERE sSN = '%s')",
//							pUdcImportInfo->ID);
//
//		/*
//
//		sprintf(rs_par.sql, "SELECT * FROM ((tMain LEFT OUTER JOIN tUdc ON tMain.nID = tUdc.nID)) \
//		WHERE DateDiff('D', dtDate, NOW()) = 0 And tUdc.sSN = '%s'",
//		pUdcImportInfo->ID);
//		*/
//
//	}
//	// id
//	else if(nType == 2)
//	{
//
//		sprintf(rs_par.sql, "SELECT * FROM tUdc WHERE nID = \
//							(SELECT TOP 1 nID FROM tMain WHERE DateDiff('D', dtSendDate, NOW()) = 0 AND nSN = %d AND bIsqc = false)",
//							atoi(pUdcImportInfo->ID));
//
//	}
//	// auto
//	else if(nType == 3)
//	{
//	}
//	else
//	{
//		return STATUS_INVALID_PARAMETERS;
//	}
//
//	sprintf(szBuf,"CTH_Db_Access::AutoImport : %s\n",rs_par.sql);
//	OutputDebugString(szBuf);
//
//
//	try
//	{
//		status = Open_Rs(&rs_par);
//
//		if(status != STATUS_SUCCESS)
//		{
//			OutputDebugString("CTH_Db_Access::AutoImport CP4\n");
//
//			status = STATUS_INVALID_PARAMETERS;
//			return status;
//		}
//
//		OutputDebugString("CTH_Db_Access::AutoImport CP5\n");
//
//		if (!IsBOF(rs_par.Rs))
//		{
//
//			OutputDebugString("CTH_Db_Access::AutoImport CP6\n");
//
//			*pbIsNew = false;
//			*pID = rs_par.Rs->GetCollect(_variant_t("nID"));
//
//
//			rs_par.Rs->PutCollect(_variant_t("sSN"),_variant_t(pUdcImportInfo->ID));
//
//			// GLU
//			if(0 != strlen(pUdcImportInfo->GLU))
//			{
//				rs_par.Rs->PutCollect(_variant_t("udc_sRes1"),
//					_variant_t(pUdcImportInfo->GLU));
//			}
//
//			// PRO
//			if(0 != strlen(pUdcImportInfo->PRO))
//			{
//				rs_par.Rs->PutCollect(_variant_t("udc_sRes2"),
//					_variant_t(pUdcImportInfo->PRO));
//			}
//
//			// BIL
//			if(0 != strlen(pUdcImportInfo->BIL))
//			{
//				rs_par.Rs->PutCollect(_variant_t("udc_sRes3"),
//					_variant_t(pUdcImportInfo->BIL));
//			}
//
//			// URO
//			if(0 != strlen(pUdcImportInfo->URO))
//			{
//				rs_par.Rs->PutCollect(_variant_t("udc_sRes4"),
//					_variant_t(pUdcImportInfo->URO));
//			}
//
//			// pH
//			if(0 != strlen(pUdcImportInfo->PH))
//			{
//				rs_par.Rs->PutCollect(_variant_t("udc_sRes5"),
//					_variant_t(pUdcImportInfo->PH));
//			}
//
//			// SG
//			if(0 != strlen(pUdcImportInfo->SG))
//			{
//				rs_par.Rs->PutCollect(_variant_t("udc_sRes6"),
//					_variant_t(pUdcImportInfo->SG));
//			}
//
//			// BLD
//			if(0 != strlen(pUdcImportInfo->BLD))
//			{
//				rs_par.Rs->PutCollect(_variant_t("udc_sRes7"),
//					_variant_t(pUdcImportInfo->BLD));
//			}
//
//			// KET
//			if(0 != strlen(pUdcImportInfo->KET))
//			{
//				rs_par.Rs->PutCollect(_variant_t("udc_sRes8"),
//					_variant_t(pUdcImportInfo->KET));
//			}
//
//			// NIT
//			if(0 != strlen(pUdcImportInfo->NIT))
//			{
//				rs_par.Rs->PutCollect(_variant_t("udc_sRes9"),
//					_variant_t(pUdcImportInfo->NIT));
//			}
//
//			// LEU
//			if(0 != strlen(pUdcImportInfo->LEU))
//			{
//				rs_par.Rs->PutCollect(_variant_t("udc_sRes10"),
//					_variant_t(pUdcImportInfo->LEU));
//			}
//
//			// VC
//			if(0 != strlen(pUdcImportInfo->VC))
//			{
//				rs_par.Rs->PutCollect(_variant_t("udc_sRes11"),
//					_variant_t(pUdcImportInfo->VC));
//			}
//
//			// MA
//			if(0 != strlen(pUdcImportInfo->MA))
//			{
//				rs_par.Rs->PutCollect(_variant_t("udc_sRes12"),
//					_variant_t(pUdcImportInfo->MA));
//			}
//
//
//			sprintf(szBuf,"pUdcImportInfo->Ext1:%s",pUdcImportInfo->Ext1);
//			OutputDebugString(szBuf);
//
//
//			// EX1
//			if(0 != strlen(pUdcImportInfo->Ext1))
//			{
//				rs_par.Rs->PutCollect(_variant_t("udc_sRes13"),
//					_variant_t(pUdcImportInfo->Ext1));
//			}
//
//			sprintf(szBuf,"pUdcImportInfo->Ext2:%s",pUdcImportInfo->Ext2);
//			OutputDebugString(szBuf);
//
//			// EX2
//			if(0 != strlen(pUdcImportInfo->Ext2))
//			{
//				rs_par.Rs->PutCollect(_variant_t("udc_sRes14"),
//					_variant_t(pUdcImportInfo->Ext2));
//			}
//
//			sprintf(szBuf,"pUdcImportInfo->Ext3:%s",pUdcImportInfo->Ext3);
//			OutputDebugString(szBuf);
//
//			// EX3
//			if(0 != strlen(pUdcImportInfo->Ext3))
//			{
//				rs_par.Rs->PutCollect(_variant_t("udc_sRes15"),
//					_variant_t(pUdcImportInfo->Ext3));
//			}
//
//			rs_par.Rs->Update();
//
//
//		}
//		else
//		{
//			OutputDebugString("CTH_Db_Access::AutoImport CP7\n");
//			status = STATUS_LOGIN_NO_US;
//		}
//
//		OutputDebugString("CTH_Db_Access::AutoImport CP8\n");
//
//	}
//	catch (_com_error e)
//	{
//		OutputDebugString("CTH_Db_Access::AutoImport CP9\n");
//		status = STATUS_EXCEPTION_ERROR;
//		DBG_MSG("AutoImport _com_error,errMessage = %s" ,
//			(const char *)(LPCTSTR)e.Description());
//	}
//
//	OutputDebugString("CTH_Db_Access::AutoImport CP10\n");
//
//	// 关闭
//	Close_Rs(rs_par.Rs);
//
//	if(status == STATUS_LOGIN_NO_US)
//	{
//
//		OutputDebugString("CTH_Db_Access::AutoImport CP11\n");
//
//
//		*pbIsNew = TRUE;
//
//		TASK_ADD Task_add;
//		memset(&Task_add,0,sizeof(Task_add));
//		Task_add.bIsQC = false;
//		/*Task_add.nQcType = 0;*/
//
//		// modify popo 2012.5.22
//		//status = AddTask(&Task_add);
//		status = AddTaskMust(&Task_add);
//
//		if(status != STATUS_SUCCESS)
//		{
//			OutputDebugString("CTH_Db_Access::AutoImport CP12\n");
//			status = STATUS_INVALID_PARAMETERS;
//			return status;
//		}
//
//		*pID = Task_add.nID;
//		memset(rs_par.sql,0,sizeof(rs_par.sql));
//		sprintf(rs_par.sql,"SELECT TOP 1 * FROM tMain LEFT OUTER JOIN tUdc ON tMain.nID = tUdc.nID \
//						   WHERE DateDiff('D', dtSendDate, NOW()) = 0 ORDER BY tMain.nID DESC");
//
//
//		status = Open_Rs(&rs_par);
//
//		if(status != STATUS_SUCCESS)
//		{
//			OutputDebugString("CTH_Db_Access::AutoImport CP13\n");
//			status = STATUS_INVALID_PARAMETERS;
//			return status;
//		}
//
//		OutputDebugString("CTH_Db_Access::AutoImport CP14\n");
//
//		try
//		{
//
//			OutputDebugString("CTH_Db_Access::AutoImport CP15\n");
//
//			if (!IsBOF(rs_par.Rs))
//			{
//				OutputDebugString("CTH_Db_Access::AutoImport CP16\n");
//
//				rs_par.Rs->PutCollect(_variant_t("sSN"),_variant_t(pUdcImportInfo->ID));
//
//				// GLU
//				if(0 != strlen(pUdcImportInfo->GLU))
//				{
//					rs_par.Rs->PutCollect(_variant_t("udc_sRes1"),
//						_variant_t(pUdcImportInfo->GLU));
//				}
//
//				// PRO
//				if(0 != strlen(pUdcImportInfo->PRO))
//				{
//					rs_par.Rs->PutCollect(_variant_t("udc_sRes2"),
//						_variant_t(pUdcImportInfo->PRO));
//				}
//
//				// BIL
//				if(0 != strlen(pUdcImportInfo->BIL))
//				{
//					rs_par.Rs->PutCollect(_variant_t("udc_sRes3"),
//						_variant_t(pUdcImportInfo->BIL));
//				}
//
//				// URO
//				if(0 != strlen(pUdcImportInfo->URO))
//				{
//					rs_par.Rs->PutCollect(_variant_t("udc_sRes4"),
//						_variant_t(pUdcImportInfo->URO));
//				}
//
//				// pH
//				if(0 != strlen(pUdcImportInfo->PH))
//				{
//					rs_par.Rs->PutCollect(_variant_t("udc_sRes5"),
//						_variant_t(pUdcImportInfo->PH));
//				}
//
//				// SG
//				if(0 != strlen(pUdcImportInfo->SG))
//				{
//					rs_par.Rs->PutCollect(_variant_t("udc_sRes6"),
//						_variant_t(pUdcImportInfo->SG));
//				}
//
//				// BLD
//				if(0 != strlen(pUdcImportInfo->BLD))
//				{
//					rs_par.Rs->PutCollect(_variant_t("udc_sRes7"),
//						_variant_t(pUdcImportInfo->BLD));
//				}
//
//				// KET
//				if(0 != strlen(pUdcImportInfo->KET))
//				{
//					rs_par.Rs->PutCollect(_variant_t("udc_sRes8"),
//						_variant_t(pUdcImportInfo->KET));
//				}
//
//				// NIT
//				if(0 != strlen(pUdcImportInfo->NIT))
//				{
//					rs_par.Rs->PutCollect(_variant_t("udc_sRes9"),
//						_variant_t(pUdcImportInfo->NIT));
//				}
//
//				// LEU
//				if(0 != strlen(pUdcImportInfo->LEU))
//				{
//					rs_par.Rs->PutCollect(_variant_t("udc_sRes10"),
//						_variant_t(pUdcImportInfo->LEU));
//				}
//
//				// VC
//				if(0 != strlen(pUdcImportInfo->VC))
//				{
//					rs_par.Rs->PutCollect(_variant_t("udc_sRes11"),
//						_variant_t(pUdcImportInfo->VC));
//				}
//
//				// MA
//				if(0 != strlen(pUdcImportInfo->MA))
//				{
//					rs_par.Rs->PutCollect(_variant_t("udc_sRes12"),
//						_variant_t(pUdcImportInfo->MA));
//				}
//
//				// EX1
//				if(0 != strlen(pUdcImportInfo->Ext1))
//				{
//					rs_par.Rs->PutCollect(_variant_t("udc_sRes13"),
//						_variant_t(pUdcImportInfo->Ext1));
//				}
//
//				// EX2
//				if(0 != strlen(pUdcImportInfo->Ext2))
//				{
//					rs_par.Rs->PutCollect(_variant_t("udc_sRes14"),
//						_variant_t(pUdcImportInfo->Ext2));
//				}
//
//				// EX3
//				if(0 != strlen(pUdcImportInfo->Ext3))
//				{
//					rs_par.Rs->PutCollect(_variant_t("udc_sRes15"),
//						_variant_t(pUdcImportInfo->Ext3));
//				}
//
//				rs_par.Rs->Update();
//
//
//				OutputDebugString("CTH_Db_Access::AutoImport CP17\n");
//			}
//
//			OutputDebugString("CTH_Db_Access::AutoImport CP18\n");
//		}
//		catch (_com_error e)
//		{
//			OutputDebugString("CTH_Db_Access::AutoImport CP19\n");
//			status = STATUS_EXCEPTION_ERROR;
//			DBG_MSG("AutoImport _com_error,errMessage = %s" ,
//				(const char *)(LPCTSTR)e.Description());
//		}
//
//		// 关闭
//		Close_Rs(rs_par.Rs);
//
//		OutputDebugString("CTH_Db_Access::AutoImport CP20\n");
//	}
//
//	OutputDebugString("CTH_Db_Access::AutoImport CP21\n");
//
//	return status;
//
//}

int TH_DB_Access::GetImportType( void )
{
	int nType = 0;

	std::string sPath = GetFilePath();

	char szLine[MAX_PATH*8] = {0};

	GetPrivateProfileString("udc",
		"bybarcode",
		"0",
		szLine,
		MAX_PATH*8,
		sPath.c_str());

	int nis_by_barcode = atoi(szLine);


	GetPrivateProfileString("udc",
		"byid",
		"0",
		szLine,
		MAX_PATH*8,
		sPath.c_str());

	int nis_by_id = atoi(szLine);



	GetPrivateProfileString("udc",
		"byauto",
		"0",
		szLine,
		MAX_PATH*8,
		sPath.c_str());

	int nis_by_auto = atoi(szLine);

	if(nis_by_barcode == 1)
	{
		nType = 1;
	}
	else if(nis_by_id == 1)
	{
		nType = 2;
	}
	else if(nis_by_auto == 1)
	{
		nType = 3;
	}
	else
	{
		nType = 0;
	}

	return nType;

}

std::string TH_DB_Access::GetFilePath( void )
{
	char path[MAX_PATH]={0};

	GetModuleFileName(NULL, path, MAX_PATH);
	PathRemoveFileSpec(path);	
	PathAppend(path,"conf.ini");
	return std::string (path);
}

KSTATUS TH_DB_Access::CheckTaskStates( BOOL IsTaday )
{

	KSTATUS status = STATUS_SUCCESS;
	OPEN_RS_PAR rs_par;
	memset(&rs_par,0,sizeof(rs_par));

	if(m_Connection == NULL)
	{
		status = STATUS_INVALID_PARAMETERS;
		return status;
	}

	if(IsTaday)
		strcpy(rs_par.sql, "SELECT * FROM (((tMain LEFT OUTER JOIN tUs ON tMain.nID = tUs.nID) \
						   LEFT OUTER JOIN tUdc ON tMain.nID = tUdc.nID) \
						   LEFT OUTER JOIN tPat ON tMain.nID = tPat.nID)\
						   WHERE DateDiff('D', dtDate, NOW()) = 0 ORDER BY tMain.nID");
	try
	{
		status = Open_Rs(&rs_par);

		if(status != STATUS_SUCCESS)
		{
			status = STATUS_INVALID_PARAMETERS;
			return status;
		}

		if (!IsBOF(rs_par.Rs))
		{
			rs_par.Rs->MoveFirst();
		}		
		for (; !IsEOF(rs_par.Rs); rs_par.Rs->MoveNext())
		{
			int nState = rs_par.Rs->GetCollect(_variant_t("nState"));
			if(nState == FINISH || nState == CHECKED)
			{

			}
			else
				rs_par.Rs->PutCollect(_variant_t("nState"),_variant_t(TESTFAIL));
		}
	}

	catch (_com_error e)
	{
		status = STATUS_EXCEPTION_ERROR;
		DBG_MSG("GetTodayTaskInfo _com_error,errMessage = %s" ,
			(const char *)(LPCTSTR)e.Description());
	}

	// 关闭
	Close_Rs(rs_par.Rs);


		return status;




}

KSTATUS TH_DB_Access::GetQcTaskInfo( QCTypeEnum QcType,QC_INFO* qc_task_info,unsigned int* pnSize )
{
	KSTATUS status = STATUS_SUCCESS;
	OPEN_RS_PAR rs_par;
	memset(&rs_par,0,sizeof(rs_par));

	if(m_Connection == NULL)
	{
		status = STATUS_INVALID_PARAMETERS;
		return status;
	}
	
	if(QcType == US_QC_TYPE)
		sprintf(rs_par.sql,"SELECT * FROM ((tMain LEFT OUTER JOIN tUs ON tMain.nID = tUs.nID) \
						  LEFT OUTER JOIN tUdc ON tMain.nID = tUdc.nID) \
						  WHERE nAttribute = 1 OR nAttribute = 2 ORDER BY tMain.nID");
	else
		sprintf(rs_par.sql,"SELECT * FROM ((tMain LEFT OUTER JOIN tUs ON tMain.nID = tUs.nID) \
						   LEFT OUTER JOIN tUdc ON tMain.nID = tUdc.nID) \
						   WHERE nAttribute = 3 OR nAttribute = 4 OR nAttribute = 5 ORDER BY tMain.nID");





	try
	{
		status = Open_Rs(&rs_par);

		if(status != STATUS_SUCCESS)
		{
			status = STATUS_INVALID_PARAMETERS;
			return status;
		}

		if (!IsBOF(rs_par.Rs))
		{
			rs_par.Rs->MoveFirst();
		}

		int nCounts = 0;
		for (int i = 0; !IsEOF(rs_par.Rs); rs_par.Rs->MoveNext(),i++)
		{
			++nCounts;
			

			if(qc_task_info == NULL)
			{

				*pnSize = nCounts;
				status = STATUS_LOGIN_MEMORY_NOTENOUGH;
			}
			else
			{
				
				_variant_t Value;
				if(QcType == US_QC_TYPE)
				{
					// us_id
					Value= rs_par.Rs->GetCollect(_variant_t("tUs.nID"));
					if(Value.vt != VT_NULL)
						qc_task_info[i].us1.nID  = Value;
					// us_bUs			
					Value= rs_par.Rs->GetCollect(_variant_t("bUs"));
					if(Value.vt != VT_NULL)
						qc_task_info[i].us1.bUs  = Value;
					//nVariableRatio
					Value= rs_par.Rs->GetCollect(_variant_t("nVariableRatio"));
					if(Value.vt != VT_NULL)
						qc_task_info[i].us1.nVariableRatio  = Value;


					for(unsigned int j =1 ; j <= MAX_US_LENS ; j++)
					{

						char szBuf[MAX_PATH]={0};
						sprintf(szBuf,"us_res%d",j);					
						/*pTaskInfo[i].us_info.us_node[j].us_res = (float)rs_par.Rs->GetCollect(_variant_t(szBuf));*/

						Value= (float)rs_par.Rs->GetCollect(_variant_t(szBuf));
						if(Value.vt != VT_NULL)
							qc_task_info[i].us1.us_node[j - 1].us_res  = Value;

					}					
				  }
				else if(QcType == UDC_QC_TYPE)
				{
					// nID				
					Value= (int)rs_par.Rs->GetCollect(_variant_t("tUdc.nID"));
					if(Value.vt != VT_NULL)
						qc_task_info[i].udc1.nID  = Value;
					//bUdc
					Value= (int)rs_par.Rs->GetCollect(_variant_t("bUdc"));
					if(Value.vt != VT_NULL)
						qc_task_info[i].udc1.bUdc  = Value;

					

					for(unsigned int x = 1 ; x <= MAX_UDC_LENS ; x ++)
					{
						char szBuf[MAX_PATH] = {0} ;

						sprintf(szBuf,"udc_sRes%d",x);
						Value = rs_par.Rs->GetCollect(_variant_t(szBuf));
						if(Value.vt != VT_NULL)
						{
							strcpy(qc_task_info[i].udc1.udc_node[x - 1].udc_res,_bstr_t(Value));
						}

						sprintf(szBuf,"sValue%d",x);
						Value = rs_par.Rs->GetCollect(_variant_t(szBuf));

						if(Value.vt != VT_NULL)
						{
							strcpy(qc_task_info[i].udc1.udc_node[x - 1].udc_value,_bstr_t(Value));
						}

						sprintf(szBuf,"bAbn%d",x);
						Value = (bool)rs_par.Rs->GetCollect(_variant_t(szBuf));
						if(Value.vt != VT_NULL)
							qc_task_info[i].udc1.udc_node[x - 1].udc_abn  = Value;

						sprintf(szBuf,"nColor%d",x);
						Value = (int)rs_par.Rs->GetCollect(_variant_t(szBuf));
						if(Value.vt != VT_NULL)
							qc_task_info[i].udc1.udc_node[x - 1].udc_col  = Value;

					}

			      }			

		       }
	    }

	}
	catch (_com_error e)
	{
		status = STATUS_EXCEPTION_ERROR;
		DBG_MSG("GetTodayTaskInfo _com_error,errMessage = %s" ,
			(const char *)(LPCTSTR)e.Description());
	}

	// 关闭
	Close_Rs(rs_par.Rs);


	return status;


}

KSTATUS TH_DB_Access::GetUsQcTaskInfo( UsQcEnum UsQcType,QC_INFO* qc_task_info,unsigned int* pnSize )
{
	KSTATUS status = STATUS_SUCCESS;
	OPEN_RS_PAR rs_par;
	memset(&rs_par,0,sizeof(rs_par));

	if(m_Connection == NULL)
	{
		status = STATUS_INVALID_PARAMETERS;
		return status;
	}

	strcpy(rs_par.sql,"SELECT * FROM ((tMain LEFT OUTER JOIN tUs ON tMain.nID = tUs.nID) \
						  LEFT OUTER JOIN tUdc ON tMain.nID = tUdc.nID) \
						  WHERE DateDiff('D', dtDate, NOW()) = 0  AND tMain.nQcType = 1 ORDER BY tMain.nID");
	try
	{
		status = Open_Rs(&rs_par);

		if(status != STATUS_SUCCESS)
		{
			status = STATUS_INVALID_PARAMETERS;
			return status;
		}

		if (!IsBOF(rs_par.Rs))
		{
			rs_par.Rs->MoveFirst();
		}

		int nCounts = 0;
		for (int i = 0; !IsEOF(rs_par.Rs); rs_par.Rs->MoveNext(),i++)
		{


			++nCounts;

			if(qc_task_info == NULL)
			{

				*pnSize = nCounts;
				status = STATUS_LOGIN_MEMORY_NOTENOUGH;
			}
			else
			{
			    // us_id
				qc_task_info[i].us1.nID = rs_par.Rs->GetCollect(_variant_t("tUs.nID"));
				// us_bUs
				qc_task_info[i].us1.bUs = rs_par.Rs->GetCollect(_variant_t("bUs"));
				//nVariableRatio
				qc_task_info[i].us1.nVariableRatio = rs_par.Rs->GetCollect(_variant_t("nVariableRatio"));


				for(unsigned int j =1 ; j <= MAX_US_LENS ; j++)
				{

					char szBuf[MAX_PATH]={0};
					sprintf(szBuf,"us_res%d",j);					
					qc_task_info[i].us1.us_node[j].us_res = (float)rs_par.Rs->GetCollect(_variant_t(szBuf));

				}	
				status = STATUS_SUCCESS;
			}
		}

	}
	catch (_com_error e)
	{
		status = STATUS_EXCEPTION_ERROR;
		DBG_MSG("GetTodayTaskInfo _com_error,errMessage = %s" ,
			(const char *)(LPCTSTR)e.Description());
	}

	// 关闭
	Close_Rs(rs_par.Rs);


	return status;

}

KSTATUS TH_DB_Access::GetUdcQcTaskInfo( UsQcEnum UsQcType,QC_INFO* qc_task_info,unsigned int* pnSize )
{
	KSTATUS status = STATUS_SUCCESS;
	OPEN_RS_PAR rs_par;
	memset(&rs_par,0,sizeof(rs_par));

	if(m_Connection == NULL)
	{
		status = STATUS_INVALID_PARAMETERS;
		return status;
	}

	strcpy(rs_par.sql,"SELECT * FROM ((tMain LEFT OUTER JOIN tUs ON tMain.nID = tUs.nID) \
						  LEFT OUTER JOIN tUdc ON tMain.nID = tUdc.nID) \
						  WHERE DateDiff('D', dtDate, NOW()) = 0  AND tMain.nQcType = 2 ORDER BY tMain.nID");



	try
	{
		status = Open_Rs(&rs_par);

		if(status != STATUS_SUCCESS)
		{
			status = STATUS_INVALID_PARAMETERS;
			return status;
		}

		if (!IsBOF(rs_par.Rs))
		{
			rs_par.Rs->MoveFirst();
		}

		int nCounts = 0;
		for (int i = 0; !IsEOF(rs_par.Rs); rs_par.Rs->MoveNext(),i++)
		{
			++nCounts;

			if(qc_task_info == NULL)
			{

				*pnSize = nCounts;
				status = STATUS_LOGIN_MEMORY_NOTENOUGH;
			}
			else
			{
				// nID
				qc_task_info[i].udc1.nID = (int)rs_par.Rs->GetCollect(_variant_t("tUdc.nID"));
				//bUdc
				qc_task_info[i].udc1.bUdc = (int)rs_par.Rs->GetCollect(_variant_t("bUdc"));

				_variant_t Value;
				for(unsigned int x = 1 ; x <= MAX_UDC_LENS ; x ++)
				{
					char szBuf[MAX_PATH] = {0} ;

					sprintf(szBuf,"udc_sRes%d",x);
					Value = rs_par.Rs->GetCollect(_variant_t(szBuf));
					if(Value.vt != VT_NULL)
					{
						strcpy(qc_task_info[i].udc1.udc_node[x - 1].udc_res,_bstr_t(Value));
					}

					sprintf(szBuf,"sValue%d",x);
					Value = rs_par.Rs->GetCollect(_variant_t(szBuf));

					if(Value.vt != VT_NULL)
					{
						strcpy(qc_task_info[i].udc1.udc_node[x - 1].udc_value,_bstr_t(Value));
					}

					sprintf(szBuf,"bAbn%d",x);
					Value = (bool)rs_par.Rs->GetCollect(_variant_t(szBuf));
					if(Value.vt != VT_NULL)
						qc_task_info[i].udc1.udc_node[x - 1].udc_abn  = Value;

					sprintf(szBuf,"nColor%d",x);
					Value = (int)rs_par.Rs->GetCollect(_variant_t(szBuf));
					if(Value.vt != VT_NULL)
						qc_task_info[i].udc1.udc_node[x - 1].udc_col  = Value;
				 }	
				status = STATUS_SUCCESS;
			}
		}

	}
	catch (_com_error e)
	{
		status = STATUS_EXCEPTION_ERROR;
		DBG_MSG("GetTodayTaskInfo _com_error,errMessage = %s" ,
			(const char *)(LPCTSTR)e.Description());
	}

	// 关闭
	Close_Rs(rs_par.Rs);

	return status;

}

BOOL TH_DB_Access::GetConfigIniFilePath()
{
	GetModuleFileName(NULL, m_nConfig_IniFile, MAX_PATH);
	PathRemoveFileSpec(m_nConfig_IniFile);
	PathAppend(m_nConfig_IniFile,CONFIG_INI_FILENAME);

	return TRUE;

}

void TH_DB_Access::SetInitializestatus( BOOL bNeedInitialize )
{
	bInitialize = bNeedInitialize;

}

KSTATUS TH_DB_Access::GetDeptInfo( Dept_Pos_Info* pUserInfo,unsigned int* pnSize )
{
	KSTATUS  status = STATUS_SUCCESS;
	OPEN_RS_PAR rs_par;

	if(m_Connection == NULL)
	{
		status = STATUS_INVALID_PARAMETERS;
		return status;
	}


	// 返回所有用户列表
	
	sprintf(rs_par.sql,"SELECT distinct(sDept) FROM tUser");
	try
	{
		status = Open_Rs(&rs_par);

		if(status != STATUS_SUCCESS)
		{
			status = STATUS_INVALID_PARAMETERS;
			return status;
		}

		if (!IsBOF(rs_par.Rs))
		{
			rs_par.Rs->MoveFirst();
		}

		int nCounts = 0;
		for (int i = 0; !IsEOF(rs_par.Rs); rs_par.Rs->MoveNext(),i++)
		{
			++nCounts;

			if(pUserInfo == NULL)
			{
				*pnSize = nCounts;
				status = STATUS_LOGIN_MEMORY_NOTENOUGH;
			}
			else
			{
				//DBG_MSG("LLL");
				_variant_t Value = rs_par.Rs->GetCollect(_variant_t("sDept"));
				if(Value.vt != VT_NULL)
					strcpy(pUserInfo[i].name,_bstr_t(Value));
				status = STATUS_SUCCESS;
			}

		}
	}
		catch (_com_error e)
		{
			status = STATUS_EXCEPTION_ERROR;
			DBG_MSG("GetUserInfo _com_error,errMessage = %s" ,
				(const char *)(LPCTSTR)e.Description());
		}

		// 关闭
		Close_Rs(rs_par.Rs);

		return status;


}

KSTATUS TH_DB_Access::GetPosInfo( Dept_Pos_Info* pUserInfo,unsigned int* pnSize )
{
	KSTATUS  status = STATUS_SUCCESS;
	OPEN_RS_PAR rs_par;

	if(m_Connection == NULL)
	{
		status = STATUS_INVALID_PARAMETERS;
		return status;
	}


	// 返回所有用户列表
	
	sprintf(rs_par.sql,"SELECT distinct(sPosition) FROM tUser");
	try
	{
		status = Open_Rs(&rs_par);

		if(status != STATUS_SUCCESS)
		{
			status = STATUS_INVALID_PARAMETERS;
			return status;
		}

		if (!IsBOF(rs_par.Rs))
		{
			rs_par.Rs->MoveFirst();
		}

		int nCounts = 0;
		for (int i = 0; !IsEOF(rs_par.Rs); rs_par.Rs->MoveNext(),i++)
		{
			++nCounts;

			if(pUserInfo == NULL)
			{
				*pnSize = nCounts;
				status = STATUS_LOGIN_MEMORY_NOTENOUGH;
			}
			else
			{
				_variant_t Value = rs_par.Rs->GetCollect(_variant_t("sPosition"));
				if(Value.vt != VT_NULL)
					strcpy(pUserInfo[i].name,_bstr_t(Value));

				status = STATUS_SUCCESS;
			}

		}
	}
	catch (_com_error e)
	{
		status = STATUS_EXCEPTION_ERROR;
		DBG_MSG("GetUserInfo _com_error,errMessage = %s" ,
			(const char *)(LPCTSTR)e.Description());
	}

	// 关闭
	Close_Rs(rs_par.Rs);

	return status;

}

KSTATUS TH_DB_Access::GetAllTaskInfo( PULONG pAllTaskID,unsigned int* pnSize )
{
	KSTATUS status = STATUS_SUCCESS;
	OPEN_RS_PAR rs_par;

	if(m_Connection == NULL)
	{
		status = STATUS_INVALID_PARAMETERS;
		return status;
	}

	
	strcpy(rs_par.sql, "SELECT * FROM (((tMain LEFT OUTER JOIN tUs ON tMain.nID = tUs.nID) \
					   LEFT OUTER JOIN tUdc ON tMain.nID = tUdc.nID) \
					   LEFT OUTER JOIN tPat ON tMain.nID = tPat.nID)\
					   WHERE 1 = 1 ORDER BY tMain.nID");
	


	try
	{
		status = Open_Rs(&rs_par);

		if(status != STATUS_SUCCESS)
		{
			status = STATUS_INVALID_PARAMETERS;
			return status;
		}


		if (!IsBOF(rs_par.Rs))
		{
			rs_par.Rs->MoveFirst();
		}

		int nCounts = 0;		
		int i = 0;

		for (; !IsEOF(rs_par.Rs); rs_par.Rs->MoveNext())
		{				
			++nCounts;					 

			if(pAllTaskID == NULL)
			{
				*pnSize = nCounts;
				status = STATUS_LOGIN_MEMORY_NOTENOUGH;
			}
			else
			{

				// ID	
				_variant_t Value = rs_par.Rs->GetCollect(_variant_t("tMain.nID"));
				if(Value.vt != VT_NULL)
					pAllTaskID[i] = Value;	

				i++;

				status = STATUS_SUCCESS;
			}
		}
	}
	catch (_com_error e)
	{
		status = STATUS_EXCEPTION_ERROR;
		DBG_MSG("GetAllTaskInfo _com_error,errMessage = %s" ,
			(const char *)(LPCTSTR)e.Description());
	}

	// 关闭
	Close_Rs(rs_par.Rs);


	return status;

}