#pragma once

#include "HXNTFSType.h"
//#include "HXBufferPool.h"
#include "HXNTFSCache.h"

class CHXNTFSPartition
{
public:
	CHXNTFSPartition(const std::wstring& strPartitionName)
		: m_strPartitionName(strPartitionName)
		, m_pCache(HXNTFSCachePtr(new HXNTFSCache))
	{}

	~CHXNTFSPartition()
	{
		Clear();
	}

public:
	CHXNTFSPartition(const CHXNTFSPartition&) = delete;
	const CHXNTFSPartition &operator=(const CHXNTFSPartition &signal) = delete;

public:
	int Initlize();
	void Clear();

public:
	const std::wstring& GetPartitionName();

	/**
	 *
	 *  函数描述:
	 *				接受OnCmd中查找命令
	 *	pDir		需要查找的目录
	 *	listFileIno	查找结果
	 *
	 *	@return 返回结果
	 */
	int FindDirectory(LPHXReadDirectory pDirectory, VecFileInfo& listFileIno);

	/**
	 *
	 *  函数描述:
					磁盘根目录且第一次读取
	 *	pDir		需要查找的目录、页号、每页大小
	 *	listFileIno	查找结果
	 *
	 *	@return 返回结果
	 */
	int		ParseRootDirectoryMFT(LPHXReadDirectory pDir, VecFileInfo& listFileIno);



	/**
	 *
	 *  函数描述:
	 *				存储子目录位置
	 */
	int GetDirectoryMFT(const std::wstring& strDir, UINT64& u64MFTPoint);
	int SetDirectoryMFT(const std::wstring& strDir, UINT64 u64MFTPoint);

	/**
	 *
	 *  函数描述:
	 *				解析$MFT属性
	 *
	 */
	int		ParseFileRecordMFT(LCNType& vecLCN);


	/**
	 *
	 *  函数描述:
	 *				获取指定位置的buffer
	 */
	LPBYTE		ReadBuffer(LARGE_INTEGER i64FilePointer, DWORD i32FileRecordSize, DWORD& i32Readsize, DWORD i32MoveMethod, CHXBufferType i32Type);

	/**
	 *
	 *  函数描述:
	 *				处理指定长度20属性体信息
	 */
	int ParseMFTListBody(LPHXFileRecordListBody pListBody, UINT64 u64AllSize, std::wstring strDir, LCNType& listLCN, BitMapType& listBitMap, LPHXAttribute& pAttributeIndexRoot, SetMFTNumber& setAlreadyParseMFT);
	//	int ParseMFTListBody(LPHXFileRecordListBody pListBody, UINT64 u64AllSize, std::wstring strDir, std::vector<LPHXAttribute>& vecAttribute, SetMFTNumber & setAlreadyParseMFT);

	int ParseMFTListPermanent(LPHXAttribute pAttribute, std::wstring strDir, LCNType& listLCN, BitMapType& listBitMap, LPHXAttribute& pAttributeIndexRoot, SetMFTNumber& setAlreadyParseMFT);
	//	int ParseMFTListPermanent(LPHXAttribute pAttribute, std::wstring strDir, std::vector<LPHXAttribute>& vecAttribute, SetMFTNumber& setAlreadyParseMFT);
	int ParseMFTListVariable(LPHXAttribute pAttribute, std::wstring strDir, LCNType& listLCN, BitMapType& listBitMap, LPHXAttribute& pAttributeIndexRoot, SetMFTNumber& setAlreadyParseMFT);
	//	int ParseMFTListVariable(LPHXAttribute pAttribute, std::wstring strDir, std::vector<LPHXAttribute>& vecAttribute, SetMFTNumber & setAlreadyParseMFT);


	int ParseMFTList(LPHXAttribute pAttribute, std::wstring strDir, LCNType& listLCN, BitMapType& listBitMap, LPHXAttribute& pAttributeIndexRoot, SetMFTNumber& setAlreadyParseMFT);
	//	int ParseMFTList(LPHXAttribute pAttribute, std::wstring strDir, std::vector<LPHXAttribute>& vecAttribute, SetMFTNumber & setAlreadyParseMFT);


		/**
		 *
		 *  函数描述:
		 *				解析属性列表(20H),只获取80H。此流程只提供于解析$MFT的LCN流程
		 *
		 */
	int		ParseMFTListOnlyData(LPHXAttribute pAttribute, LCNType& vecLCN, SetMFTNumber& setAlreadyParseMFT);
	int		ParseMFTListBodyOnlyData(UINT u64MFTSerialNumber, LCNType& vecLCN, SetMFTNumber& setAlreadyParseMFT);

	int ParseMFTListBodyOnlyData(LPHXFileRecordListBody pListBody, UINT64 u64AllSize, LPHXFindFileParam pFindFileParam, SetMFTNumber& setAlreadyParseMFT);

	/**
	 *
	 *  函数描述:
	 *				解析属性列表(20H),只获取80H。此流程只提供于文件属性大小流程
	 *
	 */
	int		ParseMFTListOnlyData(LPHXAttribute pAttribute, LPHXFindFileParam pFindFileParam, SetMFTNumber& setAlreadyParseMFT);
	int ParseMFTListBodyOnlyData(LPHXFileRecordListBody pListBody, UINT64 u64AllSize, LCNType& vecLCN, SetMFTNumber& setAlreadyParseMFT);
	int		ParseMFTListBodyOnlyData(UINT64 u64MFTSerialNumber, LPHXFindFileParam pFindFileParam, SetMFTNumber& setAlreadyParseMFT);


	int		ParseMFTListBody(std::wstring strDir, LCNType& listLCN, BitMapType& listBitMap, UINT64 u64MFTSerialNumber, LPHXAttribute& pAttributeIndexRoot, SetMFTNumber& mapAlreadyFind);


	/**
	 *
	 *  函数描述:
	 *				解析属性列表(20H)
	 *
	 */
	int ParsingMFTList(LPHXAttribute pAttribute,
		HANDLE hDisk,
		std::wstring strDir,
		LCNType& listLCN,
		BitMapType& listBitMap,
		LPHXAttribute& pAttributeIndexRoot,
		SetMFTNumber& mapAlreadyFind);

	//int ParseMFTListBody(std::wstring strDir, UINT64 u64MFTSerialNumber, std::vector<LPHXAttribute>& vecAttribute, SetMFTNumber& setAlreadyParseMFT);


	///**
	// *
	// *  函数描述:
	// *				获取第5个mft(Root MFT)
	// *
	// */
	//int		GetRootFileRecord(int i32FileRecordSize, LPBYTE & pBuffer);

	/**
	 *
	 *  函数描述:
	 *				子目录解析函数，解析子目录
	 *
	 */
	int		ParseDirectoryMFT(LPHXFileRecordHeader pFileRecord, std::wstring str, VecFileInfo& listFileInfo);

	/**
	 *
	 *  函数描述:
	 *				解析MFT属性
	 *	bFind	是否查找。TRUE为查找，查找时strDir为文件绝对路径，查找时返回值是MFT序号，-1代表无效
	 *
	 */
	int ParseDirectoryMFTAttribute(LPHXFileRecordHeader pFileRecord, std::wstring strDir, VecFileInfo& listFileInfo);

	/**
	 *
	 *  函数描述:
	 *				解析AOH属性
	 *
	 */
	int		ParseIndexAlloc(LPHXAttribute pAttribute, LCNType& listLCN);

	int		GetLCN(LPBYTE pData, LCNType& listLCN);
	int		ParseDirectoryMFTAttributeForFind(LPHXFileRecordHeader pFileRecord, std::wstring strDir, std::wstring strFileName, UINT64& u64MFTNumber);

	int		ParseMFTAttributeOnlyData(LPHXFileRecordHeader pFileRecord, LCNType& vecLCN);
	int		ParseMFTAttributeOnlyData(LPHXFileRecordHeader pFileRecord, LPHXFindFileParam pFindFileParam);



	/**
	 *
	 *  函数描述:
	 *				获取指定序号mft
	 */
	LPHXFileRecordHeader		GetSerialNumberFileRecord(INT64 i64MFTSerialNumber);


	int HaveSubFile(INT64 i64MFTSerialNumber, LPHXFileInfo pFileInfo);
	/**
	 *
	 *  函数描述:
	 *				获取子目录文件记录，再调用子目录解析函数
	 *	pDir		需要查找的目录
	 *	listFileIno	查找结果
	 *
	 *	@return 返回结果
	 */
	int		FindDirectory(LPHXReadDirectory pDir, UINT64 u64MFTLocation, VecFileInfo& listFileIno);

	/**
	 *
	 *  函数描述:
	 *				解析BOH属性
	 *
	 */
	int		ParseBitMap(LPHXAttribute pAttribute, BitMapType& listBitMap);

	/**
	 *
	 *  函数描述:
	 *				解析索引项
	 *
	 */
	int ParseIndex(LPHXIndexParam pIndexParam);

	/**
	 *
	 *  函数描述:
	 *				解析数据运行
	 *
	 */
	int		ParseDataRun(LPHXIndexParam pIndexParam, const LCNType& listLSN, const BitMapType& listBitMap);

	/**
	 *
	 *  函数描述:
	 *				解析Data(80H)属性
	 *
	 */
	int		ParseData(LPHXAttribute pAttribute, LCNType& listLCN);

	int ReadFileData(LPHXFindFileParam pParam, LPHXReadFileInfo pInfo);

	UINT64 GetLCNSize(LCNType vecLCN, int i32IndexLCN, UINT64 u64LastClusterValueSize, bool bIsLastLCN);


	int ReadFileDataBase(LPHXFindFileParam pParam, LPHXReadFileInfo pInfo);

	int ReadFileDataCompression(LPHXFindFileParam pParam, LPHXReadFileInfo pInfo);

	int ReadFileDataEncryption(LPHXFindFileParam pParam, LPHXReadFileInfo pInfo);

	int ReadFileDataSparseFile(LPHXFindFileParam pParam, LPHXReadFileInfo pInfo);


	/**
	 *
	 *  函数描述:
	 *				解析90H
	 *
	 */
	int ParseIndexRoot(LPHXAttribute pAttribute, std::wstring strDir, VecFileInfo& vecFileInfo);

	int ParseIndexRootForFind(LPHXAttribute pAttribute, std::wstring strDir, UINT64& u64MFTNumber);


	int FindFile(UINT64 i64DirectoryMFTNumber, LPHXReadFileInfo pInfo, UINT64& u64MFTNumber);

	/**
	 *
	 *  函数描述:
	 *				读取文件信息
	 */
	int ReadFile(LPHXReadFileInfo pInfo);

	/**
	 *
	 *  函数描述:
	 *				解析MFT文件名称属性，获取文件名称、大小、创建修改时间
	 */
	int		GetMFTBaseInfo(INT64 i64MFTSerialNumber, LPHXFileInfo pFileInfo);

	void	GetIndexName(LPHXIndex pIndex, std::wstring& strFileName);
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

	void Refresh();
public:

	/**
	 *
	 *  函数描述:
	 *				获取每个MFT大小
	 */
	UINT32	GetMFTSize();
	UINT32	GetBufferSize();

	/**
	 *
	 *  函数描述:
	 *				获取一个簇大小
	 *
	 */
	UINT64	GetClusterSize();

	bool IsRoot(const std::wstring& strFile);

private:
	std::wstring					m_strPartitionName;

	LCNType					m_vecLCN;
	std::unordered_map<std::wstring, UINT64>	m_hashDirectoryMFT;
	HANDLE					m_hDisk;
	//QList<LPBYTE>			m_listBufferPool;
	//CHXBufferPool			m_bufferPool;
	HXNTFSCachePtr			m_pCache;
};


_HX_DECLAR_SHARED_POINTER(CHXNTFSPartition);

