#pragma once
#include <Windows.h>
#include <QSharedPointer>
#include <QWeakPointer> 
#include <QReadWriteLock>

#define _HX_DECLAR_SHARED_POINTER(X) typedef QSharedPointer<X> X##Ptr;
#define _HX_DECLAR_WEAK_POINTER(X) typedef QWeakPointer<X> X##WeakPtr;

enum class HXFileNameSpace : int8_t
{
	HXFileNameSpace_POSIZ		= 0x00,
	HXFileNameSpace_Win32		= 0x01,
	HXFileNameSpace_DOS			= 0x02,
	HXFileNameSpace_WINAndDOS	= 0x03,
};

#pragma pack(push)
#pragma pack(1)
// BPB
typedef struct CHXBPB
{
	//	0x00
	BYTE	m_i8ArrJmp[3];
	BYTE	m_i8ArrFileSystemID[8];
	short	m_i16SectorSize;			//	������С,�ֽ�
	BYTE	m_i8ClusterSectorSize;		//	ÿ��������
	BYTE	m_i8ArrReserve[2];

	//	0x10
	BYTE	m_i8ArrZero[3];				//	��Ϊ0
	BYTE	m_i8ArrNTFSZero1[2];		//	NTFS��Ϊ0
	BYTE	m_i8Device;					//	��������
	short	m_i16Zero;
	short	m_i16TrackSectorSize;		//	�ŵ�������
	short	m_i16Head;					//	��ͷ��
	INT32	m_i32HideSector;			//	����������

	//	0x20
	BYTE	m_i8ArrNTFSZero2[4];		//	NTFS��Ϊ0
	BYTE	m_i8ArrNTFSNotUse[4];		//	NTFSδʹ��,��Ϊ80008000
	INT64	m_i64SectorTotalSize;		//	��������

	//	0x30
	INT64	m_i64MFTStartCluster;		//	MFT��ʼ��
	INT64	m_i64MFTMirrStartCluster;	//	MFTMirr��ʼ��

	//	0x40
	BYTE	m_i8FileRecord;				//	�ļ���¼��С����
	BYTE	m_i8ArrNotUse[3];
	BYTE	m_i8IndexBufferSize;		//	������С����
	BYTE	m_i8ArrNotUse2[3];
	INT64	m_i64VolumeSerialNumber;	//	�����к�

	// 0x50
	int		m_i32CheckCode;

}*PHXBPB, *LPHXBPB;

_HX_DECLAR_SHARED_POINTER(CHXBPB)
_HX_DECLAR_WEAK_POINTER(CHXBPB)

// �ļ���¼ͷ
typedef struct CHXFileRecordHeader
{
	//	0x00
	BYTE	m_i8ArrMFT[4];					//	FILE
	INT16	m_i16UpdateSequenceNumber;		//	�������кŵ�ƫ��
	INT16	m_i16UpdateSequenceNumberSize;	//	�������кŵĴ�С������
	INT64	m_i64LogSequence;				//	��־�ļ����


	//	0x10
	INT16	m_i16Sequence;					//	���к�
	INT16	m_i16HardLinkSize;				//	Ӳ��������
	INT16	m_i16FirstAttribute;			//	��һ������ƫ�Ƶ�ַ
	INT16	m_i16Flag;						//	��־�������Ƿ���ʹ��ɾ����
	UINT32	m_u32FileRecordRealSize;		//	�ļ���¼ʵ�ʳ���
	UINT32	m_i32FileRecordSize;			//	�ļ���¼���䳤��

	//	0x20
	INT64	m_i64Index;						//	�����ļ���¼����
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

//	��פ����ͷ��ǳ�פ����ͷ�Ĺ���ͷ
typedef struct CHXAttributeHeader
{
	//	0x00
	INT32	m_i32Flag;						//	���Ա�ʶ	���������Ա�ʶΪ10H��30H	���������Ա�ʶΪ90H��B0H
	INT32	m_i32TotalLength;				//	��������ͷ���ڳ���
	BYTE	m_u8PermanentFlag;				//	��פ���� 0��פ
	BYTE	m_u8NameLength;					//	���������� �ж��Ƿ�����������

	// 0x0B
	short	m_i16NameStartOffset;			//	�������ƿ�ʼƫ��
	UINT16	m_u16CompressFlage;				//	ѹ����ʶ
	short	m_i16ID;						//	����ID

	union
	{
		// ��פ����ͷ
		struct CHXPermanentAttributeHeader
		{
			//	0x10
			INT32	m_i32BodyLength;				//	�����峤��
			short	m_i32BodyOffset;				//	�����忪ʼƫ��
			BYTE	m_i8IndexFlag;
			BYTE	m_i8Reserve;
			// �������������ͷ������buffer��Ϊ�������ƺ�Ϊ������
		} m_PermanentAttribute;

		// �ǳ�פ����ͷ
		struct CHXVariableAttribute
		{
			//	0x10
			INT64	m_i64BodyStartCluster;			//	��������ʼ����غ�VCN
			INT64	m_i64BodyEndCluster;			//	�������������غ�

			//	0x20
			short	m_i16RunListOffset;				//	Run List��ƫ��
			short	m_i16CompressSize;				//	ѹ����λ��С
			INT32	m_i32NotUse;
			INT64	m_i64BodySpaceSize;				//	����������С

			//	0x30
			INT64	m_i64BodyRealSize;				//	������ʵ�ʴ�С
			// �������������ͷ������buffer��Ϊ�������ƺ�Ϊ������
		} m_VariableAttribute;
	}m_unExpand;

	//	Ŀǰ�汾ֻ��2���ֽ�
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

	//	����Ϊ����
} *PHXFileRecordListBody, *LPHXFileRecordListBody;

typedef struct CHXFileRecordStandardInformationBody
{
	//	0x00
	INT64	m_i64CreateTime;
	INT64	m_i64LastModifyTime;

	//	0x10
	INT64	m_i64LastModifyFileRecordTime;		//	MFT�޸�ʱ��
	INT64	m_i64LastVisitTime;

	//	0x20
	INT32	m_i32FileType;						//	��ͳ�ļ�����
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
	INT64	m_i64LastModifyFileRecordTime;		//	MFT�޸�ʱ��

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
	//	0x42 �����������
} *PHXFileRecordFileNameBody, *LPHXFileRecordFileNameBody;

typedef struct CHXFileRecordIndexRoot
{
	INT32	m_i32Type;				//
	INT32	m_i32CheckRule;
	INT32	m_i32IndexBufferSize;	//	ÿ�������������ķ����С ���ֽڣ�
	INT8	m_i8IndexBufferCluster;	//	ÿ����������Ĵ���
	INT8	m_i8Arr[3];				//	������
} *PHXFileRecordIndexRoot, *LPHXFileRecordIndexRoot;

typedef struct CHXFileRecordIndexHeader
{
	INT32	m_i32FirstIndexOffset;
	INT32	m_i32AllIndexRealSize;
	INT32	m_i32AllIndexSpaceSize;	//	ÿ�������������ķ����С ���ֽڣ�
	INT8	m_i8IndexFlag;			//	������ʶ��1Ϊ��������0ΪС����
	INT8	m_i8Arr[3];				//	������
} *PHXFileRecordIndexHeader, *LPHXFileRecordIndexHeader;

//	��׼����ͷ
typedef struct CHXIndexHeader
{
	//	0x00
	INT32	m_i32Flag;						//	��ΪINDX
	INT16	m_i16UpdateSequenceNumber;		//	�������к�
	INT16	m_i16UpdateSequenceNumberSize;	//	�������к��������������Ϊ��λ�Ĵ�С��S��
	INT64	m_i64LogFileSequenceNumber;

	//	0x10
	INT64	m_i64VCN;

	INT32	m_i32Offset;					//	����Ǵ�0x18��ʼ��ƫ��
	INT32	m_i32IndexRealSize;				

	//	0x20
	INT32	m_i32IndexSpaceSize;			//	����Ǵ�0x18��ʼ�Ĵ�С
	INT8	m_i8Node;
	INT8	m_i8ArrNotUse[3];
	UINT16	m_u16UpdateSequence;

	//	0x2A��ʼΪ������������ 2S-2

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
	INT16	m_i16Flag;				//	1����һ���ӽڵ㣬2Ϊ���һ����
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
	//	�ļ�������䵽8��������
	UINT8	m_u8ArrName;
	//	�ӽڵ���������VCN
} *PHXIndex, *LPHXIndex;

//	��Bufferʱ��Χ
struct CHXReadBufferRange
{
	UINT32	m_i32ReadSize;
	UINT32	m_i32AllSize;
};

////	��MFTʱ��Χ
//struct CHXReadMFTRange
//{
//	UINT32	m_i32StartIndex;
//	UINT32	m_i32EndIndex;
//};

#pragma pack(pop)



//	��������
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

typedef QVector<QPair<INT64, INT64> >	LCNType;
struct VCNData
{
	UINT64	m_u64StartCluster;
	UINT64	m_u64EndCluster;
	LCNType	m_vecLCN;

	VCNData() :m_u64StartCluster(-1L), m_u64EndCluster(-1L) {}
};
typedef QVector<VCNData>					VCNType;
typedef std::vector<INT8>							BitMapType;

//	����������Ϣ����
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
	VecFileInfo*	m_pvecFileInfo;
	QString			m_strDir;
	//QString	m_strFileName;

	BOOL			m_bFindFile;//�Ƿ�Ϊ�����ļ�
	INT64			m_i64MFTNumber;//Ŀ���ļ���MFT

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


//	����������Ϣ����
typedef struct CHXFindFileParam
{
	UINT8		m_u8PermanentFlag;
	UINT16		m_u16CompressFlage;				//	ѹ����ʶ
	UINT32		m_u8PermanentSize;

	UINT64		m_u64SpaceSize;
	UINT64		m_u64RealSize;

	LPBYTE		m_pPermanentBuffer;
	//LCNType		m_vecLVN;
	VCNType		m_vecVCN;//VCN��LCN��Ӧ��ϵ
	BitMapType	m_vecBitMap;//��VCN��Ӧ��λͼ��Ϣ

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
	CHXFindFileParam(const CHXFindFileParam &) = delete;
	CHXFindFileParam & operator=(const CHXFindFileParam &) = delete;
} *LPHXFindFileParam;

//	�ļ���¼��Ϣ
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
//	�ڴ�ȱҳ ʱ��(CLOCK)�û��㷨
struct CHXBufferData
{
	//friend class CHXBufferPool;
	//friend class HXNTFSCache;
	DWORD			m_ThreaID;
private:
	UINT16			m_u16Used;		// 0����δʹ�ã�1����ռ��
	UINT16			m_u16Flag;		// 0����δʹ�ã�1��������ʹ��
	LPBYTE			m_pBuffer;
	UINT64			m_u64BufferStart;
	UINT64			m_u64RealSize;
	UINT64			m_u64SpaceSize;
	QReadWriteLock	m_lock;
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
		QWriteLocker locker(&m_lock);
		m_u16Flag = 0;
		m_pBuffer = new BYTE[u64BufferSize];
		m_u64BufferStart = 0;
		m_u16Used = 0;
		m_u64RealSize = 0;
		m_u64SpaceSize = u64BufferSize;
	}

	bool ClockReplacementLock()
	{
		QWriteLocker locker(&m_lock);
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
		QWriteLocker locker(&m_lock);
		if (bUsed)
		{
			m_u16Used++;
			m_u16Flag = 1;
		}
		else
		{
			m_u16Used--;
			Q_ASSERT(false == (m_u16Used & 0x8000));
		}
	}

	UINT64 GetSpaceSizeLock()
	{
		QReadLocker locker(&m_lock);
		return m_u64SpaceSize;
	}

	LPBYTE GetBufferLock(LARGE_INTEGER i64FilePointer, DWORD i32FileRecordSize, DWORD & i32Readsize)
	{
		if (0 == m_u64RealSize)
		{
			return nullptr;
		}
		QReadLocker locker(&m_lock);
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
		QWriteLocker locker(&m_lock);
		m_u64RealSize = 0;
	}

	bool FreeBufferLock(LPBYTE pBuffer)
	{
		QWriteLocker locker(&m_lock);
		if (m_pBuffer <= pBuffer
			&& pBuffer <= (m_pBuffer + m_u64SpaceSize))
		{
			m_u16Used--;
			Q_ASSERT(false == (m_u16Used & 0x8000));
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
		m_lock.lockForWrite();
		return m_pBuffer;
	}
	LPBYTE ReadBufferLock()
	{
		m_lock.lockForRead();
		return m_pBuffer;
	}
	// ��Ҫlock
	void SetStart(UINT64 u64Start)
	{
		m_u64BufferStart = u64Start;
	}
	// ��Ҫlock
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
typedef QList<CHXBufferDataPtr> ListHXBufferPtr;

typedef std::vector<CHXFileRecordData> VecFileRecordData;

typedef QSet<UINT64> SetMFTNumber;


_HX_DECLAR_SHARED_POINTER(CHXFileRecordHeader)
_HX_DECLAR_SHARED_POINTER(CHXAttributeHeader)
_HX_DECLAR_SHARED_POINTER(CHXFileRecordListBody)
_HX_DECLAR_SHARED_POINTER(CHXFileRecordStandardInformationBody)
_HX_DECLAR_SHARED_POINTER(CHXFileRecordFileNameBody)
_HX_DECLAR_SHARED_POINTER(CHXFileRecordIndexRoot)
_HX_DECLAR_SHARED_POINTER(CHXFileRecordIndexHeader)
_HX_DECLAR_SHARED_POINTER(CHXIndexHeader)
_HX_DECLAR_SHARED_POINTER(CHXIndex)


