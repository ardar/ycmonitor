#pragma once
#include "stdafx.h"
#include <string>
#include <afxmt.h>


// ��־��¼����Ϣ����
enum _declspec(dllexport) LOG_LEVEL
{
	LOG_VERBOSE = 1,//��ϸ
	LOG_TERSE,//���
	LOG_INFO,//������Ϣ
	LOG_IMPORTANT,//��Ҫ��Ϣ
	LOG_ERROR,//������Ϣ
	LOG_NONE = 99,//����ӡ
};

class _declspec(dllexport) BSHelper
{
public:
	// ��ȡ�������ļ�����Ŀ¼
	static BOOL GetAppRootPath(LPTSTR szPath, UINT nMaxLen);
	// ����ǰĿ¼ת���������ļ�����Ŀ¼
	static void GoAppRootPath();
	// ��ȡ�����
	static unsigned long Random(unsigned long nMin=0, unsigned long nMax=RAND_MAX);
	// �ַ����ȶԺ���
	static int bsstrcmp(const char * _Str1, const char * _Str2);
	// ��ȡ�ַ�������
	static unsigned int bsstrlen(const char * _Str);
	// ����ת�����ַ���
	static CString Int2Str(int n);
	// �ַ���ת��������
	static int Str2Int(const char* szNumber);
	// �ַ���ת��������
	static double Str2Double(const char* szNumber);
	// wide char ת���� ansi
	static int WtoA(LPCWSTR lpWstr, LPSTR lpOutStr, UINT nOutBufLen, UINT codepage=936); /*china,936 kr,949 taiwan,950 jp,932 */
	// ansi ת���� wide char
	static int AtoW(LPCSTR lpstr, LPWSTR lpOutWStr, UINT nOutBufLen, UINT codepage=936); /*china,936 kr,949 taiwan,950 jp,932 */;
	// ���ڴ��ֽ�˳��ת
	static void ReverseMemory(BYTE *buf, int len);
	// UTF8 ת���� GBK
	static BOOL Utf8ToGBK(LPSTR lpStr, LPSTR lpOutStr, UINT nBufLen);
	// Base64����
	static int DecodeBase64(const char * pInput, char * pOutput);
	static int DecodeBase64(const char * pInput, int iSrcLen, BYTE * pOutput, int nMaxOutLen);
	// Base64����
	static int EncodeBase64(const char * pInput, char * pOutput);
	static int EncodeBase64(const BYTE * pInput, int iSrcLen, char * pOutput, int nMaxOutLen);
	static CString EncodeBase64(BYTE * pInput, int iSrcLen);
	static CString UrlEncode(BYTE* pInBuf, const int nLen);
	static CString UrlDecode(LPCTSTR url);
	static int Str2Bin(LPCTSTR sz, BYTE* buf, int nMaxLen);
	static int Bin2Str(BYTE* buf, int nBufLen, CString& sz);
	// ��buffer����ת���ɶ����Ʊ�ʾ��ʽ���ַ���
	static void BinaryFormat(BYTE *buf,int len,LPSTR lpStr);
	// ��ȡ��������
	static float GetDistance(float x1, float y1, float z1, float x2, float y2, float z2);
	// ��ȡ�ӵ�ǰ���������Ŀ�ĵ�ĳ��������м��
	static void GetNearPos(float x,float y,float dx,float dy,float &nx,float &ny,float dis);
	// ��������ȡ��
	static int GetInt(float val);
	// �ָ��ַ���
	static int SplitStr(TCHAR cSpliter, CString sz, CStringArray& strList, BOOL bSkipEmpty=FALSE);
	static int SplitStr(CString szSpliter, CString sz, CStringArray& strList, BOOL bSkipEmpty=FALSE);
	// ��ȡBuffer
	static DWORD ReadDWORD(BYTE* buf, UINT& nIndex);
	static WORD ReadWORD(BYTE* buf, UINT& nIndex);
	static BYTE ReadBYTE(BYTE* buf, UINT& nIndex);
	static CString ReadBSString(BYTE* buf, UINT& nIndex);
	static BOOL Md5File(LPCTSTR szFilePath, BYTE* buf, int nMaxLen);
	static BOOL FileExists(LPCTSTR szFilePath);
	static BOOL AutoCreateDirectory( LPCTSTR szPath );
	static CString GetFileNameInPath( LPCTSTR szPath );
	static CString GetDirInPath( LPCTSTR szPath );
private:
	static long npf(int d,int n);
	static long strToData(CString strData,int jz);
	static BOOL isRightData(char c,int jz);
};

class _declspec(dllexport) BSLogger
{
public:
	BSLogger(LPCTSTR szLogFileName, UINT nBufferSize, UINT nMaxFileSize);
	~BSLogger(void);

	// ��Ա
	// д����־����
	void Log(LPCTSTR szMsg);
	// ���������е�����д����־�ļ�
	void Flush();
	// ��ȡ��־�ļ���
	LPCTSTR GetLogFile();
	// ������־�ļ���
	void SetLogFile(LPCTSTR szLogFileName);

	// ��̬����
	// д��SysLog
	static void SysLog(LOG_LEVEL level, LPCTSTR szMessage, ...);
	// ��¼buffer log
	static void BufLog(BYTE *buf ,int len,DWORD type, char* lpTopic="");
	// ������־Ŀ¼�� nDayAgo ��ǰû�и��¹����ļ�
	static void CleanLogFolder(LPCTSTR szPath, int nDayAgo);
	// ���ڴ�����ת�����ַ���
	static int MemoryToStr(int LineStart,char *Buf, void *pPacket, int PacketSize);
private:
	CMutex m_mutexBuffer;//log��������
	UINT m_nMaxFileSize;//log�ļ�����С�����ڽ����
	UINT m_nMaxBufferSize; //���buffer��С
	UINT m_nBufferLen;//��ǰ��ʹ��
	TCHAR m_szLogFileName[MAX_PATH];//log�ļ���
	TCHAR* m_buffer;//log������ָ��
};

class _declspec(dllexport) BSAutoLock
{
public:
	BSAutoLock(CMutex* pLock);
	~BSAutoLock();
private:
	CSingleLock m_lock;
};

class _declspec(dllexport) BSAutoResetEvent
{
public:
	BSAutoResetEvent(HANDLE m_hEvent);
	~BSAutoResetEvent();
private:
	HANDLE m_hEvent;
};

class _declspec(dllexport) BSAutoResetValue
{
public:
	BSAutoResetValue(VOID* pValue, DWORD dwResetValue);
	~BSAutoResetValue();
private:
	VOID* m_pValue;
	DWORD m_dwInitValue;
	DWORD m_dwResetValue;
};

class _declspec(dllexport) BSWaitEvent
{
public:
	static const DWORD RESULT_SUCCESS = 0;
	static const DWORD RESULT_TIMEOUT = 6868;
	static const DWORD RESULT_CANCELED = 6969;
	BSWaitEvent(void);
	virtual ~BSWaitEvent(void);

	void StartEvent(DWORD dwEventId);
	BOOL WaitEvent(DWORD dwEventId, int nTimeout);
	void EndEvent(DWORD dwEventId);
	void OnEvent(DWORD dwEventId, DWORD dwResult = 0, DWORD wParam = 0, DWORD lParam = 0);
	void CancelEvent();
	DWORD GetResult(DWORD* pWParam=NULL, DWORD* pLParam=NULL);
	int WaitResult(DWORD dwEventId, int nTimeout, DWORD* pWParam=NULL, DWORD* pLParam=NULL);
private:
	HANDLE m_hWaitEvent;
	DWORD m_dwWaitEventId;
	DWORD m_dwWaitResult;
	DWORD m_dwResultWParam;
	DWORD m_dwResultLParam;
	CMutex m_mutexWaitEvent;
};

class _declspec(dllexport) BSNetPack
{
public:
	BSNetPack(UINT maxBufLen, UINT initBufLen, UINT headerLen);
	~BSNetPack();
	void ClearBuffer();
	BOOL SaveToBuffer(BYTE* buf, UINT nReadLen);
	UINT ParsePackLen();
	BOOL IsHeaderFinished();
	BOOL IsPackFinished();
	UINT GetHeaderLen();
	UINT GetPackLen();
	BYTE* GetPackBuffer();
	BOOL SplitPack(BYTE* recvBuffer, UINT nRecvLen, UINT& nSavedLen);
	CMutex* GetMutex(){return &m_mutexBuffer;};
	BOOL Lock();
	BOOL Unlock();
private:
	// ����ĳ���
	UINT MaxBufferLen;
	// ��ǰ���ĳ���
	UINT BufferLen;
	// ���İ�ͷ����
	UINT HeaderLen;
	// �����ܳ���
	UINT PackLen;
	// �������յ�����
	int ReceivedLen;
	// ����buffer
	BYTE* Buffer;
	CMutex m_mutexBuffer;

	BOOL ExpendBuffer(UINT nNewLen);
};

class _declspec(dllexport) CComInit
{
public:
	__inline CComInit(){::CoInitialize(NULL);}
	__inline ~CComInit(){::CoUninitialize();}
};
