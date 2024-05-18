#include "StdAfx.h"
#include "LCAction.h"
#include "DevManager.h"




KSTATUS CLCAction::SendlCCmdActionEx(ULONG       ActionCmd,PCHAR       pInBuffer,ULONG       nInLength,PCHAR       pOutBuffer,ULONG       nOutLength,TDEVICE_DESCRITOR   dev_type)
{
	KSTATUS          status;
	LC_COMMAND       cmd = {0};
	PROGRAM_OBJECT   prg_obj;
	
	
	status = CDevManager::GetSpecialDevTypeObject(dev_type, prg_obj);
	if( status != STATUS_SUCCESS )
		return status;
	
	if( dev_type != T_DEV_LC_TYPE )
	{
		BUFFER_INFO   buf_info = {0};
		
		buf_info.nLength = nInLength;
		buf_info.pBuffer = pInBuffer;
		
		return FMMC_EasySynchronousBuildPRP(prg_obj, NULL, PRP_AC_WRITE, WRITE_BUFFER_REQUEST, &buf_info, sizeof(buf_info), NULL, 0, FALSE , FALSE);
	}
	
	cmd.lc_header.flag   = LC_FLAG;
	cmd.lc_header.nLCCmd = (USHORT)ActionCmd;
	if( pInBuffer && nInLength )
		memcpy(cmd.Reserved, pInBuffer, nInLength);
	
	status = FMMC_EasySynchronousBuildPRP(prg_obj, NULL, PRP_AC_DEVIO, CONTROL_CMD_REQUEST, &cmd, sizeof(LC_COMMAND), NULL, 0, FALSE, FALSE);
	if( status == STATUS_SUCCESS && pOutBuffer && nOutLength )
		memcpy(pOutBuffer, cmd.Reserved, nOutLength);
	
	return status;
}

KSTATUS CLCAction::AutoDetectLCConfig(USHORT      nPortIndex,TDEVICE_DESCRITOR   dev_type)
{
	KSTATUS                          status;
	LC_COMMAND                       lc_cmd = {0};
	PROGRAM_OBJECT                   PrgObject;
	
	
	if( dev_type != T_DEV_LC_TYPE )
		return STATUS_INVALID_PARAMETERS;
	
	status = CDevManager::GetSpecialDevTypeObject(dev_type, PrgObject);
	if( status != STATUS_SUCCESS )
		return status;
	
	lc_cmd.lc_header.flag = LC_FLAG;
	//lc_cmd.lc_header.nLCCmd = LC_AUTO_DETECT;
	memcpy(lc_cmd.Reserved, &nPortIndex, sizeof(USHORT));
	status = FMMC_EasySynchronousBuildPRP(PrgObject, NULL, PRP_AC_DEVIO, CONTROL_CMD_REQUEST, &lc_cmd, sizeof(LC_COMMAND), NULL, 0, FALSE, FALSE);
	return status;

}