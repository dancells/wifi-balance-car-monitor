#include "rocker.h"

#define EMIT_PERIOD 100

rocker::rocker(QWidget *parent)
    : QWidget(parent)
{
    resize(200,200);

    // 底盘属性
    padR = qMin(width(), height()) / 2;
    padX = padR;
    padY = padR;
    // 摇杆属性
    rockerR = padR / 4;
    rockerX = padX;
    rockerY = padY;


    // 鼠标按下，定时发射rocker变化信号
    tim = new QTimer(this);
    connect(tim, &QTimer::timeout, this, [=]{emit rockerChanged();});
}

rocker::~rocker()
{

}

// 两点距离
double rocker::Pointdis(int x1,int y1,int x2,int y2)
{
    return sqrt((double)((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2)));
}

void rocker::updataRocker()
{
    // 计算rocker位置
    int rockerMaxR = padR - rockerR;
    handPadDis = Pointdis(padR, padR, mouseX, mouseY);
    if (handPadDis <= rockerMaxR) {
        rockerX = mouseX;
        rockerY = mouseY;
    } else {
        rockerX = (int)(rockerMaxR*(mouseX - padX)/handPadDis + padX);
        rockerY = (int)(rockerMaxR*(mouseY - padY)/handPadDis + padY);
    }
}


// 鼠标按下，打开定时器
void rocker::mousePressEvent(QMouseEvent* event)
{
    // 开启定时器
    tim->start(EMIT_PERIOD);

    // 重绘窗口
    mouseX = event->pos().x();
    mouseY = event->pos().y();
    updataRocker();
    update();
}

// 鼠标松开，关闭定时器，复位rocker
void rocker::mouseReleaseEvent(QMouseEvent* event)
{
    Q_UNUSED(event);

    // 重绘窗口
    mouseX = padX;
    mouseY = padY;
    updataRocker();
    update();

    // 关闭定时器
    tim->stop();
    emit rockerChanged();
}


// 鼠标移动
void rocker::mouseMoveEvent(QMouseEvent* event)
{
    static bool r=false;
    // 更新鼠标位置
    mouseX = event->pos().x();
    mouseY = event->pos().y();
    updataRocker();

    if(r==true){
        update();
        r=false;
    } else{
        r=true;
    }
}








// 绘图
void rocker::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    //自绘底盘
    painter.save();
    QRadialGradient RadialGradient(padR, padR, padR, padR, padR);//圆心2，半径1，焦点2
    RadialGradient.setColorAt(0, QColor(0, 255, 0, 255));//渐变
    RadialGradient.setColorAt(1, QColor(204, 255, 204, 190));//渐变
    painter.setBrush(RadialGradient);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(QPoint(padR, padR), padR, padR);//大圆盘
    painter.restore();

    //自绘摇杆
    painter.setPen(Qt::NoPen);
    QColor rockerColor(255, 143, 68, 255);
    painter.setBrush(rockerColor);
    painter.drawEllipse(QPoint(rockerX, rockerY), rockerR, rockerR);
}



void rocker::getNormRocker(float *x, float *y)
{
    float rockerMaxR = padR - rockerR;

    *x = (rockerX - padX) / rockerMaxR;
    *y = -(rockerY - padY) / rockerMaxR;
}
