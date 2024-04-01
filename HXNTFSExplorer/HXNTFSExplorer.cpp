#include "stdafx.h"
#include <fstream>
#include <QListWidget> 
#include <QDir>
#include <QFileSystemWatcher>
#include <QTime>

#include "../HXCommon/HXExplorerCmd.h"

#include "HXNTFSExplorer.h"


const QString g_strPluginSettingXml = "HXPlugin.xml";


/**************************************************************************************************************************************
*	2021年4月26日13:25:43	查询文件夹与拷贝复制函数需在子线程中实现，子线程完成后向界面投递消息，界面判断消息是否此时处理，如果不是
*
*
*
*
*
*
*
/**************************************************************************************************************************************/

HXNTFSExplorer::HXNTFSExplorer(QWidget *parent)
	: QMainWindow(parent)
	, m_i32CurPosion(0)
{
	ui.setupUi(this);

	Init();
}



void HXNTFSExplorer::OpenDirectory(bool checked)
{
	//QFileDialog fileDialog(this);
	//fileDialog.exec();
	//m_pPluginManage->OnCmd(HX_FILEPLUGIN_READCATALOG, );
}

HXNTFSExplorer::~HXNTFSExplorer()
{
}

void HXNTFSExplorer::closeEvent(QCloseEvent * event)
{
	m_Manager.Release();
}

void HXNTFSExplorer::Init()
{
	ui.m_actionOpen_O->setEnabled(false);
	//connect(ui.m_actionOpen_O, &QAction::triggered, this, &HXNTFSExplorer::OpenDirectory);
	connect(ui.m_actionCopy_C,	&QAction::triggered, this, &HXNTFSExplorer::Copy);
	connect(ui.m_actionPaste_P, &QAction::triggered, this, &HXNTFSExplorer::Paste);
	connect(ui.m_actionRefresh, &QAction::triggered, this, &HXNTFSExplorer::Refresh);
	connect(ui.m_actionExit,	&QAction::triggered, this, &HXNTFSExplorer::Exit);

	ui.m_actionCopy_C->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_C));
	ui.m_actionPaste_P->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_P));
	ui.m_actionExit->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_E));
	m_pMenu = new QMenu();
	m_pMenu->addAction(ui.m_actionCopy_C);
	m_pMenu->addAction(ui.m_actionPaste_P);
	m_pMenu->addAction(ui.m_actionRefresh);
	m_treeItemCurrent = nullptr;
	InitListWidget();
	InitTreeWidget();

	m_Manager.Init();

	Q_ASSERT(connect(&m_Manager, SIGNAL(fileChange()), this, SLOT(slotsFileChange())));
	Q_ASSERT(connect(&m_Manager, SIGNAL(Refresh(void*)), this, SLOT(slotListWidgetShowFile(void*))));
	ui.lineEdit_Page->setValidator(new QRegExpValidator(QRegExp("[0-9]+$")));
}

void HXNTFSExplorer::InitListWidget()
{
	ui.m_tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui.m_tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	ui.m_tableWidget->horizontalHeader()->setStretchLastSection(true); //就是这个地方
	//ui.m_tableWidget->horizontalHeader()->set(QHeaderView::Stretch);
	ui.m_tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows); //设置选择行为，以行为单位
	ui.m_tableWidget->setColumnCount(4);
	//ui.m_tableWidget->setRowCount(100);
	ui.m_tableWidget->setHorizontalHeaderLabels(QStringList() << u8"文件名称" << u8"修改日期" << u8"创建日期" << u8"大小");
	ui.m_tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	connect(ui.m_tableWidget, &QListWidget::customContextMenuRequested, this, &HXNTFSExplorer::ShowListWidgetMenuSlot);
}

int HXNTFSExplorer::GetShowRow()
{
	//计算当前页面可以展示的条数
	int rowHeight = ui.m_tableWidget->rowHeight(0);
	// int rowHeight = ui.m_tableWidget->horizontalHeader()->height(); 小了5
	if (rowHeight == 0)
	{
		rowHeight = 30;
	}

	//获取tableWidget的显示高度
	int tableViewHeight = ui.m_tableWidget->height();

	//计算一页可以显示的数据条数
	int pagestep = tableViewHeight / rowHeight - 1;//1是减去表头的高度
	if (pagestep < 0)
	{
		Q_ASSERT(0);
		return 0;
	}
	return pagestep;
}

QString HXNTFSExplorer::FileTimeToString(UINT64 u64FileTime)
{
	SYSTEMTIME sysTimeUSA, sysTime;
	FileTimeToSystemTime((FILETIME*)&(u64FileTime), &sysTimeUSA);
	SystemTimeToTzSpecificLocalTime(NULL, &sysTimeUSA, &sysTime);

	QString wstr;
	wstr += QString::number(sysTime.wYear);
	wstr += u8"-";
	wstr += QString::number(sysTime.wMonth);
	wstr += u8"-";
	wstr += QString::number(sysTime.wDay);
	wstr += u8" ";
	wstr += QString::number(sysTime.wHour);
	wstr += u8":";
	wstr += QString::number(sysTime.wMinute);
	wstr += u8":";
	wstr += QString::number(sysTime.wSecond);

	return wstr;
}

QString HXNTFSExplorer::SizeToString(UINT64 u64FileTime, int i32SizeNumber)
{
	QString str;
	UINT64 u64High = u64FileTime / 1024;
	UINT64 u64Low = u64FileTime % 1024;
	str += QString::number(u64Low);
	switch (i32SizeNumber)
	{
	case 0:
		str += u8"Byte";
		break;
	case 1:
		str += u8"KB ";
		break;
	case 2:
		str += u8"MB ";
		break;
	case 3:
		str += u8"G ";
		break;
	case 4:
		str += u8"T ";
		break;
	default:
		break;
	}
	if (u64High > 0)
	{
		str = SizeToString(u64High, ++i32SizeNumber) + str;
	}
	return str;
}

QString HXNTFSExplorer::SizeToString(UINT64 u64FileTime)
{
	QString str;
	str = SizeToString(u64FileTime, 0);
	if (0 != u64FileTime)
	{
		str += u8"(";
		str += QString::number(u64FileTime);;
		str += u8" Byte)";
	}
	return str;
}

void HXNTFSExplorer::InitTreeWidget()
{
	ui.m_treeWidget->clear();
	m_treeItemCurrent = nullptr;

	ui.m_treeWidget->setHeaderLabels(QStringList() << u8"盘符");

	QList<QTreeWidgetItem *> items;

	QFileInfoList diskList = QDir::drives();
	Q_FOREACH(const QFileInfo & info, diskList)
	{
		QTreeWidgetItem* pItem = new QTreeWidgetItem();
		QIcon icon;
		icon.addPixmap(QPixmap(u8":/HXNTFSExplorer/Resources/文件夹.png"));
		pItem->setIcon(0, icon);
		QString str = info.canonicalFilePath();
		auto str1 = info.absoluteFilePath();
		auto str2 = info.fileName();
		int i32Pos = str.indexOf('/');
		str = str.mid(0, i32Pos);
		pItem->setText(0, str);
		items.push_back(pItem);
	}
	ui.m_treeWidget->addTopLevelItems(items);

	connect(ui.m_treeWidget, &QTreeWidget::itemClicked, this, &HXNTFSExplorer::TreeWidgetItemDoubleClicked);
}

void HXNTFSExplorer::Copy(bool checked)
{
	if (nullptr == m_treeItemCurrent)
	{
		return;
	}
	QList<QTableWidgetItem*> listItem = ui.m_tableWidget->selectedItems();

	if (0 == listItem.size())
	{
		return;	
	}
	m_listCopyFile.clear();
	std::for_each(listItem.begin(), listItem.end(), [this](QTableWidgetItem* item)
	{
		if (0 != item->column())
		{
			return;
		}
		QString str = this->GetTreeWidgetItemPath(this->m_treeItemCurrent);
		str += u8"\\";
		str += item->text();
		this->m_listCopyFile.append(str);
	}
	);	
}

void HXNTFSExplorer::Paste(bool checked)
{
	if (nullptr == m_treeItemCurrent)
	{
		return;
	}
	QString strTarget = GetTreeWidgetItemPath(m_treeItemCurrent);
	m_Manager.Paste(m_listCopyFile, strTarget);
}

void HXNTFSExplorer::Refresh(bool checked)
{
	if (nullptr == m_treeItemCurrent)
	{
		return;
	}
	QString str = this->GetTreeWidgetItemPath(this->m_treeItemCurrent);
	m_Manager.NTFSRefresh(str);
	TreeWidgetItemRefresh(m_treeItemCurrent);
}

void HXNTFSExplorer::Exit(bool checked)
{
	close();
}

void HXNTFSExplorer::TreeWidgetItemDoubleClicked(QTreeWidgetItem * item, int column)
{
	if (nullptr == item)
	{
		return;
	}
	//QGuiApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	m_treeItemCurrent = item;
	m_i32CurPosion = 0;
	QString str = GetTreeWidgetItemPath(m_treeItemCurrent);
	m_Manager.MonitorDir(str);

	TreeWidgetItemRefresh(m_treeItemCurrent);
	//QGuiApplication::restoreOverrideCursor();
}


void HXNTFSExplorer::TreeWidgetItemRefresh(QTreeWidgetItem * item)
{
	if (nullptr == item)
	{
		return;
	}
	QTime time;
	time.start();
	m_Manager.ReadDirectory(GetTreeWidgetItemPath(item));

	qInfo() << "TreeWidgetItemRefresh:" << time.elapsed() / 1000.0 << "s";
}

QString HXNTFSExplorer::GetTreeWidgetItemPath(QTreeWidgetItem * item)
{
	if (nullptr == item)
	{
		return QString();
	}
	QTreeWidgetItem * pParentItem = item->parent();
	QString strCurDirectory = item->text(0);
	while (pParentItem)
	{
		strCurDirectory = pParentItem->text(0) +u8"\\" + strCurDirectory;
		pParentItem = pParentItem->parent();
	}
	return strCurDirectory;
}

//static QIntValidator hxhx(0, 30);
//	每次调用前清空table
void HXNTFSExplorer::slotListWidgetShowFile(void* pReadDirecoty)
{
	QGuiApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	QTime time;
	time.start();

	CHXReadDirectory * p = m_Manager.GetDir();
	Q_ASSERT(nullptr != p);
	VecFileInfo&listFileInfo = p->m_listFileInfo;
	int i32ListSize = listFileInfo.size();
	qInfo() << "Directory File Number:" << QString::number(i32ListSize);
	int i32MaxPage = ceil(1.0 *i32ListSize / GetShowRow());

	QString str;
	str = "/";
	str += QString::number(i32MaxPage);
	str += u8"页";
	ui.label->setText(str);
	auto pValidator = ui.lineEdit_Page->validator();
	if (nullptr != pValidator)
	{
		delete pValidator;
	}
	ui.lineEdit_Page->setValidator(new QIntValidator(0, i32MaxPage, this));

	slotTableWidgetRefresh(listFileInfo);
	slotTreeWidgetRefresh(listFileInfo);

	m_Manager.FreeDir();
	//delete p;
	qInfo() << "ListWidgetShowFile:" << time.elapsed() / 1000.0 << "s";

	QGuiApplication::restoreOverrideCursor();
}

void HXNTFSExplorer::slotTableWidgetRefresh(VecFileInfo& listFileInfo)
{
	QTime s;
	s.start();
	this->ui.m_tableWidget->setRowCount(0);
	int i32 = GetShowRow();
	QString strPage = QString::number(m_i32CurPosion / i32 + 1) ;
	ui.lineEdit_Page->setText(strPage);

	for (int i32Index = 0; i32Index < i32; i32Index++)
	{
		int i32Column = 0;
		int i32Row = m_i32CurPosion + i32Index;
		if (i32Row >= listFileInfo.size())
		{
			break;
		}
		LPHXFileInfo pInfo = listFileInfo[i32Row];

		this->ui.m_tableWidget->insertRow(this->ui.m_tableWidget->rowCount());
		this->ui.m_tableWidget->setItem(i32Index, i32Column++, new QTableWidgetItem(pInfo->m_szFileName));
		this->ui.m_tableWidget->setItem(i32Index, i32Column++, new QTableWidgetItem(FileTimeToString(pInfo->m_u64CreateTime)));
		this->ui.m_tableWidget->setItem(i32Index, i32Column++, new QTableWidgetItem(FileTimeToString(pInfo->m_u64ModifyTime)));
		this->ui.m_tableWidget->setItem(i32Index, i32Column++, new QTableWidgetItem(SizeToString(pInfo->m_u64FileRealSize)));

#ifdef _HX_ALL_LOG_
		qInfo() << "TableWidget Add:" << pInfo->m_szFileName
			<< " CreateTime:" << FileTimeToString(pInfo->m_u64CreateTime)
			<< " ModifyTime:" << FileTimeToString(pInfo->m_u64ModifyTime)
			<< " Size:" << SizeToString(pInfo->m_u64FileRealSize);
#endif // _HX_ALL_LOG_
	}
	qInfo() << "HXNTFSExplorer::slotTableWidgetRefresh:" << s.elapsed() / 1000.0 << "s";
}

void HXNTFSExplorer::slotTreeWidgetRefresh(VecFileInfo& listFileInfo)
{
	QTime s;
	s.start();
	VecFileInfo listDirInfo;
	for (int i32Index = 0; i32Index < listFileInfo.length(); i32Index++)
	{
		int i32Column = 0;
		LPHXFileInfo pInfo = listFileInfo[i32Index];
#ifdef _HX_ALL_LOG_
		qInfo() << "All Add:" << pInfo->m_szFileName
			<< " CreateTime:" << FileTimeToString(pInfo->m_u64CreateTime)
			<< " ModifyTime:" << FileTimeToString(pInfo->m_u64ModifyTime)
			<< " Size:" << SizeToString(pInfo->m_u64FileRealSize);
#endif // _HX_ALL_LOG_

		if (pInfo->m_i32FileAttributes & HX_DIRECTORYFLAG)
		{
			listDirInfo.push_back(pInfo);
		}
	}
	qInfo() << "Dir Size Number:" << QString::number(listDirInfo.size());

	//	删除子节点
	int count = m_treeItemCurrent->childCount();
	for (int i = 0; i < count; i++)
	{
		QTreeWidgetItem *childItem = m_treeItemCurrent->child(0);
		RemoveItem(childItem);
	}

	int i32DirIndex = 0;
	for (size_t i = 0; i < listDirInfo.size(); i++)
	{
		LPHXFileInfo pInfo = listDirInfo[i];
		TreeWidgetInsertItem(pInfo);
	}
	m_treeItemCurrent->setExpanded(true);
	qInfo() << "HXNTFSExplorer::slotTreeWidgetRefresh:" << s.elapsed() / 1000.0 << "s";
}
//递归删除节点
void HXNTFSExplorer::RemoveItem(QTreeWidgetItem *item)
{
	int count = item->childCount();
	if (count == 0)//没有子节点，直接删除
	{
		delete item;
		return;
	}

	for (int i = 0; i < count; i++)
	{
		QTreeWidgetItem *childItem = item->child(0);//删除子节点
		RemoveItem(childItem);
	}
	delete item;//最后将自己删除
}

void HXNTFSExplorer::ShowListWidgetMenuSlot(QPoint pos)
{
	m_pMenu->exec(QCursor::pos());
}

//void HXNTFSExplorer::ClearVecFileInfo(VecFileInfo & listFileInfo)
//{
//	std::for_each(listFileInfo.cbegin(), listFileInfo.cend(), [](LPHXFileInfo pFileInfo) {
//		delete pFileInfo; pFileInfo = nullptr;
//	});
//
//	listFileInfo.clear();
//}

void HXNTFSExplorer::TreeWidgetInsertItem(LPHXFileInfo pInfo)
{
	QTreeWidgetItem * pItemTmp = new QTreeWidgetItem(QStringList() << pInfo->m_szFileName);
	pItemTmp->setData(0, Qt::UserRole, QVariant(pInfo->m_u64SerialNumber));
	QIcon icon;
	icon.addPixmap(QPixmap(u8":/HXNTFSExplorer/Resources/文件夹.png"));
	pItemTmp->setIcon(0, icon);
	// 虚构子文件
	if (1 == pInfo->m_U32SubFile)
	{
		QTreeWidgetItem* pItemTmpChild = new QTreeWidgetItem();
		pItemTmp->addChild(pItemTmpChild);
	}
	m_treeItemCurrent->addChild(pItemTmp);
#ifdef _HX_ALL_LOG_
	qInfo() << "Tree Add:" << pInfo->m_szFileName;
#endif // _HX_ALL_LOG_
}

void HXNTFSExplorer::DirectoryChanged(const QString & path)
{
	if (nullptr == m_treeItemCurrent)
	{
		return;
	}
	//NTFSRefresh(path);
	TreeWidgetItemRefresh(m_treeItemCurrent);
}

//void HXNTFSExplorer::NTFSRefresh(const QString & path)
//{
//	m_Manager.NTFSRefresh(path);
//}

void HXNTFSExplorer::slotsFileChange()
{
	if (nullptr == m_treeItemCurrent)
	{
		return;
	}
	TreeWidgetItemRefresh(m_treeItemCurrent);
}

void HXNTFSExplorer::on_pushButton_Home_pressed()
{
	if (nullptr == m_treeItemCurrent)
	{
		return;
	}
	CHXReadDirectory * p = m_Manager.GetDir();
	Q_ASSERT(nullptr != p);
	VecFileInfo&listFileInfo = p->m_listFileInfo;
	m_i32CurPosion = 0;
	slotTableWidgetRefresh(listFileInfo);
	m_Manager.FreeDir();
}

void HXNTFSExplorer::on_pushButton_Previous_pressed()
{
	if (nullptr == m_treeItemCurrent)
	{
		return;
	}
	CHXReadDirectory * p = m_Manager.GetDir();
	VecFileInfo&listFileInfo = p->m_listFileInfo;
	int i32 = m_i32CurPosion - GetShowRow();
	if (i32 >= 0)
	{
		m_i32CurPosion = i32;
		slotTableWidgetRefresh(listFileInfo);
	}	 
	m_Manager.FreeDir();
}

void HXNTFSExplorer::on_pushButton_Next_pressed()
{
	if (nullptr == m_treeItemCurrent)
	{
		return;
	}
	CHXReadDirectory * p = m_Manager.GetDir();

	VecFileInfo&listFileInfo = p->m_listFileInfo;
	int i32 = m_i32CurPosion + GetShowRow();
	if (i32 < listFileInfo.size())
	{
		m_i32CurPosion = i32;
		slotTableWidgetRefresh(listFileInfo);
	}
	m_Manager.FreeDir();
}

void HXNTFSExplorer::on_pushButton_Last_pressed()
{
	if (nullptr == m_treeItemCurrent)
	{
		return;
	}
	CHXReadDirectory * p = m_Manager.GetDir();

	VecFileInfo&listFileInfo = p->m_listFileInfo;
	int i32ShowRow = GetShowRow();
	int i32Size = listFileInfo.size();
	if (0 == i32Size % i32ShowRow)
	{
		m_i32CurPosion = i32Size - i32ShowRow;
	}
	else
	{
		m_i32CurPosion = ((int)i32Size / i32ShowRow) * i32ShowRow;
	}
	slotTableWidgetRefresh(listFileInfo);
	m_Manager.FreeDir();
}

void HXNTFSExplorer::on_lineEdit_Page_returnPressed()
{
	if (nullptr == m_treeItemCurrent)
	{
		return;
	}
	CHXReadDirectory * p = m_Manager.GetDir();

	VecFileInfo&listFileInfo = p->m_listFileInfo;
	int i32PageSize = GetShowRow();
	int i32 = ui.lineEdit_Page->text().toInt() - 1;

	m_i32CurPosion = i32PageSize * i32;
	Q_ASSERT(m_i32CurPosion > 0 && m_i32CurPosion < listFileInfo.size());

	slotTableWidgetRefresh(listFileInfo);
	m_Manager.FreeDir();
}
