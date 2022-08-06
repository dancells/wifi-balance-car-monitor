#ifndef ROCKER_H
#define ROCKER_H

#include <QWidget>
#include <QPainter>
#include <QDrag>
#include <QMouseEvent>
#include <QtMath>
#include <QTimer>
#include <QDebug>

class rocker: public QWidget
{
    Q_OBJECT
public:
public:
    rocker(QWidget *parent = 0);
    ~rocker();
    void getNormRocker(float *x, float *y);

signals:
    void rockerChanged();

protected:
    void paintEvent(QPaintEvent *event)override;
    void mouseMoveEvent(QMouseEvent *event)override;
    void mouseReleaseEvent(QMouseEvent *event)override;
    void mousePressEvent(QMouseEvent *event)override;
    // void resizeEvent(QResizeEvent *event)override;
private:
    // 鼠标位置
    int mouseX;
    int mouseY;
    // 摇杆位置(x,y)，半径R
    int rockerX;
    int rockerY;
    int rockerR;
    // 底盘位置(x,y)，半径R
    int padX;
    int padY;
    int padR;

    //两圆圆心距离
    double handPadDis;
    // 鼠标是否按下
    bool mousePressed;
    // 定时器
    QTimer *tim;
private:
    double Pointdis(int x1,int y1,int x2,int y2);//两点距离
    void updataRocker(void);
//    int getKeyNum();
};
#endif
