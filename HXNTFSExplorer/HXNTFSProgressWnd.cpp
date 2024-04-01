#include "stdafx.h"
#include "HXNTFSProgressWnd.h"
#include "ui_HXNTFSProgressWnd.h"

CHXNTFSProgressWnd::CHXNTFSProgressWnd(QWidget *parent)
	: QWidget(parent)
{
	ui = new Ui::CHXNTFSProgressWnd();
	ui->setupUi(this);
}

CHXNTFSProgressWnd::~CHXNTFSProgressWnd()
{
	delete ui;
}

void CHXNTFSProgressWnd::Init()
{
	ui->listWidget->clear();
	QListWidgetItem * item = new QListWidgetItem();
	item->setText(u8"ÕýÔÚ¿½±´:");
	ui->listWidget->addItem(item);
	m_strCurrent.clear();
}

void CHXNTFSProgressWnd::closeEvent(QCloseEvent * e)
{
	emit signalCancel();
	QWidget::closeEvent(e);
}

void CHXNTFSProgressWnd::slotPasteFinish()
{
	QListWidgetItem * item = new QListWidgetItem();
	QString str = u8"Íê±Ï£¡";
	item->setText(str);
	ui->listWidget->addItem(item);
}

void CHXNTFSProgressWnd::on_pushButton_Cancel_clicked(bool checked)
{
	on_pushButton_Close_clicked();
}

void CHXNTFSProgressWnd::on_pushButton_Close_clicked(bool checked)
{
	QWidget::close();
}

void CHXNTFSProgressWnd::slotErrorInfo(QString str)
{
	ui->listWidget->addItem(new QListWidgetItem(str));
}

void CHXNTFSProgressWnd::slotWriteFileSize(QString strFile, quint64 u64)
{
	static int i32hx = -1;
	if (strFile == m_strCurrent)
	{
		qDebug() << "CHXNTFSProgressWnd::slotWriteFileSize :" << ui->listWidget->count();
		int i32 = ui->listWidget->count() + i32hx;
		auto p = ui->listWidget->item(i32);
		if (nullptr == p)
		{
			return;
		}
		QString str = strFile;
		str += ": ";
		str += QString::number(u64);
		str += " byte";
		p->setText(str);
	}
	else
	{
		m_strCurrent = strFile;
		QListWidgetItem * item = new QListWidgetItem();
		QString str = strFile;
		str += ": ";
		str += QString::number(u64);
		str += " byte";
		item->setText(str);
		ui->listWidget->addItem(item);
	}
}