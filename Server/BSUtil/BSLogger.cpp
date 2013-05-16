#include "StdAfx.h"
#include "BSUtil.h"

BSLogger::BSLogger(LPCTSTR szLogFileName, UINT nBufferSize, UINT nMaxFileSize)
:m_nBufferLen(0)
{
	ASSERT(szLogFileName);
	ASSERT(nBufferSize>0);
	ASSERT(nMaxFileSize>0);
	ASSERT(nBufferSize<nMaxFileSize);
	if (nBufferSize>nMaxFileSize)
	{
		nBufferSize = nMaxFileSize;
	}

	CString strFileName = szLogFileName;
	strFileName.Replace('|', '_');
	strFileName.Replace('<', '_');
	strFileName.Replace('>', '_');
	strFileName.Replace('*', '_');
	strFileName.Replace('?', '_');
	TRACE("logfile:%s, bufsize:%d, maxsize:%d\n", strFileName, nBufferSize, nMaxFileSize);
	m_szLogFileName[0]=0;
	strcpy_s(m_szLogFileName, MAX_PATH, strFileName.GetBuffer());
	m_nMaxFileSize = nMaxFileSize;
	m_nMaxBufferSize = nBufferSize;
	m_buffer = new TCHAR[m_nMaxBufferSize];
}

BSLogger::~BSLogger(void)
{
	if (m_nBufferLen>0)
	{
		Flush();
	}
	delete[] m_buffer;
}

void BSLogger::SetLogFile(LPCTSTR szLogFileName)
{
	strcpy_s(m_szLogFileName, MAX_PATH, szLogFileName);
}

LPCTSTR BSLogger::GetLogFile()
{
	return m_szLogFileName;
}

void BSLogger::Log( LPCTSTR szMsg )
{
	if (!szMsg || !m_buffer)
	{
		return;
	}
	DWORD dwLen = BSHelper::bsstrlen(szMsg);
	if (dwLen>=m_nMaxBufferSize)
	{
		dwLen = m_nMaxBufferSize-1;
	}
	ASSERT(dwLen<m_nMaxBufferSize); 
	BSAutoLock lock(&m_mutexBuffer);
	if (dwLen+m_nBufferLen >= m_nMaxBufferSize)
	{
		Flush();
	}
	ASSERT(dwLen+m_nBufferLen<m_nMaxBufferSize);
	memcpy_s(m_buffer+m_nBufferLen, m_nMaxBufferSize-m_nBufferLen, szMsg, dwLen);
	m_nBufferLen+=dwLen;
	m_buffer[m_nBufferLen] = 0;

#ifdef _DEBUG
	Flush();
#endif // _DEBUG
}

void BSLogger::Flush()
{
	CFile file;
	BOOL bOK=TRUE;
	try
	{
		if(!file.Open(m_szLogFileName,CFile::modeWrite))
		{
			if(!file.Open(m_szLogFileName,CFile::modeCreate|CFile::modeWrite))
			{
				bOK=FALSE;
				BSLogger::SysLog(LOG_ERROR, "BSLogger::Flush open file failed: %s", m_szLogFileName);
			}
		}
		if (bOK)
		{
			BSAutoLock lock(&m_mutexBuffer);
			if (m_nMaxFileSize>0 && file.GetLength()+m_nBufferLen>=m_nMaxFileSize)
			{
				file.SetLength(0);
			}
			file.SeekToEnd();
			file.Write(m_buffer, m_nBufferLen);
			//file.Write("\r\n",2);
			m_buffer[0] = 0;
			m_nBufferLen = 0;
			file.Close();
		}
		else
		{
			m_buffer[0] = 0;
			m_nBufferLen = 0;
			BSLogger::SysLog(LOG_ERROR, "BSLogger::Flush open file failed2: %s", m_szLogFileName);
		}
	}
	catch (...)
	{
		if (bOK)
		{
			file.Close();
		}
		TRACE("Warning: Logger::Flush() Ð´ÈëlogÎÄ¼þ%sÊ§°Ü\n", m_szLogFileName);
		//printf("Warning: Logger::Flush() Ð´ÈëlogÎÄ¼þ%sÊ§°Ü\n", m_szLogFileName);
		m_buffer[0] = 0;
		m_nBufferLen = 0;
	}
}

void BSLogger::CleanLogFolder(LPCTSTR szPath, int nDayAgo)
{
	try
	{
		char szFind[MAX_PATH] = {0};
		char szFile[MAX_PATH] = {0};
		CString info;
		WIN32_FIND_DATA FindFileData;
		ZeroMemory(&FindFileData, sizeof(FindFileData));

		strcpy_s(szFind,MAX_PATH,szPath);
		strcat_s(szFind,MAX_PATH,"\\*.*");

		HANDLE hFind=::FindFirstFile(szFind,&FindFileData);
		if(INVALID_HANDLE_VALUE == hFind)    return;

		while(TRUE)
		{
			strcpy_s(szFile,MAX_PATH,szPath);
			strcat_s(szFile,MAX_PATH,"\\");
			strcat_s(szFile,MAX_PATH,FindFileData.cFileName);
			if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if(FindFileData.cFileName[0]!='.')
				{
					//CleanLogFolder(szFile, nDayAgo);
				}
			}
			else
			{
				//deal with FindFileData.cFileName
				CTime time(FindFileData.ftLastWriteTime);
				CTime currTime = CTime::GetCurrentTime();
				if (currTime.GetTime()-time.GetTime() >= nDayAgo*86400)
				{
					TRACE("DeleteLogFile:%s\n", szFile);
					::DeleteFile(szFile);
				}
			}
			if(!FindNextFile(hFind,&FindFileData))    
			{
				break;
			}
		}
		FindClose(hFind);
	}
	catch(...)
	{

	}
}

void BSLogger::SysLog(LOG_LEVEL level, LPCTSTR szMessage, ...)
{
	if(!szMessage) return;

	try
	{
		if (!szMessage || BSHelper::bsstrlen(szMessage)>0x180)
		{
			return;
		}

		char szDest[0x200];

		va_list	vl;
		va_start( vl, szMessage );
		vsprintf_s( szDest, 0x200, szMessage, vl );
		va_end( vl);

		CTime time = CTime::GetCurrentTime();

		TRACE("[t%d]%s:%s\n", GetCurrentThreadId(), time.Format("%H:%M:%S"), szDest);

		TCHAR szLog[0x250];
		sprintf_s(szLog, 0x250, "[t%d]%s:%s\r\n", GetCurrentThreadId(), time.Format("%H:%M:%S"), szDest);

#ifdef _DEBUG
		printf("%s\n", szLog);
#endif // _DEBUG

		BSHelper::GoAppRootPath();

		static CMutex s_syslogMutex;
		BSAutoLock lock(&s_syslogMutex);
		/*CString tempfile;
		tempfile.Format("slog\\");
		CreateDirectoryA(tempfile,NULL);
		tempfile+="syslog.txt";*/
		//tempfile.Format("%s%s.txt",tempfile,m_pBot->m_BotStruct.szUser);
		CFile file;
		if(!file.Open("syslog.txt",CFile::modeWrite|CFile::shareDenyWrite))
		{
			if(!file.Open("syslog.txt",CFile::modeCreate|CFile::modeWrite|CFile::shareDenyWrite))return;
		}
		file.SeekToEnd();
		file.Write(szLog, BSHelper::bsstrlen(szLog));
		file.Close();
	}
	catch (...)
	{
		printf("SysLog´íÎó");
	}
}

void BSLogger::BufLog(BYTE *buf ,int len,DWORD type, char* lpTopic)
{
#ifndef _DEBUG
	return;
#endif
	try
	{
		static CMutex s_mutex;
		BSAutoLock lock(&s_mutex);
		int ncount=len/512;int nmod=len%512;
		CHAR szDebugbuf[5120]={0};
		CString sz;
		CTime time = CTime::GetCurrentTime();
		sz.Format("TYPE: %X  %d time:%s t:%u %s-----------------------------------------\r\n",type,len,time.Format("%H:%M:%S"),GetTickCount(), lpTopic);
		for (int i=0;i<ncount;i++)
		{
			memset(szDebugbuf,0,sizeof(szDebugbuf));
			MemoryToStr(i*512,szDebugbuf,(BYTE *)buf+i*512,512);
			sz+=szDebugbuf;
			sz+="\r\n";
		}
		if (nmod>0)
		{
			memset(szDebugbuf,0,sizeof(szDebugbuf));
			MemoryToStr(ncount*512,szDebugbuf,(BYTE *)buf+ncount*512,nmod);
			sz+=szDebugbuf;
			sz+="\r\n";
		}
		sz="  \r\n"+sz;

		//CWishHelper::Message(LOG_TERSE, sz);
		CFile file;

		TCHAR szIniPath[MAX_PATH] = {0};
		BSHelper::GetAppRootPath(szIniPath, MAX_PATH);

		CString tempfile;
		tempfile.Format("%sbuflog\\",szIniPath);
		CreateDirectoryA(tempfile,NULL);
		tempfile.AppendFormat("_buflog_%d.txt", 0/*GetCurrentThreadId()*/);
		//tempfile.Format("%s%s.txt",tempfile,m_pBot->m_BotStruct.szUser);
		if(!file.Open(tempfile,CFile::modeWrite|CFile::shareDenyWrite))
		{
			if(!file.Open(tempfile,CFile::modeCreate|CFile::modeWrite|CFile::shareDenyWrite))return;
		}
		file.SeekToEnd();
		file.Write("\r\n",2);
		file.Write(sz.GetBuffer(),sz.GetLength());
		file.Close();
	}
	catch (...)
	{
		printf("BufLog´íÎó");
	}

}
int BSLogger::MemoryToStr(int LineStart,char *Buf, void *pPacket, int PacketSize) 
{
	char __HexTable__[] = "0123456789ABCDEF";
	BYTE             *pPkt   = (BYTE*)pPacket;
	int              Len     = 0;
	int              Pos     = 0;
	BYTE             CurByte; 
	char             *pHexStr; 
	char             *pAsciiStr; 

	*Buf = '\0'; 

	//	Len += sprintf(&Buf[Len],"--------------- %s:0x%04X ------------------------------------\n",topic,PacketSize);
	pHexStr = &Buf[Len];

	for(int i = 0; i < PacketSize; i++, pAsciiStr++)
	{      
		Pos = i & 0xF;
		if(!Pos)
		{
			if(i)
			{
				pAsciiStr[0] = '\r';
				pAsciiStr[1] = '\n';
				pHexStr = pAsciiStr + 2;
			}

			pHexStr[0]   = __HexTable__[((i+LineStart) / 0x1000) & 0x0F];
			pHexStr[1]   = __HexTable__[((i+LineStart) %0x1000/0x100)  & 0x0F];
			pHexStr[2]   = __HexTable__[((i+LineStart) %0x100/0x10)  & 0x0F];
			pHexStr[3]   = __HexTable__[(i+LineStart)   %0x10      & 0x0F];
			pHexStr[4]   = ':';
			pHexStr[5]   = ' ';
			pHexStr[6]   = ' ';
			pHexStr      += 7;
			pAsciiStr    = pHexStr + 50;
			pAsciiStr[0] = ' ';
			pAsciiStr[1] = ' ';
			pAsciiStr    += 2;
		}

		CurByte    = pPkt[i];
		pHexStr[0] = __HexTable__[CurByte >> 4];
		pHexStr[1] = __HexTable__[CurByte & 0x0F];
		pHexStr[2] = ' ';
		pHexStr    += 3;

		if(CurByte==0 || CurByte==0x0a )
			*pAsciiStr = '.';
		else
			*pAsciiStr = CurByte;

		if(isprint(CurByte))
			*pAsciiStr = CurByte;
		else
			*pAsciiStr = '.';

		if(Pos == 7)
		{
			pHexStr[0]   = '|';
			pHexStr[1]   = ' ';
			pHexStr += 2;
		}
	}

	Len += 77 * (PacketSize >> 4);
	int Rest = 0xF - Pos;
	if(Rest & 0x08) Rest++;
	if(Rest) 
	{
		Len += 77;
		while(Rest--)
		{
			pHexStr[0]   = ' ';
			pHexStr[1]   = ' ';
			pHexStr[2]   = ' ';
			pHexStr      += 3;
			if(Rest != 7) *pAsciiStr++ = ' ';
		}
	}

	Len += 2;

	*(DWORD*)pAsciiStr = '\r'; 
	pAsciiStr[4] = '\0'; 

	return Len;
}
