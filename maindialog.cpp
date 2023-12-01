#include "maindialog.h"
#include "ui_maindialog.h"

MainDialog::MainDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::MainDialog)
{
    ui->setupUi(this);


    // 获取本地IP地址，将其显示到组合框控件（下拉列表和下拦菜单功能）
    QString localIP = getLocalIP();//本机IP
    ui->comboIP->addItem(localIP);

    // 将获取IP地址显示至Label控件
    QString strip = ("本机IP："+localIP);
    ui->label_stateip->setText(strip);

    // 初始化变量为0
    m_totalBytes = 0;
    m_bytesReceived = 0;
    m_fileNameSize = 0;

    //若检测到newConnection()信号，建立连接
    m_tcpServer = new QTcpServer(this);
    connect(m_tcpServer, SIGNAL(newConnection()), this, SLOT(onNewConnection()));

    // 新建处理聊天消息传输套接字和文件传输套接字
    m_tcpTextSocket = new QTcpSocket(this);
    m_tcpFileSocket = new QTcpSocket(this);

    // 启动监听和停止监听命令按钮切换
    ui->pushButton_StartListening->setEnabled(true);
    ui->pushButton_StopListening->setEnabled(false);


    setWindowFlags(windowFlags()&~Qt::WindowMaximizeButtonHint);    // 禁止最大化按钮
    setFixedSize(width(), height()); // 固定窗口宽度和高度

    // 设置多行文本编辑器控件里面的文本为只读属性
    ui->plainTextEdit->setReadOnly(true);

    // 设置字体颜色
    QPalette platestr1 = ui->label_server->palette();
    platestr1.setColor(QPalette::WindowText, Qt::yellow); // 设置字体颜色为黄色
    ui->label_server->setPalette(platestr1);
    // 设置字体加粗
    QFont font1 = ui->label_server->font();
    font1.setBold(true); // 设置字体加粗
    ui->label_server->setFont(font1);

    // 设置字体颜色
    QPalette platestr2 = ui->label_port->palette();
    platestr2.setColor(QPalette::WindowText, Qt::yellow); // 设置字体颜色为黄色
    ui->label_port->setPalette(platestr2);
    // 设置字体加粗
    QFont font2 = ui->label_port->font();
    font2.setBold(true); // 设置字体加粗
    ui->label_port->setFont(font2);

    // 设置字体颜色
    QPalette platestr3 = ui->groupBox->palette();
    platestr3.setColor(QPalette::WindowText, Qt::yellow); // 设置字体颜色为黄色
    ui->groupBox->setPalette(platestr3);
    // 设置字体加粗
    QFont font3 = ui->groupBox->font();
    font3.setBold(true); // 设置字体加粗
    ui->groupBox->setFont(font3);

    // 设置字体颜色
    QPalette platestr4 = ui->groupBox_2->palette();
    platestr4.setColor(QPalette::WindowText, Qt::yellow); // 设置字体颜色为黄色
    ui->groupBox_2->setPalette(platestr4);
    // 设置字体加粗
    QFont font4 = ui->groupBox_2->font();
    font4.setBold(true); // 设置字体加粗
    ui->groupBox_2->setFont(font4);

    // 设置字体颜色
    QPalette platestr5 = ui->label_3->palette();
    platestr5.setColor(QPalette::WindowText, Qt::yellow); // 设置字体颜色为黄色
    ui->label_3->setPalette(platestr5);
    // 设置字体加粗
    QFont font5 = ui->label_3->font();
    font5.setBold(true); // 设置字体加粗
    ui->label_3->setFont(font5);

    // 设置字体颜色
    QPalette platestr6 = ui->progressBar->palette();
    platestr6.setColor(QPalette::WindowText, Qt::yellow); // 设置字体颜色为黄色
    ui->progressBar->setPalette(platestr6);
    // 设置字体加粗
    QFont font6 = ui->progressBar->font();
    font6.setBold(true); // 设置字体加粗
    ui->progressBar->setFont(font6);


    // GetSystemDateTime(); // 获取系统日期时间
}

MainDialog::~MainDialog()
{
    delete ui;
}

#include <QDebug>
QString MainDialog::getLocalIP()
{
    // 获取本机IPv4地址
    QString hostName = QHostInfo::localHostName();  // 本地主机名称
    QHostInfo hostInfo = QHostInfo::fromName(hostName); // 通过主机名获取IP地址


    QString localIP = "";
    // QList<QHostAddress>：返回与hostName()主机关联的Ip地址列表
    QList<QHostAddress> addList = hostInfo.addresses();

    if(!addList.isEmpty())
    {
        for(int i = 0; i < addList.count(); ++i)
        {
            // qDebug()<<i<<endl;
            QHostAddress aHost = addList.at(i);

            // 判断本机IP协议是否为IPv4
            if(QAbstractSocket::IPv4Protocol == aHost.protocol())
            {
                localIP = aHost.toString();
                //   QMessageBox::information(this,"提示",localIP,QMessageBox::Yes);
                break;
            }
        }
    }

    return localIP;
}


void MainDialog::closeEvent(QCloseEvent *event)
{
    //关闭窗口时停止监听
    if (m_tcpServer->isListening())
        m_tcpServer->close(); //停止网络监听
    event->accept();
}

void MainDialog::onNewConnection()
{
    // 处理聊天消息
    if (m_tcpTextSocket->state() == QAbstractSocket::UnconnectedState)
    {
        // 创建socket(返回下一个挂起的连接作为已连接的QTcpSocket对象。)
        m_tcpTextSocket = m_tcpServer->nextPendingConnection();

        // // 客户端接入时，则输出数据消息
        connect(m_tcpTextSocket, SIGNAL(connected()), this, SLOT(onClientConnected()));

        // 客户端断开连接时，则输出提示数据信息
        connect(m_tcpTextSocket, SIGNAL(disconnected()), this, SLOT(onClientDisconnected()));

        // 读取缓冲区行文本数据信息
        connect(m_tcpTextSocket, SIGNAL(readyRead()), this, SLOT(onSocketReadyRead()));
        return;
    }

    // 处理文件传输
    if (m_tcpFileSocket->state() == QAbstractSocket::UnconnectedState)
    {
        // 创建socket(返回下一个挂起的连接作为已连接的QTcpSocket对象。)
        m_tcpFileSocket = m_tcpServer->nextPendingConnection(); // 创建socket

        // 客户端连接成功，则输出提示数据信息
        connect(m_tcpFileSocket, SIGNAL(connected()), this, SLOT(onFileClientConnected()));

        // 客户端断开连接，则输出提示数据信息
        connect(m_tcpFileSocket, SIGNAL(disconnected()), this, SLOT(onFileClientDisconnected()));

        // 更新进度条，接收文件数据信息
        connect(m_tcpFileSocket, SIGNAL(readyRead()), this, SLOT(updateServerProgress()));

        // 错误提示信息
        connect(m_tcpFileSocket, SIGNAL(error(QAbstractSocket::SocketError)),
                this, SLOT(displayError(QAbstractSocket::SocketError)));
    }
    ui->plainTextEdit->appendPlainText("\n[**********有客户端连接**********]");
}



void MainDialog::onClientConnected()
{
    // 客户端接入时，则输出提示数据信息

}

void MainDialog::onClientDisconnected()
{
    // 客户端断开连接时，则输出提示数据信息
    ui->plainTextEdit->appendPlainText("[**********客户端已断开和服务器端连接**********]");
    m_tcpTextSocket->close();
}

void MainDialog::onSocketReadyRead()
{
    // 处理日期时间
    QDateTime currentDateTime = QDateTime::currentDateTime();
    QString year = QString::number(currentDateTime.date().year());
    QString month = QString::number(currentDateTime.date().month());
    QString day = QString::number(currentDateTime.date().day());
    QString hour = QString::number(currentDateTime.time().hour());
    QString minute = QString::number(currentDateTime.time().minute());
    QString second = QString::number(currentDateTime.time().second());
    QString strtemp=year+"/"+month+"/"+day+" "+hour+":"+minute+":"+second;

    // 读取缓冲区行文本数据信息显示到多行文件编辑框控件
    while(m_tcpTextSocket->canReadLine())
        ui->plainTextEdit->appendPlainText("[客户端消息" + strtemp + "]：" + m_tcpTextSocket->readLine());
}


void MainDialog::updateServerProgress()  //更新进度条，接收文件数据信息
{
    // 处理日期时间
    QDateTime currentDateTime = QDateTime::currentDateTime();
    QString year = QString::number(currentDateTime.date().year());
    QString month = QString::number(currentDateTime.date().month());
    QString day = QString::number(currentDateTime.date().day());
    QString hour = QString::number(currentDateTime.time().hour());
    QString minute = QString::number(currentDateTime.time().minute());
    QString second = QString::number(currentDateTime.time().second());
    QString strtemp = year + "/" + month + "/" + day + " " + hour + ":" + minute + ":" + second;


    QDataStream in(m_tcpFileSocket);
    // in.setVersion(QDataStream::Qt_5_12);

    if (m_bytesReceived <= sizeof(qint64)*2)
    {
        // 如果接收到的数据小于16个字节，那么是刚开始接收数据，保存接收到的的头文件信息
        if((m_tcpFileSocket->bytesAvailable() >= sizeof(qint64)*2) && (m_fileNameSize == 0))
        {
            //接收数据总大小信息和文件名大小信息
            in >> m_totalBytes >> m_fileNameSize;
            m_bytesReceived += sizeof(qint64) * 2;
        }

        // bytesAvailable()用于获取当前可读取的字节数。
        if((m_tcpFileSocket->bytesAvailable() >= m_fileNameSize) && (m_fileNameSize != 0))
        {
            //接收文件名，并建立文件
            in >> m_fileName;
            ui->plainTextEdit->appendPlainText(tr("[%1 ♥♥♥♥♥♥接收文件 %2 ...♥♥♥♥♥♥]").arg(strtemp).arg(m_fileName));
            m_bytesReceived += m_fileNameSize;
            m_localFile = new QFile(QApplication::applicationDirPath() + "\\" + m_fileName);
            if(!m_localFile->open(QFile::WriteOnly))
            {
                qDebug() << "打开文件错误，请重新检查？";
                return;
            }
        }
        else
            return;
    }

    // 已经收到文件数据大小总数据 < 文件数据大小 ，则继续写入文件
    if(m_bytesReceived < m_totalBytes)
    {
        // 如果接收的数据小于总数据，那么写入文件  // bytesAvailable()用于获取当前可读取的字节数。
        m_bytesReceived += m_tcpFileSocket->bytesAvailable();
        m_inBlock = m_tcpFileSocket->readAll(); // 使用 readAll() 函数将文件中的全部数据读取到 data 变量中
        m_localFile->write(m_inBlock); // 将整块数据写入文件
        m_inBlock.resize(0);
    }

    //更新进度条
    ui->progressBar->setMaximum(m_totalBytes);
    ui->progressBar->setValue(m_bytesReceived);

    if(m_bytesReceived == m_totalBytes)
    {
        //接收数据完成时
        m_totalBytes = 0;
        m_bytesReceived = 0;
        m_fileNameSize = 0;
        m_localFile->close();
        ui->plainTextEdit->appendPlainText(QString("[%1 ♥♥♥♥♥♥接收文件 %2 成功♥♥♥♥♥♥]").arg(strtemp).arg(m_fileName));
    }
}

void MainDialog::displayError(QAbstractSocket::SocketError) //错误处理
{
    // 输出错误信息 ui->plainTextEdit->appendPlainText(m_tcpFileSocket->errorString());
    m_tcpFileSocket->close();
    ui->progressBar->reset();
}




void MainDialog::on_pushButton_StartListening_clicked()
{
    //开始监听
    QString IP = ui->comboIP->currentText();//IP地址
    quint16 port = ui->spinPort->value();//端口
    QHostAddress addr(IP);

    m_tcpServer->listen(addr, port);//
    ui->plainTextEdit->appendPlainText("[=====服务器已开始监听=====]");
    ui->plainTextEdit->appendPlainText("[IP Address：" + m_tcpServer->serverAddress().toString() + "]"
                                       + "[Port：" + QString::number(m_tcpServer->serverPort()) + "]");

    ui->pushButton_StartListening->setEnabled(false);
    ui->pushButton_StopListening->setEnabled(true);

}

void MainDialog::on_pushButton_StopListening_clicked()
{
    //停止监听
    if (m_tcpServer->isListening()) // m_tcpServer正在监听
    {
        m_tcpServer->close(); // 停止监听
        ui->pushButton_StartListening->setEnabled(true);
        ui->pushButton_StopListening->setEnabled(false);
        ui->plainTextEdit->appendPlainText("[**********服务器已停止监听**********]\n");

    }

}


void MainDialog::on_pushButton_SendData_clicked()
{
    // 处理日期时间
    QDateTime currentDateTime = QDateTime::currentDateTime();
    QString year = QString::number(currentDateTime.date().year());
    QString month = QString::number(currentDateTime.date().month());
    QString day = QString::number(currentDateTime.date().day());
    QString hour = QString::number(currentDateTime.time().hour());
    QString minute = QString::number(currentDateTime.time().minute());
    QString second = QString::number(currentDateTime.time().second());
    QString strtemp = year + "/" + month + "/" + day + " " + hour + ":" + minute + ":" + second;

    //发送一行字符串，以换行符结束
    QString  msg = ui->editMsg->toPlainText();

    if (msg.isEmpty())
    {
        QMessageBox::critical(this, "错误", "发送消息不能为空，请重新检查");
        ui->editMsg->setFocus();
        return;
    }

    ui->plainTextEdit->appendPlainText("[服务器消息 "+strtemp+"]：" + msg);
    ui->editMsg->clear();


    QByteArray  str = msg.toUtf8();
    str.append('\n');   // 添加一个换行符
    m_tcpTextSocket->write(str);
}

void MainDialog::on_pushButton_ClearMsg_clicked()
{
    ui->plainTextEdit->clear();
}

void MainDialog::on_pushButton_ExitSystem_clicked()
{
    this->close();

}


void MainDialog::onFileClientConnected()
{
    ui->plainTextEdit->appendPlainText("[♥♥♥♥♥♥♥♥♥客户端套接字已连接成功♥♥♥♥♥♥♥♥♥]");
}


void MainDialog::onFileClientDisconnected()
{
    // ui->plainTextEdit->appendPlainText("**client socket disconnected");
    m_tcpFileSocket->close();
}

void MainDialog::paintEvent(QPaintEvent *ev)  // 更改背景图片
{
    QPainter painter(this);
    painter.drawPixmap(rect(), QPixmap(":/new/prefix1/images/bgpng5.png"), QRect());
}


