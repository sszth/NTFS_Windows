#pragma once

#include "HXNTFSType.h"
//#include "HXBufferPool.h"
#include "HXNTFSCache.h"

class CHXNTFSPartition
{
public:
	CHXNTFSPartition(const QString & strPartitionName) 
		: m_strPartitionName(strPartitionName)
		, m_pCache(HXNTFSCachePtr(new HXNTFSCache))
	{}

	~CHXNTFSPartition()
	{
		Clear();
	}

public:
	int Initlize();
	void Clear();

public:
	const QString & GetPartitionName();

	/**
	 *
	 *  ��������:
	 *				����OnCmd�в�������
	 *	pDir		��Ҫ���ҵ�Ŀ¼
	 *	listFileIno	���ҽ��
	 *
	 *	@return ���ؽ��
	 */
	int FindDirectory(LPHXReadDirectory pDirectory, VecFileInfo & listFileIno);

	/**
	 *
	 *  ��������:
					���̸�Ŀ¼�ҵ�һ�ζ�ȡ
	 *	pDir		��Ҫ���ҵ�Ŀ¼��ҳ�š�ÿҳ��С
	 *	listFileIno	���ҽ��
	 *
	 *	@return ���ؽ��
	 */
	int		ParseRootDirectoryMFT(LPHXReadDirectory pDir, VecFileInfo & listFileIno);



	/**
	 *
	 *  ��������:
	 *				�洢��Ŀ¼λ��
	 */
	int GetDirectoryMFT(const QString & strDir, UINT64 & u64MFTPoint);
	int SetDirectoryMFT(const QString & strDir, UINT64 u64MFTPoint);

	/**
	 *
	 *  ��������:
	 *				����$MFT����
	 *
	 */
	int		ParseFileRecordMFT(LCNType & vecLCN);
		

	/**
	 *
	 *  ��������:
	 *				��ȡָ��λ�õ�buffer
	 */
	LPBYTE		ReadBuffer(LARGE_INTEGER i64FilePointer, DWORD i32FileRecordSize, DWORD & i32Readsize, DWORD i32MoveMethod, CHXBufferType i32Type);

	/**
	 *
	 *  ��������:
	 *				����ָ������20��������Ϣ
	 */
	int ParseMFTListBody(LPHXFileRecordListBody pListBody, UINT64 u64AllSize, QString strDir, LCNType & listLCN, BitMapType & listBitMap, LPHXAttribute & pAttributeIndexRoot, SetMFTNumber & setAlreadyParseMFT);
//	int ParseMFTListBody(LPHXFileRecordListBody pListBody, UINT64 u64AllSize, QString strDir, std::vector<LPHXAttribute>& vecAttribute, SetMFTNumber & setAlreadyParseMFT);
	
	int ParseMFTListPermanent(LPHXAttribute pAttribute, QString strDir, LCNType & listLCN, BitMapType & listBitMap, LPHXAttribute & pAttributeIndexRoot, SetMFTNumber& setAlreadyParseMFT);
//	int ParseMFTListPermanent(LPHXAttribute pAttribute, QString strDir, std::vector<LPHXAttribute>& vecAttribute, SetMFTNumber& setAlreadyParseMFT);
	int ParseMFTListVariable(LPHXAttribute pAttribute, QString strDir, LCNType & listLCN, BitMapType & listBitMap, LPHXAttribute & pAttributeIndexRoot, SetMFTNumber & setAlreadyParseMFT);
//	int ParseMFTListVariable(LPHXAttribute pAttribute, QString strDir, std::vector<LPHXAttribute>& vecAttribute, SetMFTNumber & setAlreadyParseMFT);

	
	int ParseMFTList(LPHXAttribute pAttribute, QString strDir, LCNType & listLCN, BitMapType & listBitMap, LPHXAttribute & pAttributeIndexRoot, SetMFTNumber & setAlreadyParseMFT);
//	int ParseMFTList(LPHXAttribute pAttribute, QString strDir, std::vector<LPHXAttribute>& vecAttribute, SetMFTNumber & setAlreadyParseMFT);
	

	/**
	 *
	 *  ��������:
	 *				���������б�(20H),ֻ��ȡ80H��������ֻ�ṩ�ڽ���$MFT��LCN����
	 *
	 */
	int		ParseMFTListOnlyData(LPHXAttribute pAttribute, LCNType& vecLCN, SetMFTNumber& setAlreadyParseMFT);
	int		ParseMFTListBodyOnlyData(UINT u64MFTSerialNumber, LCNType & vecLCN, SetMFTNumber & setAlreadyParseMFT);

	int ParseMFTListBodyOnlyData(LPHXFileRecordListBody pListBody, UINT64 u64AllSize, LPHXFindFileParam pFindFileParam, SetMFTNumber & setAlreadyParseMFT);

	/**
	 *
	 *  ��������:
	 *				���������б�(20H),ֻ��ȡ80H��������ֻ�ṩ���ļ����Դ�С����
	 *
	 */
	int		ParseMFTListOnlyData(LPHXAttribute pAttribute, LPHXFindFileParam pFindFileParam, SetMFTNumber& setAlreadyParseMFT);
	int ParseMFTListBodyOnlyData(LPHXFileRecordListBody pListBody, UINT64 u64AllSize, LCNType & vecLCN, SetMFTNumber & setAlreadyParseMFT);
	int		ParseMFTListBodyOnlyData(UINT64 u64MFTSerialNumber, LPHXFindFileParam pFindFileParam, SetMFTNumber & setAlreadyParseMFT);


	int		ParseMFTListBody(QString strDir, LCNType & listLCN, BitMapType & listBitMap, UINT64 u64MFTSerialNumber, LPHXAttribute & pAttributeIndexRoot, SetMFTNumber& mapAlreadyFind);


	/**
	 *
	 *  ��������:
	 *				���������б�(20H)
	 *
	 */
	int ParsingMFTList(LPHXAttribute pAttribute,
		HANDLE hDisk,
		QString strDir,
		LCNType & listLCN,
		BitMapType & listBitMap,
		LPHXAttribute & pAttributeIndexRoot,
		SetMFTNumber &mapAlreadyFind);

	//int ParseMFTListBody(QString strDir, UINT64 u64MFTSerialNumber, std::vector<LPHXAttribute>& vecAttribute, SetMFTNumber& setAlreadyParseMFT);


	///**
	// *
	// *  ��������:
	// *				��ȡ��5��mft(Root MFT)
	// *
	// */
	//int		GetRootFileRecord(int i32FileRecordSize, LPBYTE & pBuffer);

	/**
	 *
	 *  ��������:
	 *				��Ŀ¼����������������Ŀ¼
	 *
	 */
	int		ParseDirectoryMFT(LPHXFileRecordHeader pFileRecord, QString str, VecFileInfo & listFileInfo);

	/**
	 *
	 *  ��������:
	 *				����MFT����
	 *	bFind	�Ƿ���ҡ�TRUEΪ���ң�����ʱstrDirΪ�ļ�����·��������ʱ����ֵ��MFT��ţ�-1������Ч
	 *
	 */
	int ParseDirectoryMFTAttribute(LPHXFileRecordHeader pFileRecord, QString strDir, VecFileInfo & listFileInfo);

	/**
	 *
	 *  ��������:
	 *				����AOH����
	 *
	 */
	int		ParseIndexAlloc(LPHXAttribute pAttribute, LCNType & listLCN);

	int		GetLCN(LPBYTE pData, LCNType & listLCN);
	int		ParseDirectoryMFTAttributeForFind(LPHXFileRecordHeader pFileRecord, QString strDir, QString strFileName, UINT64 & u64MFTNumber);
	
	int		ParseMFTAttributeOnlyData(LPHXFileRecordHeader pFileRecord, LCNType& vecLCN);
	int		ParseMFTAttributeOnlyData(LPHXFileRecordHeader pFileRecord, LPHXFindFileParam pFindFileParam);

	
	
	/**
	 *
	 *  ��������:
	 *				��ȡָ�����mft
	 */
	LPHXFileRecordHeader		GetSerialNumberFileRecord(INT64 i64MFTSerialNumber);


	int HaveSubFile(INT64 i64MFTSerialNumber, LPHXFileInfo pFileInfo);
	/**
	 *
	 *  ��������:
	 *				��ȡ��Ŀ¼�ļ���¼���ٵ�����Ŀ¼��������
	 *	pDir		��Ҫ���ҵ�Ŀ¼
	 *	listFileIno	���ҽ��
	 *
	 *	@return ���ؽ��
	 */
	int		FindDirectory(LPHXReadDirectory pDir, UINT64 u64MFTLocation, VecFileInfo &listFileIno);

	/**
	 *
	 *  ��������:
	 *				����BOH����
	 *
	 */
	int		ParseBitMap(LPHXAttribute pAttribute, BitMapType & listBitMap);

	/**
	 *
	 *  ��������:
	 *				����������
	 *
	 */
	int ParseIndex(LPHXIndexParam pIndexParam);

	/**
	 *
	 *  ��������:
	 *				������������
	 *
	 */
	int		ParseDataRun(LPHXIndexParam pIndexParam, const LCNType & listLSN, const BitMapType & listBitMap);

	/**
	 *
	 *  ��������:
	 *				����Data(80H)����
	 *
	 */
	int		ParseData(LPHXAttribute pAttribute, LCNType & listLCN);

	int ReadFileData( LPHXFindFileParam pParam, LPHXReadFileInfo pInfo);

	UINT64 GetLCNSize(LCNType vecLCN, int i32IndexLCN, UINT64 u64LastClusterValueSize, bool bIsLastLCN);


	int ReadFileDataBase(LPHXFindFileParam pParam, LPHXReadFileInfo pInfo);

	int ReadFileDataCompression( LPHXFindFileParam pParam, LPHXReadFileInfo pInfo);

	int ReadFileDataEncryption( LPHXFindFileParam pParam, LPHXReadFileInfo pInfo);

	int ReadFileDataSparseFile( LPHXFindFileParam pParam, LPHXReadFileInfo pInfo);

	
	/**
	 *
	 *  ��������:
	 *				����90H
	 *
	 */
	int ParseIndexRoot(LPHXAttribute pAttribute, QString strDir, VecFileInfo& vecFileInfo);

	int ParseIndexRootForFind(LPHXAttribute pAttribute, QString strDir, UINT64& u64MFTNumber);


	int FindFile(UINT64 i64DirectoryMFTNumber, LPHXReadFileInfo pInfo, UINT64 & u64MFTNumber);

	/**
	 *
	 *  ��������:
	 *				��ȡ�ļ���Ϣ
	 */
	int ReadFile(LPHXReadFileInfo pInfo);

	/**
	 *
	 *  ��������:
	 *				����MFT�ļ��������ԣ���ȡ�ļ����ơ���С�������޸�ʱ��
	 */
	int		GetMFTBaseInfo(INT64 i64MFTSerialNumber, LPHXFileInfo pFileInfo);

	void	GetIndexName(LPHXIndex pIndex, QString & strFileName);
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

	void Refresh();
public:
	   	  
	/**
	 *
	 *  ��������:
	 *				��ȡÿ��MFT��С
	 */
	UINT32	GetMFTSize();
	UINT32	GetBufferSize();

	/**
	 *
	 *  ��������:
	 *				��ȡһ���ش�С
	 *
	 */
	UINT64	GetClusterSize();

	bool IsRoot(const QString & strFile);

private:
	QString					m_strPartitionName;
	
	LCNType					m_vecLCN;
	QHash<QString, UINT64>	m_hashDirectoryMFT;
	HANDLE					m_hDisk;
	//QList<LPBYTE>			m_listBufferPool;
	//CHXBufferPool			m_bufferPool;
	HXNTFSCachePtr			m_pCache;
};


_HX_DECLAR_SHARED_POINTER(CHXNTFSPartition);

