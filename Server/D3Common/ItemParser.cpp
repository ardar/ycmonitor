#include "StdAfx.h"
#include "ItemParser.h"
#include "GameFieldDef.h"
#include <regex>
using namespace std::tr1;

char CharactorOptions[5][16] = {"巫醫","武僧","狩魔獵人","秘術師","野蠻人"};
char QualityOptions[6][8] = {"次等","普通","高級","魔法","稀有","傳奇"};
char PrimaryOptions[5][16] = {"單手","雙手","副手","防具","追隨者專用"};
char SecondaryOptions[41][16] = {
	"斧","祭祀刀","弩槍","匕首","拳套武器","釘鎚","重型武器","矛","劍","魔杖",//單手
	"雙手斧","弓","武杖","弩","雙手釘鎚","雙手重型武器","長柄武器","法杖","雙手劍",//雙手
	"咒物","法器","箭筒","盾",////副手
	"護身符","腰帶","靴子","護腕","胸甲","斗蓬","手套",//防具
		"頭盔","重型腰帶","戒指","護肩","練氣石","巫毒面具","秘術帽",
	"巫女法器","盜賊道具","騎士聖物",//追隨者專用 
	"寶石",//寶石类别
};
char EquipPosOptions[10][16] = { "頭部","頸部","腰部","腳部","手腕","身軀","手掌","手指","肩膀"};

ItemParser ItemParser::s_instance;

ItemParser& ItemParser::Instance()
{
	return s_instance;
}

ItemParser::ItemParser(void)
{

}

ItemParser::~ItemParser(void)
{

}

BOOL ItemParser::ParseProperties( LPCTSTR szAttrText, D3ItemInfo* pItemInfo )
{
	CString szPropertyText = szAttrText;
	
	CStringArray strList;
	int nRet = BSHelper::SplitStr("\n", szPropertyText, strList, TRUE);
	if (nRet==0)
	{
		return FALSE;
	}
	CString szField, szValue;
	// 第一行，名字
	szValue = strList.GetAt(0);
	if(szValue.Trim().GetLength()==0)
	{
		return FALSE;
	}
	if (szValue.Find("0 枚金幣 (")==0)
	{
		CString szNum = szValue.Mid(szValue.Find('(')+1, szValue.Find(')')-1- szValue.Find('(')-1);
		szValue = szValue.Mid(0, szValue.Find('('));
		szValue.Replace("0", szNum);
	}
	strcpy_s(pItemInfo->szName, sizeof(pItemInfo->szName), szValue.Trim());
	

	//第二行 类型
	CString sz2ndLine = strList.GetAt(1);
	sz2ndLine.Trim();
	int nNext = 0;
	if(sz2ndLine.GetLength()>4)
	{
		szValue = sz2ndLine.Left(4);
		CString szFieldName = getSpecialPropertyName(1, szValue);
		if (szFieldName)
		{
			D3ItemProperty secfield;
			strcpy_s(secfield.szName, sizeof(secfield.szName), szFieldName);
			strcpy_s(secfield.szValue, sizeof(secfield.szValue), szValue);
			pItemInfo->properties.push_back(secfield);
			nNext = 4;
		}
	}
	D3ItemProperty secfield2;
	szValue = sz2ndLine.Mid(nNext);
	CString szFieldName = getSpecialPropertyName(1, szValue);
	strcpy_s(secfield2.szName, sizeof(secfield2.szName), szFieldName);
	strcpy_s(secfield2.szValue, sizeof(secfield2.szValue), szValue);
	pItemInfo->properties.push_back(secfield2);

	CString szEquipType, szEquipPos;
	int nStrListCount = strList.GetCount();
	
	int nRegExCount = m_parserRegexList.GetCount();
	for(int i=2;i<nStrListCount;i++)
	{
		CString sz = strList.GetAt(i);
		sz.Trim();
		if (i==5 || sz.GetLength()==0)
		{
			continue;
		}
		std::string szLine = sz.GetBuffer();
		BOOL bIsMatched = FALSE;
		for(int j=0;j<nRegExCount;j++)
		{
			CString strRegexLine = m_parserRegexList.GetAt(j);
			CStringArray regexParts;
			if (strRegexLine.GetLength()==0 || BSHelper::SplitStr("<D3>", strRegexLine, regexParts, TRUE)==0)
			{
				continue;
			}
			CString szRegEx = regexParts.GetAt(0);
			regex rx(szRegEx);
			std::tr1::cmatch res;
			bIsMatched = TRUE;
			if (regex_search(szLine.c_str(), res, rx)) 
			{
				D3ItemProperty field;
				int regexPartCount = regexParts.GetCount();
				for (int k=0;k<regexPartCount;k++)
				{
					CString szPart = regexParts.GetAt(k);
					if (szPart.GetLength()>4 && strcmp(szPart.Left(5), "Name:")==0)
					{
						if(szPart.Find(':')>=4)
						{
							CString szFieldName = szPart.Mid(szPart.Find(':')+1);
							strcpy_s(field.szName, sizeof(field.szName), szFieldName.Trim());
							bIsMatched = TRUE;
							continue;
						}
					}
					else if(szPart.Find('=')>0)
					{
						CString szFieldName = szPart.Left(szPart.Find('='));
						CString szFieldVal = szPart.Mid(szPart.Find('=')+1);
						bIsMatched = TRUE;
					}
					if (res.size()>k)
					{
						std::string strVal = res[k];
						CString szValue = strVal.c_str();
						szValue.Trim();
						if (strcmp(szPart, "Property")==0)
						{
							LPCTSTR szFieldName = getSpecialPropertyName(i, szValue);
							if (szFieldName)
							{
								strcpy_s(field.szName, sizeof(field.szName), szFieldName);
								strcpy_s(field.szValue, sizeof(field.szValue), szValue);
								if (strcmp(szFieldName, Field_PrimaryType)==0)
								{
									szEquipType = szValue;
								}
								else if (strcmp(szFieldName, Field_EquipPos)==0)
								{
									szEquipPos = szValue;
								}
								bIsMatched = TRUE;
							}
							else
							{
								TRACE("UnKnown Field: %s\n", szValue);
							}
						}
						else if (szPart.GetLength()>5 && strcmp(szPart.Left(6), "Value:")==0)
						{
							if(szPart.Find(':')>=5)
							{
								CString szFieldName = szPart.Mid(szPart.Find(':')+1);
								strcpy_s(field.szName, sizeof(field.szName), szFieldName.Trim());
								if(szValue.Find('%')>0)
								{
									szValue.Replace("%", "");
									int nPercent = BSHelper::Str2Int(szValue);
									szValue.Format("%d", nPercent);
								}
								strcpy_s(field.szValue, sizeof(field.szValue), szValue);
								CString szNameSet;
								if (!m_mapFieldSets.GetAt(field.szName, szNameSet))
								{
									//BSLogger::SysLog(LOG_INFO, "InvalidField: %s : %s (%d-%d) -- %s  --%s", 
									//	field.szName, field.szValue, field.nMinValue, field.nMaxValue, strRegexLine, szLine.c_str());
								}
								pItemInfo->properties.push_back(field);
								ZeroMemory(&field, sizeof(field));
								bIsMatched = TRUE;
								continue;
							}
						}
						else if (szPart.GetLength()>3 
							&& (strcmp(szPart.Left(4), "Min:")==0 || strcmp(szPart.Left(4), "Max:")==0))
						{
							if(szPart.Find(':')>=3)
							{
								CString szFieldName = szPart.Mid(szPart.Find(':')+1);
								strcpy_s(field.szName, sizeof(field.szName), szFieldName.Trim());
								if (strcmp(szPart.Left(3), "Min")==0)
								{
									field.nMinValue = BSHelper::Str2Int(szValue);
								}
								else
								{
									field.nMaxValue = BSHelper::Str2Int(szValue);
								}
								if (field.nMaxValue>0)
								{
									CString szNameSet;
									if (!m_mapFieldSets.GetAt(field.szName, szNameSet))
									{
										//BSLogger::SysLog(LOG_INFO, "InvalidField: %s : %s (%d-%d) -- %s  --%s", 
										//	field.szName, field.szValue, field.nMinValue, field.nMaxValue, strRegexLine, szLine.c_str());
									}
									pItemInfo->properties.push_back(field);
									ZeroMemory(&field, sizeof(field));
									bIsMatched = TRUE;
								}
								continue;
							}
						}
						else if (strcmp(szPart.Left(4), "Name")==0)
						{
							strcpy_s(field.szName, sizeof(field.szName), szValue);
							bIsMatched = TRUE;
						}
						else if (strcmp(szPart, "Value")==0)
						{
							if(szValue.Find('%')>0)
							{
								szValue.Replace("%", "");
								int nPercent = BSHelper::Str2Int(szValue);
								szValue.Format("%d", nPercent);
							}
							strcpy_s(field.szValue, sizeof(field.szValue), szValue);
							bIsMatched = TRUE;
						}
						else if (strcmp(szPart, "Min")==0)
						{
							field.nMinValue = BSHelper::Str2Int(szValue);
							bIsMatched = TRUE;
						}
						else if (strcmp(szPart, "Max")==0)
						{
							field.nMaxValue = BSHelper::Str2Int(szValue);
							bIsMatched = TRUE;
						}
					}
				}
				if (bIsMatched)
				{
					if(field.szName[0])
					{
						CString szNameSet;
						if (!m_mapFieldSets.GetAt(field.szName, szNameSet))
						{
							//BSLogger::SysLog(LOG_INFO, "InvalidField: %s : %s (%d-%d) -- %s  --%s", 
							//	field.szName, field.szValue, field.nMinValue, field.nMaxValue, strRegexLine, szLine.c_str());
						}
						pItemInfo->properties.push_back(field);
						//TRACE("RegExMatched: %s : %s (%d-%d) -- %s\n", 
						//	field.szName, field.szValue, field.nMinValue, field.nMaxValue, strRegexLine);
					}
					break;
				}
			}
		}
		if (!bIsMatched)
		{
			TRACE("NotMatched: %d: %s\n", i, szLine);
		}
	}
	if(szEquipPos.GetLength()>0)
	{
		D3ItemProperty typeField;
		strcpy_s(typeField.szName, sizeof(typeField.szName), Field_PrimaryType);
		strcpy_s(typeField.szValue, sizeof(typeField.szValue), PrimaryOptions[3]);//防具
		pItemInfo->properties.push_back(typeField);

		strcpy_s(pItemInfo->szEquipType, sizeof(pItemInfo->szEquipType), szEquipPos);
	}
	else
	{
		strcpy_s(pItemInfo->szEquipType, sizeof(pItemInfo->szEquipType), szEquipType);
	}

	if (szEquipType.GetLength()>0)
	{
		D3ItemProperty attprotectField;
		if (szEquipType.Compare("單手") ==0 || szEquipType.Compare("雙手") ==0 || szEquipType.Compare("副手") ==0)
		{
			//第三行 如果是武器 每秒攻击("單手","雙手","副手") 
			strcpy_s(attprotectField.szName, sizeof(attprotectField.szName), Field_AttackPerSecond);
		}
		else //或 护甲值 
		{
			strcpy_s(attprotectField.szName, sizeof(attprotectField.szName), Field_Protect);
		}
		CString szAttProtect = strList.GetAt(2);
		strcpy_s(attprotectField.szValue, sizeof(attprotectField.szValue), szAttProtect.Trim());
		pItemInfo->properties.push_back(attprotectField);
		//TRACE("AttProtect: %s: %s\n", attprotectField.szName, attprotectField.szValue);
	}
	else
	{
		TRACE("Cannot find EquipType: %s\n", szAttrText);
	}

	return TRUE;
}

LPCTSTR ItemParser::getSpecialPropertyName(int nLineNumber, LPCTSTR szValue)
{
	if (nLineNumber==0)
	{
		return Field_Name;
	}
	if (nLineNumber==5)
	{
		return Field_Job;
	}
	CString sz = szValue;
	for (int i=0;i<sizeof(PrimaryOptions);i++)
	{
		if (sz.Compare(PrimaryOptions[i])==0)
		{
			return Field_PrimaryType;
		}
	}
	for (int i=0;i<sizeof(SecondaryOptions);i++)
	{
		if (sz.Compare(SecondaryOptions[i])==0)
		{
			return Field_SecondaryType;
		}
	}
	for (int i=0;i<sizeof(EquipPosOptions);i++)
	{
		if (sz.Compare(EquipPosOptions[i])==0)
		{
			return Field_EquipPos;
		}
	}	
	for (int i=0;i<sizeof(CharactorOptions);i++)
	{
		if (sz.Compare(PrimaryOptions[i])==0)
		{
			return Field_Job;
		}
	}
	for (int i=0;i<sizeof(QualityOptions);i++)
	{
		if (sz.Compare(QualityOptions[i])==0)
		{
			return Field_Quality;
		}
	}
	return NULL;
}

void ItemParser::LoadConfig(LPCTSTR szConfigFile)
{
	if(false)
	{
		m_mapFieldSets.RemoveAll();
		m_mapFieldSets.SetAt(Field_AttackPerSecond, Field_AttackPerSecond);
		m_mapFieldSets.SetAt(Field_PrimaryType, Field_PrimaryType);
		m_mapFieldSets.SetAt(Field_SecondaryType, Field_SecondaryType);
		m_mapFieldSets.SetAt(Field_Protect, Field_Protect);
		m_mapFieldSets.SetAt(Field_Quality, Field_Quality);
		m_mapFieldSets.SetAt(Field_Name, Field_Name);
		m_mapFieldSets.SetAt(Field_EquipPos, Field_EquipPos);
		m_mapFieldSets.SetAt(Field_Job, Field_Job);
		BSHelper::GoAppRootPath();
		CStdioFile file;
		if (file.Open(".\\ah_cond.js", CFile::modeRead|CFile::shareDenyWrite))
		{
			CString szutf;
			while(file.ReadString(szutf))
			{
				CString sz;
				BSHelper::Utf8ToGBK(szutf.GetBuffer(), sz.GetBufferSetLength(0x40), 0x40);
				regex rx("\\{Name: \"(.+)\",CanSel:(.+)\\},");//strReg
				std::tr1::cmatch res;
				string s = sz.GetBuffer();
				if (regex_search(s.c_str(), res, rx)) 
				{
					if (res.size()>1)
					{
						string str1 = res[0];
						string str2 = res[1];
						m_mapFieldSets.SetAt(str2.c_str(), str2.c_str());
					}
				}
			}
		}
		file.Close();
		CStdioFile output;
		if (!output.Open(".\\fields.txt", CFile::modeReadWrite|CFile::shareDenyWrite))
		{
			if(!output.Open(".\\fields.txt", CFile::modeReadWrite|CFile::modeCreate))
			{
				ASSERT(FALSE);
			}
		}
		output.SetLength(0);
		for (int i=0;i<m_mapFieldSets.GetCount();i++)
		{
			CString sz;
			if (m_mapFieldSets.GetByIndex(i, sz))
			{
				output.Write(sz.GetBuffer(), sz.GetLength());
				output.Write("\n", 1);
			}
		}
		output.Close();
	}

	BSHelper::GoAppRootPath();
	CString szFile = szConfigFile;
	CStdioFile file;
	if (!file.Open(szFile, CFile::modeRead|CFile::shareDenyWrite))
	{
		AfxMessageBox("无法打开propertyconfig.txt");
		return;
	}
	CString sz;
	while (file.ReadString(sz))
	{
		if (sz.GetLength()==0)
		{
			continue;
		}
		m_parserRegexList.Add(sz.Trim());
	}
}

CString ItemParser::GUID2Str( GUID guid )
{
	CString sz;
	int nLen = BSHelper::Bin2Str((BYTE*)&guid, sizeof(guid), sz);
	return sz;
}

GUID ItemParser::Str2GUID( LPCTSTR szGUID )
{
	GUID guid;
	BYTE buf[0x20];
	int retLen = BSHelper::Str2Bin(szGUID, buf, sizeof(buf));
	ASSERT(retLen==sizeof(GUID));
	memcpy_s(&guid, sizeof(guid), buf, retLen);
	return guid;
}
