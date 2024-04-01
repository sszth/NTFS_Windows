#pragma once
#include <QObject>
#include <QThread>
#include <QReadWriteLock> 

#include "HXNTFSProgressWnd.h"
#include "HXExplorerPluginManage.h"

#ifdef _HX_Monitor_
#include "HXFileMonitorThread.h"
#endif // _HX_Monitor_

typedef struct CHXNTFSManagerThreadDataBase
{
	int	m_i32Flag;			//	0为CHXNTFSManagerThreadDataRead 读文件夹，
							//	1为CHXNTFSManagerThreadDataPaste 拷贝文件,
							//	2为CHXNTFSManagerThreadDataRead 刷新缓存，
	CHXNTFSManagerThreadDataBase() :m_i32Flag(-1) {}
	virtual ~CHXNTFSManagerThreadDataBase() {}
}*LPHXNTFSManagerThreadDataBase;

typedef struct CHXNTFSManagerThreadDataPaste : public CHXNTFSManagerThreadDataBase
{
	qint32			m_i32ID;
	QList<QString>	m_listCopyFile;
	QString			m_strTargetDir;
}*LPHXNTFSManagerThreadDataPaste;

typedef struct CHXNTFSManagerThreadDataRead : public CHXNTFSManagerThreadDataBase
{
	QString m_strDir;
}*LPHXNTFSManagerThreadDataRead;

class CHXNTFSReadDirecotryThread : public QThread
{
	Q_OBJECT
public:
	explicit CHXNTFSReadDirecotryThread(HANDLE hHandle = INVALID_HANDLE_VALUE, QObject* parent = nullptr);
	~CHXNTFSReadDirecotryThread();
public:
	virtual void run()override;
	void Init(HANDLE hHandle, CHXPluginManagePtr p);
signals:
	void signalReadDirectoryResult(void* findDirecoty);
	void signalErrorInfo(QString);
private:
	void	OnRead(LPHXNTFSManagerThreadDataBase p);
	void	OnRefresh(LPHXNTFSManagerThreadDataBase p);
	void	ReadDirectory(void* lparam);
	void	NTFSRefresh(const QString& path);
private:
	HANDLE m_hRequestQueue;
	CHXPluginManagePtr      m_pPluginManage;
	QAtomicInt	m_i32Flag;
};

class CHXNTFSReadFileThread : public QThread
{
	Q_OBJECT
public:
	explicit CHXNTFSReadFileThread(HANDLE hHandle = INVALID_HANDLE_VALUE, QObject *parent = nullptr);
	~CHXNTFSReadFileThread();

public:
	virtual void run()override;
	void Init(HANDLE hHandle, CHXPluginManagePtr p);
	void SetCancelPaste(int i32 = 0);
signals:
	//void signalReadDirectoryResult(void * findDirecoty);
	void signalPasteFileSize(QString strFile, quint64 u64);
	void signalPasteFinish();
	void signalErrorInfo(QString);

private:
	void	OnRead(LPHXNTFSManagerThreadDataBase p);
	//void	OnRefresh(LPHXNTFSManagerThreadDataBase p);
	void	OnPasteFile(LPHXNTFSManagerThreadDataBase p);

	void	ReadDirectory(void * lparam);
	//void	NTFSRefresh(const QString & path);
	int		ReadFile(void * param);
	void	PasteFile(QString & strCopyFileSrc, QString strNewDir);
	void	PasteDir(QString & strCopyFileSrc, QString strNewDir);
	QString CheckFileName(const QString & str, QString strNewDir);
	QString CheckDirName(const QString & str, QString strNewDir);
	
private:
	HANDLE m_hRequestQueue;
	CHXPluginManagePtr      m_pPluginManage;
	QAtomicInt	m_i32Flag;
};


class CHXNTFSManager : public QObject
{
	Q_OBJECT
public:
	CHXNTFSManager();
	void PasteFile(QString & strCopyFileSrc, QString strNewDir);
	void PasteDir(QString & strCopyFileSrc, QString strNewDir);
	QString CheckFileName(const QString & str, QString strNewDir);
	QString CheckDirName(const QString & str, QString strNewDir);
	~CHXNTFSManager();

	void Init();
	void Release();

	void SetCurrentDirectory(const QString & path);

	LPHXReadDirectory GetDir();
	void FreeDir();
	///	文件监控
signals:
	void CloseMonitorDir();
	void fileChange();
public slots:
	void MonitorDir(QString dir);
	void slotCancelPaste();
	void slotErrorInfo(QString strErrorInfo);


	///	文件拷贝
public slots:
	void Paste(const QList<QString> & listFile, const QString & strTarget);
	//void slotsReadFile(void * param);
	   

	///	文件刷新
public slots:
	void NTFSRefresh(const QString & path);


	///	文件夹读取
public slots:
	void ReadDirectory(const QString & path);
	//void slotsReadDirectory(void * lparam);
	void slotReadDirectoryResult(void* pReadDirecoty);
	///	界面刷新
signals:
	void Refresh(void *listFileInfo);
	//void signalPasteFile(CHXFilePasteData p);

private:
	BOOL QueueRequest(ULONG_PTR p);
private:
	HANDLE					m_hReadFile;
	HANDLE					m_hReadDir;
	HANDLE					m_hMonitorFile;
#ifdef _HX_Monitor_
	HXFileMonitorThread		m_threadeMonitor;
#endif // _HX_Monitor_
	QString					m_strCurrentDirectory;
	QReadWriteLock			m_lock;
	LPHXReadDirectory		m_pCurrentDir;
	//CHXFilePasteThread		m_threadPaste;
	CHXNTFSProgressWnd		m_wndProgress;
	CHXNTFSReadFileThread	m_threadReadFile;
	CHXNTFSReadDirecotryThread	m_threadReadDir;
	CHXPluginManagePtr      m_pPluginManage;
};

