#include "StdAfx.h"
#include "QueryAndSetConfig.h"
#include "DbgMsg.h"

CQueryAndSetConfig::CQueryAndSetConfig(void)
{
	memset(m_nConfig_IniFile,0,sizeof(m_nConfig_IniFile));
	//GetConfigIniFilePath();
}

CQueryAndSetConfig::~CQueryAndSetConfig(void)
{
}

KSTATUS CQueryAndSetConfig::TH_DB_GetUsInfo( GET_US_INFO* pGetUsInfo )
{
	KSTATUS status = STATUS_SUCCESS;
	pGetUsInfo->nUsCounts = 0;


	CIniParser IniObj;
	IniObj.Initialize(m_nConfig_IniFile);

	READ_PAR read_par;
	memset(&read_par,0,sizeof(read_par));


	// 读取US总条数
	read_par.bIsString = false;
	read_par.bIsfloat  = false;
	strcpy(read_par.sAppName,CONFIG_INI_US);
	strcpy(read_par.sKeyName,CONFIG_INI_US_COUNTS);

	status = IniObj.Read(&read_par);
	if(status != STATUS_SUCCESS)
	{
		status = STATUS_INVALID_PARAMETERS;
		return status;
	}
	if(read_par.nValue == 0)
	{
		status = STATUS_INVALID_PARAMETERS;
		return status;
	}

	else
	{
		pGetUsInfo->nUsCounts = read_par.nValue;	
			
		DBG_MSG("pGetUsInfo->nUsCounts = %d\n",pGetUsInfo->nUsCounts);		

		int nIndex = 1;
		for(unsigned int i = 0 ; i < pGetUsInfo->nUsCounts ; i ++)
		{
			char szBuf[MAX_PATH]={0};
			memset(&read_par,0,sizeof(read_par));
			read_par.bIsString = false;
			read_par.bIsfloat  = false;

			// INDEX
			sprintf(szBuf,"us_particle%d",nIndex);
			strcpy(read_par.sAppName,szBuf);
			strcpy(read_par.sKeyName,CONFIG_INI_INDEX);

			IniObj.Read(&read_par);
			pGetUsInfo->par[i].nIndex = read_par.nValue;


			// male_normal_MAX
			memset(&read_par,0,sizeof(read_par));
			read_par.bIsString = false;
			read_par.bIsfloat  = false;
			strcpy(read_par.sAppName,szBuf);
			strcpy(read_par.sKeyName,CONFIG_INI_MAX_NORMAL_MALE);

			IniObj.Read(&read_par);
			pGetUsInfo->par[i].nMax_normal_male = read_par.nValue;

			// male_normal_MIN
			memset(&read_par,0,sizeof(read_par));
			read_par.bIsString = false;
			read_par.bIsfloat  = false;
			strcpy(read_par.sAppName,szBuf);
			strcpy(read_par.sKeyName,CONFIG_INI_MIN_NORMAL_MALE);

			IniObj.Read(&read_par);
			pGetUsInfo->par[i].nMin_normal_male = read_par.nValue;

			// nMax_normal_female
			memset(&read_par,0,sizeof(read_par));
			read_par.bIsString = false;
			read_par.bIsfloat  = false;
			strcpy(read_par.sAppName,szBuf);
			strcpy(read_par.sKeyName,CONFIG_INI_MAX_NORMAL_FEMALE);

			IniObj.Read(&read_par);
			pGetUsInfo->par[i].nMax_normal_female = read_par.nValue;

			// nMin_normal_female
			memset(&read_par,0,sizeof(read_par));
			read_par.bIsString = false;
			read_par.bIsfloat  = false;
			strcpy(read_par.sAppName,szBuf);
			strcpy(read_par.sKeyName,CONFIG_INI_MIN_NORMAL_FEMALE);

			IniObj.Read(&read_par);
			pGetUsInfo->par[i].nMin_normal_female = read_par.nValue;

			// nMax_unusual_male
			memset(&read_par,0,sizeof(read_par));
			read_par.bIsString = false;
			read_par.bIsfloat  = false;
			strcpy(read_par.sAppName,szBuf);
			strcpy(read_par.sKeyName,CONFIG_INI_MAX_UNUSUAL_MALE);

			IniObj.Read(&read_par);
			pGetUsInfo->par[i].nMax_unusual_male = read_par.nValue;

			//nMin_unusual_male
			memset(&read_par,0,sizeof(read_par));
			read_par.bIsString = false;
			read_par.bIsfloat  = false;
			strcpy(read_par.sAppName,szBuf);
			strcpy(read_par.sKeyName,CONFIG_INI_MIN_UNUSUAL_MALE);

			IniObj.Read(&read_par);
			pGetUsInfo->par[i].nMin_unusual_male = read_par.nValue;

			//nMax_unusual_female
			memset(&read_par,0,sizeof(read_par));
			read_par.bIsString = false;
			read_par.bIsfloat  = false;
			strcpy(read_par.sAppName,szBuf);
			strcpy(read_par.sKeyName,CONFIG_INI_MAX_UNUSUAL_FEMALE);

			IniObj.Read(&read_par);
			pGetUsInfo->par[i].nMax_unusual_female = read_par.nValue;

			//nMin_unusual_female
			memset(&read_par,0,sizeof(read_par));
			read_par.bIsString = false;
			read_par.bIsfloat  = false;
			strcpy(read_par.sAppName,szBuf);
			strcpy(read_par.sKeyName,CONFIG_INI_MIN_UNUSUAL_FEMALE);

			IniObj.Read(&read_par);
			pGetUsInfo->par[i].nMin_unusual_female = read_par.nValue;

			//bIsSendtoLIS
			memset(&read_par,0,sizeof(read_par));
			read_par.bIsString = false;
			read_par.bIsfloat  = false;
			strcpy(read_par.sAppName,szBuf);
			strcpy(read_par.sKeyName,CONFIG_INI_ISTOLIS);

			IniObj.Read(&read_par);

			if( read_par.nValue >=1)
			{
				pGetUsInfo->par[i].bIsSendtoLIS =true;
			}
			else
			{
				pGetUsInfo->par[i].bIsSendtoLIS =false;
			}

			//bIsShowUI
			memset(&read_par,0,sizeof(read_par));
			read_par.bIsString = false;
			read_par.bIsfloat  = false;
			strcpy(read_par.sAppName,szBuf);
			strcpy(read_par.sKeyName,CONFIG_INI_ISSHOWUI);

			IniObj.Read(&read_par);

			if(read_par.nValue >= 1)
			{
				pGetUsInfo->par[i].bIsShowUI = true;
			}
			else
			{
				pGetUsInfo->par[i].bIsShowUI = false;
			}

			// bIsSendtoReport
			memset(&read_par,0,sizeof(read_par));
			read_par.bIsString = false;
			read_par.bIsfloat  = false;
			strcpy(read_par.sAppName,szBuf);
			strcpy(read_par.sKeyName,CONFIG_INI_ISSENDTOREPORT);

			IniObj.Read(&read_par);

			if(read_par.nValue >= 1)
			{
				pGetUsInfo->par[i].bIsSendtoReport = true;
			}
			else
			{
				pGetUsInfo->par[i].bIsSendtoReport = false;
			}


			// bIsDelete
			memset(&read_par,0,sizeof(read_par));
			read_par.bIsString = false;
			read_par.bIsfloat  = false;
			strcpy(read_par.sAppName,szBuf);
			strcpy(read_par.sKeyName,CONFIG_INI_ISDELETE);

			IniObj.Read(&read_par);

			if(read_par.nValue >= 1)
			{
				pGetUsInfo->par[i].bIsDelete = true;
			}
			else
			{
				pGetUsInfo->par[i].bIsDelete = false;
			}


			// fArgument
			memset(&read_par,0,sizeof(read_par));
			read_par.bIsString = false;
			read_par.bIsfloat  = true;
			strcpy(read_par.sAppName,szBuf);
			strcpy(read_par.sKeyName,CONFIG_INI_ARGUMENT);

			IniObj.Read(&read_par);
			pGetUsInfo->par[i].fArgument = read_par.fValue;

			// nUnit
			memset(&read_par,0,sizeof(read_par));
			read_par.bIsString = false;
			read_par.bIsfloat  = false;
			strcpy(read_par.sAppName,szBuf);
			strcpy(read_par.sKeyName,CONFIG_INI_UNIT);

			IniObj.Read(&read_par);
			pGetUsInfo->par[i].nUnit = read_par.nValue;

			// sShortName
			memset(&read_par,0,sizeof(read_par));
			read_par.bIsString = true;
			read_par.bIsfloat  = false;
			strcpy(read_par.sAppName,szBuf);
			strcpy(read_par.sKeyName,CONFIG_INI_SHORTNAME);

			IniObj.Read(&read_par);
			strcpy(pGetUsInfo->par[i].sShortName, read_par.sString);

			// sLongName
			memset(&read_par,0,sizeof(read_par));
			read_par.bIsString = true;
			read_par.bIsfloat  = false;
			strcpy(read_par.sAppName,szBuf);
			strcpy(read_par.sKeyName,CONFIG_INI_LONGNAME);

			IniObj.Read(&read_par);
			strcpy(pGetUsInfo->par[i].sLongName, read_par.sString);

			//m_CellNameColoer_R
			memset(&read_par,0,sizeof(read_par));
			read_par.bIsString = true;
			read_par.bIsfloat  = false;
			strcpy(read_par.sAppName,szBuf);
			strcpy(read_par.sKeyName,CONFIG_INI_CELLNAMECOLOR_R);

			IniObj.Read(&read_par);
			strcpy(pGetUsInfo->par[i].m_CellNameColoer_R, read_par.sString);
			/*pGetUsInfo->par[i].m_CellNameColoer = read_par.fValue;*/
			//DBG_MSG("TH_DB_Access::pGetUsInfo->par[%d].m_CellNameColoer_R = %s\n",i,pGetUsInfo->par[i].m_CellNameColoer_R);


			//m_CellNameColoer_G
			memset(&read_par,0,sizeof(read_par));
			read_par.bIsString = true;
			read_par.bIsfloat  = false;
			strcpy(read_par.sAppName,szBuf);
			strcpy(read_par.sKeyName,CONFIG_INI_CELLNAMECOLOR_G);

			IniObj.Read(&read_par);
			strcpy(pGetUsInfo->par[i].m_CellNameColoer_G, read_par.sString);
			/*pGetUsInfo->par[i].m_CellNameColoer = read_par.fValue;*/
			//DBG_MSG("TH_DB_Access::pGetUsInfo->par[%d].m_CellNameColoer_G = %s\n",i,pGetUsInfo->par[i].m_CellNameColoer_G);

			//m_CellNameColoer_B
			memset(&read_par,0,sizeof(read_par));
			read_par.bIsString = true;
			read_par.bIsfloat  = false;
			strcpy(read_par.sAppName,szBuf);
			strcpy(read_par.sKeyName,CONFIG_INI_CELLNAMECOLOR_B);

			IniObj.Read(&read_par);
			strcpy(pGetUsInfo->par[i].m_CellNameColoer_B, read_par.sString);
			/*pGetUsInfo->par[i].m_CellNameColoer = read_par.fValue;*/
			//DBG_MSG("TH_DB_Access::pGetUsInfo->par[%d].m_CellNameColoer_B = %s\n",i,pGetUsInfo->par[i].m_CellNameColoer_B);

            //m_reliability 
            memset(&read_par, 0, sizeof(read_par));
            read_par.bIsString = false;
            read_par.bIsfloat = false;
            strcpy(read_par.sAppName, szBuf);
            strcpy(read_par.sKeyName, CONFIG_INI_RELIABILITY);

            IniObj.Read(&read_par);
            pGetUsInfo->par[i].m_reliability = read_par.nValue;
            if (pGetUsInfo->par[i].m_reliability < 0)
            {
                pGetUsInfo->par[i].m_reliability = 0;
            }
            else if (pGetUsInfo->par[i].m_reliability > 100)
            {
                pGetUsInfo->par[i].m_reliability = 100;
            }

			nIndex ++;
		}// end for
	}

	//showframe
	memset(&read_par,0,sizeof(read_par));
	read_par.bIsString = false;
	read_par.bIsfloat  = false;	
	strcpy(read_par.sAppName,CONFIG_INI_US);
	strcpy(read_par.sKeyName,CONFIG_INI_SHOWCELLFRAME);
	IniObj.Read(&read_par);
	if(read_par.nValue >= 1)
		pGetUsInfo->bIsShowFrame = true;
	else
		pGetUsInfo->bIsShowFrame = false;
	//DBG_MSG("TH_DB_Access::pGetUsInfo->bIsShowFrame = %d\n",pGetUsInfo->bIsShowFrame);


	return status;


}

KSTATUS CQueryAndSetConfig::TH_DB_SetUsInfo( SET_US_INFO set_us_info )
{
	KSTATUS status = STATUS_SUCCESS;

	if(set_us_info.nUsCounts <= 0)
	{
		status = STATUS_INVALID_PARAMETERS;
		return status;
	}

	CIniParser IniObj;
	IniObj.Initialize(m_nConfig_IniFile);

	READ_PAR read_par;
	memset(&read_par,0,sizeof(read_par));

	// 读取US总条数
	read_par.bIsString = false;
	strcpy(read_par.sAppName,CONFIG_INI_US);
	strcpy(read_par.sKeyName,CONFIG_INI_US_COUNTS);

	IniObj.Read(&read_par);

	int nIndex = 1;
	for(int i = 0 ; i < read_par.nValue ; i ++)
	{
		char szBuf[MAX_PATH]={0};
		sprintf(szBuf,"us_particle%d",nIndex);
		nIndex++;

		// 清空
		IniObj.EmptyByAppName(szBuf);
	}

	// 写入counts新项
	WRITE_PAR write_par;
	strcpy(write_par.sAppName,CONFIG_INI_US);
	strcpy(write_par.sKeyName,CONFIG_INI_US_COUNTS);
	sprintf(write_par.sString,"%d",set_us_info.nUsCounts);

	IniObj.Write(write_par);


	//bIsShowFrame
	strcpy(write_par.sAppName,CONFIG_INI_US);
	strcpy(write_par.sKeyName,CONFIG_INI_SHOWCELLFRAME);
	sprintf(write_par.sString,"%d",set_us_info.bIsShowFrame);
	//DBG_MSG("TH_DB_Access::write_par.sString.bIsShowFrame = %s\n",write_par.sString);
	IniObj.Write(write_par);

	nIndex = 1;
	for(unsigned int i = 0 ; i < set_us_info.nUsCounts ; i ++)
	{
		memset(&write_par,0,sizeof(WRITE_PAR));
		char szBuf[MAX_PATH]={0};

		// 序号
		sprintf(szBuf,"us_particle%d",nIndex);
		strcpy(write_par.sAppName,szBuf);
		strcpy(write_par.sKeyName,CONFIG_INI_INDEX);
		sprintf(write_par.sString,"%d",set_us_info.par[i].nIndex);
		IniObj.Write(write_par);

		// MAX_NORMAL_MALE
		strcpy(write_par.sKeyName,CONFIG_INI_MAX_NORMAL_MALE);
		sprintf(write_par.sString,"%d",set_us_info.par[i].nMax_normal_male);
		IniObj.Write(write_par);

		// CONFIG_INI_MIN_NORMAL_MALE
		strcpy(write_par.sKeyName,CONFIG_INI_MIN_NORMAL_MALE);
		sprintf(write_par.sString,"%d",set_us_info.par[i].nMin_normal_male);
		IniObj.Write(write_par);

		// CONFIG_INI_MAX_NORMAL_FEMALE
		strcpy(write_par.sKeyName,CONFIG_INI_MAX_NORMAL_FEMALE);
		sprintf(write_par.sString,"%d",set_us_info.par[i].nMax_normal_female);
		IniObj.Write(write_par);

		// CONFIG_INI_MIN_NORMAL_FEMALE
		strcpy(write_par.sKeyName,CONFIG_INI_MIN_NORMAL_FEMALE);
		sprintf(write_par.sString,"%d",set_us_info.par[i].nMin_normal_female);
		IniObj.Write(write_par);

		// CONFIG_INI_MAX_UNUSUAL_MALE
		strcpy(write_par.sKeyName,CONFIG_INI_MAX_UNUSUAL_MALE);
		sprintf(write_par.sString,"%d",set_us_info.par[i].nMax_unusual_male);
		IniObj.Write(write_par);

		// CONFIG_INI_MIN_UNUSUAL_MALE
		strcpy(write_par.sKeyName,CONFIG_INI_MIN_UNUSUAL_MALE);
		sprintf(write_par.sString,"%d",set_us_info.par[i].nMin_unusual_male);
		IniObj.Write(write_par);

		// CONFIG_INI_MAX_UNUSUAL_FEMALE
		strcpy(write_par.sKeyName,CONFIG_INI_MAX_UNUSUAL_FEMALE);
		sprintf(write_par.sString,"%d",set_us_info.par[i].nMax_unusual_female);
		IniObj.Write(write_par);

		//CONFIG_INI_MIN_UNUSUAL_FEMALE
		strcpy(write_par.sKeyName,CONFIG_INI_MIN_UNUSUAL_FEMALE);
		sprintf(write_par.sString,"%d",set_us_info.par[i].nMin_unusual_female);
		IniObj.Write(write_par);

		// CONFIG_INI_ISTOLIS
		strcpy(write_par.sKeyName,CONFIG_INI_ISTOLIS);
		sprintf(write_par.sString,"%d",set_us_info.par[i].bIsSendtoLIS);
		IniObj.Write(write_par);

		// CONFIG_INI_ISSHOWUI
		strcpy(write_par.sKeyName,CONFIG_INI_ISSHOWUI);
		sprintf(write_par.sString,"%d",set_us_info.par[i].bIsShowUI);
		IniObj.Write(write_par);

		// CONFIG_INI_ISSENDTOREPORT
		strcpy(write_par.sKeyName,CONFIG_INI_ISSENDTOREPORT);
		sprintf(write_par.sString,"%d",set_us_info.par[i].bIsSendtoReport);
		IniObj.Write(write_par);

		// 写DELETE
		strcpy(write_par.sKeyName,CONFIG_INI_ISDELETE);
		sprintf(write_par.sString,"%d",set_us_info.par[i].bIsDelete);
		IniObj.Write(write_par);

		// CONFIG_INI_ARGUMENT
		strcpy(write_par.sKeyName,CONFIG_INI_ARGUMENT);
		sprintf(write_par.sString,"%f",set_us_info.par[i].fArgument);
		IniObj.Write(write_par);

		// CONFIG_INI_SHORTNAME
		strcpy(write_par.sKeyName,CONFIG_INI_SHORTNAME);
		strcpy(write_par.sString,set_us_info.par[i].sShortName);
		IniObj.Write(write_par);

		// CONFIG_INI_LONGNAME
		strcpy(write_par.sKeyName,CONFIG_INI_LONGNAME);
		strcpy(write_par.sString,set_us_info.par[i].sLongName);
		IniObj.Write(write_par);

		// CONFIG_INI_UNIT
		strcpy(write_par.sKeyName,CONFIG_INI_UNIT);
		sprintf(write_par.sString,"%d",set_us_info.par[i].nUnit);
		IniObj.Write(write_par);

		// CONFIG_INI_CELLNAMECOLOR_R
		strcpy(write_par.sKeyName,CONFIG_INI_CELLNAMECOLOR_R);
		sprintf(write_par.sString,"%s",set_us_info.par[i].m_CellNameColoer_R);
		//DBG_MSG("TH_DB_Access::%d.write_par.sString.m_CellNameColoer_R = %s\n",nIndex,write_par.sString);
		IniObj.Write(write_par);

		// CONFIG_INI_CELLNAMECOLOR_G
		strcpy(write_par.sKeyName,CONFIG_INI_CELLNAMECOLOR_G);
		sprintf(write_par.sString,"%s",set_us_info.par[i].m_CellNameColoer_G);
		//DBG_MSG("TH_DB_Access::%d.write_par.sString.m_CellNameColoer_G = %s\n",nIndex,write_par.sString);
		IniObj.Write(write_par);

		// CONFIG_INI_CELLNAMECOLOR_B
		strcpy(write_par.sKeyName,CONFIG_INI_CELLNAMECOLOR_B);
		sprintf(write_par.sString,"%s",set_us_info.par[i].m_CellNameColoer_B);
		//DBG_MSG("TH_DB_Access::%d.write_par.sString.m_CellNameColoer_B = %s\n",nIndex,write_par.sString);
		IniObj.Write(write_par);

        //CONFIG_INI_RELIABILITY 
        strcpy(write_par.sKeyName, CONFIG_INI_RELIABILITY);
        sprintf(write_par.sString, "%d", set_us_info.par[i].m_reliability);
        IniObj.Write(write_par);

		nIndex ++;
	} // end for

    strcpy(write_par.sAppName, "us_particle_changed");
    strcpy(write_par.sKeyName, "0");
    strcpy(write_par.sString, "1");
    IniObj.Write(write_par);


	return status;

}

KSTATUS CQueryAndSetConfig::TH_DB_GetUdcInfo( GET_UDC_INFO* pGetUdcInfo )
{
	KSTATUS status = STATUS_SUCCESS;

	if(pGetUdcInfo == NULL)
	{
		status = STATUS_INVALID_PARAMETERS;
		return status;
	}

	pGetUdcInfo->nUdcCounts = 0;

	CIniParser IniObj;
	IniObj.Initialize(m_nConfig_IniFile);

	READ_PAR read_par;
	memset(&read_par,0,sizeof(read_par));


	// 读取US总条数
	read_par.bIsString = false;
	read_par.bIsfloat  = false;
	strcpy(read_par.sAppName,CONFIG_INI_UDC);
	strcpy(read_par.sKeyName,CONFIG_INI_UDC_COUNTS);

	status = IniObj.Read(&read_par);
	if(status != STATUS_SUCCESS)
	{
		pGetUdcInfo->nUdcCounts = 0;
		status = STATUS_INVALID_PARAMETERS;
		return status;
	}

	if(read_par.nValue == 0)
	{
		pGetUdcInfo->nUdcCounts = 0;
		status = STATUS_INVALID_PARAMETERS;
		return status;
	}
	else
	{
		pGetUdcInfo->nUdcCounts = read_par.nValue;

		int nIndex = 1;
		for(unsigned int i = 0 ; i < pGetUdcInfo->nUdcCounts ; i ++)
		{
			char szBuf[MAX_PATH]={0};
			memset(&read_par,0,sizeof(read_par));
			read_par.bIsString = false;
			read_par.bIsfloat  = false;

			// INDEX
			sprintf(szBuf,"udc_particle%d",nIndex);
			strcpy(read_par.sAppName,szBuf);
			strcpy(read_par.sKeyName,CONFIG_INI_INDEX);

			IniObj.Read(&read_par);
			pGetUdcInfo->par[i].nIndex = read_par.nValue;

			// male_normal_MAX
			memset(&read_par,0,sizeof(read_par));
			read_par.bIsString = false;
			read_par.bIsfloat  = false;
			sprintf(szBuf,"udc_particle%d",nIndex);
			strcpy(read_par.sAppName,szBuf);
			strcpy(read_par.sKeyName,CONFIG_INI_MAX_NORMAL_MALE);

			IniObj.Read(&read_par);
			pGetUdcInfo->par[i].nMax_normal_male = read_par.nValue;

			// male_normal_MIN
			memset(&read_par,0,sizeof(read_par));
			read_par.bIsString = false;
			read_par.bIsfloat  = false;
			sprintf(szBuf,"udc_particle%d",nIndex);
			strcpy(read_par.sAppName,szBuf);
			strcpy(read_par.sKeyName,CONFIG_INI_MIN_NORMAL_MALE);

			IniObj.Read(&read_par);
			pGetUdcInfo->par[i].nMin_normal_male = read_par.nValue;

			// nMax_normal_female
			memset(&read_par,0,sizeof(read_par));
			read_par.bIsString = false;
			read_par.bIsfloat  = false;
			sprintf(szBuf,"udc_particle%d",nIndex);
			strcpy(read_par.sAppName,szBuf);
			strcpy(read_par.sKeyName,CONFIG_INI_MAX_NORMAL_FEMALE);

			IniObj.Read(&read_par);
			pGetUdcInfo->par[i].nMax_normal_female = read_par.nValue;

			// nMin_normal_female
			memset(&read_par,0,sizeof(read_par));
			read_par.bIsString = false;
			read_par.bIsfloat  = false;
			sprintf(szBuf,"udc_particle%d",nIndex);
			strcpy(read_par.sAppName,szBuf);
			strcpy(read_par.sKeyName,CONFIG_INI_MIN_NORMAL_FEMALE);

			IniObj.Read(&read_par);
			pGetUdcInfo->par[i].nMin_normal_female = read_par.nValue;

			// nMax_unusual_male
			memset(&read_par,0,sizeof(read_par));
			read_par.bIsString = false;
			read_par.bIsfloat  = false;
			sprintf(szBuf,"udc_particle%d",nIndex);
			strcpy(read_par.sAppName,szBuf);
			strcpy(read_par.sKeyName,CONFIG_INI_MAX_UNUSUAL_MALE);

			IniObj.Read(&read_par);
			pGetUdcInfo->par[i].nMax_unusual_male = read_par.nValue;

			//nMin_unusual_male
			memset(&read_par,0,sizeof(read_par));
			read_par.bIsString = false;
			read_par.bIsfloat  = false;
			sprintf(szBuf,"udc_particle%d",nIndex);
			strcpy(read_par.sAppName,szBuf);
			strcpy(read_par.sKeyName,CONFIG_INI_MIN_UNUSUAL_MALE);

			IniObj.Read(&read_par);
			pGetUdcInfo->par[i].nMin_unusual_male = read_par.nValue;

			//nMax_unusual_female
			memset(&read_par,0,sizeof(read_par));
			read_par.bIsString = false;
			read_par.bIsfloat  = false;
			sprintf(szBuf,"udc_particle%d",nIndex);
			strcpy(read_par.sAppName,szBuf);
			strcpy(read_par.sKeyName,CONFIG_INI_MAX_UNUSUAL_FEMALE);

			IniObj.Read(&read_par);
			pGetUdcInfo->par[i].nMax_unusual_female = read_par.nValue;

			//nMin_unusual_female
			memset(&read_par,0,sizeof(read_par));
			read_par.bIsString = false;
			read_par.bIsfloat  = false;
			sprintf(szBuf,"udc_particle%d",nIndex);
			strcpy(read_par.sAppName,szBuf);
			strcpy(read_par.sKeyName,CONFIG_INI_MIN_UNUSUAL_FEMALE);

			IniObj.Read(&read_par);
			pGetUdcInfo->par[i].nMin_unusual_female = read_par.nValue;

			//bIsSendtoLIS
			memset(&read_par,0,sizeof(read_par));
			read_par.bIsString = false;
			read_par.bIsfloat  = false;
			sprintf(szBuf,"udc_particle%d",nIndex);
			strcpy(read_par.sAppName,szBuf);
			strcpy(read_par.sKeyName,CONFIG_INI_ISTOLIS);

			IniObj.Read(&read_par);

			if( read_par.nValue >=1)
			{
				pGetUdcInfo->par[i].bIsSendtoLIS =true;
			}
			else
			{
				pGetUdcInfo->par[i].bIsSendtoLIS =false;
			}

			//bIsShowUI
			memset(&read_par,0,sizeof(read_par));
			read_par.bIsString = false;
			read_par.bIsfloat  = false;
			sprintf(szBuf,"udc_particle%d",nIndex);
			strcpy(read_par.sAppName,szBuf);
			strcpy(read_par.sKeyName,CONFIG_INI_ISSHOWUI);

			IniObj.Read(&read_par);

			if(read_par.nValue >= 1)
			{
				pGetUdcInfo->par[i].bIsShowUI = true;
			}
			else
			{
				pGetUdcInfo->par[i].bIsShowUI = false;
			}

			// bIsSendtoReport
			memset(&read_par,0,sizeof(read_par));
			read_par.bIsString = false;
			read_par.bIsfloat  = false;
			sprintf(szBuf,"udc_particle%d",nIndex);
			strcpy(read_par.sAppName,szBuf);
			strcpy(read_par.sKeyName,CONFIG_INI_ISSENDTOREPORT);

			IniObj.Read(&read_par);

			if(read_par.nValue >= 1)
			{
				pGetUdcInfo->par[i].bIsSendtoReport = true;
			}
			else
			{
				pGetUdcInfo->par[i].bIsSendtoReport = false;
			}

			// delete
			memset(&read_par,0,sizeof(read_par));
			read_par.bIsString = false;
			read_par.bIsfloat = false;

			sprintf(szBuf,"udc_particle%d",nIndex);
			strcpy(read_par.sAppName,szBuf);
			strcpy(read_par.sKeyName,CONFIG_INI_ISDELETE);

			IniObj.Read(&read_par);

			if(read_par.nValue >= 1)
			{
				pGetUdcInfo->par[i].bIsDelete = true;
			}
			else
			{
				pGetUdcInfo->par[i].bIsDelete = false;
			}


			// fArgument
			memset(&read_par,0,sizeof(read_par));
			read_par.bIsString = false;
			read_par.bIsfloat  = true;
			sprintf(szBuf,"udc_particle%d",nIndex);
			strcpy(read_par.sAppName,szBuf);
			strcpy(read_par.sKeyName,CONFIG_INI_ARGUMENT);

			IniObj.Read(&read_par);
			pGetUdcInfo->par[i].fArgument = read_par.fValue;

			// nUnit
			memset(&read_par,0,sizeof(read_par));
			read_par.bIsString = false;
			read_par.bIsfloat  = false;
			sprintf(szBuf,"udc_particle%d",nIndex);
			strcpy(read_par.sAppName,szBuf);
			strcpy(read_par.sKeyName,CONFIG_INI_UNIT);

			IniObj.Read(&read_par);
			pGetUdcInfo->par[i].nUnit = read_par.nValue;

			// sShortName
			memset(&read_par,0,sizeof(read_par));
			read_par.bIsString = true;
			read_par.bIsfloat  = false;
			sprintf(szBuf,"udc_particle%d",nIndex);
			strcpy(read_par.sAppName,szBuf);
			strcpy(read_par.sKeyName,CONFIG_INI_SHORTNAME);

			IniObj.Read(&read_par);
			strcpy(pGetUdcInfo->par[i].sShortName, read_par.sString);

			// sLongName
			memset(&read_par,0,sizeof(read_par));
			read_par.bIsString = true;
			read_par.bIsfloat  = false;
			sprintf(szBuf,"udc_particle%d",nIndex);
			strcpy(read_par.sAppName,szBuf);
			strcpy(read_par.sKeyName,CONFIG_INI_LONGNAME);

			IniObj.Read(&read_par);
			strcpy(pGetUdcInfo->par[i].sLongName, read_par.sString);


			nIndex ++;
		}// end for
	}


	return status;

}

KSTATUS CQueryAndSetConfig::TH_DB_SetUdcInfo( SET_UDC_INFO set_udc_info )
{
	KSTATUS status = STATUS_SUCCESS;

	if(set_udc_info.nUdcCounts <= 0)
	{
		status = STATUS_INVALID_PARAMETERS;
		return status;
	}

	CIniParser IniObj;
	IniObj.Initialize(m_nConfig_IniFile);

	READ_PAR read_par;
	memset(&read_par,0,sizeof(read_par));

	// 读取UDC总条数
	read_par.bIsString = false;
	strcpy(read_par.sAppName,CONFIG_INI_UDC);
	strcpy(read_par.sKeyName,CONFIG_INI_UDC_COUNTS);

	IniObj.Read(&read_par);

	int Index = 1;
	for(int i = 0 ; i < read_par.nValue ; i ++)
	{
		char szBuf[MAX_PATH]={0};
		sprintf(szBuf,"udc_particle%d",Index);
		Index++;

		// 清空
		IniObj.EmptyByAppName(szBuf);
	}

	// 写入counts新项
	WRITE_PAR write_par;
	strcpy(write_par.sAppName,CONFIG_INI_UDC);
	strcpy(write_par.sKeyName,CONFIG_INI_UDC_COUNTS);
	sprintf(write_par.sString,"%d",set_udc_info.nUdcCounts);

	IniObj.Write(write_par);

	int nIndex = 1;
	for(unsigned int i = 0 ; i < set_udc_info.nUdcCounts ; i ++)
	{
		memset(&write_par,0,sizeof(WRITE_PAR));
		char szBuf[MAX_PATH]={0};

		// 序号
		sprintf(szBuf,"udc_particle%d",nIndex);
		strcpy(write_par.sAppName,szBuf);
		strcpy(write_par.sKeyName,CONFIG_INI_INDEX);
		sprintf(write_par.sString,"%d",set_udc_info.par[i].nIndex);
		IniObj.Write(write_par);

		// MAX_NORMAL_MALE
		strcpy(write_par.sKeyName,CONFIG_INI_MAX_NORMAL_MALE);
		sprintf(write_par.sString,"%d",set_udc_info.par[i].nMax_normal_male);
		IniObj.Write(write_par);

		// CONFIG_INI_MIN_NORMAL_MALE
		strcpy(write_par.sKeyName,CONFIG_INI_MIN_NORMAL_MALE);
		sprintf(write_par.sString,"%d",set_udc_info.par[i].nMin_normal_male);
		IniObj.Write(write_par);

		// CONFIG_INI_MAX_NORMAL_FEMALE
		strcpy(write_par.sKeyName,CONFIG_INI_MAX_NORMAL_FEMALE);
		sprintf(write_par.sString,"%d",set_udc_info.par[i].nMax_normal_female);
		IniObj.Write(write_par);

		// CONFIG_INI_MIN_NORMAL_FEMALE
		strcpy(write_par.sKeyName,CONFIG_INI_MIN_NORMAL_FEMALE);
		sprintf(write_par.sString,"%d",set_udc_info.par[i].nMin_normal_female);
		IniObj.Write(write_par);

		// CONFIG_INI_MAX_UNUSUAL_MALE
		strcpy(write_par.sKeyName,CONFIG_INI_MAX_UNUSUAL_MALE);
		sprintf(write_par.sString,"%d",set_udc_info.par[i].nMax_unusual_male);
		IniObj.Write(write_par);

		// CONFIG_INI_MIN_UNUSUAL_MALE
		strcpy(write_par.sKeyName,CONFIG_INI_MIN_UNUSUAL_MALE);
		sprintf(write_par.sString,"%d",set_udc_info.par[i].nMin_unusual_male);
		IniObj.Write(write_par);

		// CONFIG_INI_MAX_UNUSUAL_FEMALE
		strcpy(write_par.sKeyName,CONFIG_INI_MAX_UNUSUAL_FEMALE);
		sprintf(write_par.sString,"%d",set_udc_info.par[i].nMax_unusual_female);
		IniObj.Write(write_par);

		//CONFIG_INI_MIN_UNUSUAL_FEMALE
		strcpy(write_par.sKeyName,CONFIG_INI_MIN_UNUSUAL_FEMALE);
		sprintf(write_par.sString,"%d",set_udc_info.par[i].nMin_unusual_female);
		IniObj.Write(write_par);

		// CONFIG_INI_ISTOLIS
		strcpy(write_par.sKeyName,CONFIG_INI_ISTOLIS);
		sprintf(write_par.sString,"%d",set_udc_info.par[i].bIsSendtoLIS);
		IniObj.Write(write_par);

		// CONFIG_INI_ISSHOWUI
		strcpy(write_par.sKeyName,CONFIG_INI_ISSHOWUI);
		sprintf(write_par.sString,"%d",set_udc_info.par[i].bIsShowUI);
		IniObj.Write(write_par);

		// CONFIG_INI_ISSENDTOREPORT
		strcpy(write_par.sKeyName,CONFIG_INI_ISSENDTOREPORT);
		sprintf(write_par.sString,"%d",set_udc_info.par[i].bIsSendtoReport);
		IniObj.Write(write_par);

		// 写DELETE
		strcpy(write_par.sKeyName,CONFIG_INI_ISDELETE);
		sprintf(write_par.sString,"%d",set_udc_info.par[i].bIsDelete);
		IniObj.Write(write_par);

		// CONFIG_INI_ARGUMENT
		strcpy(write_par.sKeyName,CONFIG_INI_ARGUMENT);
		sprintf(write_par.sString,"%f",set_udc_info.par[i].fArgument);
		IniObj.Write(write_par);

		// CONFIG_INI_SHORTNAME
		strcpy(write_par.sKeyName,CONFIG_INI_SHORTNAME);
		strcpy(write_par.sString,set_udc_info.par[i].sShortName);
		IniObj.Write(write_par);

		// CONFIG_INI_LONGNAME
		strcpy(write_par.sKeyName,CONFIG_INI_LONGNAME);
		strcpy(write_par.sString,set_udc_info.par[i].sLongName);
		IniObj.Write(write_par);

		// CONFIG_INI_UNIT
		strcpy(write_par.sKeyName,CONFIG_INI_UNIT);
		sprintf(write_par.sString,"%d",set_udc_info.par[i].nUnit);
		IniObj.Write(write_par);


		nIndex++;
	} // end for


	return status;


}

BOOL CQueryAndSetConfig::GetConfigIniFilePath()
{
	GetModuleFileName(NULL, m_nConfig_IniFile, MAX_PATH);
	PathRemoveFileSpec(m_nConfig_IniFile);
	PathAppend(m_nConfig_IniFile,CONFIG_INI_FILENAME);
	return TRUE;
}

void CQueryAndSetConfig::SetResIniPath( PCHAR path )
{
	if(path != NULL)
		strcpy(m_nConfig_IniFile, path);
	else
	{
		GetModuleFileName(NULL, m_nConfig_IniFile, MAX_PATH);
		PathRemoveFileSpec(m_nConfig_IniFile);
		PathAppend(m_nConfig_IniFile,CONFIG_INI_FILENAME);	
	}


}