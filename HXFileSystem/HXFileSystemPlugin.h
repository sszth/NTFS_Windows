#pragma once

#include "HXFileSystemPlugin_global.h"
#include "..\HXCommon\IHXPluginObject.h"
#include "..\HXCommon\IHXPluginObject.h"
#include "HXFileSystemPluginManage.h"

extern "C"
HXFILESYSTEM_EXPORT
int HX_Initialize(int, const QString&, const QString&, IHXPluginObjectPtr&);

class HXFileSystemPlugin : public IHXPluginObject
{
public:
	explicit HXFileSystemPlugin(const int& iPluginID,
		const QString& strPluginName,
		const QString& strFileName)
		:IHXPluginObject(iPluginID, strPluginName, strFileName) {}
	explicit HXFileSystemPlugin(const int&& iPluginID,
		const QString&& strPluginName,
		const QString&& strFileName)
		:IHXPluginObject(iPluginID, strPluginName, strFileName) {}
	virtual ~HXFileSystemPlugin() {}
public:
	virtual int OnCmd(uint32_t u32cmd, void* pLParam, void* pRParam) override;
	virtual int Initialize() override;
	virtual int Release() override;

private:
	CHXPluginManagePtr               m_pPluginManage;
};