#pragma once
#include <Windows.h>
#include <string>
#include <algorithm>
#ifdef _HX_USE_QT_
#include <QVector>
#else
#include <vector>
#endif // _HX_USE_QT_
#define HX_DIRECTORYFLAG			0x10000000

#define MAX_PATH          260

enum class HXExplorerPluginCmd : uint32_t
{
	HXExplorerPluginCmd_UnKonwn = 0,
	HXExplorerPluginCmd_Initialize,
	HXExplorerPluginCmd_Release,
	HXExplorerPluginCmd_ReadDirectory,
	HXExplorerPluginCmd_ReadFile,
	HXExplorerPluginCmd_ReadClose,
	HXExplorerPluginCmd_Refresh,

};

typedef struct HXFileInfo
{
	UINT64			m_u64SerialNumber;
	INT32			m_i32FileAttributes;
	UINT64			m_u64FileRealSize;
	UINT64			m_u64FileSpaceSize;
	UINT64			m_u64CreateTime;
	UINT64			m_u64ModifyTime;
	UINT32			m_U32SubFile;
	std::wstring			m_szFileName;
	//WCHAR			m_szFileName[MAX_PATH];
}*PHXFileInfo, * LPHXFileInfo;

#ifdef _HX_USE_QT_
typedef QVector<LPHXFileInfo> VecFileInfo;
#else
typedef std::vector<LPHXFileInfo> VecFileInfo;
#endif // _HX_USE_QT_
//Q_DECLARE_METATYPE(VecFileInfo);
typedef struct CHXReadDirectory
{
	std::wstring			m_wstrDirectory;
	VecFileInfo		m_listFileInfo;
	/*
	CHXReadDirectory() {}
	~CHXReadDirectory()
	{
		ClearVecFileInfo(m_listFileInfo);
	}

	CHXReadDirectory * operator=(CHXReadDirectory& other)
	{
		std::move()
	}
	*/
	void ClearVecFileInfo()
	{
		std::for_each(m_listFileInfo.cbegin(), m_listFileInfo.cend(), [](LPHXFileInfo pFileInfo) {
			if (nullptr != pFileInfo)
			{
				delete pFileInfo;
				pFileInfo = nullptr;
			}
			});

		m_listFileInfo.clear();
	}
} *LPHXReadDirectory;


typedef struct CHXReadFileInfo
{
	std::wstring			m_wstrDirectory;
	UINT64			m_u64CurPos;
	UINT64			m_u64ReadSize;
	UINT64			m_u64RealReadSize;
	UINT64			m_u64ValidPos;		//	œ‡∂‘Œª÷√
	UINT64			m_u64ValidSize;
	LPBYTE			m_pBuffer;
} *LPHXReadFileInfo;


typedef struct CHXRefreshFileInfo
{
	std::wstring			m_wstrDirectory;
} *LPHXRefreshFileInfo;