#include "StdAfx.h"
#include "UDCArc.h"
#include <malloc.h>
#include <stdio.h>
#include "DbgMsg.h"
#include "MapID.h"

#pragma pack(1)

typedef  struct _UDC_ARC_CONTEXT
{
	ULONG         nTaskID;
	PVOID         pUDCArc;

}UDC_ARC_CONTEXT, *PUDC_ARC_CONTEXT;


#pragma pack()



CUDCArc::CUDCArc()
{
	m_BillPrgObj = NULL;
	m_BillHelpler = NULL;
}




VOID
WINAPI
UDCArcDispatch(
PRP             prp,
PVOID           pContext,
KSTATUS         status
)
{
	ULONG                 nLength = 0;
	PBUFFER_INFO          buf_info;
	UDC_IMPORT_INFO_EX    udc_import_info_ex = {0};
	PLC_TEST_PAPER_INFO   pTestPInfo = NULL;
	PUDC_ARC_CONTEXT      ctx      = (PUDC_ARC_CONTEXT)pContext;
	CUDCArc              *udc_arc  = (CUDCArc *)ctx->pUDCArc;

	FMMC_GetPRPParmeters(prp, (PVOID*)&buf_info, &nLength, NULL, NULL);
	if( nLength == 0 )
		goto err_loc1;

	pTestPInfo = (PLC_TEST_PAPER_INFO)buf_info->pBuffer;
	
	if( status == STATUS_SUCCESS )
	{

		DBG_MSG("pTestPInfo->TestResult.ErrorCode = %u\n", pTestPInfo->TestResult.ErrorCode);

		if( pTestPInfo->TestResult.ErrorCode != ERROR_OPERATE_SUCCESS )
		{

err_loc1:
			UDC_IMPORT_FAIL   udc_import_fail;

			udc_import_fail.nTaskID = ctx->nTaskID;
			udc_import_fail.nErrCode = pTestPInfo->TestResult.ErrorCode;
			udc_arc->m_BillHelpler->AddStatusInformation(ThmeInformationType, UdcImportFail, &udc_import_fail, sizeof(udc_import_fail));
			goto exit_disp;
		}
		else
		{

			udc_import_info_ex.BIL = pTestPInfo->TestResult.UDCTestResult.BIL;
			udc_import_info_ex.BLD = pTestPInfo->TestResult.UDCTestResult.BLD;
			udc_import_info_ex.GLU = pTestPInfo->TestResult.UDCTestResult.GLU;
			udc_import_info_ex.KET = pTestPInfo->TestResult.UDCTestResult.KET;
			udc_import_info_ex.LEU = pTestPInfo->TestResult.UDCTestResult.LEU;
			udc_import_info_ex.NIT = pTestPInfo->TestResult.UDCTestResult.NIT;
			udc_import_info_ex.PH = pTestPInfo->TestResult.UDCTestResult.PH;
			udc_import_info_ex.PRO = pTestPInfo->TestResult.UDCTestResult.PRO;
			udc_import_info_ex.SG  = pTestPInfo->TestResult.UDCTestResult.SG;
			udc_import_info_ex.URO  = pTestPInfo->TestResult.UDCTestResult.URO;
			udc_import_info_ex.VC  = pTestPInfo->TestResult.UDCTestResult.VC;
			udc_import_info_ex.MA  = pTestPInfo->TestResult.UDCTestResult.MCA;
			udc_import_info_ex.ID  = ctx->nTaskID;

			DBG_MSG("UDCArcDispatch->Ma = %u, TEST_PAPER_INFO = %u\n", udc_import_info_ex.MA, sizeof(TEST_PAPER_INFO));


			udc_arc->m_BillHelpler->AddStatusInformation(ThmeInformationType, UdcImportEventEx, &udc_import_info_ex, sizeof(udc_import_info_ex));

		}

		
	}
	else
		goto err_loc1;
exit_disp:	
	free(ctx);
	if( pTestPInfo )
		free(pTestPInfo);
	
	FMMC_FreePRP(prp);

}


KSTATUS CUDCArc::InitializeUDCArc( PROGRAM_OBJECT BillPrgObj, CBillHelper *pBillHelper )
{
	m_BillPrgObj  = BillPrgObj;
	m_BillHelpler = pBillHelper;
	return STATUS_SUCCESS;
}

KSTATUS CUDCArc::StartUDCEvent(ULONG  nTaskID)
{
	PLC_COMMAND           lc_cmd;
	BUFFER_INFO           buf_info;
	PBILL3_EXTENSION      pExtension;
	PUDC_ARC_CONTEXT      udc_context;
	PLC_TEST_PAPER_INFO   pTestPInfo;

/*	status = m_BillHelpler->UpdateUDCStatus(nTaskID, &bFinish);
	if( status != STATUS_SUCCESS )
	{
		DBG_MSG("StartUDCEvent status = %x\n", status);
		return status;
	}*/

	udc_context = (PUDC_ARC_CONTEXT)malloc(sizeof(UDC_ARC_CONTEXT));
	if( udc_context == NULL )
		return STATUS_ALLOC_MEMORY_ERROR;

	lc_cmd = (PLC_COMMAND)malloc(sizeof(LC_COMMAND));
	if( lc_cmd == NULL )
	{
		free(udc_context);
		return STATUS_ALLOC_MEMORY_ERROR;
	}

	ZeroMemory(lc_cmd, sizeof(LC_COMMAND));
	ZeroMemory(udc_context, sizeof(UDC_ARC_CONTEXT));
	FMMC_GetPrgObjectExtension(m_BillPrgObj, (PVOID*)&pExtension);

	nTaskID = g_MapID.Get((int)nTaskID);

	if( nTaskID == 0 )
	{
		free(lc_cmd);
		return STATUS_INVAILD_DATA;
	}

	udc_context->nTaskID         = nTaskID;
	udc_context->pUDCArc         = this;
	pTestPInfo                   = (PLC_TEST_PAPER_INFO)lc_cmd;
	pTestPInfo->lc_header.flag   = LC_FLAG;
	pTestPInfo->lc_header.nLCCmd = LC_UDC_TEST_PAPER;
	buf_info.nLength             = sizeof(LC_COMMAND);
	buf_info.pBuffer             = (PCHAR)lc_cmd;


	return FMMC_EasyasynchronousBuildPRP(pExtension->o_dev.s_udc_dev, NULL, PRP_AC_WRITE, WRITE_BUFFER_REQUEST, &buf_info, sizeof(buf_info), NULL, 0, UDCArcDispatch, udc_context); //启用异步机制
}
