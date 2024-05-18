#ifndef PROT_MAPPING_H_ADADSF
#define PROT_MAPPING_H_ADADSF

//#include "ThLCDef.h"
#include <windows.h>

#define PARA_COUNT 40
#define MAX_DATA_LENGTH		256
#define SENSOR_COUNT		30

#define LC_RESTART_FLAG 1
#define LC_NO_RESTART_FLAG 0

#define LC_ALIVE_FLAG 1
#define LC_TIMEOUT_FLAG 0


//PROTO_UC_XXX: 上位机发送的命令

#define PROTO_UC_SELF_CHECK							0x01
#define PROTO_UC_ENABLE_AUTO_CHECK					0x03
#define PROTO_UC_DISABLE_AUTO_CHECK					0x02
#define PROTO_UC_NOTIFY_TASK_ID						0x09
#define PROTO_UC_LOCATE_10_MICROSCOPE				0x05
#define PROTO_UC_LOCATE_40_MICROSCOPE				0x06
#define PROTO_UC_FINISH_TAKE_PIC					0x07
#define PROTO_UC_SPECIAL_TEST						0x08
#define PROTO_UC_MOVE_SCAN_PLATFORM					0x10
#define PROTO_UC_CONTROL_LIGHT						0x11
#define PROTO_UC_SWITCH_CHANNEL						0x12
#define PROTO_UC_SWITCH_MICROSCOPE					0x13
#define PROTO_UC_RESET_X							0x14
#define PROTO_UC_RESET_Y							0x15
#define PROTO_UC_RESET_Z							0x16
#define PROTO_UC_TEST_HARDWARE						0x17
#define PROTO_UC_CLEAR_TROUBLE 						0x18
#define PROTO_UC_QUERY_Z_POS 						0x19
#define PROTO_UC_QUERY_XY_40_MICRO_DEVIA 			0x1A
#define PROTO_UC_CLEAR							 	0x1B
#define PROTO_UC_FORCE_CLEAR						0x1C
#define PROTO_UC_GET_LC_PARAMETERS					0x1D
#define PROTO_UC_SET_LC_PARAMETERS					0x1E
#define PROTO_UC_ENTER_HARDWARE_DEBUG				0x20
#define PROTO_UC_EXIT_HARDWARE_DEBUG				0x21
#define PROTO_UC_RESTART_LC							0x22
#define PROTO_UC_MOVE_Z								0x24
#define PROTO_UC_ENABLE_PIPE_IN						0x92
#define PROTO_UC_DISABLE_PIPE_IN					0x91
#define PROTO_UC_CONSUMABLES_FLAG				    0x96



//PROTO_LC_XXX: 向上位机发送的命令
#define PROTO_LC_START_SELF_CHECK					0x81
#define PROTO_LC_RESPOND_ENABLE_AUTO_CHECK			0x83
#define PROTO_LC_RESPOND_DISABLE_AUTO_CHECK			0x82
#define PROTO_LC_FINISH_ABSORBING 					0x84
#define PROTO_LC_FINISH_STANDING 					0x85 
#define PROTO_LC_FINISH_LOCATE_MICROSCOPE 			0x86 
#define PROTO_LC_UDC_CHECK							0x87
#define PROTO_LC_BARCODE_INFO						0x88
#define PROTO_LC_A_NEW_PIPE							0x89
#define PROTO_LC_FINISH_A_PIPE						0x8A
#define PROTO_LC_RETURN_Z_POS						0x8B
#define PROTO_LC_RETURN_XY_40_MICRO_DEVIA			0x8C
#define PROTO_LC_FINISH_CLEAR						0x8D
#define PROTO_LC_FINISH_FORCE_CLEAR					0x8E
#define PROTO_LC_RETURN_LC_PARAMETERS				0x90
#define PROTO_LC_FINISH_SET_LC_PARAMETERS			0x91
#define PROTO_LC_RETURN_ENTER_HARDWARE_DEBUG		0x92
#define PROTO_LC_RETURN_EXIT_HARDWARE_DEBUG			0x93
#define PROTO_LC_RESTART_SUCCESS					0x94
#define PROTO_LC_FAILURE_INFO						0xE0
#define PROTO_LC_WARNING_INFO						0xE1
#define PROTO_LC_FINISH_MOVE_SCAN_PLATFORM			0xC1
#define PROTO_LC_FINISH_RESET_Z						0xC2
#define PROTO_LC_FINISH_MOVE_Z						0xC3
#define PROTO_LC_FINISH_SELF_CHECK					0xC4
#define PROTO_LC_FINISH_SWITCH_CHANNEL				0xC5
#define PROTO_LC_FINISH_SWITCH_MICROSCOPE			0xC6
#define PROTO_LC_FINISH_RESET_X						0xC7
#define PROTO_LC_FINISH_RESET_Y						0xC8
#define PROTO_LC_QUERY_CONSUMABLES				    0x96


//UDC: 向下位机发送的命令
#define PROTO_UC_TEST_PAPER							0x72
#define PROTO_UC_CORRECT_MODULE						0x73
#define PROTO_UC_CORRECT_MODULE_EX                  0X74
#define PROTO_UC_SPECIFY_PAPER_TYPE					0x75
#define PROTO_UC_CONTROL_LED						0x77
#define PROTO_UC_FOCUSMOVE_Z                        0x78
#define PROTO_UC_GETAD                              0xB7
#define PROTO_UC_MAINTAIN                           0x1F
#define PROTO_UC_ALLOWTEST                          0x2A

//UDC: 向上位机发送的命令
#define PROTO_LC_RESPOND_TEST_PAPER					 0xE2
#define PROTO_LC_RESPOND_CORRECT_MODULE				 0xE1
#define PROTO_LC_RESPOND_CORRECT_MODULE_EX           0xEA
#define PROTO_LC_RESPOND_SPECIFY_PAPER_TYPE			 0xD3
#define PROTO_LC_RESPOND_TEST_PAPER2				 0xE4
#define PROTO_LC_RESPOND_FOCUMOVE_Z                  0x79
#define PROTO_LC_RESPOND_TASK_ID                     0x29
#define PROTO_LC_SENDAD                              0xB8
#define PROTO_LC_CLEAR_MAINTAIN_FINISH               0x8F
#define PROTO_LC_RESPOND_ALLOWTEST                   0x9A


//******************************PROTO_UC_ENABLE_AUTO_CHECK && PROTO_LC_RESPOND_ENABLE_AUTO_CHECK**********************************
//为了避免重复定义，结构体见“ThLCDef.h”－－ENABLE_AUTO_CHECK_INFO


//****************************************PROTO_LC_FINISH_ABSORBING*************************************************

#pragma pack(1)

struct FINISH_ABSORBING_INFO
{
	UCHAR HoleNum;
	UCHAR ChannelNum;
	UCHAR SuccessFlag;
};
typedef FINISH_ABSORBING_INFO *PFINISH_ABSORBING_INFO;

#pragma pack()


//****************************************PROTO_UC_NOTIFY_TASK_ID*************************************************
//为了避免重复定义，结构体见“ThLCDef.h”－－TASKID_INFO


//****************************************PROTO_LC_FINISH_STANDING*************************************************

//为了避免重复定义，结构体见“ThLCDef.h”－－FINISH_STANDING_INFO



//****************************************PROTO_UC_LOCATE_10_MICROSCOPE*************************************************
//为了避免重复定义，结构体见“ThLCDef.h”－－LOCATE_10_MICROSCOPE_INFO



//****************************************PROTO_UC_LOCATE_40_MICROSCOPE*************************************************
//为了避免重复定义，结构体见“ThLCDef.h”－－LOCATE_40_MICROSCOPE_INFO



//****************************************PROTO_UC_FINISH_TAKE_PIC*************************************************
//为了避免重复定义，结构体见“ThLCDef.h”－－FINISH_TAKE_PIC_INFO



//****************************************PROTO_LC_UDC_CHECK*************************************************

//为了避免重复定义，结构体见“ThLCDef.h”－－UDC_CHECK_INFO



//****************************************PROTO_LC_BARCODE_INFO*************************************************

//为了避免重复定义，结构体见“ThLCDef.h”－－BARCODE_INFO



//****************************************PROTO_UC_SPECIAL_TEST*************************************************

//为了避免重复定义，结构体见“ThLCDef.h”－－SPECIAL_TEST_INFO



//****************************************PROTO_UC_MOVE_SCAN_PLATFORM*************************************************

//为了避免重复定义，结构体见“ThLCDef.h”－－MOVE_SCAN_PLATFORM_INFO



//****************************************PROTO_UC_CONTROL_LIGHT*************************************************


//为了避免重复定义，结构体见“ThLCDef.h”－－CONTROL_LIGHT_INFO



//****************************************PROTO_UC_SWITCH_CHANNEL*************************************************

//为了避免重复定义，结构体见“ThLCDef.h”－－SWITCH_CHANNEL_INFO



//****************************************PROTO_UC_SWITCH_MICROSCOPE*************************************************

//为了避免重复定义，结构体见“ThLCDef.h”－－SWITCH_MICROSCOPE_INFO



//****************************************PROTO_UC_TEST_HARDWARE*************************************************

//为了避免重复定义，结构体见“ThLCDef.h”－－TEST_HARDWARE_INFO



//****************************************PROTO_UC_CLEAR_TROUBLE*************************************************

//为了避免重复定义，结构体见“ThLCDef.h”－－CLEAR_TROUBLE_INFO



//****************************************PROTO_UC_QUERY_Z_POS && PROTO_LC_RETURN_Z_POS*************************************************

//为了避免重复定义，结构体见“ThLCDef.h”－－QUERY_Z_POS_INFO



//**************************************PROTO_LC_RETURN_XY_40_MICRO_DEVIA****************************************

//为了避免重复定义，结构体见XY_40_MICRO_DEVIA_INFO



//*****************PROTO_UC_CLEAR && PROTO_LC_FINISH_CLEAR && PROTO_LC_FINISH_FORCE_CLEAR && PROTO_UC_FORCE_CLEAR********************

//为了避免重复定义，结构体见“ThLCDef.h”－－FINISH_CLEAR_INFO



//***************PROTO_UC_GET_LC_PARAMETERS && PROTO_UC_SET_LC_PARAMETERS && PROTO_LC_RETURN_LC_PARAMETERS********************

//为了避免重复定义，结构体见“ThLCDef.h”－－LC_PARAMETERS_INFO



//**********************************LC_FAILURE && PROTO_LC_FAILURE_INFO********************************************

//为了避免重复定义，结构体见“ThLCDef.h”－－LC_FAILURE_INFO



#endif  //PROT_MAPPING_H_ADADSF
