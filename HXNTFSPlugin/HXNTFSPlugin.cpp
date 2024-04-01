#include "stdafx.h"

#include <iostream>
#include <QTime>
#include <QDebug>

#include "../HXCommon/HXExplorerCmd.h"

#include "HXNTFSPlugin.h"


extern "C"
HXNTFSPLUGIN_EXPORT
int HX_Initialize(
	int iID,
	const QString & strPluginName,
	const QString & strPluginDll,
	IHXPluginObjectPtr & pPlugin)
{
	pPlugin =
		QSharedPointer<IHXPluginObject>(new CHXNTFSPlugin(iID, strPluginName, strPluginDll));
	return  0;
}


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

int CHXNTFSPlugin::OnCmd(UINT32 i32Cmd, void * pParam, void * pReturn)
{
	INT32 i32Result = 0;
	
	switch ((HXExplorerPluginCmd)i32Cmd)
	{
	case HXExplorerPluginCmd::HXExplorerPluginCmd_ReadDirectory:
	{
		QTime time;
		time.start();
		LPHXReadDirectory pDir = (LPHXReadDirectory)pParam;
		qInfo() << "Directory File Cmd Recevie:" << pDir->m_wstrDirectory;
		CHXNTFSPartitionPtr pPartition = GetPartition(pDir->m_wstrDirectory);
		if (nullptr == pPartition)
		{
			pPartition = CreatePartition(pDir->m_wstrDirectory);
		}
		i32Result = pPartition->FindDirectory(pDir, pDir->m_listFileInfo);

		std::sort(pDir->m_listFileInfo.begin(), pDir->m_listFileInfo.end(), [](auto item, auto itemTmp)
			{
				Q_ASSERT(nullptr != item && nullptr != itemTmp);
				return item->m_szFileName.compare(itemTmp->m_szFileName, Qt::CaseInsensitive) <= 0 ? true : false;
			}
		);
		QString s;
		s = time.elapsed() / 1000.0;
		qInfo() << time.elapsed() / 1000.0 << "s";
		break;
	}
	case HXExplorerPluginCmd::HXExplorerPluginCmd_ReadFile:
	{
		LPHXReadFileInfo pFileInfo = (LPHXReadFileInfo)pParam;
		qInfo() << "Read File Cmd Recevie:" << pFileInfo->m_wstrDirectory;
		CHXNTFSPartitionPtr pPartition = GetPartition(pFileInfo->m_wstrDirectory);
		if (nullptr == pPartition)
		{
			qCritical() << "Get Partition Failed!";
			return -1;
		}
		i32Result = pPartition->ReadFile(pFileInfo);
	}
	break;
	case HXExplorerPluginCmd::HXExplorerPluginCmd_Refresh:
	{
		LPHXRefreshFileInfo pFileInfo = (LPHXRefreshFileInfo)pParam;
		qInfo() << "Refresh File Cmd Recevie:" << pFileInfo->m_wstrDirectory;
		CHXNTFSPartitionPtr pPartition = GetPartition(pFileInfo->m_wstrDirectory);
		if (nullptr == pPartition)
		{
			qCritical() << "Get Partition Failed!";
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

QString CHXNTFSPlugin::GetPartitionName(const QString & strFileName)
{
	QString strPartition = strFileName;
	strPartition.replace(u8"/", u8"\\");
	int i32Pos = strPartition.indexOf(u8"\\");
	if (-1 != i32Pos)
	{
		strPartition = strPartition.mid(0, i32Pos);
	}
	return strPartition;
}


CHXNTFSPartitionPtr CHXNTFSPlugin::CreatePartition(const QString & strFileName)
{
	QString strPartition = GetPartitionName(strFileName);
	CHXNTFSPartitionPtr pPartition = QSharedPointer<CHXNTFSPartition>(new CHXNTFSPartition(strPartition));
	m_listPartition.push_back(pPartition);
	return pPartition;
}

CHXNTFSPartitionPtr CHXNTFSPlugin::GetPartition(const QString & strFileName)
{
	QString strPartition = GetPartitionName(strFileName);
	CHXNTFSPartitionPtr pResult = nullptr;
	for (size_t i = 0; i < m_listPartition.length(); i++)
	{
		if (strPartition == m_listPartition[i]->GetPartitionName())
		{
			pResult = m_listPartition[i];
		}
	}
	return pResult;
}
