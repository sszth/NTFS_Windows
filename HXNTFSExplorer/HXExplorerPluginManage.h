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
	virtual int OnCmd(UINT32 llCmd, void* pLParam, void* pRParam);

public:
	int AddPlugin(const IHXPluginObjectPtr pPlugin, const QLibraryPtr pLib);

	int RemovePlugin(int iPluginID);

	void Clear();
#ifdef HX_GOOGLETEST
	int Write(const QString& strSetting);
#endif // !HX_GOOGLETEST
	
	int Initialize(const QString& strSetting);

private:
	QMap<int, CHXPluginManageMapData>	_mapPluginManage;
};
typedef QSharedPointer<CHXPluginManage> CHXPluginManagePtr;
