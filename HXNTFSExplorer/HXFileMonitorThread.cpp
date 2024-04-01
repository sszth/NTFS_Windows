#include "stdafx.h"

#include <QDebug>


#include "HXFileMonitorThread.h"

HXFileMonitorThread::HXFileMonitorThread()
	:m_bStop(false)
	, m_hDir(INVALID_HANDLE_VALUE)
	, m_hRequestQueue(INVALID_HANDLE_VALUE)
{
	memset(&m_overlapped, 0, sizeof(m_overlapped));
	m_overlapped.hEvent = CreateEvent(NULL, false, NULL, NULL);
}

void HXFileMonitorThread::Init(HANDLE hHandle)
{
	m_hRequestQueue = hHandle;
}

void HXFileMonitorThread::run()
{
	DWORD dwBytesTransfered;
	ULONG_PTR dwCompletionKey;

	OVERLAPPED* pOverlapped;
	while (GetQueuedCompletionStatus(m_hRequestQueue, &dwBytesTransfered, &dwCompletionKey, &pOverlapped, INFINITE))
	{
		if (HX_SHUTDOWN == pOverlapped) // Shut down
		{
			qInfo() << "HXFileMonitorThread::run HX_SHUTDOWN!";
			break;
		}
		else										// Do work
		{
			LPHXFileMonitorData p = (LPHXFileMonitorData)dwCompletionKey;
			if (nullptr == p)
			{
				break;
			}

			m_bStop = false;
			Monitor(p->m_strDirctory);
			delete p;
		}
	}
}
void HXFileMonitorThread::Close()
{
	m_bStop = true; 
	SetEvent(m_overlapped.hEvent);
	//CloseHandle(m_hDir);
}

//	后可改为异步
void HXFileMonitorThread::Monitor(QString & str)
{
	char notify[1024] = "";
	DWORD cbBytes, i;

	FILE_NOTIFY_INFORMATION* pnotify = (FILE_NOTIFY_INFORMATION*)notify;
	FILE_NOTIFY_INFORMATION* tmp;

	WCHAR path[MAX_PATH + 1] = L"";
	str.toWCharArray(path);
	//GetCurrentDirectory(MAX_PATH, path);
	m_hDir = CreateFile(path, FILE_LIST_DIRECTORY,
		FILE_SHARE_READ |
		FILE_SHARE_WRITE |
		FILE_SHARE_DELETE, NULL,
		OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS |
		FILE_FLAG_OVERLAPPED, NULL);
	if (m_hDir == INVALID_HANDLE_VALUE)
	{
		return;
	}

	while (false == m_bStop)
	{
		memset(notify, 0, sizeof(notify));
		if (!ReadDirectoryChangesW(
			m_hDir, 
			&notify, 
			sizeof(notify),
			FALSE, 
			FILE_NOTIFY_CHANGE_LAST_WRITE
			| FILE_NOTIFY_CHANGE_FILE_NAME
			| FILE_NOTIFY_CHANGE_DIR_NAME
			| FILE_NOTIFY_CHANGE_SIZE,
			&cbBytes, NULL, NULL))
		{
			Q_ASSERT(0);
			break;
		}

		if (WaitForSingleObject(m_overlapped.hEvent, INFINITE) == WAIT_OBJECT_0)
		{
			if (true == m_bStop)
			{
				break;
			}
		}
		tmp = pnotify;
		if (tmp && 0 != tmp->Action)
		{
			emit fileChange();
		}
	}
	CloseHandle(m_hDir);
	m_hDir = INVALID_HANDLE_VALUE;
	qDebug() << u8"文件监控退出：" << str;
}
