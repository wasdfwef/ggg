#include "StdAfx.h"
#include "TranslateCmd.h"
#include "PRP.H"
#include "interal.h"
#include "THSPDef.h"
#include "ProtMapping.h"
#include "WaitQueue.h"
#include "Global.h"

long g_LCTimeoutFlag = LC_ALIVE_FLAG;


KSTATUS PostCmdToLC(PROGRAM_OBJECT PrgObject, PCOMM_CMD_DATA CmdData, DWORD CmdLength)
{
	KSTATUS Result = STATUS_SUCCESS;
	PDEV_EXTENSION DevExtension;
	WRITE_BUFFER_PROC WriteBuffer = {0};
	static int TotalCounter = 0;

	Result = FMMC_GetPrgObjectExtension(PrgObject, (void **)&DevExtension);
	CheckResultCode("PostCmd", "FMMC_GetPrgObjectExtension", Result);

	if( DevExtension->hSport == NULL || DevExtension->SPortPrgObject == NULL )
	{
		DbgPrint( "DevExtension->hSport == NULL || DevExtension->SPortPrgObject == NULL " );
		Result = STATUS_UNINIT_STATUS;
		goto end;
	}

	WriteBuffer.nWriteSize    = CmdLength;
	WriteBuffer.pWriteBuffer  = (PCHAR)CmdData;
	WriteBuffer.s_handle      = DevExtension->hSport;

	Result = FMMC_EasySynchronousBuildPRP(DevExtension->SPortPrgObject, NULL, PRP_AC_WRITE, WRITE_SERIAL_PORT, &WriteBuffer, sizeof(WRITE_BUFFER_PROC), NULL, 0, FALSE, FALSE);
	CheckResultCode("PostCmdToLC", "FMMC_EasySynchronousBuildPRP", Result);

// 	if (PROTO_LC_GET_STATUS == CmdData->nCmd)
// 	{
// 		++TotalCounter;
// 		DbgPrint("yyyyyyyyyyyyyyy begin get status, total counter: %d", TotalCounter);
// 	}

end:
	return Result;
}


KSTATUS SendCmdToLC(PROGRAM_OBJECT PrgObject, PCOMM_CMD_DATA CmdData, int CmdDataLen, PCHAR OutputBuffer, int OutputLen, DWORD TimeoutInterval)
{
	KSTATUS Result = STATUS_SUCCESS;
	DWORD WaitResult = 0;
	HANDLE hWaitEvent = NULL;

	hWaitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (NULL == hWaitEvent)
	{
		Result = GetLastError();
		DbgPrint( "CreateEvent failed with error = %d", Result );
		goto end;
	}

	if (CmdData->nCmd == UC_DBG_WRITE_SENSOR_CFG_REQ)
	{
		SENSOR_CFG_CONTEXT &cfg = *(SENSOR_CFG_CONTEXT*)CmdData->data;
		DbgPrint("%s UC_DBG_WRITE_SENSOR_CFG_REQ: A:%d, B:%d, C:%d, D:%d\n", __FUNCTION__, cfg.ChannelAEnabled, cfg.ChannelBEnabled, cfg.ChannelCEnabled, cfg.ChannelDEnabled);
	}

	if( !g_WaitQueue.Add(CmdData->nCmd, hWaitEvent, OutputBuffer, OutputLen) )
	{
		Result = STATUS_INSERT_QUE_FAIL;
		goto end;
	}

	Result = PostCmdToLC(PrgObject, CmdData, CmdDataLen);			 
	if ( STATUS_SUCCESS != Result)
	{
		goto end;
	}
	/*else
	{
		SetEvent(hWaitEvent);
	}*/
	// 上位机发送指令后，在此等待下位机应答
	WaitResult = WaitForSingleObject(hWaitEvent, TimeoutInterval);
	if (WAIT_TIMEOUT == WaitResult)
	{
		DbgPrint("SendCmdToLC time out.[cmd=%#x]\n", CmdData->nCmd);
		Result = STATUS_TIME_OUT;
		goto end;
	}


// 	if (LC_TIMEOUT_FLAG == InterlockedExchangeAdd(&g_LCTimeoutFlag, 0))
// 	{
// 		DbgPrint("LC_TIMEOUT_FLAG == InterlockedExchangeAdd\n");
// 		Result = STATUS_TIME_OUT;
// 	}

end:
	if (g_WaitQueue.IsItemExist(CmdData->nCmd))
	{
		g_WaitQueue.Remove(CmdData->nCmd);
	}

	if (NULL != hWaitEvent)
	{
		CloseHandle(hWaitEvent);
	}
	
	return Result;
}


KSTATUS SendCmdToLCEx(PROGRAM_OBJECT PrgObject, PCOMM_CMD_DATA CmdData, int CmdDataLen /*= COMM_CMD_LENGTH*/, PCHAR OutputBuffer /*= NULL*/, int OutputLen /*= 0*/, DWORD TimeoutInterval /*= INFINITE*/)
{
	if ((int)(*CmdData->data) == 30)
	{
		if (g_WaitQueue.SetWaitSuccess(CmdData->nCmd))
			return STATUS_SUCCESS;
	}

	KSTATUS Result = STATUS_SUCCESS;
	DWORD WaitResult = 0;
	HANDLE hWaitEvent = NULL;

	hWaitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (NULL == hWaitEvent)
	{
		Result = GetLastError();
		DbgPrint("CreateEvent failed with error = %d", Result);
		goto end;
	}

	if (!g_WaitQueue.Add(CmdData->nCmd, hWaitEvent, OutputBuffer, OutputLen))
	{
		Result = STATUS_INSERT_QUE_FAIL;
		goto end;
	}
	if ((int)(*CmdData->data) == 0)
	{
		Result = PostCmdToLC(PrgObject, CmdData, CmdDataLen);
		if (STATUS_SUCCESS != Result)
		{
			goto end;
		}
	}
	/*else
	{
	SetEvent(hWaitEvent);
	}*/


	// 上位机发送指令后，在此等待下位机应答
	WaitResult = WaitForSingleObject(hWaitEvent, TimeoutInterval);
	if (WAIT_TIMEOUT == WaitResult)
	{
		DbgPrint("SendCmdToLC time out.[cmd=%#x]\n", CmdData->nCmd);
		Result = STATUS_TIME_OUT;
		goto end;
	}


	// 	if (LC_TIMEOUT_FLAG == InterlockedExchangeAdd(&g_LCTimeoutFlag, 0))
	// 	{
	// 		DbgPrint("LC_TIMEOUT_FLAG == InterlockedExchangeAdd\n");
	// 		Result = STATUS_TIME_OUT;
	// 	}

end:
	if (g_WaitQueue.IsItemExist(CmdData->nCmd))
	{
		g_WaitQueue.Remove(CmdData->nCmd);
	}

	if (NULL != hWaitEvent)
	{
		CloseHandle(hWaitEvent);
	}

	return Result;
}

KSTATUS PostCmdToUC(PROGRAM_OBJECT PrgObject, PLC_COMMAND CmdData)
{
	KSTATUS Result = STATUS_SUCCESS;
	PDEV_EXTENSION DevExtension;
	
	Result = FMMC_GetPrgObjectExtension(PrgObject, (void **)&DevExtension);
	CheckResultCode("FMMC_GetPrgObjectExtension", "PostCmd", Result);
	
	if( DevExtension->BillPrgObject == NULL )
	{
		DbgPrint( " DevExtension->BillPrgObject == NULL " );
		Result = STATUS_UNINIT_STATUS;
		goto end;
	}
	
	Result = FMMC_EasySynchronousBuildPRP(DevExtension->BillPrgObject, NULL, PRP_AC_DEVIO, CONTROL_CMD_REQUEST, CmdData, sizeof(LC_COMMAND), NULL, 0, FALSE, FALSE);
	CheckResultCode("PostCmdToUC", "FMMC_EasySynchronousBuildPRP", Result);
	
end:
	return Result;
}	