#include "stdafx.h"

//#include <QThreadPool>
#include <QFileInfo>
#include <QDir>
#include <fstream>

//#include "HXFileMonitorThread.h"
#include "HXNTFSManager.h"
#include <QMessageBox> 

const QString g_strPluginSettingXml = "HXPlugin.xml";

CHXNTFSReadFileThread::CHXNTFSReadFileThread(HANDLE hHandle, QObject *parent)
	:QThread(parent)
	, m_hRequestQueue(hHandle)
	, m_i32Flag(0)
{

}

CHXNTFSReadFileThread::~CHXNTFSReadFileThread()
{

}

void CHXNTFSReadFileThread::run()
{
	DWORD dwBytesTransfered;
	ULONG_PTR dwCompletionKey;
	OVERLAPPED* pOverlapped;
	while (GetQueuedCompletionStatus(m_hRequestQueue, &dwBytesTransfered, &dwCompletionKey, &pOverlapped, INFINITE))
	{
		if (HX_SHUTDOWN == pOverlapped) // Shut down
		{
			qInfo() << "CHXNTFSManagerThread::run HX_SHUTDOWN!";
			break;
		}
		else										// Do work
		{
			QTime time;
			time.start();
			LPHXNTFSManagerThreadDataBase p = (LPHXNTFSManagerThreadDataBase)dwCompletionKey;
			if (nullptr == p)
			{
				break;
			}
			switch (p->m_i32Flag)
			{
			//case 0:
			//	OnRead(p);
			//	break;
			case 1:
				OnPasteFile(p);
				break;
			//case 2:
			//	OnRefresh(p);
			//	break;
			default:
				break;
			}
			qInfo() << "Run Flag:" << QString::number(p->m_i32Flag) << " Time:" << time.elapsed() / 1000.0 << "s";
		}
	}
}

void CHXNTFSReadFileThread::Init(HANDLE hHandle, CHXPluginManagePtr p)
{
	Q_ASSERT(hHandle != NULL && nullptr != p);
	m_hRequestQueue = hHandle;
	m_pPluginManage = p;
}

void CHXNTFSReadFileThread::SetCancelPaste(int i32)
{
	m_i32Flag = i32;
}
//
//void CHXNTFSReadFileThread::OnRead(LPHXNTFSManagerThreadDataBase p)
//{
//	LPHXNTFSManagerThreadDataRead pRead = (LPHXNTFSManagerThreadDataRead)p;
//
//	LPHXReadDirectory dir = new CHXReadDirectory();
//	dir->m_wstrDirectory = pRead->m_strDir;
//	qInfo() << "Find Directory:" << dir->m_wstrDirectory;
//	ReadDirectory(dir);
//	emit signalReadDirectoryResult(dir);
//	delete p;
//}
//
//void CHXNTFSReadFileThread::OnRefresh(LPHXNTFSManagerThreadDataBase p)
//{
//	LPHXNTFSManagerThreadDataRead pRead = (LPHXNTFSManagerThreadDataRead)p;
//	NTFSRefresh(pRead->m_strDir);
//
//	LPHXReadDirectory dir = new CHXReadDirectory();
//	dir->m_wstrDirectory = pRead->m_strDir;
//	qInfo() << "Refresh Directory:" << dir->m_wstrDirectory;
//	ReadDirectory((void *)dir);
//	emit signalReadDirectoryResult(dir);
//	delete p;
//}

void CHXNTFSReadFileThread::OnPasteFile(LPHXNTFSManagerThreadDataBase p)
{
	LPHXNTFSManagerThreadDataPaste pRead = (LPHXNTFSManagerThreadDataPaste)p;

	for (size_t i = 0; i < pRead->m_listCopyFile.size(); i++)
	{
		if (m_i32Flag)
		{
			break;
		}
		QString strCopyFileSrc = pRead->m_listCopyFile[i];
		QFileInfo file(strCopyFileSrc);
		if (file.isDir())
		{
			qDebug() << strCopyFileSrc << u8" is Dir!";
			QString strTmp = CheckDirName(strCopyFileSrc, pRead->m_strTargetDir);
			QDir dir;
			dir.mkdir(strTmp);
			PasteDir(strCopyFileSrc, strTmp);
		}
		else
		{
			PasteFile(strCopyFileSrc, pRead->m_strTargetDir);
		}
	}
	emit signalPasteFinish();
	delete p;
}

void CHXNTFSReadFileThread::ReadDirectory(void * lparam)
{
	m_pPluginManage->OnCmd((UINT32)HXExplorerPluginCmd::HXExplorerPluginCmd_ReadDirectory, lparam, nullptr);
}
//
//void CHXNTFSReadFileThread::NTFSRefresh(const QString & path)
//{
//	CHXRefreshFileInfo file;
//	file.m_wstrDirectory = path;
//	m_pPluginManage->OnCmd((UINT32)HXExplorerPluginCmd::HXExplorerPluginCmd_Refresh, &file, nullptr);
//}

int CHXNTFSReadFileThread::ReadFile(void * param)
{
	return m_pPluginManage->OnCmd((UINT32)HXExplorerPluginCmd::HXExplorerPluginCmd_ReadFile, param, nullptr);
}

void CHXNTFSReadFileThread::PasteFile(QString & strCopyFileSrc, QString strNewDir)
{
	CHXReadFileInfo readFile;
	readFile.m_u64ReadSize = 1024 * 1024;
	readFile.m_pBuffer = new BYTE[readFile.m_u64ReadSize];
	readFile.m_u64CurPos = 0;
	readFile.m_u64RealReadSize = 0;
	readFile.m_wstrDirectory = strCopyFileSrc;

	QString strCopyFile = CheckFileName(readFile.m_wstrDirectory, strNewDir);
	std::ofstream ofstreamLog;
	ofstreamLog.open(strCopyFile.toStdWString(), std::ofstream::out | std::ofstream::binary | std::ofstream::trunc);
	if (true != ofstreamLog.is_open())
	{
		qCritical() << "Paste Open Filed Failed!";
		Q_ASSERT_X(0, "Error", "Paste Open Filed Failed!");
		return;
	}
	UINT64 u64SizeTmp = 0;
	while (true)
	{
		if (m_i32Flag)
		{
			break;
		}
		int i32Result = ReadFile((void*)&readFile);
		if (3 == i32Result)
		{
			emit signalErrorInfo(QString(u8"压缩格式暂不支持"));
			m_i32Flag = 1;
			break;
		}
		if (0 == readFile.m_u64RealReadSize)
		{
			break;
		}
		ofstreamLog.write((const char*)readFile.m_pBuffer, readFile.m_u64RealReadSize);

		readFile.m_u64CurPos += readFile.m_u64RealReadSize;
		u64SizeTmp += readFile.m_u64RealReadSize;
		emit signalPasteFileSize(strCopyFile, u64SizeTmp);
		qInfo() << "Write File :" << strCopyFile;
		qInfo() << "Write File Size::" << QString::number(u64SizeTmp, 16);
	}
	ofstreamLog.close();
	delete[]readFile.m_pBuffer;
	readFile.m_pBuffer = nullptr;
}

void CHXNTFSReadFileThread::PasteDir(QString & strCopyFileSrc, QString strNewDir)
{
	if (true == strNewDir.isEmpty())
	{
		strNewDir = CheckDirName(strCopyFileSrc, strNewDir);
		QDir dir;
		dir.mkdir(strNewDir);
	}
	LPHXReadDirectory dir = new CHXReadDirectory();
	dir->m_wstrDirectory = strCopyFileSrc;
	qInfo() << "Find Directory:" << dir->m_wstrDirectory;
	ReadDirectory((void*)dir);

	for (size_t i = 0; i < dir->m_listFileInfo.size(); i++)
	{
		QString strCopyFile = strCopyFileSrc;
		strCopyFile += "\\";
		strCopyFile += dir->m_listFileInfo[i]->m_szFileName;
		QFileInfo file(strCopyFile);
		if (file.isDir())
		{
			qDebug() << strCopyFile << u8" is Dir!";
			QString strTmp = CheckDirName(strCopyFile, strNewDir);
			QDir dir;
			dir.mkdir(strTmp);
			PasteDir(strCopyFile, strTmp);
		}
		else
		{
			PasteFile(strCopyFile, strNewDir);
		}
	}
	delete dir;
}

QString CHXNTFSReadFileThread::CheckFileName(const QString & str, QString strNewDir)
{
	QString strCopyFile;
	if (strNewDir.isEmpty())
	{
		int i32Pos = str.lastIndexOf(u8".");
		int i32Index = 0;
		while (true)
		{
			if (0 != i32Index)
			{
				strCopyFile = str.mid(0, i32Pos);
				strCopyFile += u8"_HX_";
				strCopyFile += QString::number(i32Index++);
				strCopyFile += str.mid(i32Pos);
			}
			else
			{
				strCopyFile = str;
			}

			QFileInfo file(strCopyFile);
			if (false == file.exists())
			{
				break;
			}
		}
	}
	else
	{
		int i32Pos = str.lastIndexOf(u8"\\") + 1;
		int i32PosSuffix = str.lastIndexOf(u8".");
		//源文件名
		QString strTarget = str.mid(i32Pos, i32PosSuffix - i32Pos);
		//源文件后缀
		QString strSuffix = str.mid(i32PosSuffix);

		int i32Index = 0;
		while (true)
		{
			if (0 != i32Index)
			{
				strCopyFile = strNewDir;
				strCopyFile += u8"\\";
				strCopyFile += strTarget;
				strCopyFile += u8"_HX_";
				strCopyFile += QString::number(i32Index);
				strCopyFile += strSuffix;
			}
			else
			{
				strCopyFile = strNewDir;
				strCopyFile += u8"\\";
				strCopyFile += str.mid(i32Pos);
			}
			i32Index++;
			QFileInfo file(strCopyFile);
			if (false == file.exists())
			{
				break;
			}
		}
	}
	return strCopyFile;
}

QString CHXNTFSReadFileThread::CheckDirName(const QString & str, QString strNewDir)
{
	QString strCopyFile;
	int i32Index = 0;

	if (strNewDir.isEmpty())
	{
		while (true)
		{
			strCopyFile = str;
			if (0 != i32Index)
			{
				strCopyFile += u8"_HX_";
				strCopyFile += QString::number(i32Index++);
			}

			QFileInfo file(strCopyFile);
			if (false == file.exists())
			{
				break;
			}
		}
	}
	else
	{
		int i32Pos = str.lastIndexOf(u8"\\") + 1;
		//源文件夹名
		QString strTarget = str.mid(i32Pos);

		int i32Index = 0;
		while (true)
		{
			if (0 != i32Index)
			{
				strCopyFile = strNewDir;
				strCopyFile += u8"\\";
				strCopyFile += strTarget;
				strCopyFile += u8"_HX_";
				strCopyFile += QString::number(i32Index);
			}
			else
			{
				strCopyFile = strNewDir;
				strCopyFile += u8"\\";
				strCopyFile += strTarget;

			}
			i32Index++;
			QFileInfo file(strCopyFile);
			if (false == file.exists())
			{
				break;
			}
		}
	}
	return strCopyFile;
}


CHXNTFSManager::CHXNTFSManager()
	:m_pCurrentDir(nullptr)
	, m_hReadFile(INVALID_HANDLE_VALUE)
	, m_hMonitorFile(INVALID_HANDLE_VALUE)
{

}

CHXNTFSManager::~CHXNTFSManager()
{
	m_lock.lockForWrite();
	if (nullptr != m_pCurrentDir)
	{
		m_pCurrentDir->ClearVecFileInfo();
		delete m_pCurrentDir;
	}
	m_pCurrentDir = nullptr;
	m_lock.unlock();
}

void CHXNTFSManager::Init()
{
	//m_wndProgress.setWindowModality(Qt::ApplicationModal);        //设置阻塞类型
	//m_wndProgress.setAttribute(Qt::WA_ShowModal, true);
	m_hReadFile = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	Q_ASSERT(m_hReadFile != NULL);
	m_hReadDir = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	Q_ASSERT(m_hReadDir != NULL);

	m_pPluginManage = QSharedPointer<CHXPluginManage>(new CHXPluginManage());
	if (0 != m_pPluginManage->Initialize(g_strPluginSettingXml))
	{
		Q_ASSERT(0);
		return;
	}
	m_threadReadFile.Init(m_hReadFile, m_pPluginManage);
	m_threadReadDir.Init(m_hReadDir, m_pPluginManage);
	Q_ASSERT(connect(&m_threadReadDir,	SIGNAL(signalReadDirectoryResult(void *)),		this,			SLOT(slotReadDirectoryResult(void *))));
	Q_ASSERT(connect(&m_threadReadFile,	SIGNAL(signalPasteFileSize(QString, quint64)),	&m_wndProgress, SLOT(slotWriteFileSize(QString, quint64))));
	Q_ASSERT(connect(&m_threadReadFile,	SIGNAL(signalPasteFinish()),					&m_wndProgress, SLOT(slotPasteFinish())));
	Q_ASSERT(connect(&m_wndProgress,	SIGNAL(signalCancel()),							this,			SLOT(slotCancelPaste())));
	Q_ASSERT(connect(&m_threadReadFile,	SIGNAL(signalErrorInfo(QString)),				&m_wndProgress, SLOT(slotErrorInfo(QString))));
	
	m_threadReadFile.start();
	m_threadReadDir.start();

#ifdef _HX_Monitor_
	m_hMonitorFile = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	Q_ASSERT(m_hMonitorFile != NULL);
	m_threadeMonitor.Init(m_hMonitorFile);
	Q_ASSERT(connect(this, SIGNAL(CloseMonitorDir()), &m_threadeMonitor, SLOT(Close())));
	Q_ASSERT(connect(&m_threadeMonitor, SIGNAL(fileChange()), this, SIGNAL(fileChange())));
	m_threadeMonitor.start();
#endif // _HX_Monitor_
}

void CHXNTFSManager::Release()
{
	emit CloseMonitorDir();

	if (INVALID_HANDLE_VALUE != m_hMonitorFile)
	{
		CloseHandle(m_hMonitorFile);
		m_hMonitorFile = INVALID_HANDLE_VALUE;
	}
	if (INVALID_HANDLE_VALUE != m_hReadFile)
	{
		CloseHandle(m_hReadFile);
		m_hReadFile = INVALID_HANDLE_VALUE;
	}
	if (INVALID_HANDLE_VALUE != m_hReadDir)
	{
		CloseHandle(m_hReadDir);
		m_hReadDir = INVALID_HANDLE_VALUE;
	}
}

void CHXNTFSManager::SetCurrentDirectory(const QString & path)
{
	m_strCurrentDirectory = path;
}

LPHXReadDirectory CHXNTFSManager::GetDir()
{
	m_lock.lockForRead();

	Q_ASSERT(nullptr != m_pCurrentDir);
	return m_pCurrentDir;
}

void CHXNTFSManager::FreeDir()
{
	m_lock.unlock();
}

void CHXNTFSManager::MonitorDir(QString dir)
{
	if (true == dir.isEmpty())
	{
		return;
	}
	CHXNTFSManager::SetCurrentDirectory(dir);
#ifdef _HX_Monitor_
	emit CloseMonitorDir();
	LPHXFileMonitorData p = new CHXFileMonitorData();
	p->m_strDirctory = dir;
	Q_ASSERT(TRUE == PostQueuedCompletionStatus(m_hMonitorFile, 0, (ULONG_PTR)p, NULL));
#endif // _HX_Monitor_
}

void CHXNTFSManager::slotCancelPaste()
{
	m_threadReadFile.SetCancelPaste(1);
}

void CHXNTFSManager::slotErrorInfo(QString strErrorInfo)
{
	QMessageBox::warning(nullptr, QString(u8"错误"), strErrorInfo, QMessageBox::Ok, QMessageBox::Ok);
}

void CHXNTFSManager::Paste(const QList<QString> & listFile, const QString & strTarget)
{
	if (0 == listFile.size())
	{
		return;
	}
	m_threadReadFile.SetCancelPaste(0);

	m_wndProgress.Init();
	static int i32ID = 0;
	CHXNTFSManagerThreadDataPaste * p = new CHXNTFSManagerThreadDataPaste();
	p->m_i32Flag = 1;
	p->m_i32ID = i32ID++;
	p->m_listCopyFile = listFile;
	p->m_strTargetDir = strTarget;

	Q_ASSERT(TRUE == QueueRequest((ULONG_PTR)p));

	m_wndProgress.show();
}

void CHXNTFSManager::slotReadDirectoryResult(void* pReadDirecoty)
{
	LPHXReadDirectory p = (LPHXReadDirectory)pReadDirecoty;
	if (nullptr != pReadDirecoty && m_strCurrentDirectory == p->m_wstrDirectory)
	{
		m_lock.lockForWrite();
		if (nullptr != m_pCurrentDir)
		{
			m_pCurrentDir->ClearVecFileInfo();
			delete m_pCurrentDir;
		}
		m_pCurrentDir = p;
		m_lock.unlock();
		emit Refresh(pReadDirecoty);
	}
}

BOOL CHXNTFSManager::QueueRequest(ULONG_PTR p)
{
	if (!PostQueuedCompletionStatus(m_hReadFile, 0, (ULONG_PTR)p, NULL))
		return FALSE;
	return TRUE;
}

void CHXNTFSManager::NTFSRefresh(const QString & path)
{
	LPHXNTFSManagerThreadDataRead p = new CHXNTFSManagerThreadDataRead();
	p->m_i32Flag = 2;
	p->m_strDir = path;

	Q_ASSERT(TRUE == PostQueuedCompletionStatus(m_hReadDir, 0, (ULONG_PTR)p, NULL));
}

void CHXNTFSManager::ReadDirectory(const QString & path)
{
	CHXNTFSManagerThreadDataRead* p = new CHXNTFSManagerThreadDataRead();
	p->m_i32Flag = 0;
	p->m_strDir = path;

	Q_ASSERT(TRUE == PostQueuedCompletionStatus(m_hReadDir, 0, (ULONG_PTR)p, NULL));
}

CHXNTFSReadDirecotryThread::CHXNTFSReadDirecotryThread(HANDLE hHandle /*= INVALID_HANDLE_VALUE*/, QObject* parent /*= nullptr*/)
	: m_hRequestQueue(hHandle)
{

}

CHXNTFSReadDirecotryThread::~CHXNTFSReadDirecotryThread()
{
	if (INVALID_HANDLE_VALUE != m_hRequestQueue)
	{
		CloseHandle(m_hRequestQueue);
		m_hRequestQueue = INVALID_HANDLE_VALUE;
	}
}

void CHXNTFSReadDirecotryThread::run()
{
	DWORD dwBytesTransfered;
	ULONG_PTR dwCompletionKey;
	OVERLAPPED* pOverlapped;
	while (GetQueuedCompletionStatus(m_hRequestQueue, &dwBytesTransfered, &dwCompletionKey, &pOverlapped, INFINITE))
	{
		if (HX_SHUTDOWN == pOverlapped) // Shut down
		{
			qInfo() << "CHXNTFSReadDirecotryThread::run HX_SHUTDOWN!";
			break;
		}
		else										// Do work
		{
			QTime time;
			time.start();
			LPHXNTFSManagerThreadDataBase p = (LPHXNTFSManagerThreadDataBase)dwCompletionKey;
			if (nullptr == p)
			{
				break;
			}
			switch (p->m_i32Flag)
			{
			case 0:
				OnRead(p);
				break;
			//case 1:
			//	OnPasteFile(p);
			//	break;
			case 2:
				OnRefresh(p);
				break;
			default:
				break;
			}
			qInfo() << "Run Flag:" << QString::number(p->m_i32Flag) << " Time:" << time.elapsed() / 1000.0 << "s";
		}
	}
}

void CHXNTFSReadDirecotryThread::Init(HANDLE hHandle, CHXPluginManagePtr p)
{
	Q_ASSERT(hHandle != NULL && nullptr != p);
	m_hRequestQueue = hHandle;
	m_pPluginManage = p;
}

void CHXNTFSReadDirecotryThread::OnRefresh(LPHXNTFSManagerThreadDataBase p)
{
	LPHXNTFSManagerThreadDataRead pRead = (LPHXNTFSManagerThreadDataRead)p;
	NTFSRefresh(pRead->m_strDir);

	LPHXReadDirectory dir = new CHXReadDirectory();
	dir->m_wstrDirectory = pRead->m_strDir;
	qInfo() << "Refresh Directory:" << dir->m_wstrDirectory;
	ReadDirectory((void*)dir);
	emit signalReadDirectoryResult(dir);
	delete p;
}

void CHXNTFSReadDirecotryThread::OnRead(LPHXNTFSManagerThreadDataBase p)
{
	LPHXNTFSManagerThreadDataRead pRead = (LPHXNTFSManagerThreadDataRead)p;

	LPHXReadDirectory dir = new CHXReadDirectory();
	dir->m_wstrDirectory = pRead->m_strDir;
	qInfo() << "Find Directory:" << dir->m_wstrDirectory;
	ReadDirectory(dir);
	emit signalReadDirectoryResult(dir);
	delete p;
}


void CHXNTFSReadDirecotryThread::NTFSRefresh(const QString& path)
{
	CHXRefreshFileInfo file;
	file.m_wstrDirectory = path;
	m_pPluginManage->OnCmd((UINT32)HXExplorerPluginCmd::HXExplorerPluginCmd_Refresh, &file, nullptr);
}

void CHXNTFSReadDirecotryThread::ReadDirectory(void* lparam)
{
	m_pPluginManage->OnCmd((UINT32)HXExplorerPluginCmd::HXExplorerPluginCmd_ReadDirectory, lparam, nullptr);
}