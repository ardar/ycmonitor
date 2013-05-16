#pragma once
#include "stdafx.h"
#include <string>
#include <afxmt.h>


// 日志记录的消息级别
enum _declspec(dllexport) LOG_LEVEL
{
	LOG_VERBOSE = 1,//详细
	LOG_TERSE,//简洁
	LOG_INFO,//运行信息
	LOG_IMPORTANT,//重要信息
	LOG_ERROR,//错误信息
	LOG_NONE = 99,//不打印
};

class _declspec(dllexport) BSHelper
{
public:
	// 获取主程序文件所在目录
	static BOOL GetAppRootPath(LPTSTR szPath, UINT nMaxLen);
	// 将当前目录转到主程序文件所在目录
	static void GoAppRootPath();
	// 获取随机数
	static unsigned long Random(unsigned long nMin=0, unsigned long nMax=RAND_MAX);
	// 字符串比对函数
	static int bsstrcmp(const char * _Str1, const char * _Str2);
	// 获取字符串长度
	static unsigned int bsstrlen(const char * _Str);
	// 数字转换成字符串
	static CString Int2Str(int n);
	// 字符串转换成数字
	static int Str2Int(const char* szNumber);
	// 字符串转换成数字
	static double Str2Double(const char* szNumber);
	// wide char 转换到 ansi
	static int WtoA(LPCWSTR lpWstr, LPSTR lpOutStr, UINT nOutBufLen, UINT codepage=936); /*china,936 kr,949 taiwan,950 jp,932 */
	// ansi 转换到 wide char
	static int AtoW(LPCSTR lpstr, LPWSTR lpOutWStr, UINT nOutBufLen, UINT codepage=936); /*china,936 kr,949 taiwan,950 jp,932 */;
	// 将内存字节顺序倒转
	static void ReverseMemory(BYTE *buf, int len);
	// UTF8 转换到 GBK
	static BOOL Utf8ToGBK(LPSTR lpStr, LPSTR lpOutStr, UINT nBufLen);
	// Base64解码
	static int DecodeBase64(const char * pInput, char * pOutput);
	static int DecodeBase64(const char * pInput, int iSrcLen, BYTE * pOutput, int nMaxOutLen);
	// Base64编码
	static int EncodeBase64(const char * pInput, char * pOutput);
	static int EncodeBase64(const BYTE * pInput, int iSrcLen, char * pOutput, int nMaxOutLen);
	static CString EncodeBase64(BYTE * pInput, int iSrcLen);
	static CString UrlEncode(BYTE* pInBuf, const int nLen);
	static CString UrlDecode(LPCTSTR url);
	static int Str2Bin(LPCTSTR sz, BYTE* buf, int nMaxLen);
	static int Bin2Str(BYTE* buf, int nBufLen, CString& sz);
	// 将buffer内容转换成二进制表示形式的字符串
	static void BinaryFormat(BYTE *buf,int len,LPSTR lpStr);
	// 获取两点间距离
	static float GetDistance(float x1, float y1, float z1, float x2, float y2, float z2);
	// 获取从当前点出发，到目的点某个距离的中间点
	static void GetNearPos(float x,float y,float dx,float dy,float &nx,float &ny,float dis);
	// 四舍五入取整
	static int GetInt(float val);
	// 分割字符串
	static int SplitStr(TCHAR cSpliter, CString sz, CStringArray& strList, BOOL bSkipEmpty=FALSE);
	static int SplitStr(CString szSpliter, CString sz, CStringArray& strList, BOOL bSkipEmpty=FALSE);
	// 读取Buffer
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

	// 成员
	// 写入日志内容
	void Log(LPCTSTR szMsg);
	// 将缓冲区中的内容写入日志文件
	void Flush();
	// 获取日志文件名
	LPCTSTR GetLogFile();
	// 设置日志文件名
	void SetLogFile(LPCTSTR szLogFileName);

	// 静态函数
	// 写入SysLog
	static void SysLog(LOG_LEVEL level, LPCTSTR szMessage, ...);
	// 记录buffer log
	static void BufLog(BYTE *buf ,int len,DWORD type, char* lpTopic="");
	// 清理日志目录中 nDayAgo 天前没有更新过的文件
	static void CleanLogFolder(LPCTSTR szPath, int nDayAgo);
	// 将内存内容转换到字符串
	static int MemoryToStr(int LineStart,char *Buf, void *pPacket, int PacketSize);
private:
	CMutex m_mutexBuffer;//log缓冲区锁
	UINT m_nMaxFileSize;//log文件最大大小，大于将清空
	UINT m_nMaxBufferSize; //最大buffer大小
	UINT m_nBufferLen;//当前已使用
	TCHAR m_szLogFileName[MAX_PATH];//log文件名
	TCHAR* m_buffer;//log缓冲区指针
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
	// 最大报文长度
	UINT MaxBufferLen;
	// 当前报文长度
	UINT BufferLen;
	// 报文包头长度
	UINT HeaderLen;
	// 报文总长度
	UINT PackLen;
	// 报文已收到长度
	int ReceivedLen;
	// 报文buffer
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
