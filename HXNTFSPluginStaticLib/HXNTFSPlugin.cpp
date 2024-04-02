#include "stdafx.h"

#include <iostream>
#include <algorithm>

#ifdef _HX_USE_QT_
#include <QTime>
#include <QDebug>
#else
#include <chrono>
#endif // _HX_USE_QT_


#include "../HXCommon/HXExplorerCmd.h"

#include "HXNTFSPlugin.h"


//int HX_Initialize(
//	int iID,
//	const std::wstring & strPluginName,
//	const std::wstring & strPluginDll,
//	IHXPluginObjectPtr & pPlugin)
//{
//	
//	pPlugin = std::make_shared<CHXNTFSPlugin>(iID, strPluginName, strPluginDll);
//	return  0;
//}


CHXNTFSPlugin::~CHXNTFSPlugin()
{
	Clear();
}

int CHXNTFSPlugin::Initialize()
{
	return 0;
}

int CHXNTFSPlugin::Release()
{
	return 0;
}

int CHXNTFSPlugin::OnCmd(UINT32 i32Cmd, void* pParam, void* pReturn)
{
	INT32 i32Result = 0;

	switch ((HXExplorerPluginCmd)i32Cmd)
	{
	case HXExplorerPluginCmd::HXExplorerPluginCmd_ReadDirectory:
	{
#ifdef _HX_USE_QT_
		QTime time;
		time.start();
#else
		auto begin = std::chrono::high_resolution_clock::now();
#endif // _HX_USE_QT_
		LPHXReadDirectory pDir = (LPHXReadDirectory)pParam;
		//qInfo() << "Directory File Cmd Recevie:" << pDir->m_wstrDirectory;
		CHXNTFSPartitionPtr pPartition = GetPartition(pDir->m_wstrDirectory);
		if (nullptr == pPartition)
		{
			pPartition = CreatePartition(pDir->m_wstrDirectory);
		}
		i32Result = pPartition->FindDirectory(pDir, pDir->m_listFileInfo);

		std::sort(pDir->m_listFileInfo.begin(), pDir->m_listFileInfo.end(), [](auto item, auto itemTmp)
			{
				assert(nullptr != item && nullptr != itemTmp);
				// TODO: 大小写

				std::transform(itemTmp->m_szFileName.begin(), itemTmp->m_szFileName.end(), itemTmp->m_szFileName.begin(), ::tolower);
				std::transform(item->m_szFileName.begin(), item->m_szFileName.end(), item->m_szFileName.begin(), ::tolower);
				return item->m_szFileName.compare(itemTmp->m_szFileName) <= 0 ? true : false;
			}
		);

#ifdef _HX_USE_QT_
		std::wstring s;
		s = time.elapsed() / 1000.0;
		qInfo() << time.elapsed() / 1000.0 << "s";
#else
		auto end = std::chrono::high_resolution_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
		printf("Time measured: %.3f seconds.\n", elapsed.count() * 1e-9);
#endif // _HX_USE_QT_
		break;
	}
	case HXExplorerPluginCmd::HXExplorerPluginCmd_ReadFile:
	{
		LPHXReadFileInfo pFileInfo = (LPHXReadFileInfo)pParam;
		//qInfo() << "Read File Cmd Recevie:" << pFileInfo->m_wstrDirectory;
		CHXNTFSPartitionPtr pPartition = GetPartition(pFileInfo->m_wstrDirectory);
		if (nullptr == pPartition)
		{
			assert(0);
			//qCritical() << "Get Partition Failed!";
			return -1;
		}
		i32Result = pPartition->ReadFile(pFileInfo);
	}
	break;
	case HXExplorerPluginCmd::HXExplorerPluginCmd_Refresh:
	{
		LPHXRefreshFileInfo pFileInfo = (LPHXRefreshFileInfo)pParam;
		//qInfo() << "Refresh File Cmd Recevie:" << pFileInfo->m_wstrDirectory;
		CHXNTFSPartitionPtr pPartition = GetPartition(pFileInfo->m_wstrDirectory);
		if (nullptr == pPartition)
		{
			assert(0);
			//qCritical() << "Get Partition Failed!";
			return -1;
		}
		pPartition->Refresh();
	}
	break;
	default:
		break;
	}
	return i32Result;
}

void CHXNTFSPlugin::Clear()
{
}

size_t Replace(std::wstring& strValue, const std::wstring& strOld, const std::wstring& strNew)
{
	size_t iCount = 0;
	if (strOld.empty())
		return iCount;
	std::wstring::size_type iFind = 0;
	while (true)
	{
		iFind = strValue.find(strOld, iFind);
		if (std::wstring::npos == iFind)
			break;
		strValue.replace(iFind, strOld.size(), strNew);
		++iCount;
		iFind += strNew.size();
	}
	return iCount;
}

std::wstring CHXNTFSPlugin::GetPartitionName(const std::wstring& strFileName)
{
	std::wstring strPartition = strFileName;
	Replace(strPartition, L"/", L"\\");
	int i32Pos = strPartition.find(L"\\");
	if (-1 != i32Pos)
	{
		strPartition = strPartition.substr(0, i32Pos);
	}
	return strPartition;
}


CHXNTFSPartitionPtr CHXNTFSPlugin::CreatePartition(const std::wstring& strFileName)
{
	std::wstring strPartition = GetPartitionName(strFileName);
#ifdef _HX_USE_QT_
	CHXNTFSPartitionPtr pPartition = QSharedPointer<CHXNTFSPartition>(new CHXNTFSPartition(strPartition));
#else
	CHXNTFSPartitionPtr pPartition = std::make_shared<CHXNTFSPartition>(strPartition);
#endif // _HX_USE_QT_
	m_listPartition.push_back(pPartition);
	return pPartition;
}

CHXNTFSPartitionPtr CHXNTFSPlugin::GetPartition(const std::wstring& strFileName)
{
	std::wstring strPartition = GetPartitionName(strFileName);
	CHXNTFSPartitionPtr pResult = nullptr;
	for (size_t i = 0; i < m_listPartition.size(); i++)
	{
		if (strPartition == m_listPartition[i]->GetPartitionName())
		{
			pResult = m_listPartition[i];
		}
	}
	return pResult;
}
