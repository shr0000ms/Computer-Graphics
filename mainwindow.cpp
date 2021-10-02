#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPixmap>
#include <QImage>
#include <iostream>
#include <QMouseEvent>
#include <QPainter>
#include <QPaintDevice>
#include <QPoint>

int gridsize = 1;

QImage img=QImage(600,600,QImage::Format_RGB888);

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->x_axis->hide();
    ui->y_axis->hide();
    //img=QImage(ui->frame->width(),ui->frame->height(),QImage::Format_RGB888);
    connect(ui->frame,SIGNAL(Mouse_Pos()),this,SLOT(Mouse_Pressed()));
    connect(ui->frame,SIGNAL(sendMousePosition(QPoint&)),this,SLOT(showMousePosition(QPoint&)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::point(int x,int y, int r=255, int g=255, int b=0)
{
    if(gridsize == 1) img.setPixel(x,y,qRgb(r,g,b));
    else {
        x = (x/gridsize)*gridsize;
        y = (y/gridsize)*gridsize;
        for(int i=x+1;i <  x + gridsize;i++) {
            for(int j=y+1;j < y + gridsize;j++) {
                img.setPixel(i,j,qRgb(r,g,b));
            }
        }
    }
    ui->frame->setPixmap(QPixmap::fromImage(img));

}

void MainWindow::showMousePosition(QPoint &pos)
{
    int x = (pos.x() - ui->frame->width()/2)/gridsize;
    int y = (ui->frame->height()/2 - pos.y())/gridsize;
    ui->mouse_movement->setText(" X : "+QString::number(x)+", Y : "+QString::number(y));
}
void MainWindow::Mouse_Pressed()
{
    int x = (ui->frame->x - ui->frame->width()/2)/gridsize;
    int y = (ui->frame->height()/2 - ui->frame->y)/gridsize;
    ui->mouse_pressed->setText(" X : "+QString::number(x)+", Y : "+QString::number(y));
    point(ui->frame->x,ui->frame->y);
}

void MainWindow::on_show_axes_clicked()
{

    if(ui->show_axes->isChecked())
    {
        for(int j=0;j<=ui->frame->width();j+=gridsize)
        {
            point(img.width()/2,j);
        }
        for(int i=0;i<=ui->frame->height();i+=gridsize)
        {
            point(i,img.height()/2);
        }
    }
    else{
        on_setgridbutton_clicked();
    }
}

void MainWindow::on_resetbutton_clicked()
{
    for(int j=0;j<img.height();j++)
    {
        for(int i=0;i<img.width();i++)
        {
            img.setPixel(i,j,qRgb(0,0,0));
        }
    }
    ui->frame->setPixmap(QPixmap::fromImage(img));
}

void MainWindow::on_setgridbutton_clicked()
{
    on_resetbutton_clicked();
    gridsize=ui->gridspinbox->value();
    if(gridsize>1)
    {
        for(int i=0;i<img.width();i+=gridsize)
        {
            for(int j=0;j<img.height();j++)
            {
                img.setPixel(i,j,qRgb(255,0,0));
                img.setPixel(j,i,qRgb(255,0,0));
            }
        }
        ui->frame->setPixmap(QPixmap::fromImage(img));
    }
}

void MainWindow::on_set_point1_clicked()
{
    p1.setX(ui->frame->x);
    p1.setY(ui->frame->y);
}

void MainWindow::on_set_point2_clicked()
{
    p2.setX(ui->frame->x);
    p2.setY(ui->frame->y);
}

void MainWindow::on_DDALine_clicked()
{
    DDAline(255,255,0);
}

void MainWindow::DDAline(int r, int g, int b) {
    double x1 = p1.x()/gridsize;
    double y1 = p1.y()/gridsize;
    double x2 = p2.x()/gridsize;
    double y2 = p2.y()/gridsize;
    double xinc, yinc, step;
    double slope = fabs(y2-y1)/fabs(x2-x1);
    if(slope  <= 1.00) {
        xinc = 1;
        yinc = slope;
        step = fabs(x2 - x1);
    } else {
        xinc = 1/slope;
        yinc = 1;
        step = fabs(y2 - y1);
    }
    if(x1 > x2) xinc *= -1;
    if(y1 > y2) yinc *= -1;
    double x = x1*gridsize + gridsize/2;
    double y = y1*gridsize + gridsize/2;
    for(int i=0;i<=step;i++) {
        point(x,y,r,g,b);
        x += xinc * gridsize;
        y += yinc * gridsize;
    }
}

void swap(int &a, int &b) {
    int temp = b;
    b = a;
    a = temp;
}

void MainWindow::on_bresenhamLine_clicked()
{
    int x1 = p1.x()/gridsize;
    int y1 = p1.y()/gridsize;
    int x2 = p2.x()/gridsize;
    int y2 = p2.y()/gridsize;
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);

    int xinc = (x1 > x2 ? -1 : 1);
    int yinc = (y1 > y2 ? -1 : 1);
    bool flag = 1;
    int x = x1*gridsize + gridsize/2;
    int y = y1*gridsize + gridsize/2;
    if(dy > dx) { // if slope > 1, then swap
        swap(dx,dy);
        swap(x,y);
        swap(xinc,yinc);
        flag = 0;
    }
    int decision = 2*dy - dx;
    int step = dx;
    for(int i=0;i<=step;i++) {
        if(flag) point(x,y);
        else point(y,x);
        if(decision < 0) {
            x += xinc*gridsize;
            decision += 2*dy;
        } else {
            x += xinc*gridsize;
            y += yinc*gridsize;
            decision += 2*dy - 2*dx;
        }
    }
}
