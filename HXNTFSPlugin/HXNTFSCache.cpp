#include "stdafx.h"
#include <QDebug>
#include "HXNTFSCache.h"

const static int g_cs_i32BPBSize = 512;

#ifdef _HX_ONLYONEBUFFERPOOL_
CHXBufferPool	HXNTFSCache::m_bufferPool;
#endif // _HX_ONLYONEBUFFERPOOL_

HXNTFSCache::HXNTFSCache()
	:m_pBPB(nullptr)
{
}

HXNTFSCache::~HXNTFSCache()
{
}

// 初始化缓冲区
int	HXNTFSCache::BuildCache(const QString & strPartitionName)
{
	if (false == m_pBPB.isNull())
	{
		return 0;
	}
	m_strPartitionName = strPartitionName;
	m_bufferPool.Initialization();


	DWORD i32Error = 0;
	if (-1 == OpenDisk())
	{
		i32Error = GetLastError();
		return -1;
	}
	
	GetBPBInfo();
	CalcMFTSize();
	return 0;
}

int HXNTFSCache::BuildCache(const LCNType & vecLCN)
{
	m_vecType = vecLCN;
	return 0;
}

UINT64 HXNTFSCache::GetClusterSize()
{
	return (UINT64)m_pBPB->m_i16SectorSize * m_pBPB->m_i8ClusterSectorSize;
}

void HXNTFSCache::InitBufferPool()
{
	m_bufferPool.Initialization();
}

CHXBufferDataPtr HXNTFSCache::GetBuffer(DWORD i32FileRecordSize, DWORD & i32Readsize)
{
	CHXBufferDataPtr pBuffer = m_bufferPool.GetBuffer();
	UINT64 u64SpaceSize = pBuffer->GetSpaceSizeLock();
	if (i32FileRecordSize > u64SpaceSize)
	{
		i32Readsize = u64SpaceSize;
	}
	else
	{
		i32Readsize = i32FileRecordSize;
	}
	Q_ASSERT(nullptr != pBuffer);
	return pBuffer;
}

void HXNTFSCache::Clear()
{
}



int HXNTFSCache::ReadDiskBuffer(LARGE_INTEGER i64FilePointer, DWORD i32FileRecordSize, DWORD & i32Readsize, DWORD i32MoveMethod, LPBYTE pBuffer)
{
	if (INVALID_SET_FILE_POINTER == SetFilePointer(GetDisk(), i64FilePointer.LowPart, &i64FilePointer.HighPart, i32MoveMethod))
	{
		Q_ASSERT(0);
		return -1;
	}
	if (FALSE == ::ReadFile(GetDisk(), pBuffer, i32FileRecordSize, &i32Readsize, NULL))
	{
		Q_ASSERT(0);
		return -1;
	}
	return 0;
}

void HXNTFSCache::SetUsed(CHXBufferDataPtr pBuffer, bool bUsed)
{
	pBuffer->SetUsedLock(bUsed);
	//m_bufferPool.SetUsedLock(pBuffer, bUsed);
}

void HXNTFSCache::FreeBuffer(LPBYTE pBuffer)
{
	QWriteLocker locker(&m_lock);
	m_bufferPool.FreeBuffer(pBuffer);
}

void HXNTFSCache::FreeUsed(CHXBufferDataPtr pBuffer)
{
	SetUsed(pBuffer, false);
}

void HXNTFSCache::Refresh()
{
	m_bufferPool.SetValid();
}

LPBYTE HXNTFSCache::ReadBuffer(LARGE_INTEGER i64FilePointer, DWORD i32FileRecordSize, DWORD & i32Readsize, DWORD i32MoveMethod)
{
	QWriteLocker locker(&m_lock);
	LPBYTE pBuffer = m_bufferPool.GetPoolBuffer(i64FilePointer, i32FileRecordSize, i32Readsize);
	if (nullptr != pBuffer)
	{
		return pBuffer;
	}
	CHXBufferDataPtr pBufferData = GetBuffer(i32FileRecordSize, i32FileRecordSize);

	pBuffer = pBufferData->WriteBufferLock();
	if (-1 == ReadDiskBuffer(i64FilePointer, pBufferData->GetSpaceSize(), i32Readsize, i32MoveMethod, pBuffer))
	{
		pBufferData->UnLock();
		FreeUsed(pBufferData);
		Q_ASSERT_X(0, "Error", "Read Buffer Failed!");
		return nullptr;
	}
	pBufferData->SetStart(i64FilePointer.QuadPart);
	pBufferData->SetRealSize(i32Readsize);
	pBufferData->UnLock();

	return pBuffer;
}

CHXBPBPtr HXNTFSCache::GetBPB()
{
	return m_pBPB;
}

UINT32 HXNTFSCache::GetMFTSize()
{
	return m_u32MFTSize;
}

LPHXFileRecordHeader HXNTFSCache::GetFileRecordHeaderMFT()
{
	LARGE_INTEGER i64MFTStart = {};
	i64MFTStart.QuadPart = m_pBPB->m_i64MFTStartCluster * GetClusterSize();
	
	DWORD u32Readsize = 0;

	LPBYTE pBuffer = ReadBuffer(i64MFTStart, GetClusterSize(), u32Readsize, FILE_BEGIN);
	if (nullptr == pBuffer)
	{
		return nullptr;
	}
	LPHXFileRecordHeader pFileRecordRoot = LPHXFileRecordHeader(pBuffer);
	return pFileRecordRoot;
}

LPHXFileRecordHeader HXNTFSCache::GetRootFileRecord()
{
	LARGE_INTEGER i64MFTStart = {};
	i64MFTStart.QuadPart = m_pBPB->m_i64MFTStartCluster * GetClusterSize() + GetMFTSize() * 5;

	DWORD u32Readsize = 0;
	LPBYTE pBuffer = ReadBuffer(i64MFTStart, GetClusterSize(), u32Readsize, FILE_BEGIN);
	if (nullptr == pBuffer)
	{
		return nullptr;
	}
	LPHXFileRecordHeader pFileRecordRoot = LPHXFileRecordHeader(pBuffer);
	return pFileRecordRoot;
}

int HXNTFSCache::OpenDisk()
{
	QString strPartition;
	strPartition = u8"\\\\.\\";
	strPartition += m_strPartitionName;

	m_hDisk = CreateFile(strPartition.toStdWString().c_str(),
		GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (INVALID_HANDLE_VALUE == m_hDisk)
	{
		qCritical() << "Open Disk Failed:" << strPartition;
		return -1;
	}
	return 0;
}

int HXNTFSCache::CloseDisk()
{
	if (INVALID_HANDLE_VALUE == m_hDisk)
	{
		CloseHandle(m_hDisk);
		m_hDisk = INVALID_HANDLE_VALUE;
	}
	return 0;
}

HANDLE HXNTFSCache::GetDisk()
{
	return m_hDisk;
}

int HXNTFSCache::GetBPBInfo()
{
	DWORD u32ReadSize = 0;
	CHXBufferDataPtr pBuffer = GetBuffer(g_cs_i32BPBSize, u32ReadSize);
	if (u32ReadSize < g_cs_i32BPBSize)
	{
		Q_ASSERT(0);
	}
	pBuffer->ReadBufferLock();
	pBuffer->SetStart(0);
	pBuffer->UnLock();

	DWORD readsize = 0;
	LARGE_INTEGER i64FilePinter;
	i64FilePinter.LowPart = 0;
	i64FilePinter.HighPart = 0;
	if (INVALID_SET_FILE_POINTER == SetFilePointer(GetDisk(), i64FilePinter.LowPart, &i64FilePinter.HighPart, FILE_BEGIN))
	{
		return -1;
	}

	LPBYTE pBufferData = pBuffer->WriteBufferLock();
	if (FALSE == ::ReadFile(GetDisk(), pBufferData, g_cs_i32BPBSize, &readsize, NULL) || g_cs_i32BPBSize != readsize)
	{
		auto hx = GetLastError();
		return -1;
	}
	pBuffer->SetRealSize(readsize);
	pBuffer->UnLock();

	// BPB没有512字节
	m_pBPB = QSharedPointer<CHXBPB>(new CHXBPB);

	memcpy_s(m_pBPB.get(), sizeof(CHXBPB), pBufferData, sizeof(CHXBPB));

	pBuffer->FreeBufferLock();
	//m_bufferPool.FreeBufferLock(pBuffer->m_pBuffer);
	//m_bufferPool.RealseBuffer(pBuffer);

	return 0;
}

void HXNTFSCache::CalcMFTSize()
{
	UINT32 i32FileRecordSize = 0;
	if ((INT8)m_pBPB->m_i8FileRecord > 0)
	{
		i32FileRecordSize = m_pBPB->m_i8FileRecord * GetClusterSize();
	}
	else
	{
		INT8 i8FileRecordSize = 0 - m_pBPB->m_i8FileRecord;
		i32FileRecordSize = pow(2, i8FileRecordSize);
	}
	m_u32MFTSize = i32FileRecordSize;
}

