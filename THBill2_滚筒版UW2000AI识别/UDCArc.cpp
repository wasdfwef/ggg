#include "StdAfx.h"
#include "UDCArc.h"
#include <malloc.h>
#include <stdio.h>
#include "DbgMsg.h"

#pragma pack(1)

typedef  struct _UDC_ARC_CONTEXT
{
	ULONG         nTaskID;
	PVOID         pUDCArc;

}UDC_ARC_CONTEXT, *PUDC_ARC_CONTEXT;



//////////////
#define  ERROR_OPERATE_SUCCESS 0
#define  ERROR_POS_NOT_CORRECT 1
#define  ERROR_NO_PAPER 2

#define LC_UDC_TEST_PAPER		  0X00000049
#define LC_UDC_SPECIFY_PAPER_TYPE 0X0000004A
#define LC_UDC_CONTROL_LIGHT	  0X0000004B
#define LC_SELF_CHECK			  0X0000004C
#define LC_DISABLE_AUTO_CHECK	  0X0000004D
#define LC_RESET_X				  0X0000004E
#define LC_RESET_Y				  0X0000004F
#define LC_RESET_Z				  0X00000050
#define LC_ENTER_HARDWARE_DEBUG   0X00000051
#define LC_EXIT_HARDWARE_DEBUG    0X00000052
#define LC_RESTART_LC			  0X00000053
#define LC_MOVE_Z				  0X00000054
#define LC_UDC_CORRECT_MODULE_EX  0X00000055

#define LC_SCAN_BARCODE           0X00000056
#define LC_STOP_SCAN_BARCODE      0X00000057
#define LC_FOCUS_Z                0X00000058
#define LC_GET_AD                 0X00000059
#define LC_CLEAR_MAINTAIN         0X0000005A 
#define LC_ALLOW_TEST             0X0000005B

#define LC_CONSUMABLE_SURPLUS     0X0000005C

struct CUDCTestResult//2019.12.11
{

	WORD VC;
	WORD GLU;
	WORD BIL;
	WORD KET;
	WORD SG;
	WORD BLD;
	WORD PH;
	WORD PRO;
	WORD URO;
	WORD NIT;
	WORD LEU;
	WORD MCA;
};


struct TEST_PAPER_INFO
{
	//UCHAR nhole;
	//WORD  nID;
	UCHAR ErrorCode;
	CUDCTestResult UDCTestResult;
};
typedef TEST_PAPER_INFO *PTEST_PAPER_INFO;

typedef struct _LC_TEST_PAPER_INFO
{
	LC_HEADER   lc_header;
	TEST_PAPER_INFO TestResult;

}LC_TEST_PAPER_INFO, *PLC_TEST_PAPER_INFO;
//////////////


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
			udc_import_info_ex.MA = pTestPInfo->TestResult.UDCTestResult.MCA;
			udc_import_info_ex.ID  = ctx->nTaskID;
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

	udc_context->nTaskID         = nTaskID;
	udc_context->pUDCArc         = this;
	pTestPInfo                   = (PLC_TEST_PAPER_INFO)lc_cmd;
	pTestPInfo->lc_header.flag   = LC_FLAG;
	pTestPInfo->lc_header.nLCCmd = LC_UDC_TEST_PAPER;
	buf_info.nLength             = sizeof(LC_COMMAND);
	buf_info.pBuffer             = (PCHAR)lc_cmd;


	return FMMC_EasyasynchronousBuildPRP(pExtension->o_dev.s_udc_dev, NULL, PRP_AC_WRITE, WRITE_BUFFER_REQUEST, &buf_info, sizeof(buf_info), NULL, 0, UDCArcDispatch, udc_context); //启用异步机制
}
