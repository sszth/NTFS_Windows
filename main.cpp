#include "QtGuiApplication1.h"
#include <QtWidgets/QApplication>
#include <QApplication>
#include <QTableWidget>
#include <QTableWidgetItem>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	// 构造了一个QTableWidget的对象，并且设置为10行，5列
	QTableWidget *tableWidget = new QTableWidget();
	// 也可用下面的方法构造QTableWidget对象
	// QTableWidget *tableWidget = new QTableWidget;
	// tableWidget->setRowCount(10); //设置行数为10
	tableWidget->setColumnCount(5); //设置列数为5
	tableWidget->setWindowTitle("QTableWidget & Item");
	tableWidget->resize(350, 200); //设置表格
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
