#ifndef       _INTERAL_INCLUDE
#define       _INTERAL_INCLUDE



#include "..\..\inc\fmmc.h"
#include "THSerialPort.h"


#define  SPORTS_FLAG   ((DWORD)(BYTE)('S') | ((DWORD)(BYTE)('P') << 8) | ((DWORD)(BYTE)('T') << 16) | ((DWORD)(BYTE)('S') << 24 ))


#pragma pack(1)

typedef struct _SPORTS_PROC_INFO
{
	ULONG             InteralFlag;
	CTHSerialPort    *serial_port;

}SPORTS_PROC_INFO, *PSPORTS_PROC_INFO;

typedef struct _COMMON_HEADER_1
{
	UCHAR    flag;
	UCHAR    cmd;
	UCHAR    h_len;
	UCHAR    d_len;
	UCHAR    d_check;
	UCHAR    h_check;
	char     data[1];

}COMMON_HEADER_1, *PCOMMON_HEADER_1;

typedef struct _COMMON_HEADER_2
{
	UCHAR    flag;
	UCHAR    cmd;
	UCHAR    d_len;
	UCHAR    d_check;
	UCHAR    h_check;
	char     data[1];
}COMMON_HEADER_2, *PCOMMON_HEADER_2;

#pragma pack()


#endif