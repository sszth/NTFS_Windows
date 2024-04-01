#include "stdafx.h"
#include <QtWidgets/QApplication>
#include <qtextcodec.h>
#include <QMutex> 
#include <QDateTime>
#include <QFile>

#include "HXNTFSExplorer.h"


static QMutex mutex;


void customMessageHandler(QtMsgType type,
	const QMessageLogContext &context,
	const QString &msg)
{
	//Q_UNUSED(context)
	QDateTime _datetime = QDateTime::currentDateTime();
	QString szDate = _datetime.toString("yyyy-MM-dd hh:mm:ss.zzz");//"yyyy-MM-dd hh:mm:ss ddd"
	QString txt(szDate);

	switch (type)
	{
	case QtDebugMsg://������Ϣ��ʾ
	{
		txt += QString(" [Debug] ");
		break;
	}
	case QtInfoMsg://��Ϣ���
	{
		txt += QString(" [Info] ");
		break;
	}
	case QtWarningMsg://һ���warning��ʾ
	{
		txt += QString(" [Warning] ");
		break;
	}
	case QtCriticalMsg://���ش�����ʾ
	{
		txt += QString(" [Critical] ");
		break;
	}
	case QtFatalMsg://����������ʾ
	{
		txt += QString(" [Fatal] ");
		//abort();
		break;
	}
	default:
	{
		txt += QString(" [Trace] ");
		break;
	}
	}

	txt.append(QString(" %1").arg(context.file));
	txt.append(QString("<%1>: ").arg(context.line));
	txt.append(msg);

	mutex.lock();
	QFile file("log.txt");
	file.open(QIODevice::WriteOnly | QIODevice::Append);
	QTextStream text_stream(&file);
	text_stream << txt << "\r\n";
	file.close();
	mutex.unlock();
}

int main(int argc, char *argv[])
{
#ifdef _HX_WRITE_LOG_
	qInstallMessageHandler(customMessageHandler);
#endif // _HX_WRITE_LOG_

	QTextCodec *codec = QTextCodec::codecForName("UTF-8");
	QTextCodec::setCodecForLocale(codec);
	//Sleep(5 * 1000);

	QApplication a(argc, argv);
	HXNTFSExplorer w;
	w.setWindowState(Qt::WindowMaximized);

	QFile file(":/HXNTFSExplorer/Resources/dracula.css");
	// ֻ����ʽ�򿪸��ļ�
	file.open(QFile::ReadOnly);
	// ��ȡ�ļ�ȫ�����ݣ�ʹ��tr()��������ת��ΪQString����
	QString styleSheet = file.readAll();
	// ΪQApplication������ʽ��
	qApp->setStyleSheet(styleSheet);
	file.close();
	w.show();
	return a.exec();
}
