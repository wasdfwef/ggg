#ifndef    _REC_CELL1_INCLUDE
#define    _REC_CELL1_INCLUDE

#include "\CommonAPI\inc\THFrameDef.h"
#include "interal.h"
#include "RecHelper.h"

class  CRecCell1: public CRecHelper
{

	typedef
		BOOL
		(*TC1_LOAD_CELL_IMAGE)(
		LPCSTR   pImagePath   //图片大小640*480
		);
	
	
	typedef 
		ULONG
		(*TC1_RECOGNISING)(
		);
	
	
	typedef 
		void
		(*TC1_SAVE_REC_RESLUT)(
		PWCHAR  pIniFilePath,
		PWCHAR  pSectionName
		);

	HMODULE                        hMod;
	TC1_LOAD_CELL_IMAGE            LoadCellImage;
	TC1_RECOGNISING                CellRecognising;
	TC1_SAVE_REC_RESLUT            SaveRecReslut;


	KSTATUS                        TransStandFormat(PREC_GRAP_FULL_RECORD   rec_record);


public:
	 CRecCell1();
	~CRecCell1();

	 KSTATUS   StartRecCell(PREC_CELL_EXTENSION pExtension);
	 KSTATUS   BuildRecReslut(PREC_GRAP_FULL_RECORD   rec_reslut);

};



#endif