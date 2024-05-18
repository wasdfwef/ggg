#include "stdafx.h"
#include "pipeline_protocol.h"
#include <string.h>

// 判断是否是大端字节序
bool IsBigEndianOrder()
{
	int iVal = 1;
	char *pChar = (char*)(&iVal);
	if(*pChar==1) return false; //(0x01000000) Windows 小端序
	else return true; //(0x00000001)
}

bool CreateRequest(ENUM_DEV_MSGTYPE msgtype, char *data, unsigned short len, PMSG_PACKAGE_INFO pkg, char deviceNo)
{
	if (pkg == NULL)
	{
		return false;
	}
	SetMsgHeader(&pkg->hdr);
	SetMsgTail(&pkg->tail);
	SetMsgType(pkg, msgtype);
	SetMsgDevID(&pkg->nDevID, DEV_TYPE_URINE, deviceNo);
	SetMsgData(pkg, data, len);

	return true;
}

bool CreateStatusRequest(char *data, unsigned short len, PMSG_PACKAGE_INFO pkg, UINT deviceNo)
{
	return CreateRequest(MSG_TYPE_DEV_STATUS, data, len, pkg, (char)(deviceNo&0xFFFF));
}

bool CreateBarcodeRequest(char *barcode, unsigned short len, PMSG_PACKAGE_INFO pkg, UINT deviceNo)
{
	return CreateRequest(MSG_TYPE_DEV_BARCODE, barcode, len, pkg, (char)(deviceNo&0xFFFF));
}	

bool CreateTestResultRequest(char *result, unsigned short len, PMSG_PACKAGE_INFO pkg, UINT deviceNo)
{
	return CreateRequest(MSG_TYPE_DEV_RESULT, result, len, pkg, (char)(deviceNo&0xFFFF));
}

bool CreateHaocaiRequest(char *data, unsigned short len, PMSG_PACKAGE_INFO pkg, UINT deviceNo)
{
	return CreateRequest(MSG_TYPE_DEV_HAOCAI, data, len, pkg, (char)(deviceNo&0xFFFF));
}

bool CreateWarnRequest(char *data, unsigned short len, PMSG_PACKAGE_INFO pkg, UINT deviceNo)
{
	return CreateRequest(MSG_TYPE_DEV_ABNORMAL, data, len, pkg, (char)(deviceNo&0xFFFF));
}


// 整型数据(short,int,long,long long)的字节序转换
template<typename IntType> IntType ntoh_or_hton(IntType nValue)
{
	IntType iResult = nValue;
	int iIntTypeSize = sizeof(IntType);

	unsigned char* pInput = (unsigned char*)&nValue;
	unsigned char* pResult = (unsigned char*)&iResult;
	if (!IsBigEndianOrder())
	{
		int num = iIntTypeSize>>1;
		for(int i = 0; i < num; i++)
		{
			pResult[i] = pInput[iIntTypeSize-1-i];
			pResult[iIntTypeSize-1-i] = pInput[i];
		}
	}
	return iResult;
}




void SetMsgHeader(PMSG_HEADER h)
{
	if (h)
	{
		h->header[0] = 0x02;
		h->header[1] = 0x0D;
		h->header[2] = 0x0A;
	}
}

void SetMsgTail(PMSG_TAIL t)
{
	if (t)
	{
		t->tail[0] = 0x0D;
		t->tail[1] = 0x0A;
		t->tail[2] = 0x03;
		t->tail[3] = 0x0D;
		t->tail[4] = 0x0A;
	}
}

void SetMsgDevID(PDEV_ID di, ENUM_DEVTYPE t, char sn)
{
	if (di)
	{
		di->type = t;
		di->SN = sn;
	}
}

void SetMsgType(PMSG_PACKAGE_INFO pInfo, ENUM_DEV_MSGTYPE t)
{
	if (pInfo)
	{
		pInfo->msgtype = t;
	}
}

void SetMsgData(PMSG_PACKAGE_INFO pInfo, char *data, unsigned short len)
{
	pInfo->pData = data;
	pInfo->nDataLen = len;
	pInfo->nPackageSize = GetPackageSize(len);
}

unsigned short GetPackageSize(unsigned short nDataLen)
{
	return sizeof(MSG_HEADER) + 5 + sizeof(MSG_TAIL) + nDataLen;
}

int PackageToData(PMSG_PACKAGE_INFO pInfo, char* buff, unsigned short buffLen)
{
	if (!pInfo || !buff || buffLen == 0)
	{
		return 1;
	}
	if (pInfo->nPackageSize > buffLen)
	{
		return 2;
	}
	memcpy_s(buff, sizeof(MSG_HEADER), pInfo, sizeof(MSG_HEADER));
	*((unsigned short*)(buff+sizeof(MSG_HEADER))) = ntoh_or_hton(pInfo->nPackageSize);
	memcpy_s(buff + (sizeof(MSG_HEADER) + 2), 3, &pInfo->msgtype, 3);
	memcpy_s(buff + (sizeof(MSG_HEADER) + 5), pInfo->nDataLen, pInfo->pData, pInfo->nDataLen);
	memcpy_s(buff + (sizeof(MSG_HEADER) + 5)+pInfo->nDataLen, sizeof(MSG_TAIL), &pInfo->tail, sizeof(MSG_TAIL));

	return 0;
}

bool IsPackageHeader(PMSG_HEADER h)
{
	MSG_HEADER hdr;
	SetMsgHeader(&hdr);
	if (0 == memcmp(&hdr, h, sizeof(MSG_HEADER)))
	{
		return true;
	}
	return false;
}

bool IsPackageTail(PMSG_TAIL t)
{
	MSG_TAIL tail;
	SetMsgTail(&tail);
	if (0 == memcmp(&tail, t, sizeof(MSG_TAIL)))
	{
		return true;
	}
	return false;
}

int DataToPackage(PMSG_PACKAGE_INFO pInfo, char* buff, unsigned short buffLen)
{
	if (!pInfo || !buff || buffLen == 0)
	{
		return 1;
	}
	unsigned short offset = 0; 
	pInfo->hdr.header[0] = buff[offset++];
	pInfo->hdr.header[1] = buff[offset++];
	pInfo->hdr.header[2] = buff[offset++];
	pInfo->nPackageSize = buffLen;
	offset += 2;
	pInfo->msgtype = buff[offset++];
	pInfo->nDevID.type = (ENUM_DEVTYPE)buff[offset++];
	pInfo->nDevID.SN = buff[offset++];
	if (buffLen == 13)
	{
		pInfo->pData = NULL;
	}
	else
	{
		pInfo->pData = &buff[offset];
	}
	offset += (buffLen - 13);
	pInfo->nDataLen = buffLen - 13;
	pInfo->tail.tail[0] = buff[offset++];
	pInfo->tail.tail[1] = buff[offset++];
	pInfo->tail.tail[2] = buff[offset++];
	pInfo->tail.tail[3] = buff[offset++];
	pInfo->tail.tail[4] = buff[offset++];
	return 0;
}

bool IsMsgHeader(PMSG_HEADER h)
{
	if (!h)
	{
		return false;
	}
	if (h->header[0] == 0x02 && h->header[1] == 0x0d && h->header[2] == 0x0a)
	{
		return true;
	}
	return false;
}
