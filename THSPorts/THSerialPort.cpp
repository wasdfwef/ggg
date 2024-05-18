#include "StdAfx.h"
#include "THSerialPort.h"
#include "DbgMsg.h"
#include <stdio.h>
#include <malloc.h>
#include "LogFile.h"
#include "Frame.h"
#include <shlwapi.h>

#define  CLEAR_ALL_BUFFER(x) PurgeComm(x, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR)
#define  CLEAR_WRITE_BUFFER(x) PurgeComm(x, PURGE_TXABORT | PURGE_TXCLEAR);
#define  CLEAR_READ_BUFFER(x)  PurgeComm(x, PURGE_RXABORT | PURGE_RXCLEAR);

#define  RETRY_COUNTER     3


CTHSerialPort::CTHSerialPort()
{
	ZeroMemory(&dcb, sizeof(dcb));
	
	dcb.DCBlength             = sizeof(DCB);
	dcb.BaudRate              = CBR_9600;
	dcb.ByteSize              = DATABITS_8;
	dcb.Parity                = NOPARITY;
	dcb.StopBits              = ONESTOPBIT;
	dcb.EvtChar               = 10;
	dcb.fBinary               = 1;
	dcb.XoffLim               = 512;
	dcb.XonLim                = 2048;

	nInSize                   = 2048;
	nOutSize                  = 512;
	
	m_CommTimeouts.ReadIntervalTimeout         = MAXDWORD;
	m_CommTimeouts.ReadTotalTimeoutMultiplier  = 0;
	m_CommTimeouts.ReadTotalTimeoutConstant    = 0;
	m_CommTimeouts.WriteTotalTimeoutMultiplier = 10;
	m_CommTimeouts.WriteTotalTimeoutConstant   = 1000;

	hSPort       = hThread     = NULL;
	nOverThread = 0;

	ZeroMemory(&m_read_o, sizeof(m_read_o));
	ZeroMemory(&m_write_o, sizeof(m_write_o));
	ZeroMemory(&m_wait_o, sizeof(m_wait_o));
	m_DataParse_Way = 0;

}

CTHSerialPort::~CTHSerialPort()
{
	StopSerialPort();

	if( hSPort )
		CloseHandle(hSPort);

	if( hThread )
	{
		WaitForSingleObject(hThread,INFINITE);
		CloseHandle(hThread);
		DeleteCriticalSection(&cs);
	}

	if( m_read_o.hEvent )
		CloseHandle(m_read_o.hEvent);
	if( m_write_o.hEvent )
		CloseHandle(m_write_o.hEvent);
	if( m_wait_o.hEvent )
		CloseHandle(m_wait_o.hEvent);

	Sleep(200);

}


#define  _T
#define  SECTION_NAME  "SerialPort"

VOID  CTHSerialPort::GetConfigInfoFromFile(PCHAR  pConfigPath, DCB&dcb, COMMTIMEOUTS      &m_CommTimeouts,ULONG &nInMaxSize, ULONG &nOutMaxSize, USHORT &nPortNumber)
{
	nPortNumber               =   GetPrivateProfileInt(SECTION_NAME, _T("Port"), 0, pConfigPath);
	dcb.DCBlength             = sizeof(dcb);
	dcb.BaudRate              = (unsigned long)  GetPrivateProfileInt(SECTION_NAME, _T("BaudRate"),     CBR_9600, pConfigPath);
	dcb.ByteSize              = (unsigned char)  GetPrivateProfileInt(SECTION_NAME, _T("ByteSize"),   DATABITS_8, pConfigPath);
	dcb.Parity                = (unsigned char)  GetPrivateProfileInt(SECTION_NAME, _T("Parity"),       NOPARITY, pConfigPath);
	dcb.StopBits              = (unsigned char)  GetPrivateProfileInt(SECTION_NAME, _T("StopBits"),   ONESTOPBIT, pConfigPath);
	//m_dwEvtMask                 = (unsigned long)  GetPrivateProfileInt(SECTION_NAME, _T("EvtMask"),     EV_RXFLAG, pConfigPath);
	dcb.EvtChar               = (char)           GetPrivateProfileInt(SECTION_NAME, _T("EvtChar"),            10, pConfigPath);
	//m_nMaxInQueue               =                  GetPrivateProfileInt(SECTION_NAME, _T("MaxInQueue"),          1, pConfigPath);
	dcb.fBinary               =  1;
	nInMaxSize                = (unsigned long)  GetPrivateProfileInt(SECTION_NAME, _T("InQueue"),          2048, pConfigPath);
	nOutMaxSize               = (unsigned long)  GetPrivateProfileInt(SECTION_NAME, _T("OutQueue"),          512, pConfigPath);
	dcb.EofChar               = (char)           GetPrivateProfileInt(SECTION_NAME, _T("EofChar"),                             0, pConfigPath);
	dcb.ErrorChar             = (char)           GetPrivateProfileInt(SECTION_NAME, _T("ErrorChar"),                           0, pConfigPath);
	dcb.fAbortOnError         =                  GetPrivateProfileInt(SECTION_NAME, _T("fAbortOnError"),                       0, pConfigPath);
	dcb.fDsrSensitivity       =                  GetPrivateProfileInt(SECTION_NAME, _T("fDsrSensitivity"),                     0, pConfigPath);
	dcb.fDtrControl           =                  GetPrivateProfileInt(SECTION_NAME, _T("fDtrControl"),                         0, pConfigPath);
	dcb.fDummy2               =                  GetPrivateProfileInt(SECTION_NAME, _T("fDummy2"),                             0, pConfigPath);
	dcb.fErrorChar            =                  GetPrivateProfileInt(SECTION_NAME, _T("fErrorChar"),                          0, pConfigPath);
	dcb.fInX                  =                  GetPrivateProfileInt(SECTION_NAME, _T("fInX"),                                0, pConfigPath);
	dcb.fNull                 =                  GetPrivateProfileInt(SECTION_NAME, _T("fNull"),                               0, pConfigPath);
	dcb.fOutX                 =                  GetPrivateProfileInt(SECTION_NAME, _T("fOutX"),                               0, pConfigPath);
	dcb.fOutxCtsFlow          =                  GetPrivateProfileInt(SECTION_NAME, _T("fOutxCtsFlow"),                        0, pConfigPath);
	dcb.fOutxDsrFlow          =                  GetPrivateProfileInt(SECTION_NAME, _T("fOutxDsrFlow"),                        0, pConfigPath);
	dcb.fParity               =                  GetPrivateProfileInt(SECTION_NAME, _T("fParity"),                             0, pConfigPath);
	dcb.fRtsControl           =                  GetPrivateProfileInt(SECTION_NAME, _T("fRtsControl"),                         0, pConfigPath);
	dcb.fTXContinueOnXoff     =                  GetPrivateProfileInt(SECTION_NAME, _T("fTXContinueOnXoff"),                   0, pConfigPath);
	dcb.wReserved             = (unsigned short) GetPrivateProfileInt(SECTION_NAME, _T("wReserved"),                           0, pConfigPath);
	dcb.wReserved1            = (unsigned short) GetPrivateProfileInt(SECTION_NAME, _T("wReserved1"),                          0, pConfigPath);
	dcb.XoffChar              = (char)           GetPrivateProfileInt(SECTION_NAME, _T("XoffChar"),                            0, pConfigPath);
	dcb.XoffLim               = (unsigned short) GetPrivateProfileInt(SECTION_NAME, _T("XoffLim"),                           512, pConfigPath);
	dcb.XonChar               = (char)           GetPrivateProfileInt(SECTION_NAME, _T("XonChar"),                             0, pConfigPath);
	dcb.XonLim                = (unsigned short) GetPrivateProfileInt(SECTION_NAME, _T("XonLim"),                           2048, pConfigPath);
	m_CommTimeouts.ReadIntervalTimeout           = GetPrivateProfileInt(SECTION_NAME, _T("ReadIntervalTimeout"),          MAXDWORD, pConfigPath);
	m_CommTimeouts.ReadTotalTimeoutMultiplier    = GetPrivateProfileInt(SECTION_NAME, _T("ReadTotalTimeoutMultiplier"),          0, pConfigPath);
	m_CommTimeouts.ReadTotalTimeoutConstant      = GetPrivateProfileInt(SECTION_NAME, _T("ReadTotalTimeoutConstant"),            0, pConfigPath);
	m_CommTimeouts.WriteTotalTimeoutMultiplier   = GetPrivateProfileInt(SECTION_NAME, _T("WriteTotalTimeoutMultiplier"),        10, pConfigPath);
	m_CommTimeouts.WriteTotalTimeoutConstant     = GetPrivateProfileInt(SECTION_NAME, _T("WriteTotalTimeoutConstant"),        1000, pConfigPath);

}

VOID  CTHSerialPort::SaveConfigInformationToFile(PCHAR  pConfigPath, DCB&dcb, COMMTIMEOUTS      &m_CommTimeouts,ULONG &nInMaxSize, ULONG &nOutMaxSize, USHORT &nPortNumber)
{
	CHAR szValue[64];

	sprintf(szValue, _T("%u"), nPortNumber);
	WritePrivateProfileString(SECTION_NAME, _T("Port"),       szValue, pConfigPath);
	sprintf(szValue, _T("%u"), dcb.BaudRate);
	WritePrivateProfileString(SECTION_NAME, _T("BaudRate"),   szValue, pConfigPath);
	sprintf(szValue, _T("%u"), dcb.ByteSize);
	WritePrivateProfileString(SECTION_NAME, _T("ByteSize"),   szValue, pConfigPath);
	sprintf(szValue, _T("%u"), dcb.Parity);
	WritePrivateProfileString(SECTION_NAME, _T("Parity"),     szValue, pConfigPath);
	sprintf(szValue, _T("%u"), dcb.StopBits);
	WritePrivateProfileString(SECTION_NAME, _T("StopBits"),   szValue, pConfigPath);
//	sprintf(szValue, _T("%u"), m_dwEvtMask);
//	WritePrivateProfileString(SECTION_NAME, _T("EvtMask"),    szValue, pConfigPath);
	sprintf(szValue, _T("%u"), (unsigned char) dcb.EvtChar);
	WritePrivateProfileString(SECTION_NAME, _T("EvtChar"),    szValue, pConfigPath);
	sprintf(szValue, _T("%u"), 1);
	WritePrivateProfileString(SECTION_NAME, _T("MaxInQueue"), szValue, pConfigPath);
	sprintf(szValue, _T("%u"), nInMaxSize);
	WritePrivateProfileString(SECTION_NAME, _T("InQueue"),    szValue, pConfigPath);
	sprintf(szValue, _T("%u"), nOutMaxSize);
	WritePrivateProfileString(SECTION_NAME, _T("OutQueue"),   szValue, pConfigPath);
	sprintf(szValue, _T("%u"), dcb.EofChar);
	WritePrivateProfileString(SECTION_NAME, _T("EofChar"),                     szValue, pConfigPath);
	sprintf(szValue, _T("%u"), dcb.ErrorChar);
	WritePrivateProfileString(SECTION_NAME, _T("ErrorChar"),                   szValue, pConfigPath);
	sprintf(szValue, _T("%u"), dcb.fAbortOnError);
	WritePrivateProfileString(SECTION_NAME, _T("fAbortOnError"),               szValue, pConfigPath);
	sprintf(szValue, _T("%u"), dcb.fDsrSensitivity);
	WritePrivateProfileString(SECTION_NAME, _T("fDsrSensitivity"),             szValue, pConfigPath);
	sprintf(szValue, _T("%u"), dcb.fDtrControl);
	WritePrivateProfileString(SECTION_NAME, _T("fDtrControl"),                 szValue, pConfigPath);
	sprintf(szValue, _T("%u"), dcb.fDummy2);
	WritePrivateProfileString(SECTION_NAME, _T("fDummy2"),                     szValue, pConfigPath);
	sprintf(szValue, _T("%u"), dcb.fErrorChar);
	WritePrivateProfileString(SECTION_NAME, _T("fErrorChar"),                  szValue, pConfigPath);
	sprintf(szValue, _T("%u"), dcb.fInX);
	WritePrivateProfileString(SECTION_NAME, _T("fInX"),                        szValue, pConfigPath);
	sprintf(szValue, _T("%u"), dcb.fNull);
	WritePrivateProfileString(SECTION_NAME, _T("fNull"),                       szValue, pConfigPath);
	sprintf(szValue, _T("%u"), dcb.fOutX);
	WritePrivateProfileString(SECTION_NAME, _T("fOutX"),                       szValue, pConfigPath);
	sprintf(szValue, _T("%u"), dcb.fOutxCtsFlow);
	WritePrivateProfileString(SECTION_NAME, _T("fOutxCtsFlow"),                szValue, pConfigPath);
	sprintf(szValue, _T("%u"), dcb.fOutxDsrFlow);
	WritePrivateProfileString(SECTION_NAME, _T("fOutxDsrFlow"),                szValue, pConfigPath);
	sprintf(szValue, _T("%u"), dcb.fParity);
	WritePrivateProfileString(SECTION_NAME, _T("fParity"),                     szValue, pConfigPath);
	sprintf(szValue, _T("%u"), dcb.fRtsControl);
	WritePrivateProfileString(SECTION_NAME, _T("fRtsControl"),                 szValue, pConfigPath);
	sprintf(szValue, _T("%u"), dcb.fTXContinueOnXoff);
	WritePrivateProfileString(SECTION_NAME, _T("fTXContinueOnXoff"),           szValue, pConfigPath);
	sprintf(szValue, _T("%u"), dcb.wReserved);
	WritePrivateProfileString(SECTION_NAME, _T("wReserved"),                   szValue, pConfigPath);
	sprintf(szValue, _T("%u"), dcb.wReserved1);
	WritePrivateProfileString(SECTION_NAME, _T("wReserved1"),                  szValue, pConfigPath);
	sprintf(szValue, _T("%u"), dcb.XoffChar);
	WritePrivateProfileString(SECTION_NAME, _T("XoffChar"),                    szValue, pConfigPath);
	sprintf(szValue, _T("%u"), dcb.XoffLim);
	WritePrivateProfileString(SECTION_NAME, _T("XoffLim"),                     szValue, pConfigPath);
	sprintf(szValue, _T("%u"), dcb.XonChar);
	WritePrivateProfileString(SECTION_NAME, _T("XonChar"),                     szValue, pConfigPath);
	sprintf(szValue, _T("%u"), dcb.XonLim);
	WritePrivateProfileString(SECTION_NAME, _T("XonLim"),                      szValue, pConfigPath);
	sprintf(szValue, _T("%u"), m_CommTimeouts.ReadIntervalTimeout);
	WritePrivateProfileString(SECTION_NAME, _T("ReadIntervalTimeout"),         szValue, pConfigPath);
	sprintf(szValue, _T("%u"), m_CommTimeouts.ReadTotalTimeoutMultiplier);
	WritePrivateProfileString(SECTION_NAME, _T("ReadTotalTimeoutMultiplier"),  szValue, pConfigPath);
	sprintf(szValue, _T("%u"), m_CommTimeouts.ReadTotalTimeoutConstant);
	WritePrivateProfileString(SECTION_NAME, _T("ReadTotalTimeoutConstant"),    szValue, pConfigPath);
	sprintf(szValue, _T("%u"), m_CommTimeouts.WriteTotalTimeoutMultiplier);
	WritePrivateProfileString(SECTION_NAME, _T("WriteTotalTimeoutMultiplier"), szValue, pConfigPath);
	sprintf(szValue, _T("%u"), m_CommTimeouts.WriteTotalTimeoutConstant);
	WritePrivateProfileString(SECTION_NAME, _T("WriteTotalTimeoutConstant"),   szValue, pConfigPath);

}


KSTATUS  CTHSerialPort::CreateSerialPort(POPEN_PAR   pOpenPar)
{
	char      str[80];
	DWORD     ThreadID;
	KSTATUS   status = STATUS_SUCCESS;

	if( pOpenPar->PrgObject == NULL )
		return STATUS_INVALID_PARAMETERS;

	if( pOpenPar->SConfigPath[0] && PathFileExists(pOpenPar->SConfigPath) == TRUE )
		GetConfigInfoFromFile(pOpenPar->SConfigPath,dcb, m_CommTimeouts, nInSize, nOutSize, m_PortNumber);

	DBG_MSG("CreateSerialPort..m_PortNumber = %u\n", m_PortNumber);

	if( m_PortNumber < 1 )
		return STATUS_INVALID_PARAMETERS;

	m_DataParse_Way = (USHORT)pOpenPar->DataParseWay;
	PrgObject = pOpenPar->PrgObject;

	

	sprintf(str, "\\\\.\\COM%u", m_PortNumber);
	hSPort = CreateFile(str, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED, NULL);
	if( hSPort == INVALID_HANDLE_VALUE )
	{
		DBG_MSG("CreateSerialPort->CreateFile Fail...%x, m_PortNumber = %s\n", GetLastError(), str);
		status = STATUS_OPEN_SPORT_FAIL;
		goto o_fail;
	}

	DBG_MSG("CreateSerialPort->nInSize = %u\n", nInSize);

	SetupComm(hSPort, nInSize, nOutSize);
	SetCommState(hSPort, &dcb);
	SetCommTimeouts(hSPort, &m_CommTimeouts);
	SetCommMask(hSPort, EV_RXCHAR);
	SetFrameFlag(pOpenPar->SConfigPath);

	hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ReadThread, this, CREATE_SUSPENDED, &ThreadID);

	if( hThread == NULL )
	{
		DBG_MSG("Sport_OpenSPort->CreateThread Fail...%x\n", GetLastError());
		status = STATUS_START_THREAD_FAIL;
	}

	if( status == STATUS_SUCCESS )
	{
		m_read_o.hEvent  = CreateEvent(NULL, TRUE, FALSE, NULL);
		m_write_o.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		m_wait_o.hEvent  = CreateEvent(NULL, TRUE, FALSE, NULL);
		InitializeCriticalSection(&cs);
		CLEAR_ALL_BUFFER(hSPort);
		ResumeThread(hThread);
	}

	DBG_MSG("CreateSerialPort Finally status = %x\n", status);

o_fail:

	return status;
}



BOOL  CTHSerialPort::ReadBufferFromSerialPort(CTHSerialPort *sp, PCHAR  pBuffer, ULONG &nReadLength, BOOL &bInProgress)
{
	COMSTAT             ComStat;
	DWORD               dwErrorFlags, dwErrors = 0;
	

	bInProgress = FALSE;

	if( ClearCommError(sp->GetSPortHandle(),&dwErrorFlags,&ComStat) == FALSE )
		return FALSE;


	if( ComStat.cbInQue == 0 )
		return FALSE;

	if (ReadFile(sp->GetSPortHandle(), pBuffer, nReadLength, &nReadLength, &sp->m_read_o) == TRUE)
	{
		LogFile tLogFile;
		string bufer(pBuffer, nReadLength);
		tLogFile.Log(HexEncode(bufer), false);
		goto clear_r_buffer;
	}
		
	

	dwErrors = GetLastError();
	if ( dwErrors == ERROR_IO_PENDING )
	{
		if ( GetOverlappedResult(sp->GetSPortHandle(), &sp->m_read_o, &nReadLength, TRUE) )
			return TRUE;

		dwErrors = GetLastError();
		if( dwErrors != ERROR_IO_INCOMPLETE )
		{
			DBG_MSG("ReadBufferFromSerialPort->GetOverlappedResult Fail = %x\n", GetLastError());
			return FALSE;
		}

		bInProgress = TRUE;
	}
	else
	{
		DBG_MSG("ReadBufferFromSerialPort->ReadFile Fail = %x\n", dwErrors);
		return FALSE;
	}

clear_r_buffer:

	//if( bInProgress == FALSE )
	//	CLEAR_READ_BUFFER(sp->GetSPortHandle());

	return TRUE; 
 
}

PROGRAM_OBJECT    CTHSerialPort::GetPrgObject()
{
	return PrgObject;
}

ULONG CTHSerialPort::ReadThread(CTHSerialPort *sp)
{
	BOOL                bRet, bInProgress;
	PCHAR               pReadBuffer;
	DWORD               r, dwEventMask, UsefulCounter = 0, nTotalBytesCounter = 0;
//	CFrame              f;



	
	//鴥更大的空间用户后面的处理
	pReadBuffer = (PCHAR)malloc(sp->GetInputBufferSize() << 1);
	
	if( pReadBuffer == NULL ) 
		return 0;
		
	ZeroMemory(pReadBuffer, sp->GetInputBufferSize() << 1);
	
	while( TRUE )
	{
	    bRet = WaitCommEvent(sp->GetSPortHandle(), &dwEventMask, &sp->m_wait_o);

		if( sp->QueryWorkStatus() == FALSE )
			break;

 	    if( bRet == FALSE )
		{
			if( GetLastError() == ERROR_IO_PENDING  )
				GetOverlappedResult(sp->GetSPortHandle(), &sp->m_wait_o, &r, TRUE);
		}

		r = sp->GetInputBufferSize();

		bRet = FALSE;
		while( TRUE )
		{
			bInProgress = FALSE;
			if( ReadBufferFromSerialPort(sp, &pReadBuffer[UsefulCounter], r, bInProgress) == FALSE )
				break;
			
			UsefulCounter += r;
			if( bInProgress == FALSE )
			{
				bRet = TRUE;
				break;
			}
		}

		nTotalBytesCounter += UsefulCounter;

		if( r == 0 || UsefulCounter == 0 || bRet == FALSE )
			continue;

		sp->PickupFrame((PUCHAR)pReadBuffer, UsefulCounter, sp->GetPrgObject(), sp->m_DataParse_Way);

		if( UsefulCounter > sp->GetInputBufferSize() + 0x100 ) //垃圾数据
		{
			DBG_MSG("******垃圾数据*******\n", UsefulCounter);
			UsefulCounter = 0;
		}
	}
	
	free(pReadBuffer);
	return 0;
}

ULONG  CTHSerialPort::GetInputBufferSize()
{
	return nInSize;
}

HANDLE CTHSerialPort::GetSPortHandle()
{
	return  hSPort;
}

VOID  CTHSerialPort::StopSerialPort()
{
	InterlockedExchange(&nOverThread, 1);

}

BOOL  CTHSerialPort::QueryWorkStatus()
{
	if( InterlockedExchangeAdd(&nOverThread, 0) == 0 )
		return TRUE;

	return FALSE;
}


KSTATUS  CTHSerialPort::WriteBufferToSerialPort(PCHAR   pWriteBuffer, ULONG nWriteLength)
 {
	PCHAR                  pNewBuffer = NULL;
	COMSTAT                ComStat;
	ULONG                  nSize, j, dwErrorFlags, err;
	KSTATUS                status;

	if( pWriteBuffer == NULL || nWriteLength == 0 )
	{
		return STATUS_INVALID_PARAMETERS;
	}

	status = STATUS_SUCCESS;

	if( m_DataParse_Way == 0 )
		pNewBuffer = pWriteBuffer;
	else
	{
		pNewBuffer   = (PCHAR)malloc(nWriteLength + 1024);
		if( pNewBuffer == NULL )
			return STATUS_ALLOC_MEMORY_ERROR;

		if( m_DataParse_Way == DATA_PARSE_TYPE1 )
			nWriteLength = F1_BuildCmdData(pWriteBuffer, nWriteLength, pNewBuffer);
		else if( m_DataParse_Way == DATA_PRASE_TYPE2 )
			nWriteLength = F2_BuildCmdData(pWriteBuffer, nWriteLength, pNewBuffer);
		else if( m_DataParse_Way == DATA_PRASE_TYPE3 )
			nWriteLength = F3_BuildCmdData(pWriteBuffer, nWriteLength, pNewBuffer);
		else
			return STATUS_INVALID_PARAMETERS;
	}

	EnterCriticalSection(&cs);

	CLEAR_WRITE_BUFFER(hSPort);
	
	for( j = 0 ; j < RETRY_COUNTER ; j ++ )
	{
		nSize = 0;
		ClearCommError(hSPort,&dwErrorFlags,&ComStat);

		if (WriteFile(hSPort, pNewBuffer, nWriteLength, &nSize, &m_write_o) == TRUE)
		{
			break;
		}



		err = GetLastError();
		if( QueryWorkStatus() == FALSE )
		{
			status = STATUS_CANCEL_ACTION;
			break;
		}

		if ( err != ERROR_IO_PENDING )
		{
			DBG_MSG("Sports_WriteBufferToPorts->Fail = %x\n", err);
			continue;
		}
		//写日志
		LogFile log;

		if (m_DataParse_Way == 0)//LIS
		{			
			log.Log(pNewBuffer);
		}
		else//指令
		{
			string buffer(pNewBuffer, nWriteLength);
			log.Log(HexEncode(buffer));
		}
		if ( GetOverlappedResult(hSPort, &m_write_o, &nSize, TRUE) == TRUE )
			break;

		err = GetLastError();
		if( err != ERROR_IO_INCOMPLETE )
		{
			DBG_MSG("GetOverlappedResult->Fail = %x\n", GetLastError());
			continue;
		}
		//没有成完，继续写
		nWriteLength -= nSize;		
		pNewBuffer  = &pNewBuffer[nSize];
		j--;
		//CLEAR_WRITE_BUFFER(hSPort);
		continue;
	}

	if( j == RETRY_COUNTER )
		status = STATUS_WRITE_SPORT_FAIL;

	//CLEAR_WRITE_BUFFER(hSPort);
	LeaveCriticalSection(&cs);

	if( m_DataParse_Way != 0 && pNewBuffer )
		free(pNewBuffer);

	return status;

}

inline char ToHexUpper(unsigned int value)
 {
	 return "0123456789ABCDEF"[value & 0xF];
 }

inline char ToHexLower(unsigned int value)
 {
	 return "0123456789abcdef"[value & 0xF];
 }

inline int FromHex(unsigned int c)
{
	 return ((c >= '0') && (c <= '9')) ? int(c - '0') :
		 ((c >= 'A') && (c <= 'F')) ? int(c - 'A' + 10) :
		 ((c >= 'a') && (c <= 'f')) ? int(c - 'a' + 10) :  -1;
}

 //将数据d用16进制编码，返回值即是结果
 std::string HexEncode(const std::string& d)
 {
	 std::string hex;
	 hex.resize(d.size() * 3);
	 char* pHexData = (char*)hex.data();
	 const unsigned char* pSrcData = (const unsigned char*)d.data();
	 for (int i = 0; i < d.size(); i++)
	 {
		 pHexData[i * 3] = ToHexLower(pSrcData[i] >> 4);
		 pHexData[i * 3 + 1] = ToHexLower(pSrcData[i] & 0xf);
		 pHexData[i * 3 + 2] = ' ';
	 }

	 return hex;
 }

 //将数据d用16进制解码，返回值即是结果
 std::string HexDecode(const std::string& hex)
 {
	 std::string res;
	 res.resize(hex.size() + 1 / 2);
	 unsigned char* pResult = (unsigned char*)res.data() + res.size();
	 bool odd_digit = true;

	 for (int i = hex.size() - 1; i >= 0; i--)
	 {
		 unsigned char ch = unsigned char(hex.at(i));
		 int tmp = FromHex(ch);
		 if (tmp == -1)
			 continue;
		 if (odd_digit) {
			 --pResult;
			 *pResult = tmp;
			 odd_digit = false;
		 }
		 else {
			 *pResult |= tmp << 4;
			 odd_digit = true;
		 }
	 }

	 res.erase(0, pResult - (unsigned char*)res.data());

	 return res;
 }
