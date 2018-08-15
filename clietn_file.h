#pragma once

#include <QtWidgets/QWidget>
#include "ui_clietn_file.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <QFile>

class clietn_file : public QWidget
{
	Q_OBJECT

public:
	clietn_file(QWidget *parent = Q_NULLPTR);

private:
	Ui::clietn_fileClass ui;
	QTcpSocket* client;
	QFile* file;
	quint64 totalSize;
	quint64 sendSize;
private slots:
	void on_btn_connect_clicked();
	void onconnected();
	void on_btn_file_clicked();
};
