#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <qwt_plot_curve.h>
#include <QMainWindow>
#include <string>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void fill(QPointF beg,QPointF end);
    void middle(QPointF beg,QPointF end);
    void Polygon(QPointF beg,QPointF end);
    QVector<QwtPlotCurve*> vec;
    QVector<QPointF> point_beg;
    QVector<QPointF> point_end;
     QVector<QPointF> XX;
     QVector<QPointF> YY;
    QPointF min_poly,max_poly;
     QString path;
private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
