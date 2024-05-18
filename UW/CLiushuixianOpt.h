#ifndef __MVIS2045_LIUSHUIXIAN_H__
#define __MVIS2045_LIUSHUIXIAN_H__

#include "pipeline_manager.h"
#include <map>
#include <vector>
using namespace std;

class IConnectListener
{
public:
	~IConnectListener(){};

	virtual void OnSuccess(unsigned int s, unsigned long taskID, char *data, unsigned short len, void *userdata) = 0;
	virtual void OnFail(unsigned long taskID, char *data, unsigned short len, int nOperationType, void *userdata) = 0;
};

typedef struct {
	IConnectListener *listener;
	unsigned long taskID;
	unsigned int socket;
	char *data;
	unsigned short len;
	void *userdata;
}ST_CONNECT_CB;


class CCLiushuixianOpt : IPipelineListener
{
public:
	CCLiushuixianOpt(CPipelineManager &mgr);
	~CCLiushuixianOpt(void);

	int CreateTcpServer(const char* host, unsigned short port);
	// async
	int CreateAsyncTcpClient(const char* host, unsigned short port, IConnectListener *listener, unsigned long taskID, char *data, unsigned short len, void *userdata);
	int PostData(unsigned int s, char *data, unsigned short len);
	// sync
	bool CreateTcpClient(const char* host, unsigned short port, unsigned int &sock);
	int Wait(unsigned int sock, long timeoutMs);
	int SendData(unsigned int sock, char *data, unsigned short len);


	// 实现回调接口函数
	virtual void OnConnect(unsigned int ss, unsigned int cs, void *userdata);
	virtual void OnDisconnect(unsigned int s, void *userdata);
	virtual void OnError(unsigned int s, int nOperationType, void *userdata);
	virtual int  OnRequest(unsigned int s, MSG_PACKAGE_INFO & info, void *userdata);
	virtual void OnSendComplete(unsigned int s, char *data, unsigned short len, void *userdata);

private:
	void RespondStatus(unsigned int s, MSG_PACKAGE_INFO & info, void *userdata); // 回应状态查询
	int ParsePatientInfo(unsigned int s, char *pBuf, unsigned short nLen); // 解析病人信息

	void ParseHeader(vector<char>& buff, char *pDataBuf, unsigned short nDataLen, unsigned short &offset, int nHdrLength);
public:
	int ParseMsg(char *pDataBuf, unsigned short nDataLen, std::map<string, string> &baseInfo);

private:
	CPipelineManager &m_Mgr;
	CRITICAL_SECTION m_cs;
	map<unsigned long, vector<ST_CONNECT_CB>> m_clientSockets;
};


#endif // __MVIS2045_LIUSHUIXIAN_H__