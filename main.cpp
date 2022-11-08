#include "mainwindow.h"
#include <qwt_plot.h>
#include <QApplication>
QPointF min_poly(4,4),max_poly(10,10);
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
   MainWindow w;
   w.show();
   return a.exec();

}
