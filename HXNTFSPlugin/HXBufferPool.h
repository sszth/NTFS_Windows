#pragma once
/*******************************************************************************************************************************************
*	2021��4��30��16:51:42	���� �����ȡ�������ݲ���������
*	2021��4��30��16:51:53	Ŀǰʹ������ָ������ü����ж��Ƿ�ռ��  ����1��ռ��
*	2021��5��5��21:15:46	CHXBufferDataPtr���������Զ�������ָ�룬����ֱ�������ü����ж�.����������ö�д���������Ż������ҿ��Կ�����buffer�м���
*	2021��5��5��22:07:29	����buffer������m_u64BufferSize����
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
*	�ڴ�й©���
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
	 *  ��������:
	 *				��ȡ���û���,��ռ��
	 *
	 *	@return ���ؽ��
	 */
	CHXBufferDataPtr GetBuffer();


	/**
	 *
	 *  ��������:
	 *				�ͷ�Buffer
	 *
	 *	@return ���ؽ��
	 */
	void RealseBuffer(CHXBufferDataPtr pBuffer);

	/**
	 *
	 *  ��������:
	 *				����ռ����
	 *
	 *	@return ���ؽ��
	 */
	UINT32 Length();

	/**
	 *
	 *  ��������:
	 *				���ش�С
	 *
	 *	@return ���ؽ��
	 */
	UINT32 Size();

	/**
	 *
	 *  ��������:
	 *				����ʣ��ռ�
	 *
	 *	@return ���ؽ��
	 */
	UINT32 RemainSize();
	void FreeBuffer(LPBYTE pBuffer);
	//void FreeUsed(CHXBufferDataPtr pBuffer);
	//void SetUsedLock(CHXBufferDataPtr pBuffer, bool bUsed = true);

	void SetValid(bool bValid = false);
private:
	QReadWriteLock		m_lock;
	//	������������С
	UINT32				m_u64BufferSize;
	//	����������
	UINT32				m_u64BufferNumber;
	//	���г���
	UINT64				m_u64PoolLength;
	UINT32				m_u32Current;
	ListHXBufferPtr		m_listBufferPool;
	//QMutex				m_mutexPool;
	//std::atomic<bool>	m_bValid;		// true is valid
};

