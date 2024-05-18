﻿#ifndef    _F_ERR_INCLUDE
#define    _F_ERR_INCLUDE

#define    KSTATUS     ULONG

#define    STATUS_SUCCESS             0X00000000
#define    STATUS_INVALID_PARAMETERS  0XF0000001
#define    STATUS_INVALID_OBJECT      0XF0000002
#define    STATUS_NOT_FOUND_FILE      0XF0000003                
#define    STATUS_INVALID_XML_MODULE  0XF0000004
#define    STATUS_TINY_XML_ERROR      0XF0000005
#define    STATUS_EXCEPTION_ERROR     0XF0000006
#define    STATUS_ALLOC_MEMORY_ERROR  0XF0000007
#define    STATUS_OVER_DEPEND_COUNTER 0XF0000008
#define    STATUS_NO_REGISTER_MODULE  0XF0000009
#define    STATUS_INITIALZIE_QUE_FAIL 0XF000000A
#define    STATUS_INSERT_QUE_FAIL     0XF000000B
#define    STATUS_IDENTIFY_SAME       0XF000000C
#define    STATUS_NDEFINE_IDENTIFY    0XF000000D
#define    STATUS_INVALID_I_HEADER    0XF000000E
#define    STATUS_READ_ACCESS_MEMORY  0XF0000010
#define    STATUS_INVALID_FMMC_OBJECT 0XF0000011
#define    STATUS_NOT_FOUND_OBJECT    0XF0000012
#define    STATUS_MODULE_LOADED       0XF0000013
#define    STATUS_MODULE_LOAD_FAIL    0XF0000014
#define    STATUS_MOUDLE_NOT_STAND    0XF0000015
#define    STATUS_NOT_SPEC_DISPATCH   0XF0000016
#define    STATUS_NAME_TOO_LOGN       0XF0000017
#define    STATUS_INVALID_RELATIVE    0XF0000018
#define    STATUS_RUNTIME_ERROR       0XF0000019
#define    STATUS_SAME_OBJECT_NAME    0XF000001A
#define    STATUS_SET_PASSIVE_FLAG    0XF000001B
#define    STATUS_NO_UNIT_DATA        0XF000001C
#define    STATUS_NOT_DEV_PRG_OBJECT  0XF000001D
#define    STATUS_NOT_RELATIVTION     0XF000001E
#define    STATUS_REQUEST_COMPLETED   0XF000001F
#define    STATUS_CREATE_EVENT_FAIL   0XF0000020
#define    STATUS_CREATE_THREAD_FAIL  0XF0000021
#define    STATUS_PRGOBJECT_TRASH     0XF0000022
#define    STATUS_NOT_FOUND_VALUE     0XF0000023
#define    STATUS_PRGOBJECT_BOTTOM    0XF0000024
#define    STATUS_PROCCESS_BAD        0XF0000025
#define    STATUS_INVALID_AVOW        0XF0000026
#define    STATUS_PRGOBJECT_TOP       0XF0000027
#define    STATUS_INVALID_REQUEST     0XF0000028
#define    STATUS_OPEN_SPORT_FAIL     0XF0000029
#define    STATUS_START_THREAD_FAIL   0XF000002A
#define    STATUS_WRITE_FAIL          0XF000002B
#define    STATUS_CANCEL_ACTION       0XF000002C
#define    STATUS_WRITE_SPORT_FAIL    0XF000002D
#define    STATUS_UNINIT_STATUS       0XF000002E
#define    STATUS_TIME_OUT            0XF000002F
#define    STATUS_INVAILD_DATA        0XF0000030
#define    STATUS_OPEN_CAMERA_FAIL    0XF0000031
#define    STATUS_LOAD_FILE_FAIL      0XF0000032
#define    STATUS_MKDIR_FAIL          0XF0000033
#define    STATUS_RENAME_FAIL         0XF0000034
#define    STATUS_SYSTEM_BUSYING      0XF0000035
#define    STATUS_DEV_NOT_START       0XF0000036
#define    STATUS_DEV_START_ALREADY   0XF0000037
#define    STATUS_DATA_MATCH_ERROR    0XF0000038
#define    STATUS_BUFFER_TOO_SMALL    0XF0000039
#define    STATUS_REQUEST_PENDING     0X02000000
#define    STATUS_PRP_NEXT            0X02000001
#define    STATUS_PRP_PROCESSING      0X02000002
#define    STATUS_UNKNOW_ERROR        0XFFFFFFFE


// 1000   开始
#define    STATUS_LOGIN_PASSWORD_ERR 0X00001000
#define    STATUS_LOGIN_USER_ERR     0X00001001
#define    STATUS_LOGIN_USER_EXIST   0X00001002
#define    STATUS_LOGIN_USER_NOEXIST 0X00001003
#define    STATUS_LOGIN_USER_NO      0X00001004
#define    STATUS_LOGIN_MEMORY_NOTENOUGH 0X00001005
#define    STATUS_LOGIN_NO_US        0X00001005
#define	   STATUS_MODIFY_PWD_FAILED  0X00001007
#define	   STATUS_PWD_ERROR			 0X00001008

#endif