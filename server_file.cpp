#pragma execution_character_set("utf-8")//��ֹ����
#include "server_file.h"
#define MAMNUM 1000
#include <QMessageBox>
server_file::server_file(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	//��ʼ����������
	recvTotal = allTotal = 0;

	file = new QFile(this);
	myServer = new QTcpServer(this);

	//�����ӵĿͻ��˽��д���
	connect(myServer, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
	connect(myServer, SIGNAL((error(QAbstractSocket::SocketError socketError))), this, SLOT(onError(QAbstractSocket::SocketError socketError)));
}
//�����ӵĿͻ��˽��д���
void server_file::onNewConnection()
{
	client = myServer->nextPendingConnection();
	arrayClient.append(client);
	/*QTcpSocket* client = (QTcpSocket*)this->sender();*/


	QString msg = QString("�ͻ���[%1:%2] ���룡").arg(client->peerAddress().toString()).arg(client->peerPort());
	ui.te_show->setText(msg);

	//��ȡ����
	connect(client, SIGNAL(readyRead()), this, SLOT(onReadyread()));

//	//�ͻ��˶Ͽ�
//	connect(client, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
//
}
 
//��ȡ�ͻ��˷��͵�����
void server_file::onReadyread()
{
	//���ܵ��ֽ�����
	QByteArray ba= client->readAll();	

	//��һ�ν������� ��Ϊ�˻���ļ�������Ҫ���ܵ��ļ���С ����ʶ�������Ľ���
	if (this->recvTotal == 0)
	{
		this->allTotal = 0;
		//��ȡ�ͻ��˷��������ļ�������Ҫ�ϴ��ļ��Ĵ�С
		//eg:filename#totalsize#
		QStringList list= QString(ba).split("#");
		QString filename = list.at(0);
		this->allTotal = QString(list.at(1)).toLongLong();
		//�����ļ������ļ���С��ռ���ڴ� ��Ϊ���ǵ�һ�η��͵����� ���������ǳ������ܵ��ļ�����
		this->allTotal += ba.length();
		this->recvTotal += ba.length();

		//���ļ�
		file->setFileName(filename);
		//ѡ��ֻд���ϴ�ģʽ
		bool ret = file->open(QIODevice::WriteOnly|QIODevice::Truncate);
		if (!ret)
		{
			recvTotal = 0;
			QMessageBox::warning(this, "warning", "�����ļ�ʧ�ܣ�");
			return;
		}
		QString msg = QString("���ڽ����ļ�:%1").arg(filename);
		ui.te_show->append(msg);
		ui.progressBar->setRange(0, allTotal);		
	}
	//��ʽ��ʼ�����ļ������� �ļ��ǰ������õ����ݴ�С����������
	else
	{
		qint64 len = file->write(ba);
		this->recvTotal += len;
	}
	//���½�����
	ui.progressBar->setValue(this->recvTotal);

	//�ж��Ƿ�������
	if (this->recvTotal == this->allTotal)
	{
		file->close();
		this->recvTotal = 0;
		this->allTotal = 0;
		QString msg = QString("�����ļ��ɹ�");
		ui.te_show->setText(msg);
	}
}

//�󶨷������˵�IP�Ͷ˿�
void server_file::btn_bind_button_clicked()
{
	//����1��ͨ�������������ip
	//����2���ֶ�����ip
	QString myAddr;
	myAddr = ui.lineEdit_serverip->text();
	QString myPort = ui.lineEdit_serverport->text();
	bool ret = myServer->listen(QHostAddress(myAddr), myPort.toUInt());
	/*bool ret = server->listen(QHostAddress::Any, 8888);*/
	QString msg;
	if (!ret)
	{
		msg = QString("��ʧ��");
	}
	else
	{
		msg = QString("�󶨳ɹ�");
		ui.btn_listen->setEnabled(false);
	}
	ui.te_show->append(msg);
	myServer->setMaxPendingConnections(MAMNUM);
}

//�Է������Ĵ�����д���
void server_file::onError(QAbstractSocket::SocketError socketError)
{
	qDebug() << socketError;
}