#pragma once
#include "../HXCommon/IHXPluginObject.h"

#include <QLibrary>
#include <QMap>
#define HX_GOOGLETEST
typedef QSharedPointer<QLibrary> QLibraryPtr;

struct CHXPluginManageMapData
{
	IHXPluginObjectPtr	_pPlugin;
	QLibraryPtr			_pLib;
	CHXPluginManageMapData()
		: _pPlugin(nullptr),
		_pLib(nullptr) {}
	CHXPluginManageMapData(
		IHXPluginObjectPtr pPlugin,
		QLibraryPtr	pLib)
		:_pPlugin(pPlugin),
		_pLib(pLib) {}
	~CHXPluginManageMapData()
	{
		_pPlugin.reset();
		_pLib.reset();
	}
};
class CHXPluginManage
{
public:
	CHXPluginManage() = default;
	~CHXPluginManage() = default;

	CHXPluginManage(const CHXPluginManage& other) = delete;
	CHXPluginManage& operator= (const CHXPluginManage& other) = delete;

public:
	int Initialize(const QString& strSetting);
	virtual int OnCmd(uint32_t u32Cmd, void* pLParam, void* pRParam);

public:
	int AddPlugin(const IHXPluginObjectPtr pPlugin, const QLibraryPtr pLib);

	int RemovePlugin(int iPluginID);

	void Clear();	

protected:
	QMap<int, CHXPluginManageMapData>	_mapPluginManage;
};
typedef QSharedPointer<CHXPluginManage> CHXPluginManagePtr;
