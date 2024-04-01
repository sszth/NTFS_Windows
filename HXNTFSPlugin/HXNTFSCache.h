#pragma once

#include "HXNTFSType.h"
#include "HXBufferPool.h"



class HXNTFSCache
{
public:
	HXNTFSCache();
	~HXNTFSCache();

public:
	/**
	 *
	 *  ��������:
	 *				��������
	 */
	int		BuildCache(const QString & strPartitionName);
	int		BuildCache(const LCNType & lcn);

	/**
	 *
	 *  ��������:
	 *				��ȡBPB��Ϣ
	 */
	CHXBPBPtr GetBPB();

	/**
	 *
	 *  ��������:
	 *				��ȡһ���ش�С
	 *
	 */
	UINT64	GetClusterSize();

	/**
	 *
	 *  ��������:
	 *				��ȡÿ��MFT��С
	 */
	UINT32	GetMFTSize();


	
	//LPHXFileRecordHeader GetSerialNumberFileRecord(UINT64 u64MFTID);

	//LCNType & GetLCN();

	/**
	 *
	 *  ��������:
	 *				��ȡָ��λ�õ�buffer
	 */
	LPBYTE	ReadBuffer(LARGE_INTEGER i64FilePointer, DWORD i32FileRecordSize, DWORD & i32Readsize, DWORD i32MoveMethod);

	void FreeBuffer(LPBYTE pBuffer);
	void FreeUsed(CHXBufferDataPtr pBuffer);

	void Refresh();
private:
	/**
	 *
	 *  ��������:
	 *				��ȡ$MFT
	 */
	LPHXFileRecordHeader		GetFileRecordHeaderMFT();

	/**
	 *
	 *  ��������:
	 *				��ȡ��5��mft(Root MFT)
	 *
	 */
	LPHXFileRecordHeader GetRootFileRecord();

	/**
	 *
	 *  ��������:
					�򿪴���
	 *	strFileName	����Ŀ¼
	 *
	 *	@return ���ؽ��
	 */
	int	OpenDisk();

	/**
	 *
	 *  ��������:
	 *				�رմ��̣����������ͷ�
	 *	strFileName	����Ŀ¼
	 *
	 *	@return ���ؽ��
	 */
	int CloseDisk();

	HANDLE	GetDisk();

private:
	/**
	 *
	 *  ��������:
	 *				BPB��Ϣ
	 *
	 *	@return ���ؽ��
	 */
	int GetBPBInfo();

	/**
	 *
	 *  ��������:
	 *				����MFT��С
	 *
	 *	@return ���ؽ��
	 */
	void CalcMFTSize();

	/**
	 *
	 *  ��������:
	 *				��ʼ��������
	 *
	 *	@return ���ؽ��
	 */
	void InitBufferPool();

	/**
	 *
	 *  ��������:
	 *				��ȡ���û���
	 *
	 *	@return ���ؽ��
	 */
	CHXBufferDataPtr GetBuffer(DWORD i32FileRecordSize, DWORD & i32Readsize);
public:
	void Clear();





private:


	/**
	 *
	 *  ��������:
					��ȡ������Ϣ
	 *
	 *	@return ���ؽ��
	 */
	int ReadDiskBuffer(LARGE_INTEGER i64FilePointer, DWORD i32FileRecordSize, DWORD & i32Readsize, DWORD i32MoveMethod, LPBYTE pBuffer);

	void SetUsed(CHXBufferDataPtr pBuffer, bool bUsed = true);
private:
	UINT32			m_u32MFTSize;
	HANDLE			m_hDisk;
	QString			m_strPartitionName;
	CHXBPBPtr		m_pBPB;
#ifdef _HX_ONLYONEBUFFERPOOL_
	static CHXBufferPool	m_bufferPool;

#else
	CHXBufferPool	m_bufferPool;

#endif // _HX_ONLYONEBUFFERPOOL_



	LCNType			m_vecType;
	QReadWriteLock	m_lock;


	//LCNType					m_vecLCN;

};

_HX_DECLAR_SHARED_POINTER(HXNTFSCache);
