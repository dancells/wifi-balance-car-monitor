#include "widget.h"
#include "ui_widget.h"
#include "rocker.h"

// 构造函数
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    // 初始ui
    ui->setupUi(this);
    ui->textEdit_RBuff->document()->setMaximumBlockCount(256);

    ui->radioBtn_Ctrl->setChecked(true);
    ui->buttonGroup_Mode->setId(ui->radioBtn_Ctrl, 0);
    ui->buttonGroup_Mode->setId(ui->radioBtn_Ges, 1);
    ui->buttonGroup_Mode->setId(ui->radioBtn_Spd, 2);
    ui->buttonGroup_Mode->setId(ui->radioBtn_Erect, 3);
    ui->buttonGroup_Mode->setId(ui->radioBtn_Turn, 4);
    connect(ui->buttonGroup_Mode, SIGNAL(buttonClicked (int)), this, SLOT(OptMode(int)));

    ui->buttonGroup_CH->setId(ui->checkBox_ch0, 0);
    ui->buttonGroup_CH->setId(ui->checkBox_ch1, 1);
    ui->buttonGroup_CH->setId(ui->checkBox_ch2, 2);
    ui->buttonGroup_CH->setId(ui->checkBox_ch3, 3);
    ui->buttonGroup_CH->setId(ui->checkBox_ch4, 4);
    connect(ui->buttonGroup_CH, SIGNAL(buttonClicked (int)), this, SLOT(chSet(int)));

    // 鼠标变化
    connect(ui->rocker_Ctrl, &rocker::rockerChanged, this, &Widget::sendCtrl);



    // 获取HostIP
    QHostAddress HostIP;
    getIP(&HostIP);
//    qDebug() << "HostIP:" << HostIP.toString();

    // 实例化UDP sender
    UDP_Server = new QUdpSocket(this);
    UDP_Server->open(QIODevice::ReadWrite);
    UDP_Server->bind(HostIP, 8081, QAbstractSocket::ShareAddress);
    connect(UDP_Server, SIGNAL(readyRead()), this, SLOT(processPendingDatagram()));


    plotInit();
}

// 析构函数
Widget::~Widget()
{
    delete ui;
}

// 获取IP
QString Widget::getIP(QHostAddress *HostIP)  //获取ip地址
{
    QString localHostName = QHostInfo::localHostName();
//    qDebug() <<"localHostName: "<<localHostName;
    QHostInfo info = QHostInfo::fromName(localHostName);
    foreach(QHostAddress address,info.addresses())
    {
        if(address.protocol() == QAbstractSocket::IPv4Protocol){
            *HostIP = address;
            return address.toString();
        }
    }
    return 0;
}

// UDP消息槽函数
void Widget::processPendingDatagram() //处理等待的数据报
{
    while(UDP_Server->hasPendingDatagrams())  //拥有等待的数据报
    {
        QByteArray datagram; //拥于存放接收的数据报

        //让datagram的大小为等待处理的数据报的大小，这样才能接收到完整的数据
        datagram.resize(UDP_Server->pendingDatagramSize());
        //接收数据报，将其存放到datagram中
        UDP_Server->readDatagram(datagram.data(),datagram.size(), &targetIP, &targetPort);

        //将数据报内容显示出来
        ui->textEdit_RBuff->append(datagram);

        // 建立连接
        if (strcmp(datagram.data(), "car")==0){
            ui->pushButton_Connect->setStyleSheet("QPushButton { background-color: rgb(17, 255, 77); color:rgb(255,255,255); border: 2px solid rgb(17, 255, 77);}");
            ui->pushButton_Connect->setText("已连接");
            ui->label_Addr_v->setText(targetIP.toString()+":8080");

        // 显示姿态数据
        }else{
            switch(datagram.data()[0]){
            // 显示姿态
            case 0x21:
                displayState((GES_message *)datagram.data());
                break;
            // 绘制曲线
            case 0x22:
                plotData((PT_message *)datagram.data());
                break;
            // 显示运行信息
            case 0x23:
                displayOpt((Opt_message *)datagram.data());
                break;
            // 显示参数
            case 0x11: case 0x12: case 0x13: case 0x14:
                displayParm((PID_message *)datagram.data());
                break;
            }
        }


    }
}

// 建立连接
void Widget::on_pushButton_Connect_clicked()
{
    QByteArray datagram = "?";
    UDP_Server->writeDatagram(datagram.data(), datagram.size(), QHostAddress::Broadcast, 8080);
}


// 发送其他消息
void Widget::on_pushButton_Send_clicked()
{
    QByteArray datagram = ui->textEdit_SBuff->toPlainText().toUtf8();
    UDP_Server->writeDatagram(datagram.data(), datagram.size(), targetIP, 8080);
//    ui->textEdit_SBuff->clear();
}

// 写入姿态PID
void Widget::on_pushButton_G_clicked()
{

    M_PID.fh = 0x11;
    M_PID.is =  ui->ratioBtn_isSave->isChecked();
    M_PID.Kp = ui->lineEdit_GKp->text().toFloat();
    M_PID.Ki = ui->lineEdit_GKi->text().toFloat();
    M_PID.ft = 0x0d;

    QByteArray datagram((char *)&M_PID, sizeof (M_PID));
    UDP_Server->writeDatagram(datagram.data(), datagram.size(), targetIP, targetPort);
}

// 写入速度PID
void Widget::on_pushButton_S_clicked()
{
    M_PID.fh = 0x12;
    M_PID.is =  ui->ratioBtn_isSave->isChecked();
    M_PID.Kp = ui->lineEdit_SKp->text().toFloat();
    M_PID.Ki = ui->lineEdit_SKi->text().toFloat();
    M_PID.Kd = ui->lineEdit_SKd->text().toFloat();
    M_PID.ft = 0x0d;

    QByteArray datagram((char *)&M_PID, sizeof (M_PID));
    UDP_Server->writeDatagram(datagram.data(), datagram.size(), targetIP, targetPort);
}

// 写入转向PID
void Widget::on_pushButton_T_clicked()
{
    M_PID.fh = 0x13;
    M_PID.is =  ui->ratioBtn_isSave->isChecked();
    M_PID.Kp = ui->lineEdit_TKp->text().toFloat();
    M_PID.Ki = ui->lineEdit_TKi->text().toFloat();
    M_PID.Kd = ui->lineEdit_TKd->text().toFloat();
    M_PID.ft = 0x0d;

    QByteArray datagram((char *)&M_PID, sizeof (M_PID));
    UDP_Server->writeDatagram(datagram.data(), datagram.size(), targetIP, targetPort);
}

// 写入直立PID
void Widget::on_pushButton_E_clicked()
{
    M_PID.fh = 0x14;
    M_PID.is =  ui->ratioBtn_isSave->isChecked();
    M_PID.Kp = ui->lineEdit_EKp->text().toFloat();
    M_PID.Ki = ui->lineEdit_EKi->text().toFloat();
    M_PID.Kd = ui->lineEdit_EKd->text().toFloat();
    M_PID.ft = 0x0d;

    QByteArray datagram((char *)&M_PID, sizeof (M_PID));
    UDP_Server->writeDatagram(datagram.data(), datagram.size(), targetIP, targetPort);
}

// 显示姿态
void Widget::displayState(GES_message *ges)
{
    ui->label_Roll_v->setText(QString::number(ges->roll, 'f', 2));
    ui->label_Pitch_v->setText(QString::number(ges->pitch, 'f', 2));
    ui->label_Yaw_v->setText(QString::number(ges->yaw, 'f', 2));
}


// 曲线绘制初始化
void Widget::plotInit(void)
{
    ui->customPlot->xAxis->setLabel("Samples");
    ui->customPlot->yAxis->setLabel("Value");
}

// 数据曲线绘制
void Widget::plotData(PT_message *pt)
{
    for(int i = 0; i < pt->ch; i++){
        if(ch[i].idx <= 100){
            if (ch[i].idx == 0){
                QPen pen;
                pen.setColor(ch[i].col);
                ui->customPlot->addGraph();
                ui->customPlot->graph(i)->setPen(pen);
                ui->customPlot->graph(i)->setScatterStyle(QCPScatterStyle::ssDisc);
                ui->customPlot->graph(i)->setName("ch"+QString::number(i));
            }

            ch[i].x.append(ch[i].idx);
            ch[i].y.append(pt->data[i]);
            ch[i].idx = ch[i].idx+1;
        }else{
            ch[i].y.removeFirst();
            ch[i].y.append(pt->data[i]);
        }
        ui->customPlot->graph(i)->setData(ch[i].x, ch[i].y);
        ui->customPlot->graph(i)->setVisible(ch[i].on);
    }
    ui->customPlot->replot();
    ui->customPlot->rescaleAxes(true);
    ui->customPlot->legend->setVisible(true);
}

void Widget::OptMode(int btnID)
{
//    qDebug() << QString::number(btnID);
    char data[3] = {0x41, 0x00, 0x0d};
    data[1] = btnID;
    QByteArray datagram(data, 3);
    UDP_Server->writeDatagram(datagram.data(), datagram.size(), targetIP, targetPort);
}

void Widget::displayParm(PID_message *pid_m)
{
    switch (pid_m->fh) {
    case 0x11:
        ui->lineEdit_GKp->setText(QString::number(pid_m->Kp, 'f', 2));
        ui->lineEdit_GKi->setText(QString::number(pid_m->Ki, 'f', 2));
        break;
    case 0x12:
        ui->lineEdit_SKp->setText(QString::number(pid_m->Kp, 'f', 2));
        ui->lineEdit_SKi->setText(QString::number(pid_m->Ki, 'f', 2));
        ui->lineEdit_SKd->setText(QString::number(pid_m->Kd, 'f', 2));
        break;
    case 0x13:
        ui->lineEdit_TKp->setText(QString::number(pid_m->Kp, 'f', 2));
        ui->lineEdit_TKi->setText(QString::number(pid_m->Ki, 'f', 2));
        ui->lineEdit_TKd->setText(QString::number(pid_m->Kd, 'f', 2));
        break;
    case 0x14:
        ui->lineEdit_EKp->setText(QString::number(pid_m->Kp, 'f', 2));
        ui->lineEdit_EKi->setText(QString::number(pid_m->Ki, 'f', 2));
        ui->lineEdit_EKd->setText(QString::number(pid_m->Kd, 'f', 2));
        break;
    }
}

void Widget::displayOpt(Opt_message *opt_m)
{
    ui->progressBar_eq->setValue(opt_m->eq);
    ui->label_angle->setText(QString::number(opt_m->pitch, 'f', 2));
    ui->label_speed->setText(QString::number(opt_m->speed, 'f', 1));
    ui->label_Roll_v->setText(QString::number(opt_m->roll, 'f', 2));
    ui->label_Pitch_v->setText(QString::number(opt_m->pitch, 'f', 2));
    ui->label_Yaw_v->setText(QString::number(opt_m->yaw, 'f', 2));
}

void Widget::sendCtrl()
{
    float x, y;
    Ctrl_message ctrl;
    ui->rocker_Ctrl->getNormRocker(&x, &y);
//    qDebug() << QString::number(x,'f',2);
//    qDebug() << QString::number(y,'f',2);
    ctrl.fh = 0xaa;
    ctrl.ft = 0x0d;
    ctrl.x = x;
    ctrl.y = y;

    QByteArray datagram((char *)&ctrl, sizeof(Ctrl_message));
    UDP_Server->writeDatagram(datagram.data(), datagram.size(), targetIP, targetPort);
}

void Widget::chSet(int btnID)
{
    ch[btnID].on = ui->buttonGroup_CH->button(btnID)->isChecked();
}
