#ifndef _MVIS2045_PIPELINE_MANAGER_H_
#define _MVIS2045_PIPELINE_MANAGER_H_

#include <map>
#include <vector>
#include "../../../inc/netio.h"
#include "pipeline_protocol.h"

using namespace std;


typedef struct _ST_STREAM_SESSION{
	vector<char>		buff;
	void				*cs; // 临界区
	unsigned int        socket;
	struct sockaddr_in	addr;

	void				*userdata;

	_ST_STREAM_SESSION()
	{
		cs = 0;
		socket = 0;
		userdata = 0;
		memset(&addr, 0, sizeof(struct sockaddr_in));
	}
	_ST_STREAM_SESSION& operator=(_ST_STREAM_SESSION &rhs)
	{
		this->buff = rhs.buff;
		this->cs = rhs.cs;
		this->socket = rhs.socket;
		this->userdata = rhs.userdata;
		memmove_s(&this->addr, sizeof(struct sockaddr_in), &rhs.addr, sizeof(struct sockaddr_in));
		return *this;
	}
}ST_STREAM_SESSION, *PST_STREAM_SESSION;


class IPipelineListener
{
public:
	virtual ~IPipelineListener(){};

	virtual void OnConnect(unsigned int ss, unsigned int cs, void *userdata) = 0;
	virtual void OnDisconnect(unsigned int s, void *userdata) = 0;
	virtual void OnError(unsigned int s, int nOperationType, void *userdata) = 0;
	virtual int  OnRequest(unsigned int s, MSG_PACKAGE_INFO & info, void *userdata) = 0;
	virtual void OnSendComplete(unsigned int s, char *data, unsigned short len, void *userdata) = 0;
};


class CPipelineManager
{
	friend void GetRemoteIP(unsigned int s, char *buf, int len, unsigned short &port, CPipelineManager &clsMvis2045PipelineManager);

public:
	~CPipelineManager();
private:
	CPipelineManager();


private:
	void  *m_cs;
	map<unsigned int, ST_STREAM_SESSION> m_sessionMap;
	void *m_hNetIOLib;
	IPipelineListener *m_listener;

public:
	static void OnConnect(unsigned int ss, unsigned int cs, void *userdata);
	static void OnDisconnect(unsigned int s, void *userdata);
	static int OnRecv(unsigned int s, char *pDataBuf, long nDataLen, void *userdata);
	static void OnSend(unsigned int s, char *pDataBuf, long nDataLen, void *userdata);
	static void OnError(unsigned int s, int nOperationType, void *userdata);

	void SetListener(IPipelineListener *listener);
	void *GetSessionUserdata(unsigned int s);
	void SetSessionUserdata(unsigned int s, void *s_userdata);

	int Wait(unsigned int sock, long timeoutMs);
	int SendData(unsigned int s, char *pDataBuf, long nDataLen);
	int PostSendData(unsigned int s, char *pDataBuf, long nDataLen);

	static CPipelineManager *GetInstance();
	int CreateTcpServer(PNETIOST st);
	int CreateTcpClient(PNETIOST st, unsigned int *cs);
	int CreateAsyncTcpClient(PNETIOST st, unsigned int *cs);
	int ReleaseTcpClient(unsigned int s);
	int ReleaseAsyncTcpClient(unsigned int s);

private:
	void ParseHeader(vector<char>& buff, char *pDataBuf, unsigned short nDataLen, unsigned short &offset, int nHdrLength);
	int ParseMsg(ST_STREAM_SESSION &session, char *pDataBuf, unsigned short nDataLen, void *userdata);

};

#endif // _MVIS2045_PIPELINE_MANAGER_H_