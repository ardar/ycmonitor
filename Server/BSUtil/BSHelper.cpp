#include "StdAfx.h"
#include "BSUtil.h"
#include <process.h>
#include <afxmt.h>
#include <math.h>
#include <io.h> 
#include "md5.h"

void BSHelper::GoAppRootPath()
{
	TCHAR szPath[MAX_PATH];
	GetAppRootPath(szPath, MAX_PATH);
	SetCurrentDirectory(szPath);
}

BOOL BSHelper::GetAppRootPath(LPTSTR szPath, UINT nMaxLen)
{
	TCHAR szFolder[MAX_PATH], szAppName[_MAX_FNAME], szDrive[_MAX_DRIVE];
	TCHAR szIniPath[MAX_PATH + 1] = {0};

	GetModuleFileName((HMODULE)NULL, szIniPath, MAX_PATH);
	_tsplitpath_s(szIniPath, szDrive, _MAX_DRIVE, szFolder, MAX_PATH, szAppName, _MAX_FNAME, NULL, 0);
	if (BSHelper::bsstrlen(szDrive)+BSHelper::bsstrlen(szFolder) > nMaxLen-1)
	{
		return FALSE;
	}
	sprintf_s(szPath, nMaxLen, "%s%s", szDrive, szFolder);
	return TRUE;
}

int BSHelper::bsstrcmp(const char * _Str1, const char * _Str2)
{
	if (!_Str1 || !_Str2)
	{
		return -1;
	}
	int i=0;
	while (true)
	{
		if (i>0x1000)
		{
			return -1;
		}
		if (_Str1[i]==_Str2[i])
		{
			if(_Str1[i]!=0) 
			{
				i++;
				continue;
			}
			else
			{
				return 0;
			}
		}
		else if (_Str1[i]>_Str2[i])
		{
			return 1;
		}
		else if (_Str1[i]<_Str2[i])
		{
			return -1;
		}
		i++;
	}
	return 0;
}

unsigned int BSHelper::bsstrlen(const char * _Str)
{
	unsigned int i=0;
	while (_Str[i]!=0)
	{
		i++;
		if (i>0x1000)
		{
			i=0;
			break;
		}
	}
	return i;
}

CString BSHelper::Int2Str(int n)
{
	CString str;
	str.Format("%d", n);
	return str;
}

int BSHelper::Str2Int(LPCTSTR szNumber)
{
	int i = 0;
	if (szNumber)
	{
		i = atoi(szNumber);
	}
	return i;
}

double BSHelper::Str2Double(LPCTSTR szNumber)
{
	double i = 0.0;
	if (szNumber)
	{
		i = atof(szNumber);
	}
	return i;
}

ULONG BSHelper::Random( ULONG nMin/*=0*/, ULONG nMax/*=RAND_MAX*/ )
{
	if (nMin>nMax)
	{
		return 0;
	}
	static BOOL bInit = FALSE;
	try
	{
		if (!bInit)
		{
			bInit = TRUE;
			srand((unsigned)(GetTickCount()+GetCurrentThreadId()));
		}

		if ((nMax - nMin) + nMin == 0)
		{
			return nMin;
		}

		return (rand()+_getpid()*GetCurrentThreadId()+GetTickCount()) % (nMax+1 - nMin) + nMin;
	}
	catch(...)
	{
		BSLogger::SysLog(LOG_ERROR, "取随机数出错");
		return rand();
	}
}

int BSHelper::WtoA( LPCWSTR lpWstr, LPSTR lpStr, UINT nBufLen, UINT codepage/*=936*/ )
{
	if (lpWstr == NULL || *(WORD *)lpWstr == 0 || wcslen(lpWstr) > 4096)
	{
		strcpy_s(lpStr, nBufLen, "");
		return 2;
	}

	BOOL bUseDeFault = TRUE;
	int len = WideCharToMultiByte(codepage,0,lpWstr,wcslen(lpWstr),NULL,0,"?",&bUseDeFault);
	if (len > 0)
	{
		CStringA szRet;
		if (WideCharToMultiByte(codepage,0,lpWstr,wcslen(lpWstr),szRet.GetBuffer(len + 1),len,"?",&bUseDeFault) > 0)
		{
			szRet.ReleaseBuffer(len);
			//lpStr = (LPSTR)realloc(lpStr, szRet.GetLength()+1);
			strcpy_s(lpStr, nBufLen, szRet);
		}
		szRet.ReleaseBuffer();
	}
	else
		strcpy_s(lpStr, nBufLen, "");
	return BSHelper::bsstrlen(lpStr)+1;
}

int BSHelper::AtoW( LPCSTR lpstr, LPWSTR lpWStr, UINT nBufLen, UINT codepage/*=936*/ )
{
	CStringW szRet;
	if (lpstr == NULL || *(BYTE *)lpstr == 0 || BSHelper::bsstrlen(lpstr) > 4096)
	{
		wcscpy_s(lpWStr, nBufLen, szRet.GetBuffer());
		return 1;
	}

	BOOL bUseDeFault = TRUE;
	int len = MultiByteToWideChar(codepage,0,lpstr,BSHelper::bsstrlen(lpstr),NULL,0);
	if (len > 0)
	{		
		if (MultiByteToWideChar(codepage,0,lpstr,BSHelper::bsstrlen(lpstr),szRet.GetBuffer(len + 1),len) > 0)
		{
			szRet.ReleaseBuffer(len);
			//lpWStr = (LPWSTR)realloc(lpWStr, szRet.GetLength()*2+2);
			wcscpy_s(lpWStr, nBufLen, szRet.GetBuffer());
			return wcslen(lpWStr)*2+2;
		}
		szRet.ReleaseBuffer();
	}

	wcscpy_s(lpWStr, nBufLen, szRet.GetBuffer());
	return wcslen(lpWStr)*2+2;
}

void BSHelper::ReverseMemory( BYTE *buf,int len )
{
	BYTE *tempbuf=new BYTE [len];
	for (int i=0;i<len;i++)
	{
		tempbuf[len-i-1]=buf[i];
	}
	memcpy(buf,tempbuf,len);
	delete[] tempbuf;
}

BOOL BSHelper::Utf8ToGBK(LPSTR lpStr, LPSTR lpOutStr, UINT nBufLen) 
{
	CString strUtf8 = lpStr;
	int len=MultiByteToWideChar(CP_UTF8, 0, (LPCTSTR)strUtf8, -1, NULL,0);
	CStringW szW;
	// 	unsigned short * wszGBK = new unsigned short[len+1];
	// 	memset(wszGBK, 0, len * 2 + 2);
	MultiByteToWideChar(CP_UTF8, 0, (LPCTSTR)strUtf8, -1, szW.GetBuffer(len + 1), len);

	len = WideCharToMultiByte(CP_ACP, 0, szW, -1, NULL, 0, NULL, NULL); 
	char *szGBK=new char[len + 1];
	memset(szGBK, 0, len + 1);
	WideCharToMultiByte (CP_ACP, 0, szW, -1, szGBK, len, NULL,NULL);

	strUtf8 = szGBK;
	delete[] szGBK;
	//lpStr = (LPSTR)realloc(lpStr, strUtf8.GetLength()+1);
	if (strUtf8.GetLength()+1<=nBufLen)
	{
		strcpy_s(lpOutStr, nBufLen, strUtf8.GetBuffer());
		return TRUE;
	}
	return FALSE;
}

const BYTE Base64ValTab[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int BSHelper::EncodeBase64(const char * pInput, char * pOutput)
{
	int iSrcLen = (int)strlen(pInput);
	int iOutMaxLen = iSrcLen;
	return EncodeBase64((BYTE*)pInput, iSrcLen, pOutput, iOutMaxLen);
}

int BSHelper::EncodeBase64(const BYTE * pInput, int iSrcLen, char * pOutput, int iOutMaxLen)
{
	int i = 0;
	int loop = 0;
	int remain = 0;
	int iDstLen = 0;

	loop = iSrcLen/3;
	remain = iSrcLen%3;

	// also can encode native char one by one as decode method
	// but because all of char in native string  is to be encoded so encode 3-chars one time is easier.

	if (loop*4>iOutMaxLen)
	{
		TRACE("EncodeBase64 out of OutLen\n");
		ASSERT(FALSE);
		pOutput[0] = 0;
		return 0;
	}

	for (i=0; i < loop; i++)
	{
		BYTE a1 = (pInput[i*3] >> 2);
		BYTE a2 = ( ((pInput[i*3] & 0x03) << 4) | (pInput[i*3+1] >> 4) );
		BYTE a3 = ( ((pInput[i*3+1] & 0x0F) << 2) | ((pInput[i*3+2] & 0xC0) >> 6) );
		BYTE a4 = (pInput[i*3+2] & 0x3F);

		pOutput[i*4] = Base64ValTab[a1];
		pOutput[i*4+1] = Base64ValTab[a2];
		pOutput[i*4+2] = Base64ValTab[a3];
		pOutput[i*4+3] = Base64ValTab[a4];
	}

	iDstLen = i*4;

	if (remain == 1)
	{
		// should pad two equal sign
		i = iSrcLen-1;
		BYTE a1 = (pInput[i] >> 2);
		BYTE a2 = ((pInput[i] & 0x03) << 4);

		ASSERT(iDstLen+4<iOutMaxLen);
		if (iDstLen+4>=iOutMaxLen)
		{
			pOutput[iDstLen] = 0x00;
			return iDstLen;
		}

		pOutput[iDstLen++] = Base64ValTab[a1];
		pOutput[iDstLen++] = Base64ValTab[a2];
		pOutput[iDstLen++] = '=';
		pOutput[iDstLen++] = '=';
		pOutput[iDstLen] = 0x00;
	}
	else if (remain == 2)
	{
		// should pad one equal sign
		i = iSrcLen-2;
		BYTE a1 = (pInput[i] >> 2);
		BYTE a2 = ( ((pInput[i] & 0x03) << 4) | (pInput[i+1] >> 4));
		BYTE a3 = ( (pInput[i+1] & 0x0F) << 2);

		ASSERT(iDstLen+4<iOutMaxLen);
		if (iDstLen+4>=iOutMaxLen)
		{
			pOutput[iDstLen] = 0x00;
			return iDstLen;
		}

		pOutput[iDstLen++] = Base64ValTab[a1];
		pOutput[iDstLen++] = Base64ValTab[a2];
		pOutput[iDstLen++] = Base64ValTab[a3];
		pOutput[iDstLen++] = '=';
		pOutput[iDstLen] = 0x00;
	}
	else
	{
		// just division by 3
		pOutput[iDstLen] = 0x00;
	}

	return iDstLen;
}

CString BSHelper::EncodeBase64(BYTE * pInput, int iSrcLen)
{
	CString pOutput;

	int i = 0;
	int loop = 0;
	int remain = 0;
	int iDstLen = 0;

	loop = iSrcLen/3;
	remain = iSrcLen%3;

	// also can encode native char one by one as decode method
	// but because all of char in native string  is to be encoded so encode 3-chars one time is easier.

	//if (loop*4>iOutMaxLen)
	//{
	//	TRACE("EncodeBase64 out of OutLen\n");
	//	ASSERT(FALSE);
	//	pOutput[0] = 0;
	//	return 0;
	//}

	for (i=0; i < loop; i++)
	{
		BYTE a1 = (pInput[i*3] >> 2);
		BYTE a2 = ( ((pInput[i*3] & 0x03) << 4) | (pInput[i*3+1] >> 4) );
		BYTE a3 = ( ((pInput[i*3+1] & 0x0F) << 2) | ((pInput[i*3+2] & 0xC0) >> 6) );
		BYTE a4 = (pInput[i*3+2] & 0x3F);

		pOutput.AppendChar( Base64ValTab[a1]);//i*4
		pOutput.AppendChar( Base64ValTab[a2]);//i*4+1
		pOutput.AppendChar( Base64ValTab[a3]);//i*4+2
		pOutput.AppendChar( Base64ValTab[a4]);//i*4+3
	}

	iDstLen = i*4;

	if (remain == 1)
	{
		// should pad two equal sign
		i = iSrcLen-1;
		BYTE a1 = (pInput[i] >> 2);
		BYTE a2 = ((pInput[i] & 0x03) << 4);

		/*ASSERT(iDstLen+4<iOutMaxLen);
		if (iDstLen+4>=iOutMaxLen)
		{
			pOutput[iDstLen] = 0x00;
			return iDstLen;
		}*/

		pOutput.AppendChar( Base64ValTab[a1]);//iDstLen++,
		pOutput.AppendChar( Base64ValTab[a2]);
		pOutput.AppendChar( '=');
		pOutput.AppendChar( '=');
		//pOutput.AppendChar( 0x00);
	}
	else if (remain == 2)
	{
		// should pad one equal sign
		i = iSrcLen-2;
		BYTE a1 = (pInput[i] >> 2);
		BYTE a2 = ( ((pInput[i] & 0x03) << 4) | (pInput[i+1] >> 4));
		BYTE a3 = ( (pInput[i+1] & 0x0F) << 2);

		//ASSERT(iDstLen+4<iOutMaxLen);
		//if (iDstLen+4>=iOutMaxLen)
		//{
		//	pOutput[iDstLen] = 0x00;
		//	return iDstLen;
		//}

		pOutput.AppendChar( Base64ValTab[a1]);//iDstLen++
		pOutput.AppendChar( Base64ValTab[a2]);
		pOutput.AppendChar( Base64ValTab[a3]);
		pOutput.AppendChar( '=');
		//pOutput[iDstLen] = 0x00;
	}
	else
	{
		// just division by 3
		//pOutput[iDstLen] = 0x00;
	}

	return pOutput;
}

const BYTE Base64IdxTab[128] =
{
	255,255,255,255,  255,255,255,255,  255,255,255,255,  255,255,255,255,
	255,255,255,255,  255,255,255,255,  255,255,255,255,  255,255,255,255,
	255,255,255,255,  255,255,255,255,  255,255,255,62,   255,255,255,63,
	52,53,54,55,      56,57,58,59,      60,61,255,255,    255,255,255,255,
	255,0,1,2,        3,4,5,6,          7,8,9,10,         11,12,13,14,
	15,16,17,18,      19,20,21,22,      23,24,25,255,     255,255,255,255,
	255,26,27,28,     29,30,31,32,      33,34,35,36,      37,38,39,40,
	41,42,43,44,      45,46,47,48,      49,50,51,255,     255,255,255,255
};

int BSHelper::DecodeBase64(const char * pInput, char * pOutput)
{
	int iSrcLen = (int)strlen(pInput);
	int iOutMaxLen = iSrcLen*4/3+3;
	return DecodeBase64(pInput, iSrcLen, (BYTE*)pOutput, iOutMaxLen);
}

int BSHelper::DecodeBase64(const char * pInput, int iSrcLen, BYTE * pOutput, int nMaxOutLen)
{
	int i = 0;
	int iCnt = 0;

	BYTE * p = pOutput;

	for (i=0; i < iSrcLen; i++)
	{
		if (p-pOutput>nMaxOutLen)
		{
			TRACE("DecodeBase64 Out of max outbuf\n");
			ASSERT(FALSE);
			break;
		}

		if (pInput[i] > 127) continue;
		if (pInput[i] == '=') return p-pOutput;//return p-pOutput+1;

		BYTE a = Base64IdxTab[pInput[i]];
		if (a == 255) continue;

		switch (iCnt)
		{
		case 0:
			{
				*p = a << 2;
				iCnt++;
			}
			break;

		case 1:
			{
				*p++ |= a >> 4;
				*p = a << 4;
				iCnt++;
			}
			break;

		case 2:
			{
				*p++ |= a >> 2;
				*p = a << 6;
				iCnt++;
			}
			break;
		case 3:
			{
				*p++ |= a;
				iCnt = 0;
			}
			break;
		} 
	}

	//*p = 0x00;
	return p-pOutput;
}

void BSHelper::BinaryFormat(BYTE *buf,int len,LPSTR lpStr)
{
	CString szRet;
	for(int i=len-1;i>=0;i--)
	{
		int value=buf[i];
		char _char[255];//缓冲
		_itoa_s(value,_char,255,2);
		CString sztemp;
		sztemp.Format("%s",_char);
		int temlen=sztemp.GetLength();
		for (int j=0;j<8-temlen;j++)
		{
			sztemp="0"+sztemp;
		}
		szRet+=sztemp;
	}
	lpStr = (LPSTR)realloc(lpStr, szRet.GetLength()+1);
	return;
}

float BSHelper::GetDistance( float x1, float y1, float z1, float x2, float y2, float z2 )
{
	float x,y,z;
	x = x1 - x2;
	y = y1 - y2;
	z = z1 - z2;
	return sqrt(x*x + y*y + z*z);
}

void BSHelper::GetNearPos(float x,float y,float dx,float dy,float &nx,float &ny,float dis)
{
	float way=(float)dis;
	if (x==dx)
	{
		nx=dx;
		if (dy>y)
		{
			ny=dy-way;
		}
		if(dy<y)
		{
			ny=dy+way;
		}
		if (dy==y)
		{
			ny=dy;
		}
		return;
	}
	if (y==dy)
	{
		ny=dy;
		if (dx>x)
		{
			nx=dx-way;
		}
		if(dx<x)
		{
			nx=dx+way;
		}
		if (dx==x)
		{
			nx=dx;
		}
		return;
	}
	float fdest=GetDistance(x,y,0,dx,dy,0);
	float si=((float)abs(dy-y))/fdest;
	float co=((float)abs(dx-x))/fdest;
	float mx=way*co;
	float my=way*si;
	if (dx>x)
	{
		nx=dx-mx;
	}else
	{
		nx=dx+mx;
	}
	if (dy>y)
	{
		ny=dy-my;
	}else
	{
		ny=dy+my;
	}
}

int BSHelper::GetInt(float val)
{
	int nRet = (int)val;
	if ( val- (float)nRet >=0.5)
	{
		nRet++;
	}
	return nRet;
}

int BSHelper::SplitStr(TCHAR cSpliter, CString sz, CStringArray& strList, BOOL bSkipEmpty/*=FALSE*/)
{
	int nCurrPos = 0;
	while(nCurrPos<=sz.GetLength())
	{
		int nFindPos = sz.Find(cSpliter, nCurrPos);
		if (nFindPos<0)
		{
			nFindPos=sz.GetLength();
		}
		CString szItem = sz.Mid(nCurrPos, nFindPos-nCurrPos);
		if (!bSkipEmpty || szItem.GetLength()>0)
		{
			strList.Add(szItem);
		}
		nCurrPos=nFindPos+1;
	}
	return strList.GetCount();
}

int BSHelper::SplitStr(CString szSpliter, CString sz, CStringArray& strList, BOOL bSkipEmpty/*=FALSE*/)
{
	int nCurrPos = 0;
	while(nCurrPos<=sz.GetLength())
	{
		int nFindPos = sz.Find(szSpliter, nCurrPos);
		if (nFindPos<0)
		{
			nFindPos=sz.GetLength();
		}
		CString szItem = sz.Mid(nCurrPos, nFindPos-nCurrPos);
		if (!bSkipEmpty || szItem.GetLength()>0)
		{
			strList.Add(szItem);
		}
		nCurrPos=nFindPos+szSpliter.GetLength();
	}
	return strList.GetCount();
}

DWORD BSHelper::ReadDWORD(BYTE* buf, UINT& nIndex)
{
	DWORD val = *(DWORD*)(buf+nIndex);
	nIndex+=sizeof(val);
	return val;
}

WORD BSHelper::ReadWORD(BYTE* buf, UINT& nIndex)
{
	WORD val = *(WORD*)(buf+nIndex);
	nIndex+=sizeof(val);
	return val;
}

BYTE BSHelper::ReadBYTE(BYTE* buf, UINT& nIndex)
{
	BYTE val = *(BYTE*)(buf+nIndex);
	nIndex+=sizeof(val);
	return val;
}

CString BSHelper::ReadBSString(BYTE* buf, UINT& nIndex)
{
	DWORD dwLen = ReadDWORD(buf, nIndex);
	nIndex+=4;
	CString sz = (TCHAR*)(buf+nIndex);
	nIndex+=dwLen;
	return sz;
}

BOOL BSHelper::Md5File(LPCTSTR szFilePath, BYTE* buf, int nMaxLen)
{
	if (szFilePath==NULL || nMaxLen<0x10)
	{
		return FALSE;
	}
	MD5_CTX md5;
	if (md5file((char *)szFilePath, 0, &md5))
	{
		memcpy_s(buf, nMaxLen, md5.digest,16);
	}
	return TRUE;
}

CString BSHelper::GetFileNameInPath( LPCTSTR szPath )
{
	CString strPath = szPath;
	int nPos1 = strPath.ReverseFind('/');
	int nPos2 = strPath.ReverseFind('\\');
	int nSlashPos = nPos1>nPos2 ? nPos1 : nPos2;
	if (nSlashPos<0)
	{
		return strPath;
	}
	return strPath.Mid(nSlashPos+1);
}

CString BSHelper::GetDirInPath( LPCTSTR szPath )
{
	CString strPath = szPath;
	int nPos1 = strPath.ReverseFind('/');
	int nPos2 = strPath.ReverseFind('\\');
	int nSlashPos = nPos1>nPos2 ? nPos1 : nPos2;
	if (nSlashPos<0)
	{
		return strPath;
	}
	return strPath.Mid(0, nSlashPos);
}

BOOL BSHelper::FileExists( LPCTSTR szFilePath )
{
	return _access(szFilePath, 0) == 0;
}

BOOL BSHelper::AutoCreateDirectory( LPCTSTR szPath )
{
	CString strFilePath = szPath;
	while(strFilePath.Replace('/', '\\')>0 && strFilePath.Replace("\\\\", "\\")>0)
	{
		//Nothing todo
	}
	CStringArray strList;
	int nRet = SplitStr("\\", strFilePath, strList);
	CString szCreated = "";
	for (int i=0;i<strList.GetCount();i++)
	{
		const CString& strItem = strList.GetAt(i);
		if (strItem.GetLength()<=0)
		{
			continue;
		}
		szCreated.AppendFormat("\\%s", strItem);
		if (!FileExists(szCreated))
		{
			if(!CreateDirectory(szCreated, NULL))
			{
				return FALSE;
			}
		}
	}
	return FileExists(szPath);
}

inline BYTE toHex(const BYTE &x)
{
	return x > 9 ? x + 55: x + 48;
}

CString BSHelper::UrlEncode(BYTE* pInBuf, const int nLen)
{
	CString sOut;
	LPBYTE pOutBuf = NULL;
	//alloc out buffer
	pOutBuf = (LPBYTE)sOut.GetBuffer(nLen*3 - 2);//new BYTE [nLen  * 3];

	if(pOutBuf)
	{
		int nIn = 0;
		int nOut = 0;
		// do encoding
		while (pInBuf[nIn])
		{
			if(isalnum(pInBuf[nIn]))
				pOutBuf[nOut++] = pInBuf[nIn];
			else
				if(isspace(pInBuf[nIn]))
					pOutBuf[nOut++] = '+';
				else
				{
					pOutBuf[nOut++] = '%';
					pOutBuf[nOut++] = toHex(pInBuf[nIn]>>4);
					pOutBuf[nOut++] = toHex(pInBuf[nIn]%16);
				}
				nIn++;
		}
		pOutBuf[nOut++] = '\0';
		//sOut=pOutBuf;
		//delete [] pOutBuf;
		sOut.ReleaseBuffer();
	}
	return sOut;

}

#define IsHexNum(c) ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'))

CString utf8ToStringT(LPSTR str)  
{
	_ASSERT(str);
	USES_CONVERSION;
	WCHAR *buf;
	int length = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
	buf = new WCHAR[length+1];
	ZeroMemory(buf, (length+1) * sizeof(WCHAR));
	MultiByteToWideChar(CP_UTF8, 0, str, -1, buf, length);
	return (CString(W2T(buf)));  
}

CString BSHelper::UrlDecode(LPCTSTR url)  
{
	_ASSERT(url);
	USES_CONVERSION;
	LPSTR _url = T2A(const_cast<LPTSTR>(url));
	int i = 0;
	int length = (int)strlen(_url);
	CHAR *buf = new CHAR[length];
	ZeroMemory(buf, length);
	LPSTR p = buf;
	while(i < length)
	{
		if(i <= length -3 && _url[i] == '%' && IsHexNum(_url[i+1]) && IsHexNum(_url[i+2]))
		{
			sscanf(_url + i + 1, "%x", p++);
			i += 3;
		}
		else
		{
			*(p++) = _url[i++];  
		}
	}
	return utf8ToStringT(buf);
}

long BSHelper::npf(int d,int n)
{
	int i;
	long m=1;
	for(i=0;i<n;i++)
		m*=d;
	return m;
}
BOOL BSHelper::isRightData(char c,int jz)
{
	if((jz<2)||(jz>16))
		return FALSE;
	char ch[]="00112233445566778899aAbBcCdDeEfF";
	int i=0;
	for(i=0;i<jz;i++)
	{
		if(c==ch[i*2])return TRUE;
		if(c==ch[i*2+1])return TRUE;
	}
	return FALSE;
}
long BSHelper::strToData(CString strData,int jz)
{
	int l=strData.GetLength();
	long IntValue=0;
	int bitValue;
	int i;
	for(i=0;i<l;i++)
	{
		if(isRightData(strData[i],jz))
		{
			if(isdigit(strData[i]))
				bitValue=strData[i]-0x30;
			else
			{
				switch(strData[i])
				{
				case 'a':
					bitValue=0x0a;
					break;
				case 'b':
					bitValue=0x0b;
					break;
				case 'c':
					bitValue=0x0c;
					break;
				case 'd':
					bitValue=0x0d;
					break;
				case 'e':
					bitValue=0x0e;
					break;
				case 'f':
					bitValue=0x0f;
					break;
				case 'A':
					bitValue=0x0a;
					break;
				case 'B':
					bitValue=0x0b;
					break;
				case 'C':
					bitValue=0x0c;
					break;
				case 'D':
					bitValue=0x0d;
					break;
				case 'E':
					bitValue=0x0e;
					break;
				case 'F':
					bitValue=0x0f;
					break;
				}
			}
			IntValue=bitValue*npf(jz,l-i-1)+IntValue;
		}
		else
			return -1;
	}
	return IntValue;
}
int BSHelper::Str2Bin( LPCTSTR sz,BYTE *md5, int nMaxLen )
{
	CString szMd5=CString(sz);
	if (szMd5.GetLength()%2!=0)
	{
		szMd5 = szMd5 + CString("0");
	}
	if (szMd5.GetLength()/2 > nMaxLen)
	{
		return 0;
	}
	for (int i=0;i<szMd5.GetLength();i=i+2)
	{
		CString szKey;
		szKey.Format("%c%c", szMd5[i], szMd5[i+1]);
		md5[i/2]=strToData(szKey,16);
	}
	return szMd5.GetLength()/2;
}

int BSHelper::Bin2Str( BYTE* buf, int nBufLen, CString& sz )
{
	for (int i=0;i<nBufLen;i++)
	{
		sz.AppendFormat("%0.2X", (DWORD)buf[i]);
	}
	return sz.GetLength();
}
