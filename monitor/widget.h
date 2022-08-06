#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QtNetwork>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE


typedef  struct{
    char fh;
    char is;
    float Kp;
    float Ki;
    float Kd;
    char ft;
}PID_message;

typedef  struct{
    char fh;
    float roll;
    float pitch;
    float yaw;
    char ft;
}GES_message;

typedef struct{
    char fh;
    char ch;
    float data[5];
    char ft;
}PT_message;

typedef struct{
    char fh;
    float x;
    float y;
    char ft;
}Ctrl_message;

typedef struct{
    char fh;
    char eq;
    float speed;
    float roll;
    float pitch;
    float yaw;
    char ft;
}Opt_message;



typedef struct{
    int idx;
    bool on;
    QColor col;
    QVector<double> x;
    QVector<double> y;
}CH_info;


class Widget : public QWidget
{
    Q_OBJECT


public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    QString getIP(QHostAddress *HostIP);

private slots:
    void on_pushButton_Connect_clicked();
    void processPendingDatagram();

    void on_pushButton_Send_clicked();

    void on_pushButton_G_clicked();

    void on_pushButton_S_clicked();

    void on_pushButton_T_clicked();

    void on_pushButton_E_clicked();

    void displayState(GES_message *ges);

    void plotInit(void);
    void plotData(PT_message *pt);
    void OptMode(int btnID);
    void displayParm(PID_message *pid_m);
    void displayOpt(Opt_message *opt_m);
    void sendCtrl(void);
    void chSet(int btnID);

private:
    Ui::Widget *ui;
    QUdpSocket *UDP_Server;
    QHostAddress targetIP;
    quint16 targetPort;
    PID_message M_PID;


    CH_info ch[5]={
        {0, true, QColor(255,0,0),(QVector<double>)0, (QVector<double>)0},
        {0, true, QColor(255,153,0),(QVector<double>)0, (QVector<double>)0},
        {0, true, QColor(0,255,0),(QVector<double>)0, (QVector<double>)0},
        {0, true, QColor(0,0,255),(QVector<double>)0, (QVector<double>)0},
        {0, true, QColor(255,0,255),(QVector<double>)0, (QVector<double>)0}
                  };
};
#endif // WIDGET_H
