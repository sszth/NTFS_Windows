#pragma once
#include <qsharedpointer.h>


class IHXPluginObject
{
public:
	explicit IHXPluginObject(	const int & iPluginID, 
								const std::wstring & strPluginName,
								const std::wstring & strFileName)
								:_iPluginID(iPluginID), 
								_strPluginName(strPluginName),
								_strPluginDll(strFileName){}
	explicit IHXPluginObject(	const int && iPluginID, 
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
		return _iPluginID < other._iPluginID;  //����͸ĳ�С�ںţ�QMap�������ͳ��������QMap<std::wstring, int>Ĭ������һ����
	}
	bool operator > (const IHXPluginObject& other) const
	{
		return _iPluginID > other._iPluginID;  //����͸ĳ�С�ںţ�QMap�������ͳ��������QMap<std::wstring, int>Ĭ������һ����
	}
	bool operator == (const IHXPluginObject& other) const
	{
		return _iPluginID == other._iPluginID;  //����͸ĳ�С�ںţ�QMap�������ͳ��������QMap<std::wstring, int>Ĭ������һ����
	}

	IHXPluginObject(const IHXPluginObject& obj) = delete;
	IHXPluginObject& operator == (const IHXPluginObject& other) = delete;
protected:
	int		_iPluginID;
	std::wstring	_strPluginName;
	//	���·��������
	std::wstring	_strPluginDll;
};
typedef QSharedPointer<IHXPluginObject> IHXPluginObjectPtr;

