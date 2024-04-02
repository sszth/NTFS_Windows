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
	 *  函数描述:
	 *				建立缓存
	 */
	int		BuildCache(const std::wstring& strPartitionName);
	int		BuildCache(const LCNType& lcn);

	/**
	 *
	 *  函数描述:
	 *				获取BPB信息
	 */
	CHXBPBPtr GetBPB();

	/**
	 *
	 *  函数描述:
	 *				获取一个簇大小
	 *
	 */
	UINT64	GetClusterSize();

	/**
	 *
	 *  函数描述:
	 *				获取每个MFT大小
	 */
	UINT32	GetMFTSize();



	//LPHXFileRecordHeader GetSerialNumberFileRecord(UINT64 u64MFTID);

	//LCNType & GetLCN();

	/**
	 *
	 *  函数描述:
	 *				获取指定位置的buffer
	 */
	LPBYTE	ReadBuffer(LARGE_INTEGER i64FilePointer, DWORD i32FileRecordSize, DWORD& i32Readsize, DWORD i32MoveMethod);

	void FreeBuffer(LPBYTE pBuffer);
	void FreeUsed(CHXBufferDataPtr pBuffer);

	void Refresh();
private:
	/**
	 *
	 *  函数描述:
	 *				获取$MFT
	 */
	LPHXFileRecordHeader		GetFileRecordHeaderMFT();

	/**
	 *
	 *  函数描述:
	 *				获取第5个mft(Root MFT)
	 *
	 */
	LPHXFileRecordHeader GetRootFileRecord();

	/**
	 *
	 *  函数描述:
					打开磁盘
	 *	strFileName	磁盘目录
	 *
	 *	@return 返回结果
	 */
	int	OpenDisk();

	/**
	 *
	 *  函数描述:
	 *				关闭磁盘，并将缓存释放
	 *	strFileName	磁盘目录
	 *
	 *	@return 返回结果
	 */
	int CloseDisk();

	HANDLE	GetDisk();

private:
	/**
	 *
	 *  函数描述:
	 *				BPB信息
	 *
	 *	@return 返回结果
	 */
	int GetBPBInfo();

	/**
	 *
	 *  函数描述:
	 *				计算MFT大小
	 *
	 *	@return 返回结果
	 */
	void CalcMFTSize();

	/**
	 *
	 *  函数描述:
	 *				初始化缓冲区
	 *
	 *	@return 返回结果
	 */
	void InitBufferPool();

	/**
	 *
	 *  函数描述:
	 *				获取可用缓冲
	 *
	 *	@return 返回结果
	 */
	CHXBufferDataPtr GetBuffer(DWORD i32FileRecordSize, DWORD& i32Readsize);
public:
	void Clear();





private:


	/**
	 *
	 *  函数描述:
					读取磁盘信息
	 *
	 *	@return 返回结果
	 */
	int ReadDiskBuffer(LARGE_INTEGER i64FilePointer, DWORD i32FileRecordSize, DWORD& i32Readsize, DWORD i32MoveMethod, LPBYTE pBuffer);

	void SetUsed(CHXBufferDataPtr pBuffer, bool bUsed = true);
private:
	UINT32			m_u32MFTSize;
	HANDLE			m_hDisk;
	std::wstring			m_strPartitionName;
	CHXBPBPtr		m_pBPB;
#ifdef _HX_ONLYONEBUFFERPOOL_
	static CHXBufferPool	m_bufferPool;

#else
	CHXBufferPool	m_bufferPool;

#endif // _HX_ONLYONEBUFFERPOOL_



	LCNType			m_vecType;
#ifdef _HX_USE_QT_
	QReadWriteLock	m_lock;
#else
	std::shared_mutex m_lock;
#endif // _HX_USE_QT_
	//LCNType					m_vecLCN;
};

_HX_DECLAR_SHARED_POINTER(HXNTFSCache);
