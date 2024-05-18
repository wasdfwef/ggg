#include "stdafx.h"
#include "pipeline_manager.h"
#include <WinSock2.h>
#include "pipeline_protocol.h"
#include <WS2tcpip.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")

#ifdef _DEBUG
#pragma comment(lib, "../../../lib/libnetiod.lib")
#else
#pragma comment(lib, "../../../lib/libnetio.lib")
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



void GetRemoteIP(unsigned int s, char *buf, int len, unsigned short &port, struct sockaddr_in &addr)
{
	struct sockaddr_in& remoteAddr = addr;
	int remoteAddr_len = sizeof(sockaddr_in);
	getpeername(s, (struct sockaddr*)&remoteAddr, &remoteAddr_len);
	inet_ntop(AF_INET, &remoteAddr.sin_addr, buf, len);
	port = ntohs(remoteAddr.sin_port);
}

void GetRemoteIP(struct sockaddr_in& sa, char *buf, int len, unsigned short &port)
{
	inet_ntop(AF_INET, &sa.sin_addr, buf, len);
	port = ntohs(sa.sin_port);
}

void CPipelineManager::OnConnect(unsigned int ss, unsigned int cs, void *userdata)
{
	char strAddr[32] = { 0 };
	unsigned short port = 0;
	ST_STREAM_SESSION session;
	session.socket = cs;
	session.cs = new CRITICAL_SECTION;
	InitializeCriticalSection((CRITICAL_SECTION*)session.cs);
	GetRemoteIP(cs, strAddr, 32, port, session.addr);
	printf("OnConnect %s:%d\n", strAddr, port);
	{
		CPipelineManager *_this = (CPipelineManager*)userdata;
		CLock lock(*(CRITICAL_SECTION*)_this->m_cs);
		_this->m_sessionMap[cs] = session;
	}
	CPipelineManager *_this = (CPipelineManager*)userdata;
	if (!_this)
	{
		return;
	}
	if (_this->m_listener)
	{
		_this->m_listener->OnConnect(ss, cs, userdata);
	}
}

void CPipelineManager::OnDisconnect(unsigned int s, void *userdata)
{
	CPipelineManager *_this = (CPipelineManager*)userdata;
	if (!_this)
	{
		return;
	}
	
	if (_this->m_listener)
	{
		_this->m_listener->OnDisconnect(s, userdata);
	}

	char strAddr[32] = { 0 };
	unsigned short port = 0;
	ST_STREAM_SESSION session;
	{
		CLock lock(*(CRITICAL_SECTION*)_this->m_cs);
		map<unsigned int, ST_STREAM_SESSION>::iterator itr = _this->m_sessionMap.find(s);
		if (itr != _this->m_sessionMap.end() && itr->second.cs != NULL)
		{
			session = itr->second;
			_this->m_sessionMap.erase(itr);
		}
	}
	if (session.cs != NULL)
	{
		CLock lock(*(CRITICAL_SECTION*)session.cs);
		GetRemoteIP(session.addr, strAddr, 32, port);
		printf("OnDisconnect %s:%d\n", strAddr, port);
		vector<char>().swap(session.buff); // 释放内存占用
		DeleteCriticalSection((CRITICAL_SECTION*)session.cs);
		session.cs = NULL;
	}
}

int CPipelineManager::OnRecv(unsigned int s, char *pDataBuf, long nDataLen, void *userdata)
{
	//printf("app::OnRecv [%d, [%s], %d]\n", s, pDataBuf, nDataLen);
	CPipelineManager *_this = (CPipelineManager*)userdata;
	if (!_this)
	{
		return 0;
	}
	ST_STREAM_SESSION session;
	{
		CLock lock(*(CRITICAL_SECTION*)_this->m_cs);
		map<unsigned int, ST_STREAM_SESSION>::iterator itr = _this->m_sessionMap.find(s);
		if (itr == _this->m_sessionMap.end() || itr->second.cs == NULL)
		{
			return 0;
		}
		session = itr->second;
	}
	if(session.cs != NULL)
	{
		CLock lock(*(CRITICAL_SECTION*)session.cs);
		return _this->ParseMsg(session, pDataBuf, nDataLen, userdata);
	}

	return 0;
}

void CPipelineManager::OnSend(unsigned int s, char *pDataBuf, long nDataLen, void *userdata)
{
	printf("app::OnSend [%d, [%s], %d]\n", s, pDataBuf, nDataLen);
	CPipelineManager *_this = (CPipelineManager*)userdata;
	if (!_this)
	{
		return;
	}
	if (_this->m_listener)
	{
		_this->m_listener->OnSendComplete(s, pDataBuf, nDataLen, userdata);
	}
}

void CPipelineManager::OnError(unsigned int s, int nOperationType, void *userdata)
{
	printf("app::OnError [%d]\n", s);
	CPipelineManager *_this = (CPipelineManager*)userdata;
	if (!_this)
	{
		return;
	}
	if (_this->m_listener)
	{
		_this->m_listener->OnError(s, nOperationType, userdata);
	}
}


void CPipelineManager::SetListener(IPipelineListener *listener)
{
	m_listener = listener;
}

int CPipelineManager::SendData(unsigned int s, char *pDataBuf, long nDataLen)
{
	return NetIOSend(m_hNetIOLib, s, pDataBuf, nDataLen);
}

int CPipelineManager::PostSendData(unsigned int s, char *pDataBuf, long nDataLen)
{
	return NetIOPostSend(m_hNetIOLib, s, pDataBuf, nDataLen)?0:1;
}

CPipelineManager::CPipelineManager()
{
	m_hNetIOLib = NULL;
	m_cs = new CRITICAL_SECTION;
	InitializeCriticalSection((CRITICAL_SECTION*)m_cs);
	m_listener = NULL;

	InitNetIOLib(&m_hNetIOLib);
}


CPipelineManager::~CPipelineManager()
{
	DeleteCriticalSection((CRITICAL_SECTION*)m_cs);
	delete (CRITICAL_SECTION*)m_cs;
	map<unsigned int, ST_STREAM_SESSION>::iterator itr;
	for (itr = m_sessionMap.begin(); itr != m_sessionMap.end(); itr++)
	{
		vector<char>().swap(itr->second.buff); // 释放内存占用
		DeleteCriticalSection((CRITICAL_SECTION*)itr->second.cs);
		itr->second.cs = NULL;
	}
	m_sessionMap.clear();

	ReleaseNetIOLib(m_hNetIOLib);
}

CPipelineManager * CPipelineManager::GetInstance()
{
	static CPipelineManager clsMvis2045PipelineManager;
	return &clsMvis2045PipelineManager;
}

int CPipelineManager::CreateTcpServer(PNETIOST st)
{
	return CreateNetIOServer(m_hNetIOLib, st);
}

int CPipelineManager::CreateTcpClient(PNETIOST st, unsigned int *cs)
{
	return CreateNetIOClient(m_hNetIOLib, cs, st);
}

int CPipelineManager::CreateAsyncTcpClient(PNETIOST st, unsigned int *cs)
{
	return CreateAsyncNetIOClient(m_hNetIOLib, cs, st);
}

int CPipelineManager::ReleaseTcpClient(unsigned int s)
{
	return ReleaseNetIOClient(m_hNetIOLib, s);
}

int CPipelineManager::ReleaseAsyncTcpClient(unsigned int s)
{
	return ReleaseAsyncNetIOClient(m_hNetIOLib, s);
}

void CPipelineManager::ParseHeader(vector<char>& buff, char *pDataBuf, unsigned short nDataLen, unsigned short &offset, int nHdrLength)
{
	int nCacheSize = buff.size();
	int nHdrNeedByts = nHdrLength - nCacheSize;
	// 判断是否是消息头
	while(nCacheSize < nHdrLength && nHdrNeedByts <= (nDataLen-offset))
	{
		MSG_HEADER hdr = {0};
		for (int i = 0; i < nCacheSize; i++)
		{
			hdr.header[i] = buff[i];
		}
		for (int j = 0; j < nHdrNeedByts; j++)
		{
			hdr.header[nCacheSize+j] = pDataBuf[j+offset];
		}
		offset += nHdrNeedByts;
		if (IsMsgHeader(&hdr))
		{
			buff.resize(nHdrLength);
			memcpy(&buff[0], &hdr, nHdrLength);
		}
		else
		{
			buff.resize(nHdrLength-1);
			memcpy(&buff[0], ((char*)&hdr+1), nHdrLength-1);
		}
		nCacheSize = buff.size();
		nHdrNeedByts = nHdrLength - nCacheSize;
	}
}

int CPipelineManager::ParseMsg(ST_STREAM_SESSION &session, char *pDataBuf, unsigned short nDataLen, void *userdata)
{
	int nRet = 0;
	vector<char>& cache = session.buff;
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
		if (nCacheSize < nHdrLength+nPkgSizeLength)
		{
			if (nHdrLength-nCacheSize > nDataLen-offset)
			{
				cache.resize(nCacheSize + (nDataLen - offset));
				memcpy(&cache[nCacheSize], pDataBuf+offset, nDataLen-offset);
				offset += (nDataLen - offset);
			}
			else
			{
				cache.resize(nHdrLength+nPkgSizeLength);
				memcpy(&cache[nCacheSize], pDataBuf+offset, nHdrLength+nPkgSizeLength-nCacheSize);
				offset += (nHdrLength+nPkgSizeLength - nCacheSize);
				unsigned short nPackageSize = ntoh_or_hton(*(unsigned short*)&cache[3]);
				unsigned short nNeedBytes = nPackageSize - cache.size();
				if (nNeedBytes <= (nDataLen - offset))
				{
					cache.resize(nPackageSize);
					memcpy(&cache[nHdrLength+nPkgSizeLength], pDataBuf+offset, nNeedBytes);
					offset += nNeedBytes;
					//////////////////////////
					//printf("package size: %d\n", *(unsigned short*)&cache[3]);
					//printf("dev id: [%C,%d]\n", cache[5], cache[6]);
					MSG_PACKAGE_INFO pkg = {0};
					DataToPackage(&pkg, &cache[0],nPackageSize);
					if (this->m_listener)
					{
						nRet = this->m_listener->OnRequest(session.socket, pkg, userdata);
					}

					cache.clear();
				}
				else
				{
					cache.resize(nHdrLength+nPkgSizeLength+ (nDataLen - offset));
					memcpy(&cache[nHdrLength+nPkgSizeLength], pDataBuf + offset, nDataLen-offset);
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
				memcpy(&cache[nHdrLength+nPkgSizeLength], pDataBuf + offset, nNeedBytes);
				offset += nNeedBytes;
				///////////////////////
				//printf("package size: %d\n", *(unsigned short*)&cache[3]);
				//printf("dev id: [%C,%d]\n", cache[5], cache[6]);
				MSG_PACKAGE_INFO pkg = {0};
				DataToPackage(&pkg, &cache[0],nPackageSize);
				if (this->m_listener)
				{
					nRet = this->m_listener->OnRequest(session.socket, pkg, userdata);
				}

				cache.clear();
			}
			else
			{
				cache.resize(nHdrLength+nPkgSizeLength + (nDataLen - offset));
				memcpy(&cache[nHdrLength+nPkgSizeLength], pDataBuf + offset, nDataLen - offset);
				offset += (nDataLen - offset);
			}
		}
	}

	return nRet;
}

void * CPipelineManager::GetSessionUserdata(unsigned int s)
{
	map<unsigned int, ST_STREAM_SESSION>::iterator itr = m_sessionMap.find(s);
	if (itr != m_sessionMap.end())
	{
		return itr->second.userdata;
	}

	return NULL;
}

void CPipelineManager::SetSessionUserdata(unsigned int s, void *s_userdata)
{
	map<unsigned int, ST_STREAM_SESSION>::iterator itr = m_sessionMap.find(s);
	if (itr != m_sessionMap.end())
	{
		itr->second.userdata = s_userdata;
	}
}

int CPipelineManager::Wait(unsigned int sock, long timeoutMs)
{
	return NetIOWait(m_hNetIOLib, sock, timeoutMs);
}
