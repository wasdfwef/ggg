#include "StdAfx.h"
#include "CLiushuixianOpt.h"
#include "..\..\..\inc\CmdHelper.h"
#include "DbgMsg.h"
#include "UW.h"

#include <sstream>
#include <string>
#include <vector>
#include <memory>
using namespace std;

// 添加MessageBoxTimeout支持
extern "C"
{
    int WINAPI MessageBoxTimeoutA(IN HWND hWnd, IN LPCSTR lpText, IN LPCSTR lpCaption, IN UINT uType, IN WORD wLanguageId, IN DWORD dwMilliseconds);
    int WINAPI MessageBoxTimeoutW(IN HWND hWnd, IN LPCWSTR lpText, IN LPCWSTR lpCaption, IN UINT uType, IN WORD wLanguageId, IN DWORD dwMilliseconds);
};
#ifdef UNICODE
#define MessageBoxTimeout MessageBoxTimeoutW
#else
#define MessageBoxTimeout MessageBoxTimeoutA
#endif

class CLock
{
public:
	CLock(CRITICAL_SECTION &cs)
		:m_cs(cs)
	{
		EnterCriticalSection(&m_cs);
	}
	~CLock()
	{
		LeaveCriticalSection(&m_cs);
	}

private:
	CRITICAL_SECTION &m_cs;
};




CCLiushuixianOpt::CCLiushuixianOpt(CPipelineManager &mgr)
: m_Mgr(mgr)
{
	InitializeCriticalSection(&m_cs);
}

CCLiushuixianOpt::~CCLiushuixianOpt(void)
{
	DeleteCriticalSection(&m_cs);
}

int CCLiushuixianOpt::CreateTcpServer(const char* host, unsigned short port)
{
	NETIOST st = {0};
	sprintf_s(st.host, sizeof(st.host), "%s", host?host:"");
	st.port = port;
	st.userdata = &m_Mgr;
	st.OnConnect = m_Mgr.OnConnect;
	st.OnDisconnect = m_Mgr.OnDisconnect;
	st.OnError = m_Mgr.OnError;
	st.OnRecv = m_Mgr.OnRecv;
	st.OnSend = m_Mgr.OnSend;
	m_Mgr.SetListener(this);

	return m_Mgr.CreateTcpServer(&st);
}

int CCLiushuixianOpt::CreateAsyncTcpClient(const char* host, unsigned short port, IConnectListener *listener, unsigned long taskID, char *data, unsigned short len, void *userdata)
{
	NETIOST st;
	sprintf_s(st.host, sizeof(st.host), "%s", host?host:"");
	st.port = port;
	st.userdata = &m_Mgr;
	st.bAsyncConnect = true;
	st.OnConnect = m_Mgr.OnConnect;
	st.OnDisconnect = m_Mgr.OnDisconnect;
	st.OnError = m_Mgr.OnError;
	st.OnRecv = m_Mgr.OnRecv;
	st.OnSend = m_Mgr.OnSend;
	m_Mgr.SetListener(this);
	unsigned int cs;
	int ret = 0;

	{
		CLock lock(m_cs);
		ret = m_Mgr.CreateAsyncTcpClient(&st, &cs);
		if (0 == ret)
		{
			ST_CONNECT_CB scc = {0};
			scc.listener = listener;
			scc.taskID = taskID;
			scc.socket = cs;
			scc.data = data;
			scc.len = len;
			scc.userdata = userdata;
			//map<unsigned long, ST_CONNECT_CB>::iterator itr;
			{
				//CLock lock(m_cs);
				if (m_clientSockets.find(taskID) != m_clientSockets.end())
				{
					m_clientSockets[taskID].push_back(scc);
				}
				else
				{
					//m_clientSockets.insert(make_pair(taskID, scc));
					vector<ST_CONNECT_CB> vec;
					vec.push_back(scc);
					m_clientSockets[taskID] = vec;
				}
			}
			DBG_MSG("---CreateTcpClient--socket: %u----\n", cs);
		}
	}

	return ret;
}

int CCLiushuixianOpt::PostData(unsigned int s, char *data, unsigned short len)
{
	char msg[256];
	sprintf_s(msg, 256, "发送数据【%u】【%#x】【%u】\n", s, data, len);
	OutputDebugStringA(msg);
	return m_Mgr.PostSendData(s, data, len)>0?0:1;
}

bool CCLiushuixianOpt::CreateTcpClient(const char* host, unsigned short port, unsigned int &sock)
{
	NETIOST st = { 0 };
	sprintf_s(st.host, sizeof(st.host), "%s", host ? host : "");
	st.port = port;
	return (0 == m_Mgr.CreateTcpClient(&st, &sock));
}

int CCLiushuixianOpt::Wait(unsigned int sock, long timeoutMs)
{
	return m_Mgr.Wait(sock, timeoutMs);
}

int CCLiushuixianOpt::SendData(unsigned int sock, char *data, unsigned short len)
{
	return m_Mgr.SendData(sock, data, len);
}

/////////////////////////////////回调接口/////////////////////////////////////////////////////////////////

void CCLiushuixianOpt::OnConnect(unsigned int ss, unsigned int cs, void *userdata)
{
	char msg[256];
	sprintf_s(msg, 256, "回调接口 OnConnect【ss:%u】【cs:%u】\n", ss, cs);
	OutputDebugStringA(msg);
	if (ss == 0 && cs != 0)
	{
		// 建立新客户端连接（本机作为客户端)
		map<unsigned long, vector<ST_CONNECT_CB>>::iterator itr;
		ST_CONNECT_CB scc = {0};
		{
			CLock lock(m_cs);
			for (itr = m_clientSockets.begin(); itr != m_clientSockets.end(); itr++)
			{
				vector<ST_CONNECT_CB> &vec = itr->second;
				vector<ST_CONNECT_CB>::iterator vecItr;
				for (vecItr = vec.begin(); vecItr != vec.end(); vecItr++)
				{
					if (vecItr->socket == cs)
					{
						scc = *vecItr;
						goto FINISH;
					}
				}
			}
		}
FINISH:
		if (scc.listener)
		{
			scc.listener->OnSuccess(cs, scc.taskID, scc.data, scc.len, scc.userdata);
		}
		else
		{
			sprintf_s(msg, 256, "回调接口 OnConnect【%u】lister is NULL [%d]\n", cs, m_clientSockets.size());
			OutputDebugStringA(msg);
			m_Mgr.ReleaseAsyncTcpClient(cs);
		}
		
	}
	if (ss != 0 && cs != 0)
	{
		// 建立新客户端连接（本机作为服务端)
	}
}

void CCLiushuixianOpt::OnDisconnect(unsigned int s, void *userdata)
{
	DBG_MSG("----OnDisconnect : socket %d\n", s);
	ST_CONNECT_CB scc = {0};
	map<unsigned long, vector<ST_CONNECT_CB>>::iterator itr;
	{
		CLock lock(m_cs);
		for (itr = m_clientSockets.begin(); itr != m_clientSockets.end(); itr++)
		{
			vector<ST_CONNECT_CB> &vec = itr->second;
			vector<ST_CONNECT_CB>::iterator vecItr;
			for (vecItr = vec.begin(); vecItr != vec.end(); vecItr++)
			{
				if (vecItr->socket == s)
				{
					scc = *vecItr;
					vec.erase(vecItr);
					if (vec.size()==0)
					{
						m_clientSockets.erase(itr);
					}
					goto FINISH;
				}
			}
		}
	}
FINISH:
	if (scc.listener)
	{
		scc.listener->OnFail(scc.taskID, scc.data, scc.len, 0, scc.userdata);
	}
}

void CCLiushuixianOpt::OnError(unsigned int s, int nOperationType, void *userdata)
{
	DBG_MSG("----OnError : socket %d\n", s);
	map<unsigned long, vector<ST_CONNECT_CB>>::iterator itr;
	ST_CONNECT_CB scc = {0};
	{
		CLock lock(m_cs);
		for (itr = m_clientSockets.begin(); itr != m_clientSockets.end(); itr++)
		{
			vector<ST_CONNECT_CB> &vec = itr->second;
			vector<ST_CONNECT_CB>::iterator vecItr;
			for (vecItr = vec.begin(); vecItr != vec.end(); vecItr++)
			{
				if (vecItr->socket == s)
				{
					scc = *vecItr;
					vec.erase(vecItr);
					if (vec.size()==0)
					{
						m_clientSockets.erase(itr);
					}
					goto FINISH;
				}
			}
		}
	}
FINISH:
	if (scc.listener)
	{
		scc.listener->OnFail(scc.taskID, scc.data, scc.len, nOperationType, scc.userdata);
	}
}

int CCLiushuixianOpt::OnRequest(unsigned int s, MSG_PACKAGE_INFO & info, void *userdata)
{
	int nRet = 0;

	if (info.msgtype == MSG_TYPE_DEV_BARCODE)
	{
		// 收到条码信息病人信息
		ParsePatientInfo(s, info.pData, info.nDataLen);
		m_Mgr.ReleaseAsyncTcpClient(s);
		nRet = 1;
	}
	else if (info.msgtype == MSG_TYPE_DEV_STATUS)
	{
		// 查询设备在线
		RespondStatus(s, info, userdata);
	}
	else if (info.msgtype == MSG_TYPE_DEV_ABNORMAL)
	{
		// 解除警报
		//LC_Warning lcwarning;
		//lcwarning.WarningType = info.pData[1];
		//CancelWarning(lcwarning);
		//theApp.CancelMsgDlg();
	}
	else if (info.msgtype == MSG_TYPE_PLATFORM_RETEST)
	{
		// 重测请求
		//1.解析重测字符串[id,type]
		/*char *pGbkBUf = NULL;
		UTF8ToGBK(info.pData, &pGbkBUf);
		if (pGbkBUf)
		{
			std::string str = pGbkBUf;
			delete[] pGbkBUf;

			map<int, int> mapInfo;
			vector<string> strMid, tmp;
			SplitString(str, "\r\n", strMid);
			vector<string>::iterator itr;
			for (itr = strMid.begin(); itr != strMid.end(); itr++)
			{
				tmp.clear();
				SplitString(*itr, "        ", tmp);
				if (tmp.size()>=2)
				{
					std::string &key = trim(tmp[0]);
					std::string &val = trim(tmp[1]);
					if (!key.empty() && !val.empty())
					{
						mapInfo.insert(make_pair(atoi(key.c_str()), atoi(val.c_str())));
					}					
				}
			}  
			CAutoLock lock(theApp.m_retestCS);
			for (std::map<int, int>::iterator itr = mapInfo.begin(); itr != mapInfo.end(); itr++)
			{
				Retest_Info retest = {0};
				retest.ReTestID = itr->first;
				retest.ReTestType = itr->second;
				theApp.m_ReTestNums.push_back(retest);
			}
		}*/
	}

	return nRet;
}

void CCLiushuixianOpt::OnSendComplete(unsigned int s, char *data, unsigned short len, void *userdata)
{

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CCLiushuixianOpt::RespondStatus(unsigned int s, MSG_PACKAGE_INFO & info, void *userdata)
{
	//获取系统当前时间,并转成字符串  
	COleDateTime date;  
	date = COleDateTime::GetCurrentTime();  

	if (info.msgtype == MSG_TYPE_DEV_STATUS)
	{
		char *strData = NULL, printBuf = NULL;
		int nPrintGbkSize = 0;
		if (info.nDataLen > 0)
		{
			strData = new char[info.nDataLen+1];
			strData[info.nDataLen] = 0;
			memcpy(strData, info.pData, info.nDataLen);
			char *printBuf = NULL;
			nPrintGbkSize = UTF8ToGBK(strData, &printBuf);
		}

		stringstream ss;
		ss<<date.GetYear()<<"-"<<date.GetMonth()<<"-"<<date.GetDay()<<" "<<date.GetHour()<<":"<<date.GetMinute()<<":"<<date.GetSecond()
			<<"收到请求："<<info.msgtype<<"\r\n"
			<<"设备类型："<<info.nDevID.type<<"  设备编号："<<int(info.nDevID.SN)<<"\r\n";
		if (printBuf)
		{
			ss<<"数据类容："<<printBuf<<"\r\n";
		}
		OutputDebugStringA(ss.str().c_str());

		char *pBuf = info.pData;
		CreateStatusRequest(pBuf, info.nDataLen, &info, theApp.m_nDeviceNo);
		char *pkg = new char[info.nPackageSize];
		if (pkg)
		{
			PackageToData(&info, pkg, info.nPackageSize);
			PostData(s, pkg, info.nPackageSize);

			SAFE_RELEASE_ARRAY(pkg);
		}
		SAFE_RELEASE_ARRAY(pBuf);
	}
}

int CCLiushuixianOpt::ParsePatientInfo(unsigned int s, char *pBuf, unsigned short nLen)
{
	char *info = NULL;
	char *data = new char[nLen+1];
	data[nLen] = 0;
	memcpy(data, pBuf, nLen);
	int nBufLen = UTF8ToGBK(data, &info);
	DBG_MSG("%s", info);

	std::shared_ptr<char> sp1(data, [&](char* p){SAFE_RELEASE_ARRAY(data); });
	std::shared_ptr<char> sp2(info, [&](char* p){SAFE_RELEASE_ARRAY(info); });

	map<unsigned long, vector<ST_CONNECT_CB>>::iterator iterator;
	ST_CONNECT_CB scc = {0};
	{
		CLock lock(m_cs);
		for (iterator = m_clientSockets.begin(); iterator != m_clientSockets.end(); iterator++)
		{
			vector<ST_CONNECT_CB> &vec = iterator->second;
			vector<ST_CONNECT_CB>::iterator vecItr;
			for (vecItr = vec.begin(); vecItr != vec.end(); vecItr++)
			{
				if (vecItr->socket == s)
				{
					scc = *vecItr;
					goto FINISH;
				}
			}
		}
		if (iterator == m_clientSockets.end())
		{
			return 1;
		}
	}

FINISH:
	// 解析病人信息
	map<string, string> mapInfo;
	vector<string> strMid, tmp;
	SplitString(info, "\r\n", strMid);
	vector<string>::iterator itr;
	for (itr = strMid.begin(); itr != strMid.end(); itr++)
	{
		tmp.clear();
		SplitString(*itr, "：", tmp);
		if (tmp.size()>=2)
		{
			mapInfo.insert(make_pair(tmp.at(0), tmp.at(1)));
		}
	}  

    string strErrCode = mapInfo["错误码"];
    if (strErrCode.empty())
    {
        MessageBoxTimeoutA(AfxGetApp()->GetMainWnd()->GetSafeHwnd(), "获取标本信息失败", "提示", MB_OK | MB_ICONINFORMATION, 0, 5000);
        return 1;
    }
    if (atoi(strErrCode.c_str()) != 0)
    {
        char str[256];
        sprintf_s(str, "获取标本信息失败\n错误码：%s\n错误信息：%s", strErrCode.c_str(), mapInfo["错误信息"].c_str());
        MessageBoxTimeoutA(AfxGetApp()->GetMainWnd()->GetSafeHwnd(), str, "提示", MB_OK | MB_ICONINFORMATION, 0, 5000);
        return 2;
    }

	TASK_INFO  task_info = { 0 };
	KSTATUS state = Access_GetTaskInfo(scc.taskID, USER_TYPE_INFO, &task_info);
	if( state != STATUS_SUCCESS )
		return 3;
	//更新病人信息
	map<string, string>::const_iterator iter;
	iter = mapInfo.find("姓名");
	if (iter != mapInfo.end())
	{
		strcpy_s(task_info.pat_info.sName, 32, iter->second.c_str());
	}
	iter = mapInfo.find("性别");
	if (iter != mapInfo.end())
	{
		if (iter->second == string("男"))
		{
			task_info.pat_info.nSex = 1;
		}
		else if (iter->second == string("女"))
		{
			task_info.pat_info.nSex = 2;
		}
		else
		{
			task_info.pat_info.nSex = 0;
		}
	}
	iter = mapInfo.find("年龄");
	if (iter != mapInfo.end())
	{
		task_info.pat_info.nAge = atoi(iter->second.c_str());
	}
	iter = mapInfo.find("顺序号");
	if (iter != mapInfo.end() && !iter->second.empty())
	{
        string strSeqNo = iter->second;
        for (int j = strSeqNo.length() - 1; j >= 0; j--)
        {
            if (!isdigit(strSeqNo.at(j)))
            {
                strSeqNo.erase(strSeqNo.begin(), strSeqNo.begin() + j+1);
                break;
            }
        }
        task_info.main_info.nSN = atoi(strSeqNo.c_str());
        task_info.pat_info.nNo = atoi(strSeqNo.c_str());
	}
	iter = mapInfo.find("测试项目");
	if (iter != mapInfo.end() && !iter->second.empty())
	{
		int nType = atoi(iter->second.c_str());
		//task_info.main_info.nTestType = 0;
	}
    iter = mapInfo.find("科室");
    if (iter != mapInfo.end())
    {
        strcpy_s(task_info.main_info.sDepartment, 32, iter->second.c_str());
    }
    iter = mapInfo.find("住院号");
    if (iter != mapInfo.end())
    {
        strcpy_s(task_info.pat_info.sHospital, 32, iter->second.c_str());
    }
    iter = mapInfo.find("床号");
    if (iter != mapInfo.end())
    {
        strcpy_s(task_info.pat_info.sSickBed, 32, iter->second.c_str());
    }
    iter = mapInfo.find("颜色");
    if (iter != mapInfo.end())
    {
        strcpy_s(task_info.main_info.sColor, 32, iter->second.c_str());
    }
    iter = mapInfo.find("性状");
    if (iter != mapInfo.end())
    {
        strcpy_s(task_info.main_info.sTransparency, 32, iter->second.c_str());
    }
    iter = mapInfo.find("临床诊断");
    if (iter != mapInfo.end())
    {
        strcpy_s(task_info.main_info.sDiagnosis, 255, iter->second.c_str());
    }
    iter = mapInfo.find("送检医生");
    if (iter != mapInfo.end())
    {
        strcpy_s(task_info.main_info.sDoctor, 32, iter->second.c_str());
    }
    /*iter = mapInfo.find("送检日期");
    if (iter != mapInfo.end())
    {
        strcpy_s(task_info.main_info.sAuditor, 32, iter->second.c_str());
    }*/

	if (Access_UpdateTaskInfo(USER_TYPE_INFO, task_info.main_info.nID, &task_info) != STATUS_SUCCESS)
	if( state != STATUS_SUCCESS )
	{
		DBG_MSG("更新病人信息失败: [%d][%d][%s]!!!!!!!!", task_info.main_info.nID, task_info.main_info.nSN, task_info.main_info.sCode);
		return 3;
	}
	DBG_MSG("更新病人信息成功: [%d][%d][%s]!!!!!!!!", task_info.main_info.nID, task_info.main_info.nSN, task_info.main_info.sCode); 

    TASK_INFO *tinfo = new TASK_INFO;
    if (tinfo) *tinfo = task_info;
    AfxGetApp()->GetMainWnd()->PostMessage(WM_UPDATE_TASKUI, (WPARAM)task_info.main_info.nID, (LPARAM)tinfo);


	return 0;
}

void CCLiushuixianOpt::ParseHeader(vector<char>& buff, char *pDataBuf, unsigned short nDataLen, unsigned short &offset, int nHdrLength)
{
	int nCacheSize = buff.size();
	int nHdrNeedByts = nHdrLength - nCacheSize;
	// 判断是否是消息头
	while (nCacheSize < nHdrLength && nHdrNeedByts <= (nDataLen - offset))
	{
		MSG_HEADER hdr = { 0 };
		for (int i = 0; i < nCacheSize; i++)
		{
			hdr.header[i] = buff[i];
		}
		for (int j = 0; j < nHdrNeedByts; j++)
		{
			hdr.header[nCacheSize + j] = pDataBuf[j + offset];
		}
		offset += nHdrNeedByts;
		if (IsMsgHeader(&hdr))
		{
			buff.resize(nHdrLength);
			memcpy(&buff[0], &hdr, nHdrLength);
		}
		else
		{
			buff.resize(nHdrLength - 1);
			memcpy(&buff[0], ((char*)&hdr + 1), nHdrLength - 1);
		}
		nCacheSize = buff.size();
		nHdrNeedByts = nHdrLength - nCacheSize;
	}
}

int CCLiushuixianOpt::ParseMsg(char *pDataBuf, unsigned short nDataLen, std::map<string, string> &baseInfo)
{
	int ret = 0;
	vector<char> cache;
	int nCacheSize = 0;
	int nHdrLength = sizeof(MSG_HEADER);	// 包头字段长度
	int nPkgSizeLength = 2;					// 包大小字段长度
	unsigned short offset = 0;
	while (offset < nDataLen)
	{
		ParseHeader(cache, pDataBuf, nDataLen, offset, nHdrLength);
		if (nDataLen <= offset)
		{
			break;
		}
		nCacheSize = cache.size();
		if (nCacheSize < nHdrLength + nPkgSizeLength)
		{
			if (nHdrLength - nCacheSize > nDataLen - offset)
			{
				cache.resize(nCacheSize + (nDataLen - offset));
				memcpy(&cache[nCacheSize], pDataBuf + offset, nDataLen - offset);
				offset += (nDataLen - offset);
			}
			else
			{
				cache.resize(nHdrLength + nPkgSizeLength);
				memcpy(&cache[nCacheSize], pDataBuf + offset, nHdrLength + nPkgSizeLength - nCacheSize);
				offset += (nHdrLength + nPkgSizeLength - nCacheSize);
				unsigned short nPackageSize = ntoh_or_hton(*(unsigned short*)&cache[3]);
				unsigned short nNeedBytes = nPackageSize - cache.size();
				if (nNeedBytes <= (nDataLen - offset))
				{
					cache.resize(nPackageSize);
					memcpy(&cache[nHdrLength + nPkgSizeLength], pDataBuf + offset, nNeedBytes);
					offset += nNeedBytes;
					//////////////////////////
					//printf("package size: %d\n", *(unsigned short*)&cache[3]);
					//printf("dev id: [%C,%d]\n", cache[5], cache[6]);
					MSG_PACKAGE_INFO pkg = { 0 };
					DataToPackage(&pkg, &cache[0], nPackageSize);
					if (pkg.msgtype == MSG_TYPE_DEV_BARCODE)
					{
						char *info = NULL;
						char *data = new char[pkg.nDataLen + 1];
						data[pkg.nDataLen] = 0;
						memcpy(data, pkg.pData, pkg.nDataLen);
						int nBufLen = UTF8ToGBK(data, &info);
						DBG_MSG("%s", info);

						std::shared_ptr<char> sp1(data, [&](char* p){SAFE_RELEASE_ARRAY(data); });
						std::shared_ptr<char> sp2(info, [&](char* p){SAFE_RELEASE_ARRAY(info); });

						// 解析病人信息
						vector<string> strMid, tmp;
						SplitString(info, "\r\n", strMid);
						vector<string>::iterator itr;
						for (itr = strMid.begin(); itr != strMid.end(); itr++)
						{
							tmp.clear();
							SplitString(*itr, "：", tmp);
							if (tmp.size() >= 2)
							{
								baseInfo.insert(make_pair(tmp.at(0), tmp.at(1)));
								if (tmp[0] == "测试项目") {
									ret = atoi(tmp[1].c_str());
									if (ret == 1) ret = 2;
									else if (ret == 2) ret = 1;
								}
							}
						}
						break;
					}

					cache.clear();
				}
				else
				{
					cache.resize(nHdrLength + nPkgSizeLength + (nDataLen - offset));
					memcpy(&cache[nHdrLength + nPkgSizeLength], pDataBuf + offset, nDataLen - offset);
					offset += (nDataLen - offset);
				}
			}
		}
		else
		{
			unsigned short nPackageSize = ntoh_or_hton(*(unsigned short*)&cache[nHdrLength]);
			unsigned short nNeedBytes = nPackageSize - nCacheSize;
			if (nNeedBytes <= (nDataLen - offset))
			{
				cache.resize(nPackageSize);
				memcpy(&cache[nHdrLength + nPkgSizeLength], pDataBuf + offset, nNeedBytes);
				offset += nNeedBytes;
				///////////////////////
				//printf("package size: %d\n", *(unsigned short*)&cache[3]);
				//printf("dev id: [%C,%d]\n", cache[5], cache[6]);
				MSG_PACKAGE_INFO pkg = { 0 };
				DataToPackage(&pkg, &cache[0], nPackageSize);
				if (pkg.msgtype == MSG_TYPE_DEV_BARCODE)
				{
					char *info = NULL;
					char *data = new char[pkg.nDataLen + 1];
					data[pkg.nDataLen] = 0;
					memcpy(data, pkg.pData, pkg.nDataLen);
					int nBufLen = UTF8ToGBK(data, &info);
					DBG_MSG("%s", info);

					std::shared_ptr<char> sp1(data, [&](char* p){SAFE_RELEASE_ARRAY(data); });
					std::shared_ptr<char> sp2(info, [&](char* p){SAFE_RELEASE_ARRAY(info); });

					// 解析病人信息
					vector<string> strMid, tmp;
					SplitString(info, "\r\n", strMid);
					vector<string>::iterator itr;
					for (itr = strMid.begin(); itr != strMid.end(); itr++)
					{
						tmp.clear();
						SplitString(*itr, "：", tmp);
						if (tmp.size() >= 2)
						{
							baseInfo.insert(make_pair(tmp.at(0), tmp.at(1)));
							if (tmp[0] == "测试项目") {
								ret = atoi(tmp[1].c_str());
								if (ret == 1) ret = 2;
								else if (ret == 2) ret = 1;
							}
						}
					}
					break;
				}

				cache.clear();
			}
			else
			{
				cache.resize(nHdrLength + nPkgSizeLength + (nDataLen - offset));
				memcpy(&cache[nHdrLength + nPkgSizeLength], pDataBuf + offset, nDataLen - offset);
				offset += (nDataLen - offset);
			}
		}
	}

	return ret;
}
