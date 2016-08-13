//读写锁封装
#if defined(_LINUX_)
#include <pthread.h>

class CRWLockCom
{
public:
	CRWLockCom();
	~CRWLockCom();
	
	void Create();
	void Destroy();
	
	BOOL32 TakeRdLock(BOOL32 bBlock = TRUE);
	BOOL32 TakeWrLock(BOOL32 bBlock = TRUE);
	void Give();

private:
	BOOL32 m_bInit;
	pthread_rwlock_t m_tRWlock;
};

//读写锁，离开作用域自动解锁
enum
{
	enLockRd,
	enLockWr,
	enLockNone
};
class CAutoRWLockCom
{
public:
	CAutoRWLockCom(CRWLockCom* pRWLock, u32 dwLockType, BOOL32 bBlock = TRUE);
	~CAutoRWLockCom();
	BOOL32 IsLocked();  //查询是否获取到了锁

private:
	CRWLockCom* m_pRWLock;
	BOOL32      m_bLock;
	u32         m_dwLockType;  //读锁或写锁
};

CRWLockCom::CRWLockCom()
{
    m_bInit = FALSE;
}

CRWLockCom::~CRWLockCom()
{
    Destroy();
}

void CRWLockCom::Create()
{
    if (m_bInit)
    {
        return;
    }
    
    s32 nRet = pthread_rwlock_init(&m_tRWlock,NULL);
    if (nRet != 0)
    {
        MError("#### pthread_rwlock_init error ####\n");
        return;
    }

    MDebug("pthread_rwlock_init success\n");
    m_bInit = TRUE;
}

void CRWLockCom::Destroy()
{
    if (!m_bInit)
    {
        return;
    }

    s32 nRet = pthread_rwlock_destroy(&m_tRWlock);
    if (nRet != 0)
    {
        MError("#### pthread_rwlock_destroy error ####\n");
    }

    m_bInit = FALSE;
}

BOOL32 CRWLockCom::TakeRdLock(BOOL32 bBlock)
{
    if (!m_bInit)
    {
        return FALSE;
    }

    s32 nRet = 0;
    if (!bBlock)
    {
        nRet = pthread_rwlock_tryrdlock(&m_tRWlock);
    }
    else
    {
        nRet = pthread_rwlock_rdlock(&m_tRWlock);
    }

    if (nRet != 0)
    {
        return FALSE;
    }

    return TRUE;
}

BOOL32 CRWLockCom::TakeWrLock(BOOL32 bBlock)
{
    if (!m_bInit)
    {
        return FALSE;
    }

    s32 nRet = 0;
    if (!bBlock)
    {
        nRet = pthread_rwlock_trywrlock(&m_tRWlock);
    }
    else
    {
        nRet = pthread_rwlock_wrlock(&m_tRWlock);
    }

    if (nRet != 0)
    {
        return FALSE;
    }

    return TRUE;
}

void CRWLockCom::Give()
{
    if (!m_bInit)
    {
        return;
    }
    
    s32 nRet = pthread_rwlock_unlock(&m_tRWlock);
    if (nRet != 0)
    {
        MError("#### pthread_rwlock_unlock error ####\n");
    }
}

CAutoRWLockCom::CAutoRWLockCom(CRWLockCom* pRWLock, u32 dwLockType, BOOL32 bBlock)
{
    m_pRWLock = NULL;
    m_bLock = FALSE;
    m_dwLockType = enLockNone;
    
    if (pRWLock == NULL)
    {
        MError("#### pRWLock NULL ####\n");
        return;
    }

    if ((dwLockType != enLockRd) && (dwLockType != enLockWr))
    {
        MError("#### dwLockType error ####\n");
        return;
    }

    BOOL32 bRet = TRUE;
    if (dwLockType == enLockRd)
    {
        bRet = pRWLock->TakeRdLock(bBlock);
    }
    else
    {
        bRet = pRWLock->TakeWrLock(bBlock);
    }

    if (!bRet)
    {
        return;
    }

    m_pRWLock = pRWLock;
    m_bLock = TRUE;
    m_dwLockType = dwLockType;
}

CAutoRWLockCom::~CAutoRWLockCom()
{
    if (m_pRWLock != NULL)
    {
        if (m_bLock)
        {
            m_pRWLock->Give();
        }
    }

    m_pRWLock = NULL;
    m_bLock = FALSE;
    m_dwLockType = enLockNone;
}

BOOL32 CAutoRWLockCom::IsLocked()
{
    return m_bLock;
}

#endif