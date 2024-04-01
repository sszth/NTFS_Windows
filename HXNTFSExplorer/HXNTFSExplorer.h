#pragma once
/*******************************************************************************************************************************************
*	2021年5月11日14:15:11	connect使用类型必须是注册  如果未注册只有使用Qt::DirectConnection标识 因为内部有一道类型转换
*	2021年5月11日16:21:18	之前使用QTablewidget全部显示10w条数据，然后切换20条数据目录。其占用的缓存并未复原，查看代码主要数据使用QVector存储，QVector只要不resize()占用的内存不会释放，如果使用QTableView自写model在内部进行释放。
*	2021年5月17日13:59:55	树状文件夹刷新问题。如果数量过大，由后台每隔一定时间投递一次数据，
*							同时记录当前位置，刷新后把用户数据还回。
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
	//	初始化
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

	//	暂时未用参数
	QString					m_strCurDirectory;

	QList<QString>			m_listCopyFile; 
	CHXNTFSManager			m_Manager;
	int						m_i32CurPosion;
};
