#pragma execution_character_set("utf-8")//防止乱码
#include "server_file.h"
#define MAMNUM 1000
#include <QMessageBox>
server_file::server_file(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	//初始化数据容量
	recvTotal = allTotal = 0;

	file = new QFile(this);
	myServer = new QTcpServer(this);

	//对链接的客户端进行处理
	connect(myServer, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
	connect(myServer, SIGNAL((error(QAbstractSocket::SocketError socketError))), this, SLOT(onError(QAbstractSocket::SocketError socketError)));
}
//对链接的客户端进行处理
void server_file::onNewConnection()
{
	client = myServer->nextPendingConnection();
	arrayClient.append(client);
	/*QTcpSocket* client = (QTcpSocket*)this->sender();*/


	QString msg = QString("客户端[%1:%2] 连入！").arg(client->peerAddress().toString()).arg(client->peerPort());
	ui.te_show->setText(msg);

	//读取内容
	connect(client, SIGNAL(readyRead()), this, SLOT(onReadyread()));

//	//客户端断开
//	connect(client, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
//
}
 
//读取客户端发送的数据
void server_file::onReadyread()
{
	//接受的字节数据
	QByteArray ba= client->readAll();	

	//第一次接受数据 是为了获得文件名和需要接受的文件大小 来标识进度条的进度
	if (this->recvTotal == 0)
	{
		this->allTotal = 0;
		//获取客户端发过来的文件名及需要上传文件的大小
		//eg:filename#totalsize#
		QStringList list= QString(ba).split("#");
		QString filename = list.at(0);
		this->allTotal = QString(list.at(1)).toLongLong();
		//加上文件名及文件大小所占的内存 因为这是第一次发送的数据 接下来才是持续接受的文件数据
		this->allTotal += ba.length();
		this->recvTotal += ba.length();

		//打开文件
		file->setFileName(filename);
		//选择只写和上传模式
		bool ret = file->open(QIODevice::WriteOnly|QIODevice::Truncate);
		if (!ret)
		{
			recvTotal = 0;
			QMessageBox::warning(this, "warning", "创建文件失败！");
			return;
		}
		QString msg = QString("正在接收文件:%1").arg(filename);
		ui.te_show->append(msg);
		ui.progressBar->setRange(0, allTotal);		
	}
	//正式开始接受文件的数据 文件是按照设置的数据大小来持续传输
	else
	{
		qint64 len = file->write(ba);
		this->recvTotal += len;
	}
	//更新进度条
	ui.progressBar->setValue(this->recvTotal);

	//判断是否接收完毕
	if (this->recvTotal == this->allTotal)
	{
		file->close();
		this->recvTotal = 0;
		this->allTotal = 0;
		QString msg = QString("接收文件成功");
		ui.te_show->setText(msg);
	}
}

//绑定服务器端的IP和端口
void server_file::btn_bind_button_clicked()
{
	//方法1：通过程序遍历网卡ip
	//方法2：手动设置ip
	QString myAddr;
	myAddr = ui.lineEdit_serverip->text();
	QString myPort = ui.lineEdit_serverport->text();
	bool ret = myServer->listen(QHostAddress(myAddr), myPort.toUInt());
	/*bool ret = server->listen(QHostAddress::Any, 8888);*/
	QString msg;
	if (!ret)
	{
		msg = QString("绑定失败");
	}
	else
	{
		msg = QString("绑定成功");
		ui.btn_listen->setEnabled(false);
	}
	ui.te_show->append(msg);
	myServer->setMaxPendingConnections(MAMNUM);
}

//对服务器的错误进行处理
void server_file::onError(QAbstractSocket::SocketError socketError)
{
	qDebug() << socketError;
}