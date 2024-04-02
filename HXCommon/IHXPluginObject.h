#pragma once
#ifdef _HX_USE_QT_
#include <qsharedpointer.h>
#else
#include <memory>
#endif // _HX_USE_QT_


class IHXPluginObject
{
public:
	explicit IHXPluginObject(const int & iPluginID,
		const std::wstring & strPluginName,
		const std::wstring & strFileName)
		:_iPluginID(iPluginID),
		_strPluginName(strPluginName),
		_strPluginDll(strFileName) {}
	explicit IHXPluginObject(const int && iPluginID,
		const std::wstring && strPluginName,
		const std::wstring && strFileName)
		:_iPluginID(iPluginID),
		_strPluginName(strPluginName),
		_strPluginDll(strFileName) {}
	virtual ~IHXPluginObject() {}

public:
	virtual int OnCmd(uint32_t cmd, void* pLParam, void* pRParam) { return 0; }
	virtual int Initialize() = 0;
	virtual int Release() = 0;

	const int& GetID() const
	{
		return _iPluginID;
	}

	const std::wstring & GetPluginName() const
	{
		return _strPluginName;
	}

	const std::wstring& GetPluginDll() const
	{
		return _strPluginDll;
	}

	bool operator < (const IHXPluginObject& other) const
	{
		return _iPluginID < other._iPluginID;  //这里就改成小于号，QMap里的排序就成了升序和QMap<std::wstring, int>默认排序一样了
	}
	bool operator > (const IHXPluginObject& other) const
	{
		return _iPluginID > other._iPluginID;  //这里就改成小于号，QMap里的排序就成了升序和QMap<std::wstring, int>默认排序一样了
	}
	bool operator == (const IHXPluginObject& other) const
	{
		return _iPluginID == other._iPluginID;  //这里就改成小于号，QMap里的排序就成了升序和QMap<std::wstring, int>默认排序一样了
	}

	IHXPluginObject(const IHXPluginObject& obj) = delete;
	IHXPluginObject& operator == (const IHXPluginObject& other) = delete;
protected:
	int		_iPluginID;
	std::wstring	_strPluginName;
	//	相对路径加名称
	std::wstring	_strPluginDll;
};
#ifdef _HX_USE_QT_
typedef QSharedPointer<IHXPluginObject> IHXPluginObjectPtr;
#else
typedef std::shared_ptr<IHXPluginObject> IHXPluginObjectPtr;
#endif // _HX_USE_QT_

