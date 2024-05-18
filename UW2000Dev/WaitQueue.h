#ifndef WAIT_QUEUE_H_ALDFLADA
#define WAIT_QUEUE_H_ALDFLADA

#include <map>
#include <strsafe.h>
#include "ProtMapping.h"
using namespace std;

//#define BARCODE_LENGTH 50


#define  INVALID_CMD 0xFF
// struct LC_TASKINFO
// {
// 	WORD ID;
// 	UCHAR HoleNum;
// 	CHAR Barcode[BARCODE_LENGTH];
// 	UCHAR BarcodeLength;
// 
// 	LC_TASKINFO()
// 	{
// 		ID = 0;
// 		HoleNum = 0;
// 		BarcodeLength = 0;
// 		ZeroMemory(Barcode, BARCODE_LENGTH);
// 	}
// 	
// 	LC_TASKINFO(WORD ID, UCHAR HoleNum, CHAR *Barcode, UCHAR BarcodeLength)
// 	{
// 		ID =ID;
// 		HoleNum = HoleNum;
// 		BarcodeLength = BarcodeLength;
// 		StringCbCopy(Barcode, BarcodeLength + 1, Barcode);
// 	}
// };


class WaitQueue
{
public:
	WaitQueue();
	~WaitQueue();
	bool Add(UCHAR Cmd, HANDLE hEvent, CHAR *OutputBuffer, int OutputLength);
	bool Remove(UCHAR Cmd);
	bool SetWaitSuccess(UCHAR Cmd);
	bool SetWaitFailed(UCHAR Cmd);
	void Clear();
	bool GetCmdBuffer(UCHAR Cmd, CHAR **OutputBuffer, int &OutputBufferLength);
	bool IsItemExist(UCHAR Cmd);

private:
	struct WaitContex
	{
		UCHAR m_Cmd;
		HANDLE m_hEvent;
		CHAR *m_OutputBuffer;
		int m_OutputLength;

		WaitContex()
		{
			m_Cmd = INVALID_CMD;
			m_hEvent = NULL;
			m_OutputBuffer = NULL;
			m_OutputLength = 0;
		}

		WaitContex(UCHAR Cmd, HANDLE hEvent, CHAR *OutputBuffer, int OutputLength)
		{
			m_Cmd = Cmd;
			m_hEvent = hEvent;
			m_OutputBuffer = OutputBuffer;
			m_OutputLength = OutputLength;
		}
	};

	
	bool SetItemStatusAndRemove(UCHAR Cmd);

	typedef multimap<UCHAR, WaitContex> WaitContextQueue;
	CRITICAL_SECTION m_Cs;
	WaitContextQueue m_Queue;
};

#endif  //WAIT_QUEUE_H_ALDFLADA