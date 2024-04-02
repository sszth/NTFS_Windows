/*******************************************************************************************************************************************
*	2021年4月23日14:02:43	使用$MFT的DataRun获取同一文件夹下30w个文件。之前只能获取5w多个
*	2021年4月23日15:09:55	后续可以将属性获取放在一堆，返回属性的一个列表（但是20H不好处理）
*	2021年4月23日15:35:13	一个文件记录属性中只有1个的有10H、20H,其他例如80H、A0H可能有多个
*	2021年4月23日16:27:07	读取文件信息：1.读取在90H下的文件信息；2.读取在80H内部文件信息
*	2021年4月25日09:38:47	加密、稀疏可能为常驻.先不支持加密、稀疏、压缩情况，看后续时间
*	2021年4月25日10:19:10	$MFT中CHXFileRecordHeader::m_i16Flag 0代表被删除、1代表文件、2代表目录
*							$Bitmap中保存磁盘可用簇信息
*							普通文件记录中0xB0属性只与0x90属性有关，代表位图可用索引信息
*	2021年4月26日09:12:42	文件目录后续变成类
*	2021年4月27日09:28:48	0x30H属性可能有多个								存在硬链接情况,目前30属性只获取最后一个的创建时间、名称用索引中名称、大小访问80属性，已解决
*	2021年4月27日12:46:02	文件大小 以获取到第一个80属性为准暂时未考虑多流情况
*	2021年4月27日13:40:24	目录有0x90没有0xB0时，解析出有名称为null情况	已解决
*	2021年4月27日14:21:37	使用windows管理工具创建VHD，在L"G:\\$RECYCLE.BIN\\S-1-5-21-3682035386-2047908854-1240857724-1001\\"存在B0属性长度不正确情况
*	2021年4月28日15:38:05	在目录信息中存储目录索引位置,
*	2021年4月30日16:48:35	如果对每个mft和索引建立缓冲10w能0.5s  30w1.3s。但如果磁盘为500G如果填满MFT大概60G 对缓冲要求太大 使用缓冲池优化缓冲
*	2021年5月6日10:02:56	可能会遇到一个簇小于MFT大小情况
*	2021年5月6日10:03:32	获取$MFT时需要先获取80 再获取20属性
*	2021年5月6日11:11:31	读物理文件需要注意缓存区大小以及释放buffer
*	2021年5月8日14:55:06	如果有空余时间实用USN实现文件监控
*
*	2021年5月8日09:30:28	使用$MFT的DataRun获取同一文件夹下30w个文件的MFT。之前只能获取5w多个
*							一个文件记录属性中只有1个的有10H、20H,其他例如80H、A0H可能有多个
*							读取文件信息：1.读取在90H下的文件信息；2.读取在80H内部文件信息
*							$MFT中CHXFileRecordHeader::m_i16Flag 0代表被删除、1代表文件、2代表目录
*							0x30H属性可能有多个
*							文件大小 以获取到第一个80属性为准，暂时未考虑多流情况
*							MFT大小来自BPB、索引大小来自索引项分配大小加上固定头
*							如果读取出的信息就1、2个字节错误，可能更新序列未正确处理
*							如果找不到对应文件的文件记录，可能是没有走$MFT找对应文件记录
*							如果写入文件多出一些0D 可能是未以二进制写文件
*							如果写入文件多出一些00000  写入时未获取实际大小
*							考虑实现文件监控，使用QFileSystemWatcher进行监控会遇到2个问题：
*							1.文件正在写入，索引中长度为0;
*							2.文件在缓存，还未写入。索引中无对应信息
*							如果有空余时间实用USN实现文件监控
*
*							QFileSystemWatcher在windows下实际调用qfilesystemwatcher_win.cpp进行监听，使用WinAPI FindNextChangeNotification。
*							当变化时遍历缓存的文件信息列表与实际存储对比，判断文件参数、文件变更、文件移动，并将变化的文件通过信号发射出来.
*							FindNextChangeNotification无法像ReadDirectoryChangesW获取具体哪一个文件，也不能获取到具体变更细节。
*							例如创建文件刚开始数据还在缓存中FindNextChangeNotification会立即通知，ReadDirectoryChangesW可以控制在缓存写入完毕时通知。
*
*							windows文件系统在拷贝时会隔断时间刷新正在拷贝的文件大小。
*							目前猜测可能通过以下几种方式实现
*							1.通过USN；
*							2.每隔一段时间检测80属性,必须在产生索引后；
*	2021年5月17日09:37:01	目前监控有问题  是否监控子目录参数无效。暂时不动
*	2021年5月17日09:37:24	20属性分常驻和非常驻 非常驻下是data run
*
*
*

*	内存泄漏检查
*******************************************************************************************************************************************/
#pragma once
#include <Windows.h>
#include <unordered_map>
//#include <qhash.h>
#include <set>

#include "../HXCommon/HXExplorerCmd.h"
#include "../HXCommon/IHXPluginObject.h"
#include "../HXCommon/HXFileSystemCmd.h"

#include "../HXCommon/IHXPluginObject.h"
#include "../HXCommon/HXExplorerCmd.h"
#include "HXNTFSPartition.h"
#include "HXNTFSType.h"



//	后续将每个Disk分开，将操作传入Disk进行
class CHXNTFSPlugin : public IHXPluginObject
{
public:

	explicit CHXNTFSPlugin(const int& iPluginID,
		const std::wstring& strPluginName,
		const std::wstring& strFileName)
		:IHXPluginObject(iPluginID, strPluginName, strFileName) {}
	explicit CHXNTFSPlugin(const int&& iPluginID,
		const std::wstring&& strPluginName,
		const std::wstring&& strFileName)
		:IHXPluginObject(iPluginID, strPluginName, strFileName) {}

	virtual ~CHXNTFSPlugin();

public:

	/**
	 *
	 *  函数描述：外部交互接口
	 *	pParam 参数一
	 *	pReturn 参数二
	 *
	 *	返回结果
	 */
	virtual int OnCmd(UINT32 cmd, void* pLParam, void* pRParam) override;
	virtual int Initialize() override;
	virtual int Release() override;


private:
	void	Clear();


	/******************************************************** 分区信息 ********************************************************/
private:
	CHXNTFSPartitionPtr	GetPartition(const std::wstring& strFileName);
	CHXNTFSPartitionPtr	CreatePartition(const std::wstring& strFileName);
	std::wstring GetPartitionName(const std::wstring& strFileName);

private:
#ifdef _HX_USE_QT_
	QList<CHXNTFSPartitionPtr>	m_listPartition;
#else
	std::vector<CHXNTFSPartitionPtr>	m_listPartition;
#endif // _HX_USE_QT_
};





//int HX_Initialize(int, const std::wstring&, const std::wstring&, IHXPluginObjectPtr&);
