#ifndef   _THSERIALPORT_INCLUDE
#define   _THSERIALPORT_INCLUDE

#include "..\..\inc\fmmc.h"
#include "..\..\inc\THFrameDef.h"
#include "Frame.h"
#include <iostream>

using namespace std;

class  CTHSerialPort: public CFrame
{
private:

	DCB               dcb;
	HANDLE            hThread;
	HANDLE            hSPort;
	PROGRAM_OBJECT    PrgObject;
	LONG              nOverThread;
	ULONG             nInSize;
	ULONG             nOutSize;

	USHORT            m_PortNumber;
	
	COMMTIMEOUTS      m_CommTimeouts;
	CRITICAL_SECTION  cs;   //只用于写同步

	static ULONG ReadThread(CTHSerialPort *);
	static BOOL  ReadBufferFromSerialPort(CTHSerialPort *sp, PCHAR  pBuffer, ULONG &nReadLength, BOOL &bInProgress);
	VOID         StopSerialPort();

public:

	CTHSerialPort();
	~CTHSerialPort();

	KSTATUS           CreateSerialPort(POPEN_PAR   pOpenPar);
	KSTATUS           WriteBufferToSerialPort(PCHAR   pWriteBuffer, ULONG nWriteLength);
	PROGRAM_OBJECT    GetPrgObject();
	ULONG             GetInputBufferSize();
	HANDLE            GetSPortHandle();
	BOOL              QueryWorkStatus();
	static     VOID   GetConfigInfoFromFile(PCHAR  pConfigPath, DCB&dcb, COMMTIMEOUTS     &m_CommTimeouts, ULONG &nInMaxSize, ULONG &nOutMaxSize, USHORT &nPortNumber);
	static     VOID   SaveConfigInformationToFile(PCHAR  pConfigPath, DCB&dcb, COMMTIMEOUTS     &m_CommTimeouts, ULONG &nInMaxSize, ULONG &nOutMaxSize, USHORT &nPortNumber);
	OVERLAPPED        m_wait_o;                     // 用于辅助线程
	OVERLAPPED        m_read_o;                     // 用于异步读
	OVERLAPPED        m_write_o;                    // 用于异步写
	USHORT            m_DataParse_Way;



};

inline  char ToHexUpper(unsigned int value);

inline char ToHexLower(unsigned int value);
inline  int FromHex(unsigned int c);
std::string HexEncode(const std::string& d);
std::string HexDecode(const std::string& hex);
#endif