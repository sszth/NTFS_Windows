/*******************************************************************************************************************************************
*	2021��4��23��14:02:43	ʹ��$MFT��DataRun��ȡͬһ�ļ�����30w���ļ���֮ǰֻ�ܻ�ȡ5w���
*	2021��4��23��15:09:55	�������Խ����Ի�ȡ����һ�ѣ��������Ե�һ���б�����20H���ô���
*	2021��4��23��15:35:13	һ���ļ���¼������ֻ��1������10H��20H,��������80H��A0H�����ж��
*	2021��4��23��16:27:07	��ȡ�ļ���Ϣ��1.��ȡ��90H�µ��ļ���Ϣ��2.��ȡ��80H�ڲ��ļ���Ϣ
*	2021��4��25��09:38:47	���ܡ�ϡ�����Ϊ��פ.�Ȳ�֧�ּ��ܡ�ϡ�衢ѹ�������������ʱ��
*	2021��4��25��10:19:10	$MFT��CHXFileRecordHeader::m_i16Flag 0����ɾ����1�����ļ���2����Ŀ¼
*							$Bitmap�б�����̿��ô���Ϣ
*							��ͨ�ļ���¼��0xB0����ֻ��0x90�����йأ�����λͼ����������Ϣ
*	2021��4��26��09:12:42	�ļ�Ŀ¼���������
*	2021��4��27��09:28:48	0x30H���Կ����ж��								����Ӳ�������,Ŀǰ30����ֻ��ȡ���һ���Ĵ���ʱ�䡢���������������ơ���С����80���ԣ��ѽ��
*	2021��4��27��12:46:02	�ļ���С �Ի�ȡ����һ��80����Ϊ׼��ʱδ���Ƕ������
*	2021��4��27��13:40:24	Ŀ¼��0x90û��0xB0ʱ��������������Ϊnull���	�ѽ��
*	2021��4��27��14:21:37	ʹ��windows�����ߴ���VHD����L"G:\\$RECYCLE.BIN\\S-1-5-21-3682035386-2047908854-1240857724-1001\\"����B0���Գ��Ȳ���ȷ���
*	2021��4��28��15:38:05	��Ŀ¼��Ϣ�д洢Ŀ¼����λ��, 
*	2021��4��30��16:48:35	�����ÿ��mft��������������10w��0.5s  30w1.3s�����������Ϊ500G�������MFT���60G �Ի���Ҫ��̫�� ʹ�û�����Ż�����
*	2021��5��6��10:02:56	���ܻ�����һ����С��MFT��С���
*	2021��5��6��10:03:32	��ȡ$MFTʱ��Ҫ�Ȼ�ȡ80 �ٻ�ȡ20����
*	2021��5��6��11:11:31	�������ļ���Ҫע�⻺������С�Լ��ͷ�buffer
*	2021��5��8��14:55:06	����п���ʱ��ʵ��USNʵ���ļ����
*
*	2021��5��8��09:30:28	ʹ��$MFT��DataRun��ȡͬһ�ļ�����30w���ļ���MFT��֮ǰֻ�ܻ�ȡ5w���
*							һ���ļ���¼������ֻ��1������10H��20H,��������80H��A0H�����ж��
*							��ȡ�ļ���Ϣ��1.��ȡ��90H�µ��ļ���Ϣ��2.��ȡ��80H�ڲ��ļ���Ϣ
*							$MFT��CHXFileRecordHeader::m_i16Flag 0����ɾ����1�����ļ���2����Ŀ¼
*							0x30H���Կ����ж��
*							�ļ���С �Ի�ȡ����һ��80����Ϊ׼����ʱδ���Ƕ������
*							MFT��С����BPB��������С��������������С���Ϲ̶�ͷ
*							�����ȡ������Ϣ��1��2���ֽڴ��󣬿��ܸ�������δ��ȷ����
*							����Ҳ�����Ӧ�ļ����ļ���¼��������û����$MFT�Ҷ�Ӧ�ļ���¼
*							���д���ļ����һЩ0D ������δ�Զ�����д�ļ�
*							���д���ļ����һЩ00000  д��ʱδ��ȡʵ�ʴ�С
*							����ʵ���ļ���أ�ʹ��QFileSystemWatcher���м�ػ�����2�����⣺
*							1.�ļ�����д�룬�����г���Ϊ0;
*							2.�ļ��ڻ��棬��δд�롣�������޶�Ӧ��Ϣ
*							����п���ʱ��ʵ��USNʵ���ļ����
*
*							QFileSystemWatcher��windows��ʵ�ʵ���qfilesystemwatcher_win.cpp���м�����ʹ��WinAPI FindNextChangeNotification��
*							���仯ʱ����������ļ���Ϣ�б���ʵ�ʴ洢�Աȣ��ж��ļ��������ļ�������ļ��ƶ��������仯���ļ�ͨ���źŷ������.
*							FindNextChangeNotification�޷���ReadDirectoryChangesW��ȡ������һ���ļ���Ҳ���ܻ�ȡ��������ϸ�ڡ�
*							���紴���ļ��տ�ʼ���ݻ��ڻ�����FindNextChangeNotification������֪ͨ��ReadDirectoryChangesW���Կ����ڻ���д�����ʱ֪ͨ��
*
*							windows�ļ�ϵͳ�ڿ���ʱ�����ʱ��ˢ�����ڿ������ļ���С��
*							Ŀǰ�²����ͨ�����¼��ַ�ʽʵ��
*							1.ͨ��USN��
*							2.ÿ��һ��ʱ����80����,�����ڲ���������
*	2021��5��17��09:37:01	Ŀǰ���������  �Ƿ�����Ŀ¼������Ч����ʱ����
*	2021��5��17��09:37:24	20���Էֳ�פ�ͷǳ�פ �ǳ�פ����data run
*							
*							
*							

*	�ڴ�й©���	
*******************************************************************************************************************************************/
#pragma once
#include <Windows.h>
#include <unordered_map>
#include <qhash.h>
#include <set>

#include "../HXCommon/HXExplorerCmd.h"
#include "../HXCommon/IHXPluginObject.h"
#include "../HXCommon/HXFileSystemCmd.h"


#include "hxntfsplugin_global.h"

#include "../HXCommon/IHXPluginObject.h"
#include "../HXCommon/HXExplorerCmd.h"
#include "HXNTFSPartition.h"
#include "HXNTFSType.h"



//	������ÿ��Disk�ֿ�������������Disk����
class CHXNTFSPlugin : public IHXPluginObject
{
public:

	explicit CHXNTFSPlugin(const int& iPluginID,
		const QString& strPluginName,
		const QString& strFileName)
		:IHXPluginObject(iPluginID, strPluginName, strFileName) {}
	explicit CHXNTFSPlugin(const int&& iPluginID,
		const QString&& strPluginName,
		const QString&& strFileName)
		:IHXPluginObject(iPluginID, strPluginName, strFileName) {}

	virtual ~CHXNTFSPlugin();

public:

	/**
	 *
	 *  �����������ⲿ�����ӿ�
	 *	pParam ����һ
	 *	pReturn ������
	 *
	 *	���ؽ��
	 */
	virtual int OnCmd(UINT32 cmd, void* pLParam, void* pRParam) override;
	virtual int Initialize() override;
	virtual int Release() override;


private:	   	 	   
	void	Clear();


	/******************************************************** ������Ϣ ********************************************************/
private:
	CHXNTFSPartitionPtr	GetPartition(const QString & strFileName);
	CHXNTFSPartitionPtr	CreatePartition(const QString & strFileName);
	QString GetPartitionName(const QString & strFileName);

private:
	QList<CHXNTFSPartitionPtr>	m_listPartition;
};





extern "C"
HXNTFSPLUGIN_EXPORT
int HX_Initialize(int, const QString&, const QString&, IHXPluginObjectPtr&);
