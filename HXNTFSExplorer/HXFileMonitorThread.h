#pragma once
#include <QThread>

//#define _HX_USE_Completion_

typedef struct CHXFileMonitorData
{
	QString m_strDirctory;
}*LPHXFileMonitorData;

class HXFileMonitorThread : public QThread
{
	Q_OBJECT
public:
	HXFileMonitorThread();
	//QString m_strPartition;

	void Init(HANDLE hHandle);

	virtual void run();

	void Monitor(QString & str);

signals:
	void fileChange();
public slots:
	void Close();

private:
	OVERLAPPED	m_overlapped;
	bool		m_bStop;
	HANDLE		m_hDir;
	HANDLE		m_hRequestQueue;
};

