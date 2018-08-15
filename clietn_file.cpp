#pragma execution_character_set("utf-8")//防止乱码
#include "clietn_file.h"
#include <QFileDialog>
#include <QDebug>
#define MSG_LEN 1024*3

clietn_file::clietn_file(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	client = new QTcpSocket(this);
	file = new QFile(this);
}

void clietn_file::on_btn_connect_clicked()
{
	QString serverip = ui.lineEdit_serverip->text();
	QString serverport = ui.lineEdit_serverport->text();
	client->connectToHost(QHostAddress(serverip), serverport.toUInt());//
	connect(client, SIGNAL(connected()), this, SLOT(onconnected()));
}

void clietn_file::onconnected()
{
	QString msg = QString("连接服务器成功");
	ui.te_show->setText(msg);
	ui.btn_connect->setEnabled(false);
	totalSize = 0;
	sendSize = 0;
}

void clietn_file::on_btn_file_clicked()
{
	QString filename = QFileDialog::getOpenFileName(this, "上传文件", "", "video(*.mp4)");
	if (filename.isEmpty())
	{
		return;
	}
	qDebug() << filename;
	QString msg = QString("准备发送[%1]文件").arg(filename);
	ui.te_show->append(msg);

	file->setFileName(filename);
	//设置只读和任何的文件
	bool ret = file->open(QIODevice::ReadOnly | QIODevice::Unbuffered);
	if (!ret)
	{
		return;
	}
	ui.progressBar->setValue(0);
	this->totalSize = 0;
	this->sendSize = 0;

	//获取文件大小
	this->totalSize = file->size();
	ui.progressBar->setRange(0, totalSize);
	msg = QString("文件的总大小为：%1").arg(this->totalSize);
	ui.te_show->append(msg);

	//发送头给服务器filename#totalsize
	QFileInfo info(filename);
	msg = QString("%1#%2#").arg(info.fileName()).arg(this->totalSize);
	ui.te_show->append(msg);
	//第一发送文件名和文件的大小信息 因为是文本信息所以用toUtf8()
	client->write(msg.toUtf8());
	//等待服务器端收完再进行接下来的操作
	client->waitForBytesWritten();

	//准备边读文件边发送数据
	quint64 len;
	//当文件没有读到末尾
	while (!file->atEnd())
	{
		//以每次MSG_LEN大小进行数据读取
		QByteArray ba = file->read(MSG_LEN);
		//将读取的数据项服务器端进行传输
		len = client->write(ba);
		//等待服务器端收完再进行接下来的操作
		client->waitForBytesWritten();
		//更新客户端进度条
		this->sendSize += len;
		ui.progressBar->setValue(sendSize);

	}
	file->close();
	if (this->sendSize == this->totalSize)
	{
		msg = QString("上传文件成功");
		/*ui.te_show->append(msg);
		this->totalSize = 0;
		this->sendSize = 0;
		file->close();*/
	}
	else
	{
		msg = QString("上传文件失败[totalSize:%1 sendSize:%2]").arg(this->totalSize).arg(sendSize);
	}
	ui.te_show->append(msg);
}