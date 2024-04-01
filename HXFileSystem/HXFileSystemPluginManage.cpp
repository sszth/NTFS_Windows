#include "stdafx.h"
//#include <qmessagebox.h>
#include <QTextStream>

#include <QtXml\qdom.h>
#include <QFile>
#include <QDebug>

#include "../HXCommon/HXExplorerPluginID.h"
#include "../HXCommon/HXExplorerCmd.h"

#include "HXFileSystemPluginManage.h"


int CHXPluginManage::OnCmd(uint32_t llCmd, void* pLParam, void* pRParam)
{
	int nRet = 0;
	HXExplorerPluginCmd u32Cmd = (HXExplorerPluginCmd)llCmd;
	switch (u32Cmd)
	{
	case HXExplorerPluginCmd::HXExplorerPluginCmd_Initialize:
		break;
	case HXExplorerPluginCmd::HXExplorerPluginCmd_Release:
		break;
	case HXExplorerPluginCmd::HXExplorerPluginCmd_ReadDirectory:
	case HXExplorerPluginCmd::HXExplorerPluginCmd_Refresh:
	case HXExplorerPluginCmd::HXExplorerPluginCmd_ReadFile:
	case HXExplorerPluginCmd::HXExplorerPluginCmd_ReadClose:
		nRet = _mapPluginManage[_HX_PLUGIN_FILESYSTEM_ID_]._pPlugin->OnCmd(llCmd, pLParam, pRParam);
		break;
	default:
		break;
	}
	return nRet;
}

int CHXPluginManage::AddPlugin(const IHXPluginObjectPtr pPlugin, const QLibraryPtr pLib)
{
	_mapPluginManage[pPlugin->GetID()] = CHXPluginManageMapData(pPlugin, pLib);
	return 0;
}

int CHXPluginManage::RemovePlugin(int iPluginID)
{
	_mapPluginManage.remove(iPluginID);
	return 0;
}

void CHXPluginManage::Clear()
{
	for (auto pluginIter = _mapPluginManage.begin(); pluginIter != _mapPluginManage.end(); ++pluginIter)
	{
		pluginIter->_pPlugin->Release();
		pluginIter->_pPlugin.reset();
		if (true == pluginIter->_pLib->isLoaded())
		{
			if (false == pluginIter->_pLib->unload())
			{
				Q_ASSERT(0);
			}
		}
	}
	_mapPluginManage.clear();
}

int CHXPluginManage::Initialize(const QString& strSetting)
{
	QFile file(strSetting);
	if (!file.open(QIODevice::ReadOnly))
	{
		return -1;
	}

	QString errorStr;
	int errorLine;
	int errorColumn;
	{
		QDomDocument doc;
		//填充dom树
		if (!doc.setContent(&file, false, &errorStr, &errorLine,
			&errorColumn))//形参2，是否创建命名空间
		{
			QString strTmp;
			strTmp = QObject::tr("Parse error at line ");
			strTmp += errorLine;
			strTmp += QObject::tr(", column ");
			strTmp += errorColumn;
			strTmp += QObject::tr(": ");
			strTmp += errorStr;
			//inline static int critical(QWidget * parent, const QString & title,
			//	const QString & text,
			//	StandardButton button0, StandardButton button1 )
			//QMessageBox::critical(nullptr,
			//	QObject::tr("Error"),
			//	strTmp,
			//	QMessageBox::Ok,
			//	QMessageBox::Yes);
			return -1;
		}

		QDomElement docElement = doc.documentElement();

		QDomNode domNode = docElement.firstChild();
		while (!domNode.isNull())  //如果节点不空
		{
			QDomElement indexElement = domNode.toElement();
			if (indexElement.hasAttribute("ID") && indexElement.hasAttribute("Name"))
			{
				QString strPluginID = indexElement.attribute("ID");
				int iPluginID = strPluginID.toInt();
				QString strPluginName = indexElement.attribute("Name");
				QString strPluginDll = indexElement.text();

				using FUNC_Initialize = int (*)(int, const QString&, const QString&, IHXPluginObjectPtr&);
				QLibraryPtr pLib = QSharedPointer<QLibrary>(new QLibrary());
				pLib->setFileName(strPluginDll);
				if (false == pLib->load())
				{
					Q_ASSERT(0);
				}
				FUNC_Initialize funcInitialize = (FUNC_Initialize)pLib->resolve("HX_Initialize");
				if (nullptr == funcInitialize)
				{
					Q_ASSERT(0);
				}
				IHXPluginObjectPtr pPlugin;
				if (0 != funcInitialize(iPluginID, strPluginName, strPluginDll, pPlugin))
				{
					Q_ASSERT(0);
				}

				if (0 != pPlugin->Initialize())
				{
					Q_ASSERT(0);
				}
				AddPlugin(pPlugin, pLib);
				qInfo() << "HXNTFSExplorer Add Plugin:" << strPluginName;
			}

			//下一个兄弟节点,nextSiblingElement()是下一个兄弟元素
			domNode = domNode.nextSibling();
		}
	}
	file.close();

	return 0;
}
