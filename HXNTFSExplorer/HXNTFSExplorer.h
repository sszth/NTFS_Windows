#pragma once
/*******************************************************************************************************************************************
*	2021��5��11��14:15:11	connectʹ�����ͱ�����ע��  ���δע��ֻ��ʹ��Qt::DirectConnection��ʶ ��Ϊ�ڲ���һ������ת��
*	2021��5��11��16:21:18	֮ǰʹ��QTablewidgetȫ����ʾ10w�����ݣ�Ȼ���л�20������Ŀ¼����ռ�õĻ��沢δ��ԭ���鿴������Ҫ����ʹ��QVector�洢��QVectorֻҪ��resize()ռ�õ��ڴ治���ͷţ����ʹ��QTableView��дmodel���ڲ������ͷš�
*	2021��5��17��13:59:55	��״�ļ���ˢ�����⡣������������ɺ�̨ÿ��һ��ʱ��Ͷ��һ�����ݣ�
*							ͬʱ��¼��ǰλ�ã�ˢ�º���û����ݻ��ء�
*	
*	
*	
*	
*	
*	
*	
*	
*	
*	
*	
*	
*	
*	
*	
*	
*
*	
*	
*	
*	
*	
*	
*	
*	
*	
*	
*	
*	
*	
*	
*	
*
*	
*	
*	
*	
*
*	
*	
*	
*	
*
*
*
*
*
*	
*******************************************************************************************************************************************/

#include <QtWidgets/QMainWindow>
#include "ui_HXNTFSExplorer.h"

#include "HXNTFSManager.h"

class HXNTFSExplorer : public QMainWindow
{
	Q_OBJECT

public:
	HXNTFSExplorer(QWidget *parent = Q_NULLPTR);
	~HXNTFSExplorer();

protected:
	void closeEvent(QCloseEvent *event);
private:
	//	��ʼ��
	void	Init();
	void	InitTreeWidget();
	void	InitListWidget();

	int		GetShowRow();

	QString FileTimeToString(UINT64 u64FileTime);
	QString SizeToString(UINT64 u64FileTime, int i32SizeNumber);
	QString SizeToString(UINT64 u64FileTime);
private slots:
	//void NTFSRefresh(const QString & path);
	void Refresh(bool checked);
	void TreeWidgetItemRefresh(QTreeWidgetItem * item);
	void Paste(bool checked);





	void OpenDirectory(bool checked);
	void Copy(bool checked);
	void Exit(bool checked);

	void TreeWidgetItemDoubleClicked(QTreeWidgetItem *item, int column);
	QString GetTreeWidgetItemPath(QTreeWidgetItem *item);
	void slotListWidgetShowFile(void* pReadDirecoty);
	void slotTableWidgetRefresh(VecFileInfo& listFileInfo);
	void slotTreeWidgetRefresh(VecFileInfo& listFileInfo);
	void RemoveItem(QTreeWidgetItem * item);
	void ShowListWidgetMenuSlot(QPoint pos);

	//void ClearVecFileInfo(VecFileInfo & listFileInfo);

	void TreeWidgetInsertItem(LPHXFileInfo pInfo);

	void DirectoryChanged(const QString &path);



	void slotsFileChange();

	void on_pushButton_Home_pressed();
	void on_pushButton_Previous_pressed();
	void on_pushButton_Next_pressed();
	void on_pushButton_Last_pressed();
	void on_lineEdit_Page_returnPressed();

private:
	Ui::HXNTFSExplorerClass ui;

	QMenu *					m_pMenu;

	QTreeWidgetItem	*		m_treeItemCurrent;

	//	��ʱδ�ò���
	QString					m_strCurDirectory;

	QList<QString>			m_listCopyFile; 
	CHXNTFSManager			m_Manager;
	int						m_i32CurPosion;
};
