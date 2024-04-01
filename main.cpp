#include "QtGuiApplication1.h"
#include <QtWidgets/QApplication>
#include <QApplication>
#include <QTableWidget>
#include <QTableWidgetItem>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	// ������һ��QTableWidget�Ķ��󣬲�������Ϊ10�У�5��
	QTableWidget *tableWidget = new QTableWidget();
	// Ҳ��������ķ�������QTableWidget����
	// QTableWidget *tableWidget = new QTableWidget;
	// tableWidget->setRowCount(10); //��������Ϊ10
	tableWidget->setColumnCount(5); //��������Ϊ5
	tableWidget->setWindowTitle("QTableWidget & Item");
	tableWidget->resize(350, 200); //���ñ��
	QStringList header;
	header << "Month" << "Description";
	tableWidget->setHorizontalHeaderLabels(header);
	tableWidget->insertRow(tableWidget->rowCount());
	tableWidget->setItem(0, 0, new QTableWidgetItem("Jan"));
	tableWidget->insertRow(tableWidget->rowCount());
	tableWidget->setItem(1, 0, new QTableWidgetItem("Feb"));
	tableWidget->insertRow(tableWidget->rowCount());
	tableWidget->setItem(2, 0, new QTableWidgetItem("Mar"));

	tableWidget->setItem(0, 1, new QTableWidgetItem(QIcon(":/Image/IED.png"), "Jan's month"));
	tableWidget->setItem(1, 1, new QTableWidgetItem(QIcon(":/Image/IED.png"), "Feb's month"));
	tableWidget->setItem(2, 1, new QTableWidgetItem(QIcon(":/Image/IED.png"), "Mar's month"));
	tableWidget->show();

	return a.exec();
}
