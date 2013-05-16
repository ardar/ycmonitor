#pragma once


class ItemParser
{
public:
	static ItemParser& Instance();

	void LoadConfig(LPCTSTR szConfigFile);
	BOOL ParseProperties( LPCTSTR szAttrText, D3ItemInfo* pItemInfo );
	CString GUID2Str(GUID guid);
	GUID Str2GUID(LPCTSTR szGUID);
private:
	static ItemParser s_instance;
	ItemParser(void);
	virtual ~ItemParser(void);
	LPCTSTR getSpecialPropertyName(int nLineNumber, LPCTSTR szValue);
	CStringArray m_parserRegexList;
	BSMap<CString, LPCTSTR, CString, LPCTSTR> m_mapFieldSets;
};
