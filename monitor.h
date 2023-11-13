#ifndef MONITOR_H
#define MONITOR_H

#include <QWidget>
#include <QtCharts/qchart.h>
#include <QtCharts/qchartview.h>
#include <QCharRef>
#include <QChartView>
#include <QWidget>

#include <QMainWindow>
#include <QTimer>
#include <QTime>
#include <QMessageBox>
#include <QSplineSeries>
#include <memory>

#include <QDateTimeAxis>
#include <QValueAxis>
#include <QLegend>
#include <QLegendMarker>


using namespace QtCharts;

namespace Ui {
class monitor;
}

class monitor : public QWidget
{
    Q_OBJECT

public:
    explicit monitor(QWidget *parent = nullptr);
    ~monitor();

private slots:
    void on_comboBox_activated(const QString &arg1);

private:
    Ui::monitor *ui;
    QChart *grafica;
    QSplineSeries *generador_de_linea;
    QDateTime *maxX;
    double  *maxY;

};

#endif // MONITOR_H
