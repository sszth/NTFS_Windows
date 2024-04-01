#pragma once

#include <QWidget>
namespace Ui { class CHXNTFSProgressWnd; };

class CHXNTFSProgressWnd : public QWidget
{
	Q_OBJECT

public:
	CHXNTFSProgressWnd(QWidget *parent = Q_NULLPTR);
	~CHXNTFSProgressWnd();

public:
	void Init();
	void closeEvent(QCloseEvent * e) override;
public slots:
	void slotWriteFileSize(QString strFile, quint64 u64);
	void slotPasteFinish();
	void on_pushButton_Cancel_clicked(bool checked = false);
	void on_pushButton_Close_clicked(bool checked = false);
	void slotErrorInfo(QString str);
	
signals:
	void signalCancel();


private:
	Ui::CHXNTFSProgressWnd *ui;
	QString m_strCurrent;
};
