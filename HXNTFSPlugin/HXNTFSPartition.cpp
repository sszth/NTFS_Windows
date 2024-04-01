#include "stdafx.h"
//#include <Qset>
//#include <QPair>
//#include <QDebug>

#include "HXNTFSPartition.h"

//	BPB大小
const static int g_cs_i32BPBSize = 512;
const static bool g_cs_bFindMFTBase = false;
int CHXNTFSPartition::Initlize()
{
	return 0;
}

void CHXNTFSPartition::Clear()
{
}

const std::wstring& CHXNTFSPartition::GetPartitionName()
{
	return m_strPartitionName;
}

int CHXNTFSPartition::FindDirectory(LPHXReadDirectory pDirectory, VecFileInfo& listFileIno)
{
	//	1.判断是否为根目录且第一次查找
	UINT64 i64MFTLocation;
	if (0 == GetDirectoryMFT(pDirectory->m_wstrDirectory, i64MFTLocation))
	{
		//qInfo() << "Partition Root File Cmd Recevie :" << pDirectory->m_wstrDirectory;
		ParseRootDirectoryMFT(pDirectory, listFileIno);
		return 0;
	}

	//qInfo() << "Partition File Cmd Recevie:" << pDirectory->m_wstrDirectory;
	if (-1 == FindDirectory(pDirectory, i64MFTLocation, listFileIno))
	{
		return -1;
	}

	return 0;
}

LPBYTE CHXNTFSPartition::ReadBuffer(LARGE_INTEGER i64FilePointer, DWORD i32FileRecordSize, DWORD& i32Readsize, DWORD i32MoveMethod, CHXBufferType i32Type)
{
	LPBYTE pBuffer = m_pCache->ReadBuffer(i64FilePointer, i32FileRecordSize, i32Readsize, i32MoveMethod);
	if (CHXBufferType_INDEX == i32Type)
	{
		LPHXIndexHeader pIndexHeader = (LPHXIndexHeader)pBuffer;

		if (pIndexHeader->m_i32Flag != 0x58444e49)
		{
			Q_ASSERT(0);
			int x = 0;
			x++;
		}
	}
	else if (CHXBufferType_MFT == i32Type)
	{
		LPHXFileRecordHeader pIndexHeader = (LPHXFileRecordHeader)pBuffer;

		if (false == (pIndexHeader->m_i8ArrMFT[0] == 0x46
			&& pIndexHeader->m_i8ArrMFT[1] == 0x49
			&& pIndexHeader->m_i8ArrMFT[2] == 0x4c
			&& pIndexHeader->m_i8ArrMFT[3] == 0x45))
		{
			Q_ASSERT(0);
			int x = 0;
			x++;
		}
	}
	switch (i32Type)
	{
	case CHXBufferType_INDEX:
	{
		//	实际使用大小
		UINT32 u32FileSize = min(i32Readsize, i32FileRecordSize);
		UINT32 u32FileIndex = 0;
		LPHXIndexHeader pIndexHeader = (LPHXIndexHeader)pBuffer;
		UINT32 u32SectorSize = m_pCache->GetBPB()->m_i16SectorSize;
		while (u32FileIndex < u32FileSize)
		{
			if (pIndexHeader->m_i32Flag != 0x58444e49)
			{
				break;
			}
			int i32 = pIndexHeader->m_i16UpdateSequenceNumberSize - 1;
			for (size_t i = 0; i < i32; i++)
			{
				UINT16 hx = *(UINT16*)((LPBYTE)pIndexHeader + u32SectorSize * (i + 1) - 2);
				if (pIndexHeader->m_u16UpdateSequence == *(UINT16*)((LPBYTE)pIndexHeader + u32SectorSize * (i + 1) - 2))
				{
					LPBYTE pIndexHeaderEnd = (LPBYTE)pIndexHeader + u32SectorSize * (i + 1) - 2;
					LPBYTE pIndexHeaderUpdateNumber = LPBYTE((&pIndexHeader->m_u16UpdateSequence) + i + 1);
#ifdef _DEBUG
					UINT16 i16hx = *(UINT16*)(pIndexHeaderEnd);
					UINT16 i16hx2 = *(UINT16*)(pIndexHeaderUpdateNumber);
#endif // _DEBUG
					memcpy_s(pIndexHeaderEnd, 2, pIndexHeaderUpdateNumber, 2);
				}
			}
			u32FileIndex += GetClusterSize();
			pIndexHeader = (LPHXIndexHeader)(pBuffer + u32FileIndex);
		}
	}
	break;
	case CHXBufferType_MFT:
	{
		// 只会取一个文件记录  所以更新序列只处理一个
		LPHXFileRecordHeader pMFT = (LPHXFileRecordHeader)pBuffer;
		UINT32 u32SectorSize = m_pCache->GetBPB()->m_i16SectorSize;
		int i32 = pMFT->m_u32FileRecordRealSize / u32SectorSize;
		for (size_t i = 0; i < i32; i++)
		{
			if (false == (pMFT->m_i8ArrMFT[0] == 0x46
				&& pMFT->m_i8ArrMFT[1] == 0x49
				&& pMFT->m_i8ArrMFT[2] == 0x4c
				&& pMFT->m_i8ArrMFT[3] == 0x45))
			{
				break;
			}
			if (pMFT->m_i16UpdateIndex == *(INT16*)((LPBYTE)pMFT + u32SectorSize * (i + 1) - 2))
			{
				memcpy_s((LPBYTE)pMFT + u32SectorSize * (i + 1) - 2, 2, ((INT16*)(&pMFT->m_i32UpdateArray) + i), 2);
			}
		}
	}
	break;
	case CHXBufferType_Unknown:
	default:
		break;
	}
	return pBuffer;
}

int CHXNTFSPartition::ParseMFTListBody(LPHXFileRecordListBody pListBody,
	UINT64 u64AllSize,
	std::wstring strDir,
	LCNType& listLCN,
	BitMapType& listBitMap,
	LPHXAttribute& pAttributeIndexRoot,
	SetMFTNumber& setAlreadyParseMFT)
{
	UINT64 u64RealReadSize = 0;
	while (pListBody && u64RealReadSize < u64AllSize)
	{
		INT64 i64MFTSerialNumber = pListBody->m_u64AttributeMFT & 0x0000FFFFFFFFFFFF;
		if (setAlreadyParseMFT.end() == setAlreadyParseMFT.find(i64MFTSerialNumber))
		{
			setAlreadyParseMFT.insert(i64MFTSerialNumber);
			ParseMFTListBody(strDir, listLCN, listBitMap, i64MFTSerialNumber, pAttributeIndexRoot, setAlreadyParseMFT);
		}
		u64RealReadSize += pListBody->m_u16Length;
		pListBody = (LPHXFileRecordListBody)((LPBYTE)pListBody + pListBody->m_u16Length);
	}
	return 0;
}

//int CHXNTFSPartition::ParseMFTListBody(LPHXFileRecordListBody pListBody,
//	UINT64 u64AllSize,
//	std::wstring strDir,
//	std::vector<LPHXAttribute> & vecAttribute,
//	SetMFTNumber& setAlreadyParseMFT)
//{
//	UINT64 u64RealReadSize = 0L;
//	while (pListBody && u64RealReadSize < u64AllSize)
//	{
//		INT64 i64MFTSerialNumber = pListBody->m_u64AttributeMFT & 0x0000FFFFFFFFFFFF;
//		if (setAlreadyParseMFT.end() == setAlreadyParseMFT.find(i64MFTSerialNumber))
//		{
//			setAlreadyParseMFT.insert(i64MFTSerialNumber);
//			ParseMFTListBody(strDir, i64MFTSerialNumber, vecAttribute, setAlreadyParseMFT);
//		}
//		u64RealReadSize += pListBody->m_u16Length;
//		pListBody = (LPHXFileRecordListBody)((LPBYTE)pListBody + pListBody->m_u16Length);
//	}
//	return 0;
//}

int CHXNTFSPartition::ParseMFTList(LPHXAttribute pAttribute,
	std::wstring strDir,
	LCNType& listLCN,
	BitMapType& listBitMap,
	LPHXAttribute& pAttributeIndexRoot,
	SetMFTNumber& setAlreadyParseMFT)
{
	if (0 == pAttribute->m_u8PermanentFlag)
	{
		return ParseMFTListPermanent(pAttribute, strDir, listLCN, listBitMap, pAttributeIndexRoot, setAlreadyParseMFT);
	}
	else
	{
		return ParseMFTListVariable(pAttribute, strDir, listLCN, listBitMap, pAttributeIndexRoot, setAlreadyParseMFT);
	}
}

//int CHXNTFSPartition::ParseMFTList(LPHXAttribute pAttribute,
//	std::wstring strDir,
//	std::vector<LPHXAttribute> & vecAttribute,
//	SetMFTNumber& setAlreadyParseMFT)
//{
//	if (0 == pAttribute->m_u8PermanentFlag)
//	{
//		return ParseMFTListPermanent(pAttribute, strDir, vecAttribute, setAlreadyParseMFT);
//	}
//	else
//	{
//		return ParseMFTListVariable(pAttribute, strDir, vecAttribute, setAlreadyParseMFT);
//	}
//}

// 20H常驻没有属性名称
int CHXNTFSPartition::ParseMFTListPermanent(LPHXAttribute pAttribute,
	std::wstring strDir,
	LCNType& listLCN,
	BitMapType& listBitMap,
	LPHXAttribute& pAttributeIndexRoot,
	SetMFTNumber& setAlreadyParseMFT)
{
	DWORD u32RealReadSize = pAttribute->GetHeaderSize();
	LPHXFileRecordListBody pListBody = (LPHXFileRecordListBody)((LPBYTE)pAttribute + u32RealReadSize);
	return ParseMFTListBody(pListBody, pAttribute->GeAllSize() - pAttribute->GetHeaderSize(), strDir, listLCN, listBitMap, pAttributeIndexRoot, setAlreadyParseMFT);
	//while (pListBody && u32RealReadSize < pAttribute->GeAllSize())
	//{
	//	INT64 i64MFTSerialNumber = pListBody->m_u64AttributeMFT & 0x0000FFFFFFFFFFFF;
	//	if (setAlreadyParseMFT.end() == setAlreadyParseMFT.find(i64MFTSerialNumber))
	//	{
	//		setAlreadyParseMFT.insert(i64MFTSerialNumber);
	//		ParseMFTListBody(strDir, listLCN, listBitMap, i64MFTSerialNumber, pAttributeIndexRoot, setAlreadyParseMFT);
	//	}
	//	u32RealReadSize += pListBody->m_u16Length;
	//	pListBody = (LPHXFileRecordListBody)((LPBYTE)pListBody + pListBody->m_u16Length);
	//}
	//return 0;
}

//int CHXNTFSPartition::ParseMFTListPermanent(LPHXAttribute pAttribute,	
//	std::wstring strDir,
//	std::vector<LPHXAttribute> & vecAttribute,
//	SetMFTNumber& setAlreadyParseMFT)
//{
//	DWORD u32RealReadSize = pAttribute->GetHeaderSize();
//	LPHXFileRecordListBody pListBody = (LPHXFileRecordListBody)((LPBYTE)pAttribute + u32RealReadSize);
//	return ParseMFTListBody(pListBody, u32RealReadSize, strDir, vecAttribute, setAlreadyParseMFT);
//}

// 20H非常驻
int CHXNTFSPartition::ParseMFTListVariable(LPHXAttribute pAttribute,
	std::wstring strDir,
	LCNType& listLCN,
	BitMapType& listBitMap,
	LPHXAttribute& pAttributeIndexRoot,
	SetMFTNumber& setAlreadyParseMFT)
{
	//	20的data run
	LCNType listLCNTmp;
	ParseData(pAttribute, listLCNTmp);
	int i32hxNumber = 0;
	LPBYTE pDataRunBuffer = nullptr;
	INT64 i64Offset = 0;
	UINT64 u64BodySize = pAttribute->m_unExpand.m_VariableAttribute.m_i64BodyRealSize;
	//	定位到数据运行 TODO:如果一条DataRun过大  readbuffer会超大
	for (auto iter = listLCNTmp.begin(); iter != listLCNTmp.end(); iter++)
	{
		LARGE_INTEGER i64FirstDataRunPointer;
		i64Offset += iter->first;
		i64FirstDataRunPointer.QuadPart = i64Offset * GetClusterSize();
		DWORD u32RealReadSize = 0;
		INT32 i32AllIndexSize = iter->second * GetClusterSize();
		i32AllIndexSize = min(u64BodySize, i32AllIndexSize);
		u64BodySize -= i32AllIndexSize;

		pDataRunBuffer = ReadBuffer(i64FirstDataRunPointer, i32AllIndexSize, u32RealReadSize, FILE_BEGIN, CHXBufferType_Unknown);
		if (nullptr == pDataRunBuffer)
		{
			auto hx = GetLastError();
			Q_ASSERT(0);
			return -1;
		}
		if (u32RealReadSize < i32AllIndexSize)
		{
			INT32 i32RemainSize = (i32AllIndexSize - u32RealReadSize);
			LPHXFileRecordListBody pListBody = (LPHXFileRecordListBody)pDataRunBuffer;

			while (pListBody && i32RemainSize > 0)
			{
				ParseMFTListBody(pListBody, u32RealReadSize, strDir, listLCN, listBitMap, pAttributeIndexRoot, setAlreadyParseMFT);
				m_pCache->FreeBuffer(pDataRunBuffer);
				if (i32RemainSize <= 0)
				{
					break;
				}
				i64FirstDataRunPointer.QuadPart = i64FirstDataRunPointer.QuadPart + u32RealReadSize;
				u32RealReadSize = 0;
				pDataRunBuffer = ReadBuffer(i64FirstDataRunPointer, i32RemainSize, u32RealReadSize, FILE_BEGIN, CHXBufferType_Unknown);
				Q_ASSERT(nullptr != pDataRunBuffer);
				if (i32RemainSize < u32RealReadSize)
				{
					u32RealReadSize = i32RemainSize;
					i32RemainSize = 0;
				}
				else
				{
					i32RemainSize = i32RemainSize - u32RealReadSize;
				}
				Q_ASSERT_X(pDataRunBuffer != nullptr, "error", "ParseDataRun is null!");
			}
		}
		else
		{
			LPHXFileRecordListBody pListBody = (LPHXFileRecordListBody)pDataRunBuffer;
			ParseMFTListBody(pListBody, i32AllIndexSize, strDir, listLCN, listBitMap, pAttributeIndexRoot, setAlreadyParseMFT);
			m_pCache->FreeBuffer(pDataRunBuffer);
		}
	}
	return 0;
}

//int CHXNTFSPartition::ParseMFTListVariable(LPHXAttribute pAttribute,
//	std::wstring strDir,
//	std::vector<LPHXAttribute> & vecAttribute,
//	SetMFTNumber& setAlreadyParseMFT)
//{
//	//	20的data run
//	LCNType listLCNTmp;
//	ParseData(pAttribute, listLCNTmp);
//	int i32hxNumber = 0;
//	LPBYTE pDataRunBuffer = nullptr;
//	INT64 i64Offset = 0;
//	//	定位到数据运行 TODO:如果一条DataRun过大  readbuffer会超大
//	for (auto iter = listLCNTmp.begin(); iter != listLCNTmp.end(); iter++)
//	{
//		LARGE_INTEGER i64FirstDataRunPointer;
//		i64Offset += iter->first;
//		i64FirstDataRunPointer.QuadPart = i64Offset * GetClusterSize();
//		DWORD u32RealReadSize = 0;
//		INT32 i32AllIndexSize = iter->second * GetClusterSize();
//
//		pDataRunBuffer = ReadBuffer(i64FirstDataRunPointer, i32AllIndexSize, u32RealReadSize, FILE_BEGIN, CHXBufferType_INDEX);
//		if (nullptr == pDataRunBuffer)
//		{
//			auto hx = GetLastError();
//			Q_ASSERT(0);
//			return -1;
//		}
//		if (u32RealReadSize < i32AllIndexSize)
//		{
//			INT32 i32RemainSize = (i32AllIndexSize - u32RealReadSize);
//			LPHXFileRecordListBody pListBody = (LPHXFileRecordListBody)pDataRunBuffer;
//
//			while (pListBody && i32RemainSize > 0)
//			{
//				ParseMFTListBody(pListBody, u32RealReadSize, strDir, vecAttribute, setAlreadyParseMFT);
//				m_pCache->FreeBuffer(pDataRunBuffer);
//
//				pDataRunBuffer = ReadBuffer(i64FirstDataRunPointer, i32AllIndexSize, u32RealReadSize, FILE_BEGIN, CHXBufferType_INDEX);
//				if (nullptr == pDataRunBuffer)
//				{
//					auto hx = GetLastError();
//					Q_ASSERT(0);
//					return -1;
//				}
//				if (i32RemainSize <= 0)
//				{
//					break;
//				}
//				i64FirstDataRunPointer.QuadPart = i64FirstDataRunPointer.QuadPart + u32RealReadSize;
//				u32RealReadSize = 0;
//				pDataRunBuffer = ReadBuffer(i64FirstDataRunPointer, i32RemainSize, u32RealReadSize, FILE_BEGIN, CHXBufferType_INDEX);
//				Q_ASSERT(nullptr != pDataRunBuffer);
//				if (i32RemainSize < u32RealReadSize)
//				{
//					u32RealReadSize = i32RemainSize;
//					i32RemainSize = 0;
//				}
//				else
//				{
//					i32RemainSize = i32RemainSize - u32RealReadSize;
//				}
//				Q_ASSERT_X(pDataRunBuffer != nullptr, "error", "ParseDataRun is null!");
//			}
//		}
//		else
//		{
//			LPHXFileRecordListBody pListBody = (LPHXFileRecordListBody)pDataRunBuffer;
//			ParseMFTListBody(pListBody, u32RealReadSize, strDir, vecAttribute, setAlreadyParseMFT); 
//			m_pCache->FreeBuffer(pDataRunBuffer);
//		}
//	}
//	return 0;
//}
int CHXNTFSPartition::ParseMFTListBodyOnlyData(LPHXFileRecordListBody pListBody, UINT64 u64AllSize, LPHXFindFileParam pFindFileParam, SetMFTNumber& setAlreadyParseMFT)
{
	UINT64 u64RealReadSize = 0;
	while (pListBody && u64RealReadSize < u64AllSize)
	{
		INT64 i64MFTSerialNumber = pListBody->m_u64AttributeMFT & 0x0000FFFFFFFFFFFF;
		if (setAlreadyParseMFT.end() == setAlreadyParseMFT.find(i64MFTSerialNumber))
		{
			setAlreadyParseMFT.insert(i64MFTSerialNumber);
			ParseMFTListBodyOnlyData(i64MFTSerialNumber, pFindFileParam, setAlreadyParseMFT);
		}
		u64RealReadSize += pListBody->m_u16Length;
		pListBody = (LPHXFileRecordListBody)((LPBYTE)pListBody + pListBody->m_u16Length);
	}
	return 0;
}

int CHXNTFSPartition::ParseMFTListOnlyData(LPHXAttribute pAttribute, LPHXFindFileParam pFindFileParam, SetMFTNumber& setAlreadyParseMFT)
{
	if (0 == pAttribute->m_u8PermanentFlag)
	{
		LPHXFileRecordListBody pListBody = (LPHXFileRecordListBody)((LPBYTE)pAttribute + pAttribute->GetHeaderSize());
		return ParseMFTListBodyOnlyData(pListBody, pAttribute->GeAllSize() - pAttribute->GetHeaderSize(), pFindFileParam, setAlreadyParseMFT);
	}
	else
	{
		//	20的data run
		LCNType listLCNTmp;
		ParseData(pAttribute, listLCNTmp);
		int i32hxNumber = 0;
		LPBYTE pDataRunBuffer = nullptr;
		INT64 i64Offset = 0;
		UINT64 u64BodySize = pAttribute->m_unExpand.m_VariableAttribute.m_i64BodyRealSize;
		//	定位到数据运行 TODO:如果一条DataRun过大  readbuffer会超大
		for (auto iter = listLCNTmp.begin(); iter != listLCNTmp.end(); iter++)
		{
			LARGE_INTEGER i64FirstDataRunPointer;
			i64Offset += iter->first;
			i64FirstDataRunPointer.QuadPart = i64Offset * GetClusterSize();
			DWORD u32RealReadSize = 0;
			INT32 i32AllIndexSize = iter->second * GetClusterSize();
			i32AllIndexSize = min(u64BodySize, i32AllIndexSize);
			u64BodySize -= i32AllIndexSize;

			pDataRunBuffer = ReadBuffer(i64FirstDataRunPointer, i32AllIndexSize, u32RealReadSize, FILE_BEGIN, CHXBufferType_Unknown);
			if (nullptr == pDataRunBuffer)
			{
				auto hx = GetLastError();
				Q_ASSERT(0);
				return -1;
			}
			if (u32RealReadSize < i32AllIndexSize)
			{
				INT32 i32RemainSize = (i32AllIndexSize - u32RealReadSize);
				LPHXFileRecordListBody pListBody = (LPHXFileRecordListBody)pDataRunBuffer;

				while (pListBody && i32RemainSize > 0)
				{
					ParseMFTListBodyOnlyData(pListBody, u32RealReadSize, pFindFileParam, setAlreadyParseMFT);
					m_pCache->FreeBuffer(pDataRunBuffer);
					if (i32RemainSize <= 0)
					{
						break;
					}
					i64FirstDataRunPointer.QuadPart = i64FirstDataRunPointer.QuadPart + u32RealReadSize;
					u32RealReadSize = 0;
					pDataRunBuffer = ReadBuffer(i64FirstDataRunPointer, i32RemainSize, u32RealReadSize, FILE_BEGIN, CHXBufferType_Unknown);
					Q_ASSERT(nullptr != pDataRunBuffer);
					if (i32RemainSize < u32RealReadSize)
					{
						u32RealReadSize = i32RemainSize;
						i32RemainSize = 0;
					}
					else
					{
						i32RemainSize = i32RemainSize - u32RealReadSize;
					}
					assert(0);
					//Q_ASSERT_X(pDataRunBuffer != nullptr, "error", "ParseDataRun is null!");
				}
			}
			else
			{
				LPHXFileRecordListBody pListBody = (LPHXFileRecordListBody)pDataRunBuffer;
				ParseMFTListBodyOnlyData(pListBody, i32AllIndexSize, pFindFileParam, setAlreadyParseMFT);
				m_pCache->FreeBuffer(pDataRunBuffer);
			}
		}

		//qCritical() << u8"CHXNTFSPartition::ParseMFTListOnlyData 非常驻";
	}

	return 0;
}

int CHXNTFSPartition::ParseMFTListBodyOnlyData(LPHXFileRecordListBody pListBody, UINT64 u64AllSize, LCNType& vecLCN, SetMFTNumber& setAlreadyParseMFT)
{
	UINT64 u64RealReadSize = 0;
	while (pListBody && u64RealReadSize < u64AllSize)
	{
		INT64 i64MFTSerialNumber = pListBody->m_u64AttributeMFT & 0x0000FFFFFFFFFFFF;
		if (setAlreadyParseMFT.end() == setAlreadyParseMFT.find(i64MFTSerialNumber))
		{
			setAlreadyParseMFT.insert(i64MFTSerialNumber);
			ParseMFTListBodyOnlyData(i64MFTSerialNumber, vecLCN, setAlreadyParseMFT);
		}
		u64RealReadSize += pListBody->m_u16Length;
		pListBody = (LPHXFileRecordListBody)((LPBYTE)pListBody + pListBody->m_u16Length);
	}
	return 0;
}

int CHXNTFSPartition::ParseMFTListOnlyData(LPHXAttribute pAttribute, LCNType& vecLCN, SetMFTNumber& setAlreadyParseMFT)
{
	if (0 == pAttribute->m_u8PermanentFlag)
	{
		DWORD u32RealReadSize = pAttribute->GetHeaderSize();
		LPHXFileRecordListBody pListBody = (LPHXFileRecordListBody)((LPBYTE)pAttribute + u32RealReadSize);
		return ParseMFTListBodyOnlyData(pListBody, pAttribute->GeAllSize() - u32RealReadSize, vecLCN, setAlreadyParseMFT);
	}
	else
	{
		//	20的data run
		LCNType listLCNTmp;
		ParseData(pAttribute, listLCNTmp);
		int i32hxNumber = 0;
		LPBYTE pDataRunBuffer = nullptr;
		INT64 i64Offset = 0;
		UINT64 u64BodySize = pAttribute->m_unExpand.m_VariableAttribute.m_i64BodyRealSize;
		//	定位到数据运行 TODO:如果一条DataRun过大  readbuffer会超大
		for (auto iter = listLCNTmp.begin(); iter != listLCNTmp.end(); iter++)
		{
			LARGE_INTEGER i64FirstDataRunPointer;
			i64Offset += iter->first;
			i64FirstDataRunPointer.QuadPart = i64Offset * GetClusterSize();
			DWORD u32RealReadSize = 0;
			INT32 i32AllIndexSize = iter->second * GetClusterSize();
			i32AllIndexSize = min(u64BodySize, i32AllIndexSize);
			u64BodySize -= i32AllIndexSize;

			pDataRunBuffer = ReadBuffer(i64FirstDataRunPointer, i32AllIndexSize, u32RealReadSize, FILE_BEGIN, CHXBufferType_Unknown);
			if (nullptr == pDataRunBuffer)
			{
				auto hx = GetLastError();
				Q_ASSERT(0);
				return -1;
			}
			if (u32RealReadSize < i32AllIndexSize)
			{
				INT32 i32RemainSize = (i32AllIndexSize - u32RealReadSize);
				LPHXFileRecordListBody pListBody = (LPHXFileRecordListBody)pDataRunBuffer;

				while (pListBody && i32RemainSize > 0)
				{
					ParseMFTListBodyOnlyData(pListBody, u32RealReadSize, vecLCN, setAlreadyParseMFT);
					m_pCache->FreeBuffer(pDataRunBuffer);
					if (i32RemainSize <= 0)
					{
						break;
					}
					i64FirstDataRunPointer.QuadPart = i64FirstDataRunPointer.QuadPart + u32RealReadSize;
					u32RealReadSize = 0;
					pDataRunBuffer = ReadBuffer(i64FirstDataRunPointer, i32RemainSize, u32RealReadSize, FILE_BEGIN, CHXBufferType_Unknown);
					Q_ASSERT(nullptr != pDataRunBuffer);
					if (i32RemainSize < u32RealReadSize)
					{
						u32RealReadSize = i32RemainSize;
						i32RemainSize = 0;
					}
					else
					{
						i32RemainSize = i32RemainSize - u32RealReadSize;
					}
					//Q_ASSERT_X(pDataRunBuffer != nullptr, "error", "ParseDataRun is null!");
				}
			}
			else
			{
				LPHXFileRecordListBody pListBody = (LPHXFileRecordListBody)pDataRunBuffer;
				ParseMFTListBodyOnlyData(pListBody, i32AllIndexSize, vecLCN, setAlreadyParseMFT);
				m_pCache->FreeBuffer(pDataRunBuffer);
			}
		}
		//qCritical() << u8"CHXNTFSPartition::ParseMFTListOnlyData 非常驻";
	}

	return 0;
}

int CHXNTFSPartition::ParseMFTListBodyOnlyData(UINT u64MFTSerialNumber, LCNType& vecLCN, SetMFTNumber& setAlreadyParseMFT)
{
	LPHXFileRecordHeader pMFT = GetSerialNumberFileRecord(u64MFTSerialNumber);
	if (nullptr == pMFT)
	{
		assert(0);
		//qCritical() << "ParseMFTListBodyOnlyData Failed";
		return -1;
	}

	INT32 i32HasReadByte = pMFT->m_i16FirstAttribute;
	//	获取索引根和索引分配				
	while (i32HasReadByte < pMFT->m_u32FileRecordRealSize)
	{
		LPHXAttribute pAttributeTmp = (LPHXAttribute)((BYTE*)pMFT + i32HasReadByte);
		switch (pAttributeTmp->m_i32Flag)
		{
		case HXAttribute_Data:
		{
			ParseData(pAttributeTmp, vecLCN);
		}
		break;
		case HXAttribute_IndexRoot:
		case HXAttribute_IndexAllocation:
		case HXAttribute_Bitmap:
			break;
		default:
			break;
		}
		if (0 == pAttributeTmp->m_i32TotalLength)
		{
			break;
		}
		i32HasReadByte += pAttributeTmp->m_i32TotalLength;
	}
	i32HasReadByte = pMFT->m_i16FirstAttribute;
	//	获取索引根和索引分配				
	while (i32HasReadByte < pMFT->m_u32FileRecordRealSize)
	{
		LPHXAttribute pAttributeTmp = (LPHXAttribute)((BYTE*)pMFT + i32HasReadByte);
		switch (pAttributeTmp->m_i32Flag)
		{
		case HXAttribute_List:
			if (-1 == ParseMFTListOnlyData(pAttributeTmp, vecLCN, setAlreadyParseMFT))
			{
				assert(0);
				//qCritical() << "ParseMFTListBodyOnlyData->ParseMFTListOnlyData Failed";
				return -1;
			}
			break;
		case HXAttribute_IndexRoot:
		case HXAttribute_IndexAllocation:
		case HXAttribute_Bitmap:
			break;
		default:
			break;
		}
		if (0 == pAttributeTmp->m_i32TotalLength)
		{
			break;
		}
		i32HasReadByte += pAttributeTmp->m_i32TotalLength;
	}
	m_pCache->FreeBuffer((LPBYTE)pMFT);
	return 0;
}
//
//int CHXNTFSPartition::ParseMFTListBody(
//	std::wstring strDir,
//	UINT64 u64MFTSerialNumber,
//	std::vector<LPHXAttribute> &vecAttribute,
//	SetMFTNumber& setAlreadyParseMFT)
//{
//	LPHXFileRecordHeader pMFT = GetSerialNumberFileRecord(u64MFTSerialNumber);
//	if (nullptr == pMFT)
//	{
//		Q_ASSERT(0);
//		qCritical() << "ParseMFTListBody->GetSerialNumberFileRecord Failed";
//		return -1;
//	}
//
//	INT32 i32HasReadByte = pMFT->m_i16FirstAttribute;
//
//	//	获取索引根和索引分配				
//	while (i32HasReadByte < pMFT->m_u32FileRecordRealSize)
//	{
//		LPHXAttribute pAttributeTmp = (LPHXAttribute)((BYTE*)pMFT + i32HasReadByte);
//		switch (pAttributeTmp->m_i32Flag)
//		{
//		case HXAttribute_List:
//			if (-1 == ParseMFTList(pAttributeTmp, strDir, vecAttribute, setAlreadyParseMFT))
//			{
//				Q_ASSERT(0);
//				return -1;
//			}
//			break;
//		case HXAttribute_IndexRoot:
//		case HXAttribute_IndexAllocation:
//		case HXAttribute_Bitmap:
//			vecAttribute.push_back(pAttributeTmp);
//			break;
//		default:
//			break;
//		}
//		if (0 == pAttributeTmp->m_i32TotalLength)
//		{
//			break;
//		}
//		i32HasReadByte += pAttributeTmp->m_i32TotalLength;
//	}
//	m_pCache->FreeBuffer((LPBYTE)pMFT);
//	return 0;
//}

int CHXNTFSPartition::GetDirectoryMFT(const std::wstring& strDir, UINT64& u64MFTPoint)
{
	u64MFTPoint = 0L;
	auto iter = m_hashDirectoryMFT.find(strDir);
	if (iter != m_hashDirectoryMFT.end())
	{		
		u64MFTPoint = iter->second;
		return 1;
	}
	return 0;
}

int CHXNTFSPartition::ParseRootDirectoryMFT(LPHXReadDirectory pDir, VecFileInfo& listFileInfo)
{
	m_pCache->BuildCache(m_strPartitionName);
	LPHXFileRecordHeader pFileRecordRoot = GetRootFileRecord();
	if (nullptr == pFileRecordRoot)
	{
		assert(0);
		//Q_ASSERT_X(nullptr != pFileRecordRoot, "Error", "Get Root File Record Failed!");
		return -1;
	}

	ParseFileRecordMFT(m_vecLCN);
	m_pCache->BuildCache(m_vecLCN);
	//qInfo() << "Partition Root MFT LCN Number:" << std::wstring::number(m_vecLCN.size());
	ParseDirectoryMFT(pFileRecordRoot, pDir->m_wstrDirectory, listFileInfo);
	m_pCache->FreeBuffer((LPBYTE)pFileRecordRoot);
	return 0;
}

int CHXNTFSPartition::SetDirectoryMFT(const std::wstring& strDir, UINT64 u64MFTPoint)
{
	m_hashDirectoryMFT[strDir] = u64MFTPoint;
	return 0;
}

int CHXNTFSPartition::ParseFileRecordMFT(LCNType& vecLCN)
{
	LPHXFileRecordHeader pFileRecord = GetFileRecordHeaderMFT();
	ParseMFTAttributeOnlyData(pFileRecord, vecLCN);
	m_pCache->FreeBuffer((LPBYTE)pFileRecord);
	return 0;
}

int CHXNTFSPartition::ParseMFTAttributeOnlyData(LPHXFileRecordHeader pFileRecord, LCNType& vecLCN)
{
	SetMFTNumber mft;
	//	获取索引根和索引分配 先解析80防止20找不到数据
	INT32 i32HasReadByte = pFileRecord->m_i16FirstAttribute;
	while (i32HasReadByte < pFileRecord->m_u32FileRecordRealSize)
	{
		LPHXAttribute pAttributeTmp = (LPHXAttribute)((BYTE*)pFileRecord + i32HasReadByte);
		if (HXAttribute_Data == pAttributeTmp->m_i32Flag)
		{
			ParseData(pAttributeTmp, vecLCN);
		}
		else if (HXAttribute_Unknow == pAttributeTmp->m_i32Flag)
		{
			break;
		}
		if (0 == pAttributeTmp->m_i32TotalLength)
		{
			break;
		}
		i32HasReadByte += pAttributeTmp->m_i32TotalLength;
	}
	//	解析20
	i32HasReadByte = pFileRecord->m_i16FirstAttribute;
	while (i32HasReadByte < pFileRecord->m_u32FileRecordRealSize)
	{
		LPHXAttribute pAttributeTmp = (LPHXAttribute)((BYTE*)pFileRecord + i32HasReadByte);
		if (HXAttribute_List == pAttributeTmp->m_i32Flag)
		{
			SetMFTNumber mftSet;
			mftSet.insert(pFileRecord->m_i64FileRecordReferenceNumber);
			if (-1 == ParseMFTListOnlyData(pAttributeTmp, vecLCN, mftSet))
			{
				Q_ASSERT(0);
				return -1;
			}
		}
		else if (HXAttribute_Unknow == pAttributeTmp->m_i32Flag)
		{
			break;
		}
		if (0 == pAttributeTmp->m_i32TotalLength)
		{
			break;
		}
		i32HasReadByte += pAttributeTmp->m_i32TotalLength;
	}
	return 0;
}

int CHXNTFSPartition::ParseMFTAttributeOnlyData(LPHXFileRecordHeader pFileRecord, LPHXFindFileParam pFindFileParam)
{
	//	获取索引根和索引分配
	INT32 i32HasReadByte = pFileRecord->m_i16FirstAttribute;
	while (i32HasReadByte < pFileRecord->m_u32FileRecordRealSize)
	{
		LPHXAttribute pAttributeTmp = (LPHXAttribute)((BYTE*)pFileRecord + i32HasReadByte);
		if (HXAttribute_List == pAttributeTmp->m_i32Flag)
		{
			SetMFTNumber mftSet;
			mftSet.insert(pFileRecord->m_i64FileRecordReferenceNumber);
			if (-1 == ParseMFTListOnlyData(pAttributeTmp, pFindFileParam, mftSet))
			{
				Q_ASSERT(0);
				return -1;
			}
		}
		else if (HXAttribute_Data == pAttributeTmp->m_i32Flag)
		{
			pFindFileParam->m_u8PermanentFlag = pAttributeTmp->m_u8PermanentFlag;
			pFindFileParam->m_u16CompressFlage = pAttributeTmp->m_u16CompressFlage;
			if (0 == pFindFileParam->m_u8PermanentFlag)
			{
				// 如果为常驻属性 不会超过一个mft大小
				pFindFileParam->m_u8PermanentSize = pAttributeTmp->m_unExpand.m_PermanentAttribute.m_i32BodyLength;
				pFindFileParam->m_pPermanentBuffer = new BYTE[pFindFileParam->m_u8PermanentSize];
				if (0 != pFindFileParam->m_u8PermanentSize)
				{
					LPBYTE pBody = (LPBYTE)pAttributeTmp + pAttributeTmp->GetHeaderSize();
					memcpy_s(pFindFileParam->m_pPermanentBuffer, pFindFileParam->m_u8PermanentSize, pBody, pFindFileParam->m_u8PermanentSize);
				}
				//pFindFileParam->m_pPermanentBuffer[pFindFileParam->m_u8PermanentSize - 1] = '\0';
				return 0;
			}
			else
			{
				VCNData data;
				data.m_u64StartCluster = pAttributeTmp->m_unExpand.m_VariableAttribute.m_i64BodyStartCluster;
				data.m_u64EndCluster = pAttributeTmp->m_unExpand.m_VariableAttribute.m_i64BodyEndCluster;
				ParseData(pAttributeTmp, data.m_vecLCN);
				pFindFileParam->m_vecVCN.push_back(data);
				//	以获取到第一个80属性为准暂时未考虑多流情况
				if (0 == pFindFileParam->m_u64SpaceSize)
				{
					pFindFileParam->m_u64SpaceSize = pAttributeTmp->m_unExpand.m_VariableAttribute.m_i64BodySpaceSize;
					pFindFileParam->m_u64RealSize = pAttributeTmp->m_unExpand.m_VariableAttribute.m_i64BodyRealSize;
				}
			}
		}
		else if (HXAttribute_Unknow == pAttributeTmp->m_i32Flag)
		{
			break;
		}
		if (0 == pAttributeTmp->m_i32TotalLength)
		{
			break;
		}
		i32HasReadByte += pAttributeTmp->m_i32TotalLength;
	}
	return 0;
}

int CHXNTFSPartition::ParseDirectoryMFT(LPHXFileRecordHeader pFileRecord, std::wstring strDir, VecFileInfo& listFileInfo)
{
	ParseDirectoryMFTAttribute(pFileRecord, strDir, listFileInfo);
	return 0;
}

int CHXNTFSPartition::ParseDirectoryMFTAttribute(LPHXFileRecordHeader pFileRecord, std::wstring strDir, VecFileInfo& listFileInfo)
{
	//	4.根据索引位置找索引，完成后迁为单个函数。返回每条DataRun位置索引位置和位图信息
	LCNType listLCN;
	BitMapType listBitMap;
	//	索引根
	LPHXAttribute pAttributeRoot = nullptr;

	SetMFTNumber setAlreadyParseMFT;
	setAlreadyParseMFT.insert(pFileRecord->m_i64FileRecordReferenceNumber);
	//	获取索引根和索引分配
	INT32 i32HasReadByte = pFileRecord->m_i16FirstAttribute;
	while (i32HasReadByte < pFileRecord->m_u32FileRecordRealSize)
	{
		LPHXAttribute pAttributeTmp = (LPHXAttribute)((BYTE*)pFileRecord + i32HasReadByte);
		if (HXAttribute_List == pAttributeTmp->m_i32Flag)
		{
			if (-1 == ParseMFTList(pAttributeTmp, strDir, listLCN, listBitMap, pAttributeRoot, setAlreadyParseMFT))
			{
				Q_ASSERT(0);
				return -1;
			}
		}
		else if (HXAttribute_IndexRoot == pAttributeTmp->m_i32Flag)
		{
			pAttributeRoot = pAttributeTmp;
		}
		//	解析AOH属性
		else if (HXAttribute_IndexAllocation == pAttributeTmp->m_i32Flag)
		{
			if (-1 == ParseIndexAlloc(pAttributeTmp, listLCN))
			{
				Q_ASSERT(0);
				return -1;
			}
		}
		//	解析BOH属性
		else if (HXAttribute_Bitmap == pAttributeTmp->m_i32Flag)
		{
			if (-1 == ParseBitMap(pAttributeTmp, listBitMap))
			{
				Q_ASSERT(0);
				return -1;
			}
		}
		else if (HXAttribute_Unknow == pAttributeTmp->m_i32Flag)
		{
			break;
		}
		if (0 == pAttributeTmp->m_i32TotalLength)
		{
			break;
		}
		i32HasReadByte += pAttributeTmp->m_i32TotalLength;
	}
	if (nullptr == pAttributeRoot)
	{
		return -1;
	}

	//	校验大小索引
	int i32Result = ParseIndexRoot(pAttributeRoot, strDir, listFileInfo);
	if (-1 == i32Result)
	{
		return -1;
	}
	else if (0 == i32Result)
	{
		return 0;
	}

	//	处理DataRun
	CHXIndexParam indexParam;
	indexParam.m_strDir = strDir;
	//indexParam.m_pBuffer = (LPBYTE)pFileRecord;
	indexParam.m_pvecFileInfo = &listFileInfo;
	ParseDataRun(&indexParam, listLCN, listBitMap);
	return 0;
}

int CHXNTFSPartition::ParseIndexAlloc(LPHXAttribute pAttribute, LCNType& listLCN)
{
	listLCN.clear();

	int i32OffsetAll = 0;
	LPBYTE pDataRun = (LPBYTE)pAttribute + pAttribute->GetHeaderSize();
	while (pAttribute->m_unExpand.m_PermanentAttribute.m_i32BodyOffset + i32OffsetAll != pAttribute->GeAllSize())
	{
		int i32Offset = GetLCN(pDataRun, listLCN);
		if (-1 == i32Offset)
		{
			//	读取错误
			break;
		}
		pDataRun += i32Offset;
		i32OffsetAll += i32Offset;
	}

	return 0;
}

int CHXNTFSPartition::GetLCN(LPBYTE pDataRun, LCNType& listLCN)
{
	INT8 i8High = *pDataRun >> 4;
	INT8 i8low = *pDataRun & 0x0f;
	if (0 == i8High || 0 == i8low)
	{
		//	读取失败
		return -1;
	}
	pDataRun += 1;
	std::vector<UINT8> vecDataRunClusterSize;
	for (size_t i = 0; i < i8low; i++)
	{
		vecDataRunClusterSize.push_back(*(pDataRun + i));
	}

	INT64 i64ClusterSize = 0;
	for (INT32 i = i8low - 1; i >= 0; i--)
	{
		i64ClusterSize = i64ClusterSize * 0x100 + vecDataRunClusterSize[i];
	}

	pDataRun += i8low;
	std::vector<UINT8> vecDataRunCluster;
	for (size_t i = 0; i < i8High; i++)
	{
		vecDataRunCluster.push_back(*(pDataRun + i));
	}
	INT64 i64Cluster = 0;

	//	LSN如果为负数处理
	bool bMinus = false;
	if (vecDataRunCluster[i8High - 1] >> 7)
	{
		bMinus = true;
	}
	for (INT32 i = i8High - 1; i >= 0; i--)
	{
		UINT8 i8Tmp = vecDataRunCluster[i];
		if (bMinus)
		{
			i8Tmp = i8Tmp ^ 0xFF;
		}
		i64Cluster = i64Cluster * 0x100 + i8Tmp;
	}
	if (bMinus)
	{
		i64Cluster = -(i64Cluster + 1);
	}
	listLCN.push_back(QPair<INT64, INT64>(i64Cluster, i64ClusterSize));
	return i8High + i8low + 1;
}

int CHXNTFSPartition::ParseDirectoryMFTAttributeForFind(LPHXFileRecordHeader pFileRecord, std::wstring strDir, std::wstring strFileName, UINT64& u64MFTNumber)
{
	//	4.根据索引位置找索引，完成后迁为单个函数。返回每条DataRun位置索引位置和位图信息
	LCNType listLCN;
	BitMapType listBitMap;
	//	索引根
	LPHXAttribute pAttributeRoot = nullptr;
	SetMFTNumber setAlreadyParseMFT;
	setAlreadyParseMFT.insert(pFileRecord->m_i64FileRecordReferenceNumber);
	//	获取索引根和索引分配
	INT32 i32HasReadByte = pFileRecord->m_i16FirstAttribute;
	while (i32HasReadByte < pFileRecord->m_u32FileRecordRealSize)
	{
		LPHXAttribute pAttributeTmp = (LPHXAttribute)((BYTE*)pFileRecord + i32HasReadByte);
		if (HXAttribute_List == pAttributeTmp->m_i32Flag)
		{
			if (-1 == ParseMFTList(pAttributeTmp, strDir, listLCN, listBitMap, pAttributeRoot, setAlreadyParseMFT))
			{
				return -1;
			}
		}
		else if (HXAttribute_IndexRoot == pAttributeTmp->m_i32Flag)
		{
			pAttributeRoot = pAttributeTmp;
		}
		//	解析AOH属性
		else if (HXAttribute_IndexAllocation == pAttributeTmp->m_i32Flag)
		{
			if (-1 == ParseIndexAlloc(pAttributeTmp, listLCN))
			{
				return -1;
			}
		}
		//	解析BOH属性
		else if (HXAttribute_Bitmap == pAttributeTmp->m_i32Flag)
		{
			if (-1 == ParseBitMap(pAttributeTmp, listBitMap))
			{
				return -1;
			}
		}
		else if (HXAttribute_Unknow == pAttributeTmp->m_i32Flag)
		{
			break;
		}
		if (0 == pAttributeTmp->m_i32TotalLength)
		{
			break;
		}
		i32HasReadByte += pAttributeTmp->m_i32TotalLength;
	}
	if (nullptr == pAttributeRoot)
	{
		return -1;
	}

	//	校验大小索引
	int i32Result = ParseIndexRootForFind(pAttributeRoot, strDir, u64MFTNumber);
	if (-1 == i32Result)
	{
		return -1;
	}
	else if (0 == i32Result)
	{
		return 0;
	}

	//	处理DataRun
	CHXIndexParam indexParam;
	indexParam.m_bFindFile = TRUE;
	indexParam.m_strDir = strDir;
	//indexParam.m_pBuffer = (LPBYTE)pFileRecord;
	//indexParam.m_strFileName = strFileName;
	ParseDataRun(&indexParam, listLCN, listBitMap);

	u64MFTNumber = indexParam.m_i64MFTNumber;
	return 0;
}

UINT32 CHXNTFSPartition::GetMFTSize()
{
	return m_pCache->GetMFTSize();
}

UINT32 CHXNTFSPartition::GetBufferSize()
{
	return GetMFTSize();
}

UINT64 CHXNTFSPartition::GetClusterSize()
{
	return m_pCache->GetClusterSize();
}

bool CHXNTFSPartition::IsRoot(const std::wstring& strFile)
{
	return 0 == strFile.compare(m_strPartitionName);
}

LPHXFileRecordHeader CHXNTFSPartition::GetSerialNumberFileRecord(INT64 i64MFTSerialNumber)
{
	LPHXFileRecordHeader pFileRecord = nullptr;
	UINT32 i32FileRecordSize = GetBufferSize();
	INT64 i64MFTOffset = i64MFTSerialNumber * GetMFTSize();
	// 目标簇位置
	INT64 i64ClusterOffset = 0;
	DWORD u32RealReadSize = 0;
	Q_ASSERT(m_vecLCN.size() != 0);
	for (size_t i = 0; i < m_vecLCN.size(); i++)
	{
		i64ClusterOffset += m_vecLCN[i].first * GetClusterSize();
		INT64 i64MFTClusterOffset = i64MFTOffset - m_vecLCN[i].second * GetClusterSize();
		if (i64MFTClusterOffset < 0)
		{
			//	偏移就在此簇
			LARGE_INTEGER i64Location;
			i64Location.QuadPart = i64MFTOffset + i64ClusterOffset;
			pFileRecord = (LPHXFileRecordHeader)ReadBuffer(i64Location, i32FileRecordSize, u32RealReadSize, FILE_BEGIN, CHXBufferType_MFT);
			return pFileRecord;
		}
		else
		{
			i64MFTOffset = i64MFTClusterOffset;
		}
	}
	return nullptr;
}

// 可能会存在
int CHXNTFSPartition::HaveSubFile(INT64 i64MFTSerialNumber, LPHXFileInfo pFileInfo)
{
	if (nullptr == pFileInfo)
	{
		return -1;
	}
	LPHXFileRecordHeader pMFT = GetSerialNumberFileRecord(i64MFTSerialNumber);
	if (nullptr == pMFT)
	{
		return -1;
	}

	INT32 i32HasReadByte = pMFT->m_i16FirstAttribute;

	VecFileInfo listFileInfo;
	//	获取索引根和索引分配				
	while (i32HasReadByte < pMFT->m_u32FileRecordRealSize)
	{
		LPHXAttribute pAttributeTmp = (LPHXAttribute)((BYTE*)pMFT + i32HasReadByte);

		if (HXAttribute_IndexRoot == pAttributeTmp->m_i32Flag)
		{
			std::wstring strDir;
			// TODO:可能存在目录过深情况  目前暂时不考虑
			int i32Result = ParseIndexRoot(pAttributeTmp, strDir, listFileInfo);
		}
		else if (HXAttribute_Unknow == pAttributeTmp->m_i32Flag)
		{
			break;
		}

		if (0 == pAttributeTmp->m_i32TotalLength)
		{
			break;
		}
		i32HasReadByte += pAttributeTmp->m_i32TotalLength;
	}
	if (listFileInfo.size() > 0)
	{
		UINT32 u32SubDir = 0;
		for (int i = 0; i < listFileInfo.size(); i++)
		{
			if (listFileInfo[i]->m_i32FileAttributes & HX_DIRECTORYFLAG)
			{
				u32SubDir = 1;
			}
			delete listFileInfo[i];
		}
		pFileInfo->m_U32SubFile = u32SubDir;
	}
	m_pCache->FreeBuffer((LPBYTE)pMFT);
	return 0;
}

int CHXNTFSPartition::GetMFTBaseInfo(INT64 i64MFTSerialNumber, LPHXFileInfo pFileInfo)
{
	if (nullptr == pFileInfo)
	{
		return -1;
	}
	LPHXFileRecordHeader pMFT = GetSerialNumberFileRecord(i64MFTSerialNumber);
	if (nullptr == pMFT)
	{
		return -1;
	}

	INT32 i32HasReadByte = pMFT->m_i16FirstAttribute;

	//	文件名 目前只用获取最后一个
	//std::vector<LPHXAttribute> arrFileName;
	LPHXAttribute pAttributeFileName = nullptr;

	//	获取索引根和索引分配				
	while (i32HasReadByte < pMFT->m_u32FileRecordRealSize)
	{
		LPHXAttribute pAttributeTmp = (LPHXAttribute)((BYTE*)pMFT + i32HasReadByte);
		if (HXAttribute_FileName == pAttributeTmp->m_i32Flag)
		{
			//arrFileName.push_back(pAttributeTmp);
			pAttributeFileName = pAttributeTmp;
		}
		else if (HXAttribute_Unknow == pAttributeTmp->m_i32Flag)
		{
			break;
		}

		if (0 == pAttributeTmp->m_i32TotalLength)
		{
			break;
		}
		i32HasReadByte += pAttributeTmp->m_i32TotalLength;
	}

	//	文件名
	if (pAttributeFileName)
	{
		LPHXFileRecordFileNameBody pBody = (LPHXFileRecordFileNameBody)((LPBYTE)pAttributeFileName + pAttributeFileName->m_unExpand.m_PermanentAttribute.m_i32BodyOffset);

		pFileInfo->m_u64CreateTime = pBody->m_i64CreateTime;
		pFileInfo->m_u64ModifyTime = pBody->m_i64LastModifyTime;
		pFileInfo->m_i32FileAttributes = pBody->m_i32Flag;
	}
	CHXFindFileParam pParam;
	ParseMFTAttributeOnlyData(pMFT, &pParam);
	if (pFileInfo->m_i32FileAttributes & HX_DIRECTORYFLAG)
	{
		pFileInfo->m_u64FileSpaceSize = -1;
		pFileInfo->m_u64FileRealSize = -1;
	}
	else
	{
		if (0 == pParam.m_u8PermanentFlag)
		{
			pFileInfo->m_u64FileSpaceSize = pParam.m_u8PermanentSize;
			pFileInfo->m_u64FileRealSize = pParam.m_u8PermanentSize;
		}
		else
		{
			pFileInfo->m_u64FileSpaceSize = pParam.m_u64SpaceSize;
			pFileInfo->m_u64FileRealSize = pParam.m_u64RealSize;
		}
	}

	m_pCache->FreeBuffer((LPBYTE)pMFT);
	return 0;
}

void CHXNTFSPartition::GetIndexName(LPHXIndex pIndex, std::wstring& strFileName)
{
	int i32NameLength = pIndex->m_u8FileNameLength;
	wchar_t* p = new wchar_t[i32NameLength];
	memcpy_s(p, i32NameLength * 2, (&pIndex->m_u8ArrName), i32NameLength * 2);
	//p[i32NameLength] = 0;
	strFileName = p;
	delete[]p;
}

LPHXFileRecordHeader CHXNTFSPartition::GetFileRecordHeaderMFT()
{
	LARGE_INTEGER i64MFTStart = {};
	i64MFTStart.QuadPart = m_pCache->GetBPB()->m_i64MFTStartCluster * GetClusterSize();

	DWORD u32Readsize = 0;
	return (LPHXFileRecordHeader)ReadBuffer(i64MFTStart, GetClusterSize(), u32Readsize, FILE_BEGIN, CHXBufferType_MFT);
	//return m_pCache->GetFileRecordHeaderMFT();
}

LPHXFileRecordHeader CHXNTFSPartition::GetRootFileRecord()
{
	LARGE_INTEGER i64MFTStart = {};
	i64MFTStart.QuadPart = m_pCache->GetBPB()->m_i64MFTStartCluster * GetClusterSize() + GetMFTSize() * 5;

	DWORD u32Readsize = 0;
	return (LPHXFileRecordHeader)ReadBuffer(i64MFTStart, GetClusterSize(), u32Readsize, FILE_BEGIN, CHXBufferType_MFT);
}

void CHXNTFSPartition::Refresh()
{
	m_pCache->Refresh();
}

int CHXNTFSPartition::ParseMFTListBodyOnlyData(UINT64 u64MFTSerialNumber, LPHXFindFileParam pFindFileParam, SetMFTNumber& setAlreadyParseMFT)
{
	LPHXFileRecordHeader pMFT = GetSerialNumberFileRecord(u64MFTSerialNumber);
	if (nullptr == pMFT)
	{
		return -1;
	}

	INT32 i32HasReadByte = pMFT->m_i16FirstAttribute;

	//	获取索引根和索引分配				
	while (i32HasReadByte < pMFT->m_u32FileRecordRealSize)
	{
		LPHXAttribute pAttributeTmp = (LPHXAttribute)((BYTE*)pMFT + i32HasReadByte);
		switch (pAttributeTmp->m_i32Flag)
		{
		case HXAttribute_List:
			if (-1 == ParseMFTListOnlyData(pAttributeTmp, pFindFileParam, setAlreadyParseMFT))
			{
				return -1;
			}
			break;
		case HXAttribute_Data:
		{
			pFindFileParam->m_u8PermanentFlag = pAttributeTmp->m_u8PermanentFlag;
			pFindFileParam->m_u16CompressFlage = pAttributeTmp->m_u16CompressFlage;
			if (0 == pFindFileParam->m_u8PermanentFlag)
			{
				// 如果为常驻属性 不会超过一个mft大小
				pFindFileParam->m_u8PermanentSize = pAttributeTmp->m_i32TotalLength - pAttributeTmp->GetHeaderSize() + 1;
				pFindFileParam->m_pPermanentBuffer = new BYTE[pFindFileParam->m_u8PermanentSize];
				LPBYTE pBody = (LPBYTE)pAttributeTmp + pAttributeTmp->GetHeaderSize();
				memcpy_s(pFindFileParam->m_pPermanentBuffer, pFindFileParam->m_u8PermanentSize, pBody, pFindFileParam->m_u8PermanentSize);
				return 0;
			}
			else
			{
				VCNData data;
				data.m_u64StartCluster = pAttributeTmp->m_unExpand.m_VariableAttribute.m_i64BodyStartCluster;
				data.m_u64EndCluster = pAttributeTmp->m_unExpand.m_VariableAttribute.m_i64BodyEndCluster;
				ParseData(pAttributeTmp, data.m_vecLCN);
				pFindFileParam->m_vecVCN.push_back(data);
				pFindFileParam->m_u64SpaceSize = pAttributeTmp->m_unExpand.m_VariableAttribute.m_i64BodySpaceSize;
				pFindFileParam->m_u64RealSize = pAttributeTmp->m_unExpand.m_VariableAttribute.m_i64BodyRealSize;
			}
		}
		break;
		case HXAttribute_IndexRoot:
		case HXAttribute_IndexAllocation:
		case HXAttribute_Bitmap:
			break;
		default:
			break;
		}
		if (0 == pAttributeTmp->m_i32TotalLength)
		{
			break;
		}
		i32HasReadByte += pAttributeTmp->m_i32TotalLength;
	}
	m_pCache->FreeBuffer((LPBYTE)pMFT);
	return 0;
}

int CHXNTFSPartition::FindDirectory(LPHXReadDirectory pDir, UINT64 u64MFTLocation, VecFileInfo& listFileIno)
{
	DWORD i32Error = 0;
	DWORD u32RealReadSize = 0;

	LPHXFileRecordHeader pFileRecord = GetSerialNumberFileRecord(u64MFTLocation);
	if (nullptr == pFileRecord)
	{
		return -1;
	}


	std::wstring wstrDirectory = pDir->m_wstrDirectory;
	int i32Disk = wstrDirectory.find(L"\\", 0);
	//	测试用例a:\\b\\  a:\\c
	if (i32Disk != (wstrDirectory.length() - 1))
	{
		wstrDirectory += L"\\";
	}
	ParseDirectoryMFT(pFileRecord, wstrDirectory, listFileIno);

	m_pCache->FreeBuffer((LPBYTE)pFileRecord);
	return 0;
}

int CHXNTFSPartition::ParseBitMap(LPHXAttribute pAttribute, BitMapType& listBitMap)
{
	listBitMap.clear();

	int i32ReadBuffer = pAttribute->GetHeaderSize();
	LPBYTE pData = (LPBYTE)pAttribute + i32ReadBuffer;
	UINT8 u8 = 0;
	while (i32ReadBuffer < pAttribute->GeAllSize())
	{
		u8 = *pData;
		listBitMap.push_back(u8);
		pData++;
		i32ReadBuffer++;
	}
	return 0;
}

int CHXNTFSPartition::ParseIndex(LPHXIndexParam pIndexParam)
{
	INT32 i32Index = 0;

#ifdef _HX_USE_QT_
	UINT32 u32FIle = pIndexParam->m_strDir.lastIndexOf(u8'\\') + 1;
	std::wstring wstrFileName = pIndexParam->m_strDir.mid(u32FIle, pIndexParam->m_strDir.length() - 1);
#else
	UINT32 u32FIle = pIndexParam->m_strDir.rfind(u8'\\') + 1;
	std::wstring wstrFileName = pIndexParam->m_strDir.substr(u32FIle, pIndexParam->m_strDir.length() - 1);
#endif // _HX_USE_QT_
	while (pIndexParam->m_pIndex && pIndexParam->m_pIndex->m_i64MFT != 0L && (-1 == pIndexParam->m_i32AllSize || pIndexParam->m_i32ReadSize < pIndexParam->m_i32AllSize))
	{
		INT64 i64MFTSerialNumber = pIndexParam->m_pIndex->m_i64MFT & 0x0000FFFFFFFFFFFF;
		if (HXFileNameSpace::HXFileNameSpace_DOS == (HXFileNameSpace)pIndexParam->m_pIndex->m_u8FileNameSpace
			|| pIndexParam->m_pIndex->m_i64FileFlag & 0x6)
		{
			pIndexParam->m_i32ReadSize += pIndexParam->m_pIndex->m_u16IndexSize;
			pIndexParam->m_pIndex = (LPHXIndex)((LPBYTE)pIndexParam->m_pIndex + pIndexParam->m_pIndex->m_u16IndexSize);
			continue;
		}

		LPHXFileInfo pFileInfo = new HXFileInfo();
		GetIndexName(pIndexParam->m_pIndex, pFileInfo->m_szFileName);
		if (g_cs_bFindMFTBase)
		{
			int i32GetMFTResult = GetMFTBaseInfo(i64MFTSerialNumber, pFileInfo);
			if (-1 == i32GetMFTResult)
			{
				delete pFileInfo;
				return -1;
			}
		}
		else
		{
			pFileInfo->m_i32FileAttributes = pIndexParam->m_pIndex->m_i64FileFlag;
			pFileInfo->m_u64CreateTime = pIndexParam->m_pIndex->m_i64CreateTime;
			pFileInfo->m_u64ModifyTime = pIndexParam->m_pIndex->m_i64LastModifyTime;
			pFileInfo->m_u64FileSpaceSize = pIndexParam->m_pIndex->m_i64FileSpaceSize;
			pFileInfo->m_u64FileRealSize = pIndexParam->m_pIndex->m_i64FileRealSize;
		}

		if (TRUE == pIndexParam->m_bFindFile)
		{
			if (wstrFileName == pFileInfo->m_szFileName)
			{
				delete pFileInfo;
				pIndexParam->m_i64MFTNumber = pIndexParam->m_pIndex->m_i64MFT;
				return 1;
			}
		}
		else
		{
			if (HX_DIRECTORYFLAG & pFileInfo->m_i32FileAttributes)
			{
				HaveSubFile(i64MFTSerialNumber, pFileInfo);
				std::wstring strDirecotry;
				strDirecotry = pIndexParam->m_strDir;
				if (strDirecotry.rfind(L"\\") != (strDirecotry.size() - 1))
				{
					strDirecotry += L"\\";
				}
				strDirecotry += pFileInfo->m_szFileName;
				SetDirectoryMFT(strDirecotry, i64MFTSerialNumber);
			}
			pIndexParam->m_pvecFileInfo->push_back(pFileInfo);
		}
		if (TRUE == pIndexParam->m_bFindFile)
		{
			delete pFileInfo;
		}
		if (0 == pIndexParam->m_pIndex->m_u16IndexSize || -1 == pIndexParam->m_pIndex->m_u16IndexSize)
		{
			break;
		}
		pIndexParam->m_i32ReadSize += pIndexParam->m_pIndex->m_u16IndexSize;
		pIndexParam->m_pIndex = (LPHXIndex)((LPBYTE)pIndexParam->m_pIndex + pIndexParam->m_pIndex->m_u16IndexSize);

	}
	if (TRUE == pIndexParam->m_bFindFile)
	{
		return -1;
	}
	return 1;
}
int CHXNTFSPartition::ParseData(LPHXAttribute pAttribute, LCNType& listLCN)
{
	int i32OffsetAll = 0;
	LPBYTE pDataRun = (LPBYTE)pAttribute + pAttribute->GetHeaderSize();
	while (pAttribute->m_unExpand.m_PermanentAttribute.m_i32BodyOffset + i32OffsetAll != pAttribute->GeAllSize())
	{
		int i32Offset = GetLCN(pDataRun, listLCN);
		if (-1 == i32Offset)
		{
			//	读取错误
			break;
		}
		pDataRun += i32Offset;
		i32OffsetAll += i32Offset;
	}

	return 0;
}

int CHXNTFSPartition::ParseDataRun(LPHXIndexParam pIndexParam, const LCNType& listLCN, const BitMapType& listBitMap)
{
	int i32hxNumber = 0;
	LPBYTE pDataRunBuffer = nullptr;
	INT64 i64Offset = 0;
	//	定位到数据运行 TODO:如果一条DataRun过大  readbuffer会超大
	for (auto iter = listLCN.begin(); iter != listLCN.end(); iter++)
	{
		LARGE_INTEGER i64FirstDataRunPointer;
		i64Offset += iter->first;
		i64FirstDataRunPointer.QuadPart = i64Offset * GetClusterSize();
		DWORD i32RealReadSize = 0;
		INT32 i32AllIndexSize = iter->second * GetClusterSize();

		pDataRunBuffer = ReadBuffer(i64FirstDataRunPointer, i32AllIndexSize, i32RealReadSize, FILE_BEGIN, CHXBufferType_INDEX);
		if (nullptr == pDataRunBuffer)
		{
			auto hx = GetLastError();
			Q_ASSERT(0);
			return -1;
		}
		if (i32RealReadSize < i32AllIndexSize)
		{
			INT32 i32RemainSize = (i32AllIndexSize - i32RealReadSize);
			//i32RemainSize = ceil(i32RemainSize) * m_pCache->GetBPB()->m_i16SectorSize;
			LPHXIndexHeader pIndexHeader = (LPHXIndexHeader)pDataRunBuffer;
			while (1)
			{
				INT32 i32Cultset = 0;
				while (i32Cultset < i32RealReadSize)
				{
					INT32	i32ReadSize = ((LPBYTE)&pIndexHeader->m_i32Offset - (LPBYTE)pIndexHeader) + pIndexHeader->m_i32Offset;
					pIndexParam->m_pIndex = (LPHXIndex)((LPBYTE)&pIndexHeader->m_i32Offset + pIndexHeader->m_i32Offset);
					pIndexParam->m_i32ReadSize = i32ReadSize;
					pIndexParam->m_i32AllSize = pIndexHeader->m_i32IndexRealSize;
					i32hxNumber++;
					ParseIndex(pIndexParam);
					i32Cultset += GetClusterSize();
					pIndexHeader = (LPHXIndexHeader)((LPBYTE)pIndexHeader + GetClusterSize());
				}
				m_pCache->FreeBuffer(pDataRunBuffer);
				if (i32RemainSize <= 0)
				{
					break;
				}
				i64FirstDataRunPointer.QuadPart = i64FirstDataRunPointer.QuadPart + i32RealReadSize;
				i32RealReadSize = 0;
				pDataRunBuffer = ReadBuffer(i64FirstDataRunPointer, i32RemainSize, i32RealReadSize, FILE_BEGIN, CHXBufferType_INDEX);
				Q_ASSERT(nullptr != pDataRunBuffer);
				pIndexHeader = (LPHXIndexHeader)pDataRunBuffer;
				if (i32RemainSize < i32RealReadSize)
				{
					i32RealReadSize = i32RemainSize;
					i32RemainSize = 0;
				}
				else
				{
					i32RemainSize = i32RemainSize - i32RealReadSize;
				}
				Q_ASSERT_X(pDataRunBuffer != nullptr, "error", "ParseDataRun is null!");
			}
		}
		else
		{
			LPHXIndexHeader pIndexHeader = (LPHXIndexHeader)pDataRunBuffer;
			INT32 i32Cultset = 0;
			while (i32Cultset < iter->second)
			{
				INT32	i32ReadSize = ((LPBYTE)&pIndexHeader->m_i32Offset - (LPBYTE)pIndexHeader) + pIndexHeader->m_i32Offset;
				pIndexParam->m_pIndex = (LPHXIndex)((LPBYTE)&pIndexHeader->m_i32Offset + pIndexHeader->m_i32Offset);
				pIndexParam->m_i32ReadSize = i32ReadSize;
				pIndexParam->m_i32AllSize = pIndexHeader->m_i32IndexRealSize;

				ParseIndex(pIndexParam);
				i32hxNumber++;
				i32Cultset++;
				pIndexHeader = (LPHXIndexHeader)((LPBYTE)pIndexHeader + GetClusterSize());
			}
			m_pCache->FreeBuffer(pDataRunBuffer);
		}

	}
	return 0;
}

int CHXNTFSPartition::ParseIndexRoot(LPHXAttribute pAttribute, std::wstring strDir, VecFileInfo& vecFileInfo)
{
	int i32OffsetAll = 0;
	LPHXFileRecordIndexRoot pIndexRoot = (LPHXFileRecordIndexRoot)((LPBYTE)pAttribute + pAttribute->GetHeaderSize());
	LPHXFileRecordIndexHeader pIndexHeader = (LPHXFileRecordIndexHeader)((LPBYTE)pIndexRoot + sizeof(CHXFileRecordIndexRoot));


	CHXIndexParam indexParam;
	indexParam.m_bFindFile = FALSE;
	indexParam.m_pIndex = (LPHXIndex)((LPBYTE)pIndexHeader + pIndexHeader->m_i32FirstIndexOffset);
	indexParam.m_strDir = strDir;
	//indexParam.m_pBuffer = pBuffer;
	indexParam.m_i32ReadSize = pAttribute->GetHeaderSize();
	indexParam.m_i32AllSize = pIndexHeader->m_i32AllIndexRealSize;
	//indexParam.m_i32StartIndex = i64ReadMFTRange.m_i32StartIndex;
	//indexParam.m_i32EndIndex = i64ReadMFTRange.m_i32EndIndex;
	indexParam.m_pvecFileInfo = &vecFileInfo;
	ParseIndex(&indexParam);

	if (1 == pIndexHeader->m_i8IndexFlag)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

int CHXNTFSPartition::ParseIndexRootForFind(LPHXAttribute pAttribute, std::wstring strDir, UINT64& u64MFTNumber)
{
	u64MFTNumber = -1;
	LPHXFileRecordIndexRoot pIndexRoot = (LPHXFileRecordIndexRoot)((LPBYTE)pAttribute + pAttribute->GetHeaderSize());
	LPHXFileRecordIndexHeader pIndexHeader = (LPHXFileRecordIndexHeader)((LPBYTE)pIndexRoot + sizeof(CHXFileRecordIndexRoot));
	if (1 == pIndexHeader->m_i8IndexFlag)
	{
		return 1;
	}

	LPHXIndex pIndex = (LPHXIndex)((LPBYTE)pIndexHeader + pIndexHeader->m_i32FirstIndexOffset);
	INT32	i32ReadSize = pAttribute->GetHeaderSize();

	CHXIndexParam indexParam;
	indexParam.m_bFindFile = TRUE;
	indexParam.m_pIndex = pIndex;
	indexParam.m_strDir = strDir;
	//indexParam.m_pBuffer = pBuffer;
	indexParam.m_i32ReadSize = i32ReadSize;
	indexParam.m_i32AllSize = pAttribute->m_i32TotalLength;
	indexParam.m_i64MFTNumber = -1;
	if (1 == ParseIndex(&indexParam))
	{
		u64MFTNumber = indexParam.m_i64MFTNumber;
	}

	return 0;
}

int CHXNTFSPartition::FindFile(UINT64 i64DirectoryMFTNumber, LPHXReadFileInfo pInfo, UINT64& u64MFTNumber)
{
	LPHXFileRecordHeader pDir = GetSerialNumberFileRecord(i64DirectoryMFTNumber);

	// 已经获取目录索引查找时只需传递文件名
	std::wstring wstrFileName = pInfo->m_wstrDirectory;
	size_t pos = wstrFileName.find(L'\\') + 1;
	wstrFileName = wstrFileName.substr(pos, wstrFileName.length() - pos);
	int iRes = ParseDirectoryMFTAttributeForFind(pDir, pInfo->m_wstrDirectory, wstrFileName, u64MFTNumber);

	m_pCache->FreeBuffer((LPBYTE)pDir);
	return iRes;
}

int CHXNTFSPartition::ReadFile(LPHXReadFileInfo pInfo)
{
	pInfo->m_u64RealReadSize = 0;

	std::wstring strFile = pInfo->m_wstrDirectory;
	int i32NPos = strFile.rfind(L'\\');
	strFile = strFile.substr(0, i32NPos);

	UINT64 i64MFTLocation;
	if (0 == GetDirectoryMFT(strFile, i64MFTLocation))
	{
		if (IsRoot(strFile))
		{
			i64MFTLocation = 5L;
		}
		else
		{
			Q_ASSERT_X(0, "error", "error");
			return -1;
		}
	}

	//	找目录中文件的Index 得到MFT序号
	UINT64 i64MFTNumber = -1L;
	FindFile(i64MFTLocation, pInfo, i64MFTNumber);
	if (-1 == i64MFTNumber)
	{
		return -1;
	}

	i64MFTNumber = i64MFTNumber & 0x0000FFFFFFFFFFFF;
	LPHXFileRecordHeader pFileRecordHeader = GetSerialNumberFileRecord(i64MFTNumber);
	Q_ASSERT_X(nullptr != pFileRecordHeader, "Error", "Get MFT Error!");
	//	获取80H的数据运行
	//UINT32 u16CompressFlage;
	CHXFindFileParam param;
	ParseMFTAttributeOnlyData(pFileRecordHeader, &param);

	int i32Result = ReadFileData(&param, pInfo);
	Q_ASSERT(-1 != i32Result);

	m_pCache->FreeBuffer((LPBYTE)pFileRecordHeader);
	return i32Result;
}

int CHXNTFSPartition::ReadFileData(LPHXFindFileParam pParam, LPHXReadFileInfo pInfo)
{
	int i32Result = 0;
	switch (pParam->m_u16CompressFlage)
	{
	case HXCompressFlag_Unkown:
		i32Result = ReadFileDataBase(pParam, pInfo);
		break;
		//	以下暂时不支持
	case HXCompressFlag_Compression:
	case HXCompressFlag_Encryption:
	case HXCompressFlag_SparseFile:
		i32Result = 3;
		break;
		//case HXCompressFlag_Compression:
		//	i32Result = ReadFileDataCompression(pParam, pInfo);
		//	break;
		//case HXCompressFlag_Encryption:
		//	i32Result = ReadFileDataEncryption( pParam, pInfo);
		//	break;
		//case HXCompressFlag_SparseFile:
		//	i32Result = ReadFileDataSparseFile(pParam, pInfo);
		//	break;
	default:
		i32Result = 3;
		break;
	}
	return i32Result;
}

UINT64 CHXNTFSPartition::GetLCNSize(LCNType vecLCN, int i32Index, UINT64 u64LastClusterValueSize, bool bIsLastLCN)
{
	UINT64 u64Size = 0;
#ifdef _HX_USE_QT_
	if ((i32Index < vecLCN.length() - 1) || (false == bIsLastLCN))
	{
		u64Size = vecLCN[i32Index].second * GetClusterSize();
	}
	else if (i32Index == vecLCN.length() - 1)
	{
		u64Size = vecLCN[i32Index].second * GetClusterSize() - u64LastClusterValueSize;
	}
#else
	if ((i32Index < vecLCN.size() - 1) || (false == bIsLastLCN))
	{
		u64Size = vecLCN[i32Index].second * GetClusterSize();
	}
	else if (i32Index == vecLCN.size() - 1)
	{
		u64Size = vecLCN[i32Index].second * GetClusterSize() - u64LastClusterValueSize;
	}
#endif // _HX_USE_QT_
	return u64Size;
}

int CHXNTFSPartition::ReadFileDataBase(LPHXFindFileParam pParam, LPHXReadFileInfo pInfo)
{
	pInfo->m_u64RealReadSize = 0;
	if (0 == pParam->m_u8PermanentFlag)
	{
		if (pInfo->m_u64CurPos < pParam->m_u8PermanentSize)
		{
			memcpy_s(pInfo->m_pBuffer, pParam->m_u8PermanentSize, pParam->m_pPermanentBuffer + pInfo->m_u64CurPos, pParam->m_u8PermanentSize - pInfo->m_u64CurPos);
			pInfo->m_u64RealReadSize = pParam->m_u8PermanentSize - pInfo->m_u64CurPos;
		}
	}
	else
	{
		LPBYTE pDataRunBuffer = nullptr;
		UINT64 u64ReadStart = pInfo->m_u64CurPos;
		//	能够读取的最大大小
		UINT64 u64ReadSize = pInfo->m_u64ReadSize;
		if (0 != u64ReadSize % m_pCache->GetBPB()->m_i16SectorSize)
		{
			assert(0);
			//qCritical() << "ReadFileDataBase Failed";
			return -1;
		}
		//最后一个簇无效大小
		UINT64 u64LastClusterValueSize = pParam->m_u64SpaceSize - pParam->m_u64RealSize;
		BOOL bContinueRead = FALSE;
		bool bLastVCN = false;
		//	在此文件中整个的偏移
		//UINT64 u64VCNOffset = 0;
		// 一个80属性一个VCN
		for (auto iterVCN = pParam->m_vecVCN.begin(); iterVCN != pParam->m_vecVCN.end(); iterVCN++)
		{
			UINT64 u64VCNSize = (iterVCN->m_u64EndCluster - iterVCN->m_u64StartCluster + 1) * GetClusterSize();
			if (pParam->m_vecVCN.end() - 1 == iterVCN)
			{
				u64VCNSize = u64VCNSize - u64LastClusterValueSize;
				bLastVCN = true;
			}
			if (u64ReadStart >= u64VCNSize)
			{
				u64ReadStart = u64ReadStart - u64VCNSize;
				continue;
			}
			UINT64 u64StartLCN = 0;	//文件读取起点位置
			UINT64 u64LCNOffset = 0;//当前LCN起始簇
			UINT64 u64LCNSize = 0;	//每一个80属性中单独一条LCN大小
			for (u64StartLCN = 0; u64StartLCN != iterVCN->m_vecLCN.size(); u64StartLCN++)
			{
				u64LCNSize = GetLCNSize(iterVCN->m_vecLCN, u64StartLCN, u64LastClusterValueSize, bLastVCN);
				u64LCNOffset += iterVCN->m_vecLCN[u64StartLCN].first;
				if (u64ReadStart >= u64LCNSize)
				{
					u64ReadStart = u64ReadStart - u64LCNSize;
					continue;
				}
				else
				{
					break;
				}
			}
			if (u64StartLCN == iterVCN->m_vecLCN.size())
			{
				Q_ASSERT_X(0, "Error", "VCN Or LCN Parse Failed!");
				continue;
			}

			//	读文件起始点
			LARGE_INTEGER i64FilePos;
			i64FilePos.QuadPart = u64LCNOffset * GetClusterSize() + u64ReadStart;

			//	有效读取大小
			UINT32 u32ReadSize = min(u64ReadSize, u64LCNSize - u64ReadStart);
			//u64ReadSize -= u32ReadSize;
			DWORD u32ReadlReadSize = 0;
			//	校验读取大小
			LPBYTE pBuffer = ReadBuffer(i64FilePos, u32ReadSize, u32ReadlReadSize, FILE_BEGIN, CHXBufferType_Unknown);
			int i32ReadCopySize = min(u32ReadSize, u32ReadlReadSize);

			if (u64StartLCN == iterVCN->m_vecLCN.size())
			{
				Q_ASSERT_X(0, "Error", "VCN Or LCN Parse Failed!");
				return -1;
			}
			memcpy_s(pInfo->m_pBuffer, pInfo->m_u64ReadSize, pBuffer, i32ReadCopySize);
			m_pCache->FreeBuffer(pBuffer);

			pInfo->m_u64RealReadSize = i32ReadCopySize;
			break;
		}
	}
	return 0;
}
int CHXNTFSPartition::ReadFileDataCompression(LPHXFindFileParam pParam, LPHXReadFileInfo pInfo)
{
	return 0;
}
int CHXNTFSPartition::ReadFileDataEncryption(LPHXFindFileParam pParam, LPHXReadFileInfo pInfo)
{
	return 0;
}
int CHXNTFSPartition::ReadFileDataSparseFile(LPHXFindFileParam pParam, LPHXReadFileInfo pInfo)
{
	return 0;
}

int CHXNTFSPartition::ParseMFTListBody(
	std::wstring strDir,
	LCNType& listLCN,
	BitMapType& listBitMap,
	UINT64 u64MFTSerialNumber,
	LPHXAttribute& pAttributeIndexRoot,
	SetMFTNumber& setAlreadyParseMFT)
{
	LPHXFileRecordHeader pMFT = GetSerialNumberFileRecord(u64MFTSerialNumber);
	if (nullptr == pMFT)
	{
		return -1;
	}

	INT32 i32HasReadByte = pMFT->m_i16FirstAttribute;

	//	获取索引根和索引分配				
	while (i32HasReadByte < pMFT->m_u32FileRecordRealSize)
	{
		LPHXAttribute pAttributeTmp = (LPHXAttribute)((BYTE*)pMFT + i32HasReadByte);
		if (HXAttribute_List == pAttributeTmp->m_i32Flag)
		{
			if (-1 == ParseMFTList(pAttributeTmp, strDir, listLCN, listBitMap, pAttributeIndexRoot, setAlreadyParseMFT))
			{
				return -1;
			}
		}
		else if (HXAttribute_IndexRoot == pAttributeTmp->m_i32Flag)
		{
			pAttributeIndexRoot = (pAttributeIndexRoot == nullptr ? pAttributeTmp : pAttributeIndexRoot);
		}
		//	解析AOH属性
		else if (HXAttribute_IndexAllocation == pAttributeTmp->m_i32Flag)
		{
			if (-1 == ParseIndexAlloc(pAttributeTmp, listLCN))
			{
				return -1;
			}
		}
		//	解析BOH属性
		else if (HXAttribute_Bitmap == pAttributeTmp->m_i32Flag)
		{
			if (-1 == ParseBitMap(pAttributeTmp, listBitMap))
			{
				return -1;
			}
		}
		else if (HXAttribute_Unknow == pAttributeTmp->m_i32Flag)
		{
			break;
		}
		if (0 == pAttributeTmp->m_i32TotalLength)
		{
			break;
		}
		i32HasReadByte += pAttributeTmp->m_i32TotalLength;
	}
	m_pCache->FreeBuffer((LPBYTE)pMFT);
	return 0;
}