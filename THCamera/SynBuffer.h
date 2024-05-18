#ifndef     _SYNBUFFER_INCLUDE
#define     _SYNBUFFER_INCLUDE

class   CSynBuffer
{
	PCHAR      m_TransBuffer;
	HANDLE     m_CopyOverEvent;
public:
	
	CSynBuffer();
	~CSynBuffer();

	BOOL    Create();
	BOOL    CopySynBuffer(PCHAR  pBuffer, ULONG  nBufferSize);
	BOOL    SetSynBufferAndWait(PCHAR  pSynBuffer);
	
	
};




#endif