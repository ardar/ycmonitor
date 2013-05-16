
#include <afxmt.h>

class _declspec(dllexport) BSAutoLock
{
public:
	BSAutoLock(CSyncObject* pLock);
	~BSAutoLock();
private:
	CSingleLock m_lock;
};