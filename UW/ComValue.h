#pragma once

#define VIDEO_CY_VIEW_TAB 0
#define THUM_VIEW_TAB  1
#define IMAGE_VIEW_TAB 2
#define CELL_VIEW_TAB  3
#define UDC_VIEW_TAB 4

#define BYST_TYPE  6
#define IMP_TYPE   7
#define SPERM_TYPE 8
#define BYST_TYPE_EX  503
#define SPERM_TYPE_EX 506

#define DEFAULT_IMAGELIST_COUNT    10
#define INDEX_BUF_LEN              16
#define CELLTYPE_BUF_LEN           32
#define NAME_BUF_LEN               32
#define KEY_BUF_LEN                16
#define SECTION_BUF_LEN            32
#define LIST_ITEM_LEN              90

#define PIC_10X_NUM     10
#define PIC_40X_NUM_PER 16

#define CLASS_INI_NAME             _T("Config\\Class.ini")
#define CLASS_SECTION_NAME         _T("ClassConfig")
#define CONFIGDATA_INI_NAME        _T("Config\\ConfigData.ini")
#define HOSPITAL_SECTION_NAME      _T("Hospital")
#define DEPARTMENT_SECTION_NAME    _T("Department")
#define COLOR_SECTION_NAME         _T("Color")
#define TRANSPARENCY_SECTION_NAME  _T("Transparency")
#define UDC_CFG_PATH               _T("Config\\Udc.xml")
#define UW_EQUIPMENT_INI           _T("Config\\uw.ini")
#define US_CRITERION_INI           _T("Config\\UsCriterion.ini")
#define VIDEO_INI_NAME             _T("Config\\video.ini")
#define CONFIG_INI_NAME            _T("Config\\debug.ini")
#define REPORT_INI_NAME			   _T("Config\\Report.ini")
#define INFO_INI_NAME              _T("info.ini")


#define WM_THUMVIEW_CLICK_PIC      WM_USER+100  //thum view 点击图片
#define WM_CELLVIEW_CLICK_PIC      WM_USER+101  //cell view 点击图片
#define WM_CELLVIEW_MODIFY_CELL    WM_USER+102  //cell view 修改细胞类型
#define WM_IMAGEVIEW_INSERT_CELL   WM_USER+103  //image view 添加细胞
#define WM_IMAGEVIEW_MODIFY_CELL   WM_USER+104  //image view 修改细胞
#define WM_IMAGEVIEW_DELETE_CELL   WM_USER+105  //image view 删除细胞
#define WM_IMAGEVIEW_LBTN_DCLICK   WM_USER+106  //image view 鼠标左键双击
#define WM_IMAGEVIEW_IMAGE_NEXT    WM_USER+107  //iamge view 上一张图像
#define WM_IMAGEVIEW_IMAGE_FORWARD WM_USER+108  //image view 下一张图像
#define WM_UPDATE_CELL_NUM         WM_USER+109

#define WM_FINDRECORD_LOOKINFO     WM_USER+120  //查询记录视图 查看详细信息
#define WM_FINDRECORD_DELINFO      WM_USER+121  //查询记录视图 删除数据
#define WM_TODAYLIST_LOOKINFO      WM_USER+122  //当天记录视图 查看详细信息
#define WM_TODAYLIST_DELINFO       WM_USER+123  //当天记录视图 删除数据
#define WM_TODAYLIST_ADDINFO       WM_USER+124  //当天记录视图 添加数据
#define WM_WNDPAT_UPDATEDATA       WM_USER+125  //病人信息窗口 更新数据

#define WM_EQUIPMENT_MSG           WM_USER+130  // 下位机消息
#define WM_UDCTEST_MSG             WM_USER+131  // UDC检测模块消息
#define WM_UDCTEST_DEBUG_MSG       WM_USER+132  

