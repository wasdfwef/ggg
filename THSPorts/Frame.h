#ifndef    _FRAME_INCLUDE
#define    _FRAME_INCLUDE


#include "..\..\inc\fmmc.h"
#include "..\..\inc\THFrameDef.h"
#include "BuildFrame.h"

class   CFrame: public CBuildFrame
{

private:
	BOOL  PickupFrameForType1(PUCHAR  pBuffer, ULONG  &nLength, PROGRAM_OBJECT  TargetObject);
	BOOL  PickupFrameForType2(PUCHAR  pBuffer, ULONG  &nLength, PROGRAM_OBJECT  TargetObject);
	BOOL  PickupFrameForType3(PUCHAR  pBuffer, ULONG  &nLength, PROGRAM_OBJECT  TargetObject);
	BOOL  GetFrameHeaderForType2(PUCHAR  pBuffer, ULONG  &nLength);
	BOOL  IsVaildFrameForType2(PUCHAR  pBuffer, ULONG &nLength);
	BOOL  IsVaildFrameForType3(PUCHAR  pBuffer, ULONG &nLength);
	BOOL  AnsyFrameRequest(PUCHAR  pBuffer, ULONG  nLength, BOOLEAN bParsePro,PROGRAM_OBJECT  TargetObject);

	//同步发送请求,用于尿干化数据发送.因为干化数据是不需要底层进行解析数据,如果使用异步机制有可能由于框架线程切换问题导致上层收到数据顺序被打乱

	BOOL  SynFrameRequest(PUCHAR  pBuffer, ULONG  nLength, BOOLEAN bParsePro,PROGRAM_OBJECT  TargetObject);  

protected:
	static void  WINAPI FrameRequestCompletion(PRP  prp, PVOID  pContext, KSTATUS   status);

public:

	CFrame();
	BOOL   PickupFrame(PUCHAR  pBuffer, ULONG  &nLength, PROGRAM_OBJECT  TargetObject, USHORT  FrameType = 0);
	BOOL   TestPickupFrameForType2(PUCHAR  pBuffer, ULONG  &nLength, ULONG &nRightNum);
    

};



#endif