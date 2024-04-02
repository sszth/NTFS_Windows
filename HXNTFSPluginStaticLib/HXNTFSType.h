#pragma once
#include <Windows.h>
#include <cassert>

#ifdef _HX_USE_QT_

#include <QSharedPointer>
#include <QWeakPointer> 
#include <QReadWriteLock>
#define _HX_DECLAR_SHARED_POINTER(X) typedef QSharedPointer<X> X##Ptr;
#define _HX_DECLAR_WEAK_POINTER(X) typedef QWeakPointer<X> X##WeakPtr;

#else

#include <memory>
#include <shared_mutex>
#include <utility>

#define _HX_DECLAR_SHARED_POINTER(X) typedef std::shared_ptr<X> X##Ptr;
#define _HX_DECLAR_WEAK_POINTER(X) typedef std::weak_ptr<X> X##WeakPtr;

#endif // _HX_USE_QT_


enum class HXFileNameSpace : int8_t
{
	HXFileNameSpace_POSIZ = 0x00,
	HXFileNameSpace_Win32 = 0x01,
	HXFileNameSpace_DOS = 0x02,
	HXFileNameSpace_WINAndDOS = 0x03,
};

#pragma pack(push)
#pragma pack(1)
// BPB
typedef struct CHXBPB
{
	//	0x00
	BYTE	m_i8ArrJmp[3];
	BYTE	m_i8ArrFileSystemID[8];
	short	m_i16SectorSize;			//	扇区大小,字节
	BYTE	m_i8ClusterSectorSize;		//	每簇扇区数
	BYTE	m_i8ArrReserve[2];

	//	0x10
	BYTE	m_i8ArrZero[3];				//	总为0
	BYTE	m_i8ArrNTFSZero1[2];		//	NTFS总为0
	BYTE	m_i8Device;					//	介质描述
	short	m_i16Zero;
	short	m_i16TrackSectorSize;		//	磁道扇区数
	short	m_i16Head;					//	磁头数
	INT32	m_i32HideSector;			//	隐藏扇区数

	//	0x20
	BYTE	m_i8ArrNTFSZero2[4];		//	NTFS总为0
	BYTE	m_i8ArrNTFSNotUse[4];		//	NTFS未使用,总为80008000
	INT64	m_i64SectorTotalSize;		//	扇区总数

	//	0x30
	INT64	m_i64MFTStartCluster;		//	MFT起始簇
	INT64	m_i64MFTMirrStartCluster;	//	MFTMirr起始簇

	//	0x40
	BYTE	m_i8FileRecord;				//	文件记录大小描述
	BYTE	m_i8ArrNotUse[3];
	BYTE	m_i8IndexBufferSize;		//	索引大小描述
	BYTE	m_i8ArrNotUse2[3];
	INT64	m_i64VolumeSerialNumber;	//	卷序列号

	// 0x50
	int		m_i32CheckCode;

}*PHXBPB, *LPHXBPB;

_HX_DECLAR_SHARED_POINTER(CHXBPB)
_HX_DECLAR_WEAK_POINTER(CHXBPB)

// 文件记录头
typedef struct CHXFileRecordHeader
{
	//	0x00
	BYTE	m_i8ArrMFT[4];					//	FILE
	INT16	m_i16UpdateSequenceNumber;		//	更新序列号的偏移
	INT16	m_i16UpdateSequenceNumberSize;	//	更新序列号的大小与数组
	INT64	m_i64LogSequence;				//	日志文件序号


	//	0x10
	INT16	m_i16Sequence;					//	序列号
	INT16	m_i16HardLinkSize;				//	硬链接数量
	INT16	m_i16FirstAttribute;			//	第一个属性偏移地址
	INT16	m_i16Flag;						//	标志，代表是否在使用删除等
	UINT32	m_u32FileRecordRealSize;		//	文件记录实际长度
	UINT32	m_i32FileRecordSize;			//	文件记录分配长度

	//	0x20
	INT64	m_i64Index;						//	基本文件记录索引
	INT16	m_i16NextID;
	INT16	m_i16End;
	INT32	m_i64FileRecordReferenceNumber;

	//	0x30
	INT16	m_i16UpdateIndex;
	INT32	m_i32UpdateArray;

	UINT32 GetAllSize()
	{
		return m_i32FileRecordSize;
	}
} *PHXFileRecordHeader, *LPHXFileRecordHeader;

//	常驻属性头与非常驻属性头的公共头
typedef struct CHXAttributeHeader
{
	//	0x00
	INT32	m_i32Flag;						//	属性标识	无名称属性标识为10H和30H	有名称属性标识为90H、B0H
	INT32	m_i32TotalLength;				//	包括属性头在内长度
	BYTE	m_u8PermanentFlag;				//	常驻属性 0常驻
	BYTE	m_u8NameLength;					//	属性名长度 判断是否有属性名称

	// 0x0B
	short	m_i16NameStartOffset;			//	属性名称开始偏移
	UINT16	m_u16CompressFlage;				//	压缩标识
	short	m_i16ID;						//	属性ID

	union
	{
		// 常驻属性头
		struct CHXPermanentAttributeHeader
		{
			//	0x10
			INT32	m_i32BodyLength;				//	属性体长度
			short	m_i32BodyOffset;				//	属性体开始偏移
			BYTE	m_i8IndexFlag;
			BYTE	m_i8Reserve;
			// 如果有名称属性头，以下buffer先为属性名称后为属性体
		} m_PermanentAttribute;

		// 非常驻属性头
		struct CHXVariableAttribute
		{
			//	0x10
			INT64	m_i64BodyStartCluster;			//	属性体起始虚拟簇号VCN
			INT64	m_i64BodyEndCluster;			//	属性体结束虚拟簇号

			//	0x20
			short	m_i16RunListOffset;				//	Run List的偏移
			short	m_i16CompressSize;				//	压缩单位大小
			INT32	m_i32NotUse;
			INT64	m_i64BodySpaceSize;				//	属性体分配大小

			//	0x30
			INT64	m_i64BodyRealSize;				//	属性体实际大小
			// 如果有名称属性头，以下buffer先为属性名称后为属性体
		} m_VariableAttribute;
	}m_unExpand;

	//	目前版本只用2个字节
	INT32 GeAllSize()
	{
		return m_i32TotalLength & 0x0000FFFF;
	}

	INT32 GetHeaderSize()
	{
		if (0 == m_u8PermanentFlag)
		{
			return GetPermanentAttributeSize();
		}
		else
		{
			return GetVariableAttributeSize();
		}
	}

private:
	INT32 GetPermanentAttributeSize()
	{
		int i32OffsetBody = 0;
		i32OffsetBody = m_unExpand.m_PermanentAttribute.m_i32BodyOffset;
		if (0 >= i32OffsetBody)
		{
			if (0 == m_u8NameLength)
			{
				i32OffsetBody = 0x18;
			}
			else
			{
				i32OffsetBody = m_i16NameStartOffset + m_u8NameLength * 2;
				i32OffsetBody = i32OffsetBody % 8 ? (i32OffsetBody / 8 + 1) * 8 : i32OffsetBody;
			}
		}
		return i32OffsetBody;
	}

	INT32 GetVariableAttributeSize()
	{
		int i32OffsetBody = 0;
		i32OffsetBody = m_unExpand.m_VariableAttribute.m_i16RunListOffset;
		if (0 >= i32OffsetBody)
		{
			if (0 == m_u8NameLength)
			{
				i32OffsetBody = m_unExpand.m_VariableAttribute.m_i16RunListOffset;
			}
			else
			{
				i32OffsetBody = m_i16NameStartOffset + m_u8NameLength * 2;
				i32OffsetBody = i32OffsetBody % 8 ? (i32OffsetBody / 8 + 1) * 8 : i32OffsetBody;
			}
		}
		return i32OffsetBody;
	}
} *PHXAttribute, *LPHXAttribute;

typedef struct CHXFileRecordListBody
{
	//	0x00
	UINT32	m_u32Type;
	UINT16	m_u16Length;
	UINT8	m_u8NameLength;
	UINT8	m_u8NameOffset;
	INT64	m_i64StartVCN;

	//	0x10
	UINT64	m_u64AttributeMFT;
	INT16	m_i16AttributeID;

	//	以下为名称
} *PHXFileRecordListBody, *LPHXFileRecordListBody;

typedef struct CHXFileRecordStandardInformationBody
{
	//	0x00
	INT64	m_i64CreateTime;
	INT64	m_i64LastModifyTime;

	//	0x10
	INT64	m_i64LastModifyFileRecordTime;		//	MFT修改时间
	INT64	m_i64LastVisitTime;

	//	0x20
	INT32	m_i32FileType;						//	传统文件属性
	INT32	m_i32MaxVersion;
	INT32	m_i32Version;
	INT32	m_i32ClassificationID;

	//	0x30
	INT32	m_i32OwnerID;
	INT32	m_i32SecurityID;
	INT64	m_i64QuotaManagement;
	//	0x40
	INT64	m_i64UpdateSerialNumber;
} *PHXFileRecordStandardInformationBody, *LPHXFileRecordStandardInformationBody;

typedef struct CHXFileRecordFileNameBody
{
	//	0x00
	INT64	m_i64ParentMFT;
	INT64	m_i64CreateTime;

	//	0x10
	INT64	m_i64LastModifyTime;
	INT64	m_i64LastModifyFileRecordTime;		//	MFT修改时间

	//	0x20
	INT64	m_i64LastVisitTime;
	INT64	m_i64FileSpaceSize;

	//	0x30
	INT64	m_i64FileRealSize;
	INT32	m_i32Flag;
	INT32	m_i32ESAAndReparse;

	//	0x40
	INT8	m_i8NameLength;
	INT8	m_i8NameSpace;
	//	0x42 后面代表名称
} *PHXFileRecordFileNameBody, *LPHXFileRecordFileNameBody;

typedef struct CHXFileRecordIndexRoot
{
	INT32	m_i32Type;				//
	INT32	m_i32CheckRule;
	INT32	m_i32IndexBufferSize;	//	每个索引缓冲区的分配大小 （字节）
	INT8	m_i8IndexBufferCluster;	//	每个索引缓冲的簇数
	INT8	m_i8Arr[3];				//	无意义
} *PHXFileRecordIndexRoot, *LPHXFileRecordIndexRoot;

typedef struct CHXFileRecordIndexHeader
{
	INT32	m_i32FirstIndexOffset;
	INT32	m_i32AllIndexRealSize;
	INT32	m_i32AllIndexSpaceSize;	//	每个索引缓冲区的分配大小 （字节）
	INT8	m_i8IndexFlag;			//	索引标识。1为大索引，0为小索引
	INT8	m_i8Arr[3];				//	无意义
} *PHXFileRecordIndexHeader, *LPHXFileRecordIndexHeader;

//	标准索引头
typedef struct CHXIndexHeader
{
	//	0x00
	INT32	m_i32Flag;						//	总为INDX
	INT16	m_i16UpdateSequenceNumber;		//	更新序列号
	INT16	m_i16UpdateSequenceNumberSize;	//	更新序列号与更新数组以字为单位的大小（S）
	INT64	m_i64LogFileSequenceNumber;

	//	0x10
	INT64	m_i64VCN;

	INT32	m_i32Offset;					//	算的是从0x18开始的偏移
	INT32	m_i32IndexRealSize;

	//	0x20
	INT32	m_i32IndexSpaceSize;			//	算的是从0x18开始的大小
	INT8	m_i8Node;
	INT8	m_i8ArrNotUse[3];
	UINT16	m_u16UpdateSequence;

	//	0x2A开始为更新序列数组 2S-2

	INT32 GetAllSize()
	{
		return m_i32IndexSpaceSize + 0x18;
	}
} *PHXIndexHeader, *LPHXIndexHeader;

typedef struct CHXIndex
{
	//	0x00
	INT64	m_i64MFT;
	UINT16	m_u16IndexSize;
	UINT16	m_u16NameSize;
	INT16	m_i16Flag;				//	1包含一个子节点，2为最后一个项
	INT16	m_i16NotUse;

	//	0x10
	INT64	m_i64ParentsMFT;
	INT64	m_i64CreateTime;

	//	0x20
	INT64	m_i64LastModifyTime;
	INT64	m_i64LastModifyFileRecordTime;

	//	0x30
	INT64	m_i64LastVisitTime;
	INT64	m_i64FileSpaceSize;

	//	0x40
	INT64	m_i64FileRealSize;
	INT64	m_i64FileFlag;

	//	0x50
	UINT8	m_u8FileNameLength;
	UINT8	m_u8FileNameSpace;
	//	文件名称填充到8的整数倍
	UINT8	m_u8ArrName;
	//	子节点索引所在VCN
} *PHXIndex, *LPHXIndex;

//	读Buffer时范围
struct CHXReadBufferRange
{
	UINT32	m_i32ReadSize;
	UINT32	m_i32AllSize;
};

////	读MFT时范围
//struct CHXReadMFTRange
//{
//	UINT32	m_i32StartIndex;
//	UINT32	m_i32EndIndex;
//};

#pragma pack(pop)



//	属性类型
enum HXAttributeType
{
	HXAttribute_StandardInformation = 0x10,
	HXAttribute_List = 0x20,
	HXAttribute_FileName = 0x30,
	HXAttribute_ObjectID = 0x40,
	HXAttribute_SecurityDescriptor = 0x50,
	HXAttribute_VolumeName = 0x60,
	HXAttribute_VolumeInformation = 0x70,
	HXAttribute_Data = 0x80,
	HXAttribute_IndexRoot = 0x90,
	HXAttribute_IndexAllocation = 0xA0,
	HXAttribute_Bitmap = 0xB0,
	HXAttribute_ReparsePoint = 0xC0,
	HXAttribute_EAInformation = 0xD0,
	HXAttribute_EA = 0xD0,
	HXAttribute_EFS = 0x100,
	HXAttribute_Unknow = 0xffffffff,
};

enum HXCompressFlag : UINT16
{
	HXCompressFlag_Unkown = 0x0000,
	HXCompressFlag_Compression = 0x0001,
	HXCompressFlag_Encryption = 0x4000,
	HXCompressFlag_SparseFile = 0x8000,
};

#ifdef _HX_USE_QT_
typedef std::vector<QPair<INT64, INT64> >	LCNType;
#else
typedef std::vector<std::pair<INT64, INT64> >	LCNType;
#endif // _HX_USE_QT_

struct VCNData
{
	UINT64	m_u64StartCluster;
	UINT64	m_u64EndCluster;
	LCNType	m_vecLCN;

	VCNData() :m_u64StartCluster(-1L), m_u64EndCluster(-1L) {}
};
typedef std::vector<VCNData>					VCNType;
typedef std::vector<INT8>							BitMapType;

//	查找索引信息参数
typedef struct CHXIndexParam
{
	LPHXIndex		m_pIndex;
	//LPHXDiskParam	m_pDiskParam;
	LPBYTE			m_pBuffer;
	HANDLE			m_hDisk;
	UINT32			m_i32ReadSize;
	UINT32			m_i32AllSize;
	//UINT32			m_i32StartIndex;
	//UINT32			m_i32EndIndex;
	VecFileInfo* m_pvecFileInfo;
#ifdef _HX_USE_QT_
	std::wstring			m_strDir;
#else
	std::wstring	m_strDir;
#endif // _HX_USE_QT_
	//std::wstring	m_strFileName;

	BOOL			m_bFindFile;//是否为查找文件
	INT64			m_i64MFTNumber;//目标文件的MFT

	CHXIndexParam() : m_pIndex(nullptr),
		m_pvecFileInfo(nullptr),
		m_pBuffer(nullptr),
		m_hDisk(INVALID_HANDLE_VALUE),
		m_i32ReadSize(0),
		m_i32AllSize(0),
		//m_i32StartIndex(0),
		//m_i32EndIndex(0),
		m_bFindFile(FALSE),
		m_i64MFTNumber(-1L)
	{

	}
} *LPHXIndexParam;


//	查找索引信息参数
typedef struct CHXFindFileParam
{
	UINT8		m_u8PermanentFlag;
	UINT16		m_u16CompressFlage;				//	压缩标识
	UINT32		m_u8PermanentSize;

	UINT64		m_u64SpaceSize;
	UINT64		m_u64RealSize;

	LPBYTE		m_pPermanentBuffer;
	//LCNType		m_vecLVN;
	VCNType		m_vecVCN;//VCN和LCN对应关系
	BitMapType	m_vecBitMap;//和VCN对应的位图信息

	CHXFindFileParam() : m_pPermanentBuffer(nullptr),
		m_u8PermanentFlag(0),
		m_u8PermanentSize(0),
		m_u16CompressFlage(0),
		m_u64SpaceSize(0),
		m_u64RealSize(0)
	{

	}
	~CHXFindFileParam()
	{
		_HX_DELETE_PTR_ARRAY_(m_pPermanentBuffer);
	}
	CHXFindFileParam(const CHXFindFileParam&) = delete;
	CHXFindFileParam& operator=(const CHXFindFileParam&) = delete;
} *LPHXFindFileParam;

//	文件记录信息
struct CHXFileRecordData
{
	UINT64	m_u64VCNStart;
	UINT64	m_u64VCNEnd;
	LCNType	m_vecLCN;
};

enum CHXBufferType
{
	CHXBufferType_Unknown = 0,
	CHXBufferType_INDEX,
	CHXBufferType_MFT,
};

class CHXBufferPool;
//	内存缺页 时钟(CLOCK)置换算法
struct CHXBufferData
{
	//friend class CHXBufferPool;
	//friend class HXNTFSCache;
	DWORD			m_ThreaID;
private:
	UINT16			m_u16Used;		// 0代表未使用，1代表占用
	UINT16			m_u16Flag;		// 0代表未使用，1代表正在使用
	LPBYTE			m_pBuffer;
	UINT64			m_u64BufferStart;
	UINT64			m_u64RealSize;
	UINT64			m_u64SpaceSize;
#ifdef _HX_USE_QT_
	QReadWriteLock	m_lock;
#else
	std::shared_mutex m_lock;
#endif // _HX_USE_QT_

public:
	CHXBufferData() :
		m_pBuffer(nullptr)
		, m_u64RealSize(0)
		, m_u64SpaceSize(0)
		, m_u16Flag(0)
		, m_u64BufferStart(0)
		, m_ThreaID(-1)
	{}

	void InitLock(UINT64 u64BufferSize)
	{
#ifdef _HX_USE_QT_
		QWriteLocker locker(&m_lock);
#else
		std::unique_lock<std::shared_mutex> locker(m_lock);
#endif // _HX_USE_QT_

		m_u16Flag = 0;
		m_pBuffer = new BYTE[u64BufferSize];
		m_u64BufferStart = 0;
		m_u16Used = 0;
		m_u64RealSize = 0;
		m_u64SpaceSize = u64BufferSize;
	}

	bool ClockReplacementLock()
	{
#ifdef _HX_USE_QT_
		QWriteLocker locker(&m_lock);
#else
		std::unique_lock<std::shared_mutex> locker(m_lock);
#endif // _HX_USE_QT_
		if (0 == m_u16Used)
		{
			if (0 == m_u16Flag)
			{
				return true;
			}
			else
			{
				m_u16Flag = 0;
			}
		}
		return false;
	}

	void SetUsedLock(bool bUsed)
	{
#ifdef _HX_USE_QT_
		QWriteLocker locker(&m_lock);
#else
		std::unique_lock<std::shared_mutex> locker(m_lock);
#endif // _HX_USE_QT_
		if (bUsed)
		{
			m_u16Used++;
			m_u16Flag = 1;
		}
		else
		{
			m_u16Used--;
			assert(false == (m_u16Used & 0x8000));
		}
	}

	UINT64 GetSpaceSizeLock()
	{
#ifdef _HX_USE_QT_
		QReadLocker locker(&m_lock);
#else
		std::shared_lock<std::shared_mutex> locker(m_lock);
#endif // _HX_USE_QT_
		return m_u64SpaceSize;
	}

	LPBYTE GetBufferLock(LARGE_INTEGER i64FilePointer, DWORD i32FileRecordSize, DWORD& i32Readsize)
	{
		if (0 == m_u64RealSize)
		{
			return nullptr;
		}
#ifdef _HX_USE_QT_
		QReadLocker locker(&m_lock);
#else
		std::shared_lock<std::shared_mutex> locker(m_lock);
#endif // _HX_USE_QT_
		LPBYTE pBuffer = nullptr;

		if (i64FilePointer.QuadPart >= m_u64BufferStart
			&& (i64FilePointer.QuadPart + i32FileRecordSize) <= (m_u64BufferStart + m_u64RealSize))
		{
			INT64 i64Offset = i64FilePointer.QuadPart - m_u64BufferStart;
			pBuffer = m_pBuffer + i64Offset;
			i32Readsize = m_u64SpaceSize - i64Offset;
			m_u16Used++;
			m_u16Flag = 1;
		}
		return pBuffer;
	}

	void ResetLock()
	{
#ifdef _HX_USE_QT_
		QWriteLocker locker(&m_lock);
#else
		std::unique_lock<std::shared_mutex> locker(m_lock);
#endif // _HX_USE_QT_
		m_u64RealSize = 0;
	}

	bool FreeBufferLock(LPBYTE pBuffer)
	{
#ifdef _HX_USE_QT_
		QWriteLocker locker(&m_lock);
#else
		std::unique_lock<std::shared_mutex> locker(m_lock);
#endif // _HX_USE_QT_
		if (m_pBuffer <= pBuffer
			&& pBuffer <= (m_pBuffer + m_u64SpaceSize))
		{
			m_u16Used--;
#ifdef _HX_USE_QT_
			assert(false == (m_u16Used & 0x8000));
#else
			assert(false == (m_u16Used & 0x8000));
#endif // _HX_USE_QT_
			return true;
		}
		return false;
	}
	bool FreeBufferLock()
	{
		SetUsedLock(false);
		return true;
	}

	LPBYTE WriteBufferLock()
	{
#ifdef _HX_USE_QT_
		m_lock.lockForWrite();
#else
		m_lock.lock();
#endif // _HX_USE_QT_
		return m_pBuffer;
	}
	LPBYTE ReadBufferLock()
	{
#ifdef _HX_USE_QT_
		m_lock.lockForRead();
#else
		m_lock.lock_shared();
#endif // _HX_USE_QT_
		return m_pBuffer;
	}
	// 需要lock
	void SetStart(UINT64 u64Start)
	{
		m_u64BufferStart = u64Start;
	}
	// 需要lock
	void SetRealSize(UINT64 u64RealSize)
	{
		m_u64RealSize = u64RealSize;
	}
	UINT64 GetSpaceSize()
	{
		return m_u64SpaceSize;
	}

	void UnLock()
	{
		m_lock.unlock();
	}

	~CHXBufferData()
	{
		if (nullptr != m_pBuffer)
		{
			delete[] m_pBuffer;
		}
		m_pBuffer = nullptr;
	}

};
_HX_DECLAR_SHARED_POINTER(CHXBufferData)

#ifdef _HX_USE_QT_

typedef QList<CHXBufferDataPtr> ListHXBufferPtr;
typedef QSet<UINT64> SetMFTNumber;

#else

typedef std::vector<CHXBufferDataPtr> VectorHXBufferPtr;
typedef std::set<UINT64> SetMFTNumber;

#endif // _HX_USE_QT_

typedef std::vector<CHXFileRecordData> VecFileRecordData;


_HX_DECLAR_SHARED_POINTER(CHXFileRecordHeader)
_HX_DECLAR_SHARED_POINTER(CHXAttributeHeader)
_HX_DECLAR_SHARED_POINTER(CHXFileRecordListBody)
_HX_DECLAR_SHARED_POINTER(CHXFileRecordStandardInformationBody)
_HX_DECLAR_SHARED_POINTER(CHXFileRecordFileNameBody)
_HX_DECLAR_SHARED_POINTER(CHXFileRecordIndexRoot)
_HX_DECLAR_SHARED_POINTER(CHXFileRecordIndexHeader)
_HX_DECLAR_SHARED_POINTER(CHXIndexHeader)
_HX_DECLAR_SHARED_POINTER(CHXIndex)


