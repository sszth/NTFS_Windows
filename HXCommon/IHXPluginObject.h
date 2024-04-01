#pragma once
#include <qsharedpointer.h>


class IHXPluginObject
{
public:
	explicit IHXPluginObject(	const int & iPluginID, 
								const QString & strPluginName,
								const QString & strFileName)
								:_iPluginID(iPluginID), 
								_strPluginName(strPluginName),
								_strPluginDll(strFileName){}
	explicit IHXPluginObject(	const int && iPluginID, 
								const QString && strPluginName,
								const QString && strFileName)
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

	const QString & GetPluginName() const
	{
		return _strPluginName;
	}

	const QString& GetPluginDll() const
	{
		return _strPluginDll;
	}

	bool operator < (const IHXPluginObject& other) const
	{
		return _iPluginID < other._iPluginID;  //这里就改成小于号，QMap里的排序就成了升序和QMap<QString, int>默认排序一样了
	}
	bool operator > (const IHXPluginObject& other) const
	{
		return _iPluginID > other._iPluginID;  //这里就改成小于号，QMap里的排序就成了升序和QMap<QString, int>默认排序一样了
	}
	bool operator == (const IHXPluginObject& other) const
	{
		return _iPluginID == other._iPluginID;  //这里就改成小于号，QMap里的排序就成了升序和QMap<QString, int>默认排序一样了
	}

	IHXPluginObject(const IHXPluginObject& obj) = delete;
	IHXPluginObject& operator == (const IHXPluginObject& other) = delete;
protected:
	int		_iPluginID;
	QString	_strPluginName;
	//	相对路径加名称
	QString	_strPluginDll;
};
typedef QSharedPointer<IHXPluginObject> IHXPluginObjectPtr;

