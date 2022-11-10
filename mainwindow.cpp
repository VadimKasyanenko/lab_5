#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <qwt_plot.h>
#include <iostream>
#include <qwt_plot_grid.h>
#include <QFile>
#include <qwt_legend.h>
#include <qwt_plot_curve.h>
#include <qwt_symbol.h>
#include <qwt_plot_magnifier.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_picker.h>
#include <qwt_picker_machine.h>
#include <qwt_plot_dict.h>
#include <QFileDialog>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->widget->setCanvasBackground( Qt::white ); // цвет фона
    ui->widget->setAxisTitle(QwtPlot::yLeft, "Y");
    ui->widget->setAxisTitle(QwtPlot::xBottom, "X");
    ui->widget->setAxisScale(QwtPlot::xBottom, 0,20);
    ui->widget->setAxisScale(QwtPlot::yLeft,0,20);
    QwtPlotGrid *grid = new QwtPlotGrid();
    grid->setMajorPen(QPen( Qt::gray, 1 ));
    grid->attach( ui->widget );
    QwtPlotMagnifier *magnifier = new QwtPlotMagnifier(ui->widget->canvas());
    magnifier->setMouseButton(Qt::MidButton);
    QwtPlotPanner *d_panner = new QwtPlotPanner( ui->widget->canvas() );
    d_panner->setMouseButton( Qt::RightButton );
    QwtPlotPicker *d_picker = new QwtPlotPicker(
                               QwtPlot::xBottom, QwtPlot::yLeft,
                   QwtPlotPicker::CrossRubberBand,
                   QwtPicker::ActiveOnly,
                   ui->widget->canvas() );
    d_picker->setRubberBandPen( QColor( Qt::red ) );
    d_picker->setTrackerPen( QColor( Qt::black ) );
    d_picker->setStateMachine( new QwtPickerDragPointMachine() );
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::fill(QPointF beg,QPointF end)
{
    QwtPlotCurve *curve = new QwtPlotCurve();
    QPolygonF points(2);
    points[0] = beg;
    points[1] = end;
    curve->setSamples( points );
    vec.push_back(curve);
    vec.last()->attach( ui->widget );
}
void MainWindow::middle(QPointF beg,QPointF end)
{
    if (end.rx()-beg.rx()==0 && end.ry()-beg.ry()!=0)
    {
        if(abs(beg.ry()-end.ry())<0.1)
        {
            return;
        }
        if(beg.rx()>max_poly.rx() || beg.rx()<min_poly.rx() || (beg.ry()>max_poly.ry() && end.ry()>max_poly.ry()) || (beg.ry()<min_poly.ry() && end.ry()<min_poly.ry()))
        {
            return;
        }
        if((beg.rx()>=min_poly.rx()) && (beg.rx()<=max_poly.rx()) && (end.rx()>=min_poly.rx()) && (end.rx()<=max_poly.rx()) && (beg.ry()>=min_poly.ry()) && (beg.ry()<=max_poly.ry()) && (end.ry()>=min_poly.ry()) && (end.ry()<=max_poly.ry()))
        {
            QPolygonF points(2);
            points[0] = beg;
            points[1] = end;
            QwtPlotCurve *curve = new QwtPlotCurve();
            curve->setPen( Qt::blue, 1 );
            curve->setSamples( points );
            curve->attach( ui->widget );
            ui->widget->replot();
            return;
        }
        middle(beg,QPointF((beg.rx()+end.rx())/2.0,(beg.ry()+end.ry())/2.0));
        middle(QPointF((beg.rx()+end.rx())/2.0,(beg.ry()+end.ry())/2.0),end);
    }
    if(beg.rx()>end.rx())
    {
        int tmp;
        tmp = beg.rx();
        beg.setX(end.rx());
        end.setX(tmp);
        tmp = beg.ry();
        beg.setY(end.ry());
        end.setY(tmp);
    }
    double k,b;
    k=1.0*(end.ry()-beg.ry())/(end.rx()-beg.rx());
    b=end.ry()-k*end.rx();
   if(sqrt((beg.rx()-end.rx())*(beg.rx()-end.rx())+(beg.ry()-end.ry())*(beg.ry()-end.ry()))<0.1)
   {
       return;
   }
   if((beg.rx()<min_poly.rx() && ((k*min_poly.rx()+b)<min_poly.ry()||(k*min_poly.rx()+b)>max_poly.ry())) || (end.rx()>max_poly.rx() && ((k*max_poly.rx()+b)<min_poly.ry()||(k*max_poly.rx()+b)>max_poly.ry()))
           || (beg.ry()>max_poly.ry() && ((max_poly.ry()-b)/k<min_poly.rx() || (max_poly.ry()-b)/k>max_poly.rx())) || (beg.ry()<min_poly.ry() && ((min_poly.ry()-b)/k<min_poly.rx() || (min_poly.ry()-b)/k>max_poly.rx())))
   {
       return;
   }
   if((beg.rx()>=min_poly.rx()) && (beg.rx()<=max_poly.rx()) && (end.rx()>=min_poly.rx()) && (end.rx()<=max_poly.rx()) && (beg.ry()>=min_poly.ry()) && (beg.ry()<=max_poly.ry()) && (end.ry()>=min_poly.ry()) && (end.ry()<=max_poly.ry()))
   {
       QPolygonF points(2);
       points[0] = beg;
       points[1] = end;
       QwtPlotCurve *curve = new QwtPlotCurve();
       curve->setPen( Qt::blue, 1 );
       curve->setSamples( points );
       curve->attach( ui->widget );
       ui->widget->replot();
       return;
   }
   middle(beg,QPointF((beg.rx()+end.rx())/2.0,(beg.ry()+end.ry())/2.0));
   middle(QPointF((beg.rx()+end.rx())/2.0,(beg.ry()+end.ry())/2.0),end);
}
void MainWindow::Polygon(QPointF beg,QPointF end)
{
    ui->widget->insertLegend( new QwtLegend() );
    if(beg.rx()>end.rx())
    {
        int tmp;
        tmp = beg.rx();
        beg.setX(end.rx());
        end.setX(tmp);
        tmp = beg.ry();
        beg.setY(end.ry());
        end.setY(tmp);
    }
    double k,b;

    if (end.rx()-beg.rx()==0 && end.ry()-beg.ry()!=0)
    {
        QPolygonF points(2);
        if(beg.rx()<=max_poly.rx() && beg.rx()>=min_poly.rx())
        {
        if(beg.ry()>=min_poly.ry() && beg.ry()<=max_poly.ry())
        {
        points[0] = beg;
        if(end.ry()>max_poly.ry())
        {
         points[1] = QPointF(beg.rx(),max_poly.ry()) ;
        }
        else if(end.ry()<min_poly.ry())
        {
         points[1] = QPointF(beg.rx(),min_poly.ry()) ;
        }
        else
        {
            points[1] = end;
        }
        }
        else if(beg.ry()<min_poly.ry() && end.ry() <=max_poly.ry()  && end.ry() >=min_poly.ry())
        {
            points[0] = QPointF(beg.rx(),min_poly.ry());
            points[1] = end;
        }

        else if(beg.ry()<min_poly.ry() && end.ry() >max_poly.ry())
        {
            points[0] = QPointF(beg.rx(),min_poly.ry());
            points[1] = QPointF(beg.rx(),max_poly.ry());
        }
        else if(beg.ry()>max_poly.ry() && end.ry() >=min_poly.ry() && end.ry() <=max_poly.ry())
        {
            points[0] = QPointF(beg.rx(),max_poly.ry());
            points[1] = end;
        }
        else if(beg.ry()>max_poly.ry() && end.ry() <min_poly.ry())
        {
            points[0] = QPointF(beg.rx(),max_poly.ry());
            points[1] = QPointF(beg.rx(),min_poly.ry());
        }
        QwtPlotCurve *curve = new QwtPlotCurve();
        curve->setTitle( "Clipped line" );
        curve->setPen( Qt::blue, 1 );
        XX.push_back(points[0]);
        YY.push_back(points[1]);
        curve->setSamples( points );
        curve->attach( ui->widget );
        ui->widget->replot();
        return;
        }
        return;
    }

    k=1.0*(end.ry()-beg.ry())/(end.rx()-beg.rx());
    b=end.ry()-k*end.rx();
    if(beg.rx()>=min_poly.rx() && beg.rx()<=max_poly.rx() && end.rx()>=min_poly.rx() && end.rx()<=max_poly.rx() && beg.ry()>=min_poly.ry() && beg.ry()<=max_poly.ry() && end.ry()>=min_poly.ry() && end.ry()<=max_poly.ry())
    {
        QPolygonF points(2);
        points[0] = beg;
        points[1] = end;
        QwtPlotCurve *curve = new QwtPlotCurve();
        curve->setTitle( "Clipped line" );
        curve->setPen( Qt::blue, 1 );
        XX.push_back(points[0]);
        YY.push_back(points[1]);
        curve->setSamples( points );
        curve->attach( ui->widget );
        ui->widget->replot();
        return;
    }
    if(beg.rx()<=max_poly.rx() && beg.rx()>=min_poly.rx() && beg.ry()<=max_poly.ry() && beg.ry()>=min_poly.ry())
    {
        QPolygonF points(2);
        points[0] = beg;
        if(end.rx()<=max_poly.rx() && end.ry()>max_poly.ry())
        {
        points[1] = QPointF((max_poly.ry()-b)/k,max_poly.ry());
        }
        else if (end.rx()<=max_poly.rx() && end.ry()<min_poly.ry()) {
             points[1] = QPointF((min_poly.ry()-b)/k,min_poly.ry());
        }
        QwtPlotCurve *curve = new QwtPlotCurve();
        curve->setTitle( "Clipped line" );
        curve->setPen( Qt::blue, 1 );
        XX.push_back(points[0]);
        YY.push_back(points[1]);
        curve->setSamples( points );
        curve->attach( ui->widget );
        ui->widget->replot();
        return;
    }
    if ((beg.rx()<min_poly.rx()) && ((k*min_poly.rx()+b)>=min_poly.ry()) && ((k*min_poly.rx()+b)<=max_poly.ry()) && (min_poly.rx()<end.rx()))
    {
        QPolygonF points(2);
        points[0] =  QPointF(min_poly.rx(),(k*min_poly.rx()+b));
        if((end.rx()<=max_poly.rx()) && (end.ry()<=max_poly.ry()) && (end.ry()>=min_poly.ry()))
        {
        points[1] = end;
        }
        else if (((k*max_poly.rx()+b)<=max_poly.ry()) && ((k*max_poly.rx()+b)>=min_poly.ry()))
        {
         points[1] = QPointF(max_poly.rx(),k*max_poly.rx()+b);
        }
        else if (((max_poly.ry()-b)/k>=min_poly.rx()) && ((max_poly.ry()-b)/k<=max_poly.rx()))
        {
          points[1] = QPointF((max_poly.ry()-b)/k,max_poly.ry());
        }
        else if (((min_poly.ry()-b)/k>=min_poly.rx()) && ((min_poly.ry()-b)/k<=max_poly.rx()))
        {
         points[1] = QPointF((min_poly.ry()-b)/k,min_poly.ry());
        }
        QwtPlotCurve *curve = new QwtPlotCurve();
        curve->setTitle( "Clipped line" );
        curve->setPen( Qt::blue, 1 );
        XX.push_back(points[0]);
        YY.push_back(points[1]);
        curve->setSamples( points );
        curve->attach( ui->widget );
        ui->widget->replot();
        return;
    }
    if ((end.rx()>max_poly.rx()) && ((k*max_poly.rx()+b)>=min_poly.ry()) && ((k*max_poly.rx()+b)<=max_poly.ry()) && (max_poly.rx()>beg.rx()))
    {
        QPolygonF points(2);
        points[1] =  QPointF(max_poly.rx(),(k*max_poly.rx()+b));
        if(beg.rx()>=min_poly.rx() && beg.ry()<=max_poly.ry() && beg.ry()>=min_poly.ry())
        {
        points[0] = beg;
        }
        else if (((k*min_poly.rx()+b)<=max_poly.ry()) && ((k*min_poly.rx()+b)>=min_poly.ry()))
        {
         points[0] = QPointF(min_poly.rx(),k*min_poly.rx()+b);
        }
        else if (((max_poly.ry()-b)/k>=min_poly.rx()) && ((max_poly.ry()-b)/k<=max_poly.rx()))
        {
          points[0] = QPointF((max_poly.ry()-b)/k,max_poly.ry());
        }
        else if (((min_poly.ry()-b)/k>=min_poly.rx()) && ((min_poly.ry()-b)/k<=max_poly.rx()))
        {
         points[0] = QPointF((min_poly.ry()-b)/k,min_poly.ry());
        }
        QwtPlotCurve *curve = new QwtPlotCurve();
        curve->setTitle( "Clipped line" );
        curve->setPen( Qt::blue, 1 );
        XX.push_back(points[0]);
        YY.push_back(points[1]);
        curve->setSamples( points );
        curve->attach( ui->widget );
        ui->widget->replot();
        return;
    }
    if ((beg.ry()>max_poly.ry()) && ((max_poly.ry()-b)/k>=min_poly.rx()) && ((max_poly.ry()-b)/k<=max_poly.rx()) && (max_poly.ry()>end.ry()))
    {
        QPolygonF points(2);
        points[0] =  QPointF((max_poly.ry()-b)/k,max_poly.ry());
        if(end.rx()<=max_poly.rx() && end.ry()<=max_poly.ry() && end.ry()>=min_poly.ry())
        {
        points[1] = end;
        }
        else if (((k*max_poly.rx()+b)<=max_poly.ry()) && ((k*max_poly.rx()+b)>=min_poly.ry()))
        {
         points[1] = QPointF(max_poly.rx(),k*max_poly.rx()+b);
        }
        else if (((min_poly.ry()-b)/k>=min_poly.rx()) && ((min_poly.ry()-b)/k<=max_poly.rx()))
        {
          points[1] = QPointF((min_poly.ry()-b)/k,min_poly.ry());
        }

        QwtPlotCurve *curve = new QwtPlotCurve();
        curve->setTitle( "Clipped line" );
        curve->setPen( Qt::blue, 1 );
        XX.push_back(points[0]);
        YY.push_back(points[1]);
        curve->setSamples( points );
        curve->attach( ui->widget );
        ui->widget->replot();
        return;
    }
    if ((beg.ry()<min_poly.ry()) && ((min_poly.ry()-b)/k>=min_poly.rx()) && ((min_poly.ry()-b)/k<=max_poly.rx()) && (min_poly.ry()<end.ry()))
    {
        QPolygonF points(2);
        points[0] = QPointF((min_poly.ry()-b)/k,min_poly.ry());
        if(end.rx()<=max_poly.rx() && end.ry()<=max_poly.ry() && end.ry()>=min_poly.ry())
        {
        points[1] = end;
        }
        else    if (((k*max_poly.rx()+b)<=max_poly.ry()) && ((k*max_poly.rx()+b)>=min_poly.ry()))
        {
         points[1] = QPointF(max_poly.rx(),k*max_poly.rx()+b);
        }
        else if (((max_poly.ry()-b)/k>=min_poly.rx()) && ((max_poly.ry()-b)/k<=max_poly.rx()))
        {
          points[1] = QPointF((max_poly.ry()-b)/k,max_poly.ry());
        }
        QwtPlotCurve *curve = new QwtPlotCurve();
        curve->setTitle( "Clipped line" );
        curve->setPen( Qt::blue, 1 );
        XX.push_back(points[0]);
        YY.push_back(points[1]);
        curve->setSamples( points );
        curve->attach( ui->widget );
        ui->widget->replot();
        return;
    }
}
void MainWindow::on_pushButton_clicked()
{
    int size;
    double x1,y1,x2,y2;
    QFile file(path);
    file.open(QIODevice::ReadOnly);
    QTextStream fin(&file);
    fin >> size;
    //vec.resize(size);
    for(int i=0;i<size;i++)
    {
        fin >> x1>>y1>>x2>>y2;
        point_beg.push_back(QPointF(x1,y1));
        point_end.push_back(QPointF(x2,y2));
        fill(point_beg.last(),point_end.last());
    }
    fin >> x1>>y1>>x2>>y2;
    min_poly.setX(x1);
    min_poly.setY(y1);
    max_poly.setX(x2);
    max_poly.setY(y2);
    QPolygonF points ;
    points << QPointF( x1, y1 ) // координаты x, y
            << QPointF( x1, y2 ) << QPointF( x2, y2 )
            << QPointF( x2, y1 ) << QPointF( x1, y1 );
    QwtPlotCurve *curve = new QwtPlotCurve();
    curve->setTitle( "Polygon" );
    curve->setPen( Qt::red, 1 );
    curve->setSamples( points );
    curve->attach( ui->widget );
    for(int i=0;i<size;i++)
    {
        middle(point_beg[i],point_end[i]);
    }
    ui->widget->replot();
    file.close();
}

void MainWindow::on_pushButton_2_clicked()
{
    int size;
    double x1,y1,x2,y2;
    QFile file(path);
    file.open(QIODevice::ReadOnly);
    QTextStream fin(&file);
    fin >> size;
    //vec.resize(size);
    for(int i=0;i<size;i++)
    {
        fin >> x1>>y1>>x2>>y2;
        point_beg.push_back(QPointF(x1,y1));
        point_end.push_back(QPointF(x2,y2));
        fill(point_beg.last(),point_end.last());
    }
    fin >> x1>>y1>>x2>>y2;
    min_poly.setX(x1);
    min_poly.setY(y1);
    max_poly.setX(x2);
    max_poly.setY(y2);
    QPolygonF points ;
    points << QPointF( x1, y1 )
            << QPointF( x1, y2 ) << QPointF( x2, y2 )
            << QPointF( x2, y1 ) << QPointF( x1, y1 );
    QwtPlotCurve *curve = new QwtPlotCurve();
    curve->setTitle( "Polygon" );
    curve->setPen( Qt::red, 1 );
    curve->setSamples( points );
    curve->attach( ui->widget );
    for(int i=0;i<size;i++)
    {
        Polygon(point_beg[i],point_end[i]);
    }
    for(int i=0;i<XX.size();i++)
    {
        qDebug()<< XX[i] << YY[i] <<endl;
    }
    ui->widget->replot();
    file.close();
}

void MainWindow::on_pushButton_3_clicked()
{
    ui->widget->QwtPlotDict::detachItems(
         QwtPlotItem::Rtti_PlotCurve,
          true );
}

void MainWindow::on_pushButton_4_clicked()
{
    QString arr[]={".txt"};
    QString arch;
         QString fileName = QFileDialog ::getOpenFileName(
            this,
            tr("open a file."),
            "D:/",
            tr("files(*txt)"));
       path = fileName;
}
