#include "stdafx.h"
#include <algorithm>
//#include <QDir> 
//#include <QDebug>
#include "HXBufferPool.h"

const static UINT64 g_cs_u64BufferSize = 1024 * 1024;
const static UINT64 g_cs_u64BufferNumber = 32;

CHXBufferPool::CHXBufferPool() :
	m_u64BufferSize(0)
	, m_u64BufferNumber(g_cs_u64BufferNumber)
	, m_u32Current(0)
	, m_u64PoolLength(0)
{
}

CHXBufferPool::~CHXBufferPool()
{
	Clear();
}

void CHXBufferPool::Initialization()
{
	if (0 != m_u64BufferNumber && m_u64BufferNumber == m_listBufferPool.size())
	{
		return;
	}
	m_u64BufferSize = g_cs_u64BufferSize;
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	m_u64BufferNumber = sysInfo.dwNumberOfProcessors*2;

	for (size_t i = 0; i < m_u64BufferNumber; i++)
	{
		CHXBufferDataPtr pBuffer = CHXBufferDataPtr(new CHXBufferData);
		pBuffer->InitLock(m_u64BufferSize);
		m_listBufferPool.push_back(pBuffer);
	}
	//qInfo() << "BufferPool Max Size:" << std::wstring::number(m_u64BufferNumber*m_u64BufferSize);
	//qInfo() << "BufferPool Number:" << std::wstring::number(m_u64BufferNumber);
	//qInfo() << "BufferPool Page Size:" << std::wstring::number(m_u64BufferSize);
	m_u32Current = 0;
	m_u64PoolLength = 0;
	//m_bValid = true;
}

void CHXBufferPool::Clear()
{
	m_listBufferPool.clear();
	m_u32Current = 0;
	m_u64BufferSize = 0;
	m_u64BufferNumber = 0;
	m_u64PoolLength = 0;
}

LPBYTE CHXBufferPool::GetPoolBuffer(LARGE_INTEGER i64FilePointer, DWORD i32FileRecordSize, DWORD & i32Readsize)
{
#ifdef _HX_USE_QT_
	QReadLocker locker(&m_lock);
#else
	std::shared_lock<std::shared_mutex> lck(m_lock);
#endif // _HX_USE_QT_
	
	LPBYTE pBuffer = nullptr;

#ifdef _HX_USE_QT_
	for (size_t i = 0; i < m_listBufferPool.length(); i++)
	{
		pBuffer = m_listBufferPool[i]->GetBufferLock(i64FilePointer, i32FileRecordSize, i32Readsize);
		if (nullptr != pBuffer)
		{
			m_listBufferPool[i]->m_ThreaID = GetCurrentThreadId();
			break;
		}
	}
#else
	std::for_each(m_listBufferPool.begin(), m_listBufferPool.end(), [i64FilePointer, i32FileRecordSize, &i32Readsize, &pBuffer](CHXBufferDataPtr pListBuffer) {
		pBuffer = pListBuffer->GetBufferLock(i64FilePointer, i32FileRecordSize, i32Readsize);
		if (nullptr != pBuffer){
			pListBuffer->m_ThreaID = GetCurrentThreadId();
			return;
		}
	});
#endif // _HX_USE_QT_
	return pBuffer;
}

CHXBufferDataPtr CHXBufferPool::GetBuffer()
{
	std::shared_lock<std::shared_mutex> lck(m_lock);
	//QReadLocker locker(&m_lock);
	CHXBufferDataPtr pBuffer = nullptr;
	UINT u32Index = m_u32Current;
	for (size_t i = 0; 1; i++)
	{
		//assert(i < m_u64BufferNumber*10);
		m_u32Current = m_u32Current % m_u64BufferNumber;
		pBuffer = m_listBufferPool[m_u32Current++];
		if (true == pBuffer->ClockReplacementLock())
		{
			pBuffer->SetUsedLock(true);
			m_u64PoolLength++;
			pBuffer->m_ThreaID = GetCurrentThreadId();
			break;
		}
		if (0 == i % m_u64BufferNumber)
		{
			Sleep(0*1000);
		}
	}
	assert(nullptr != pBuffer);
	return pBuffer;
}

void CHXBufferPool::RealseBuffer(CHXBufferDataPtr pBuffer)
{
	m_u64PoolLength--;
	pBuffer->SetUsedLock(false);
}

UINT32 CHXBufferPool::Length()
{
	return m_u64PoolLength;
}

UINT32 CHXBufferPool::Size()
{
	return m_u64BufferNumber;
}

UINT32 CHXBufferPool::RemainSize()
{
	return m_u64BufferNumber - m_u64PoolLength;
}

void CHXBufferPool::FreeBuffer(LPBYTE pBuffer)
{
#ifdef _HX_USE_QT_
	for (size_t i = 0; i < m_listBufferPool.length(); i++)
	{
		if (m_listBufferPool[i]->FreeBufferLock(pBuffer))
		{
			break;
		}
	}
#else
	std::for_each(m_listBufferPool.begin(), m_listBufferPool.end(), [pBuffer](CHXBufferDataPtr pListBuffer) {
		if (pListBuffer->FreeBufferLock(pBuffer)){
			return;
		}
		});
#endif // _HX_USE_QT_
}

//void CHXBufferPool::FreeUsed(CHXBufferDataPtr pBuffer)
//{
//	SetUsedLock(pBuffer, false);
//}
//
//void CHXBufferPool::SetUsedLock(CHXBufferDataPtr pBuffer, bool bUsed)
//{
//	pBuffer->SetUsedLock(bUsed);
//}

void CHXBufferPool::SetValid(bool bValid)
{
#ifdef _HX_USE_QT_
	for (size_t i = 0; i < m_listBufferPool.length(); i++)
	{
		m_listBufferPool[i]->ResetLock();
	}
#else
	std::for_each(m_listBufferPool.begin(), m_listBufferPool.end(), [](CHXBufferDataPtr pBuffer) {
		pBuffer->ResetLock();
		});
#endif // _HX_USE_QT_
	//m_bValid = bValid;
}
