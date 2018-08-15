#pragma once

#include <QtWidgets/QWidget>
#include "ui_server_file.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <QFile>
class server_file : public QWidget
{
	Q_OBJECT

public:
	server_file(QWidget *parent = Q_NULLPTR);

private:
	Ui::server_fileClass ui;
	QTcpServer* myServer;
	QTcpSocket* client;
	QList<QTcpSocket*> arrayClient;

	//�����ļ�ʵ������
	qint64 recvTotal;
	//�ļ���ʵ������
	qint64 allTotal;

	QFile* file;
private slots:
	void btn_bind_button_clicked();
	void onError(QAbstractSocket::SocketError socketError);
	void onNewConnection();
	void onReadyread();

}; 
