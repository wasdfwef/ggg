#ifndef _MVIS2045_PIPELINE_PROTOCOL_H_
#define _MVIS2045_PIPELINE_PROTOCOL_H_

/**
* MVIS2045 流水线通信协议
*/
/**
数据格式:
02 0D 0A + 数据包长度(2字节) + 消息类型(1字节) + 设备ID(2字节) + 数据类容 + 0D 0A 03 0D 0A

数据包长度: 这个数据包长度，包括头尾和内容
设备ID: 两个字节，第一个字节表示设备类型，第二个字节表示设备编号
*/

// 设备类型定义符
typedef enum
{
	DEV_TYPE_UNKNOWN		= 0,
	DEV_TYPE_BLOOD			= 'X', // 血液
	DEV_TYPE_URINE			= 'N', // 尿液
	DEV_TYPE_SHIT			= 'D', // 大便
	DEV_TYPE_LEUCORRHEA		= 'B', // 白带
}ENUM_DEVTYPE;

// 消息类型定义
typedef enum
{
	MSG_TYPE_DEV_UNKNOWN  = 0,
	MSG_TYPE_DEV_BARCODE  = 'Q', // 发送条码，查询病人信息及测试项目的请求
	MSG_TYPE_DEV_ABNORMAL = 'E', // 上位机软件向中控软件发送设备异常通知
	MSG_TYPE_DEV_STATUS   = 'S', // 上位机软件向中控发送检验状态变化通知
	MSG_TYPE_DEV_RESULT   = 'R', // 上位机软件向中控软件发送检验结果
	MSG_TYPE_DEV_HAOCAI   = 'C', // 上位机软件向中控软件发送耗材数量
}ENUM_DEV_MSGTYPE;

typedef enum
{
	MSG_TYPE_PLATFORM_UNKNOWN  = 0,
	MSG_TYPE_PLATFORM_BARCODE  = 'Q', // 中控软件向上位机软件发送病人信息及测试项目
	MSG_TYPE_PLATFORM_ABNORMAL = 'E', // 中控软件向上位机软件发送异常处理完成通知
	MSG_TYPE_PLATFORM_RETEST   = 'D', // 中控软件向上位机软件发送重测请求
}ENUM_PLATFORM_MSGTYPE;

typedef struct {
	char type;
	char SN;
}DEV_ID, *PDEV_ID;

typedef struct {
	char header[3]; // 02 0D 0A
}MSG_HEADER, *PMSG_HEADER;

typedef struct {
	char tail[5]; // 0D 0A 03 0D 0A
}MSG_TAIL, *PMSG_TAIL;

typedef struct {
	MSG_HEADER		hdr;
	unsigned short	nPackageSize;
	char			msgtype;
	DEV_ID			nDevID;
	char *			pData;
	unsigned short  nDataLen;
	MSG_TAIL		tail;
}MSG_PACKAGE_INFO, *PMSG_PACKAGE_INFO;

void SetMsgHeader(PMSG_HEADER h);
bool IsMsgHeader(PMSG_HEADER h);
void SetMsgTail(PMSG_TAIL t);
void SetMsgDevID(PDEV_ID di, ENUM_DEVTYPE t, char sn);
void SetMsgType(PMSG_PACKAGE_INFO pInfo, ENUM_DEV_MSGTYPE t);
void SetMsgData(PMSG_PACKAGE_INFO pInfo, char *data, unsigned short len);

unsigned short GetPackageSize(unsigned short nDataLen);
int PackageToData(PMSG_PACKAGE_INFO pInfo, char* buff, unsigned short buffLen);
int DataToPackage(PMSG_PACKAGE_INFO pInfo, char* buff, unsigned short buffLen);

bool IsPackageHeader(PMSG_HEADER h);
bool IsPackageTail(PMSG_TAIL t);
int  ParseData(char *buff, long nBufflen, char *cacheBuff, long nCacheBuffLen, void *fnCompletePackageCallback, void *fnNonCompletePackageCallback);

bool CreateStatusRequest(char *data, unsigned short len, PMSG_PACKAGE_INFO pkg, UINT deviceNo);
bool CreateBarcodeRequest(char *barcode, unsigned short len, PMSG_PACKAGE_INFO pkg, UINT deviceNo);
bool CreateTestResultRequest(char *result, unsigned short len, PMSG_PACKAGE_INFO pkg, UINT deviceNo);
bool CreateHaocaiRequest(char *data, unsigned short len, PMSG_PACKAGE_INFO pkg, UINT deviceNo);
bool CreateHaocaiRequest(char *data, unsigned short len, PMSG_PACKAGE_INFO pkg, UINT deviceNo);
bool CreateWarnRequest(char *data, unsigned short len, PMSG_PACKAGE_INFO pkg, UINT deviceNo);

// 整型数据(short,int,long,long long)的字节序转换
template<typename IntType> IntType ntoh_or_hton(IntType nValue);

#endif // _MVIS2045_PIPELINE_PROTOCOL_H_
