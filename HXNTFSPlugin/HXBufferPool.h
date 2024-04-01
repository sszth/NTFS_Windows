#pragma once
/*******************************************************************************************************************************************
*	2021年4月30日16:51:42	缓存 负责读取磁盘数据并建立缓存
*	2021年4月30日16:51:53	目前使用智能指针的引用计数判断是否被占用  大于1被占用
*	2021年5月5日21:15:46	CHXBufferDataPtr后续考虑自定义智能指针，可以直接用引用计数判断.锁的问题可用读写锁来进行优化，并且可以考虑在buffer中加锁
*	2021年5月5日22:07:29	单个buffer必须在m_u64BufferSize以下
*	
*	
*	
*	
*	
*	
*	
*	
*	
*
*	内存泄漏检查
*******************************************************************************************************************************************/
#include <QMutex> 
//#include <atomic>

class CHXBufferPool
{
public:
	CHXBufferPool();
	~CHXBufferPool();

public:
	void Initialization();
	void Clear();

	LPBYTE GetPoolBuffer(LARGE_INTEGER i64FilePointer, DWORD i32FileRecordSize, DWORD & i32Readsiz);

public:
	/**
	 *
	 *  函数描述:
	 *				获取可用缓冲,并占用
	 *
	 *	@return 返回结果
	 */
	CHXBufferDataPtr GetBuffer();


	/**
	 *
	 *  函数描述:
	 *				释放Buffer
	 *
	 *	@return 返回结果
	 */
	void RealseBuffer(CHXBufferDataPtr pBuffer);

	/**
	 *
	 *  函数描述:
	 *				返回占用数
	 *
	 *	@return 返回结果
	 */
	UINT32 Length();

	/**
	 *
	 *  函数描述:
	 *				返回大小
	 *
	 *	@return 返回结果
	 */
	UINT32 Size();

	/**
	 *
	 *  函数描述:
	 *				返回剩余空间
	 *
	 *	@return 返回结果
	 */
	UINT32 RemainSize();
	void FreeBuffer(LPBYTE pBuffer);
	//void FreeUsed(CHXBufferDataPtr pBuffer);
	//void SetUsedLock(CHXBufferDataPtr pBuffer, bool bUsed = true);

	void SetValid(bool bValid = false);
private:
	QReadWriteLock		m_lock;
	//	单个缓冲区大小
	UINT32				m_u64BufferSize;
	//	缓冲区个数
	UINT32				m_u64BufferNumber;
	//	池中长度
	UINT64				m_u64PoolLength;
	UINT32				m_u32Current;
	ListHXBufferPtr		m_listBufferPool;
	//QMutex				m_mutexPool;
	//std::atomic<bool>	m_bValid;		// true is valid
};

