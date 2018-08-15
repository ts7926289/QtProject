#pragma execution_character_set("utf-8")//��ֹ����
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
	QString msg = QString("���ӷ������ɹ�");
	ui.te_show->setText(msg);
	ui.btn_connect->setEnabled(false);
	totalSize = 0;
	sendSize = 0;
}

void clietn_file::on_btn_file_clicked()
{
	QString filename = QFileDialog::getOpenFileName(this, "�ϴ��ļ�", "", "video(*.mp4)");
	if (filename.isEmpty())
	{
		return;
	}
	qDebug() << filename;
	QString msg = QString("׼������[%1]�ļ�").arg(filename);
	ui.te_show->append(msg);

	file->setFileName(filename);
	//����ֻ�����κε��ļ�
	bool ret = file->open(QIODevice::ReadOnly | QIODevice::Unbuffered);
	if (!ret)
	{
		return;
	}
	ui.progressBar->setValue(0);
	this->totalSize = 0;
	this->sendSize = 0;

	//��ȡ�ļ���С
	this->totalSize = file->size();
	ui.progressBar->setRange(0, totalSize);
	msg = QString("�ļ����ܴ�СΪ��%1").arg(this->totalSize);
	ui.te_show->append(msg);

	//����ͷ��������filename#totalsize
	QFileInfo info(filename);
	msg = QString("%1#%2#").arg(info.fileName()).arg(this->totalSize);
	ui.te_show->append(msg);
	//��һ�����ļ������ļ��Ĵ�С��Ϣ ��Ϊ���ı���Ϣ������toUtf8()
	client->write(msg.toUtf8());
	//�ȴ��������������ٽ��н������Ĳ���
	client->waitForBytesWritten();

	//׼���߶��ļ��߷�������
	quint64 len;
	//���ļ�û�ж���ĩβ
	while (!file->atEnd())
	{
		//��ÿ��MSG_LEN��С�������ݶ�ȡ
		QByteArray ba = file->read(MSG_LEN);
		//����ȡ��������������˽��д���
		len = client->write(ba);
		//�ȴ��������������ٽ��н������Ĳ���
		client->waitForBytesWritten();
		//���¿ͻ��˽�����
		this->sendSize += len;
		ui.progressBar->setValue(sendSize);

	}
	file->close();
	if (this->sendSize == this->totalSize)
	{
		msg = QString("�ϴ��ļ��ɹ�");
		/*ui.te_show->append(msg);
		this->totalSize = 0;
		this->sendSize = 0;
		file->close();*/
	}
	else
	{
		msg = QString("�ϴ��ļ�ʧ��[totalSize:%1 sendSize:%2]").arg(this->totalSize).arg(sendSize);
	}
	ui.te_show->append(msg);
}