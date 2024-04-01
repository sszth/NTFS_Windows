#include "stdafx.h"
#include "HXFileSystemPlugin.h"
#include "../HXCommon/HXExplorerCmd.h"
#include "HXFileSystemPluginManage.h"

#include "HXFileSystemPluginManage.h"

const QString g_strPluginSettingXml = "HXFileSystemPlugin.xml";

extern "C"
HXFILESYSTEM_EXPORT
int HX_Initialize(
	int iID,
	const QString & strPluginName,
	const QString & strPluginDll,
	IHXPluginObjectPtr & pPlugin)
{
	pPlugin =
		QSharedPointer<IHXPluginObject>(new HXFileSystemPlugin(iID, strPluginName, strPluginDll));
	return  0;
}


int HXFileSystemPlugin::OnCmd(uint32_t cmd, void * pLParam, void * pRParam)
{
	int i32Result = 0;
	HXExplorerPluginCmd u32Cmd = (HXExplorerPluginCmd)cmd;
	switch (u32Cmd)
	{
	case HXExplorerPluginCmd::HXExplorerPluginCmd_Initialize:
		Initialize();
		break;
	case HXExplorerPluginCmd::HXExplorerPluginCmd_Release:
		Release();
		break;
	case HXExplorerPluginCmd::HXExplorerPluginCmd_ReadDirectory:
	case HXExplorerPluginCmd::HXExplorerPluginCmd_ReadFile:
	case HXExplorerPluginCmd::HXExplorerPluginCmd_ReadClose:
	case HXExplorerPluginCmd::HXExplorerPluginCmd_Refresh:
		i32Result = m_pPluginManage->OnCmd(cmd, pLParam, pRParam);
		break;
	default:
		break;
	}
	return i32Result;
}

int HXFileSystemPlugin::Initialize()
{
	m_pPluginManage = QSharedPointer<CHXPluginManage>(new CHXPluginManage());
	m_pPluginManage->Initialize(g_strPluginSettingXml);
	return 0;
}

int HXFileSystemPlugin::Release()
{
	return 0;
}
