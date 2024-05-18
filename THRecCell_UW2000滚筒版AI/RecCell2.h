#ifndef   _RECCELL2_INCLUDE
#define   _RECCELL2_INCLUDE

#include "\CommonAPI\inc\THFrameDef.h"
#include "interal.h"
#include "RecHelper.h"

class CRecCell2 : public CRecHelper
{
	typedef struct US_RESULT
	{
		double dPerimeter;//周长
		double dArea;     //面积
		int    nType;     //类型
		int    nLeft;     //位置
		int    nTop;      //位置
		int    nRight;    //位置
		int    nBottom;   //位置
		int    nNumber;
		int    nFlag;
	}CUS_Result;

	typedef 
	BOOL 
	(*TC2_RECOGNISING) (
	char *      pGrapPath,   //图片大小:659*493
	CUS_Result* rec_reslut, 
	int*        rec_counter
	);
	
	HMODULE                        hMod;
	CUS_Result                    *m_RecReslut;
	TC2_RECOGNISING                Cell2Recognising;
	

	void      SaveRecReslut(PCHAR  pIniPath, PCHAR pSessionName, int nRecCounter);
	
public:
	 CRecCell2();
	~CRecCell2();
	
	KSTATUS   StartRecCell(PREC_CELL_EXTENSION pExtension);
	KSTATUS   BuildRecReslut(PREC_GRAP_FULL_RECORD   grap_record);

};


#endif