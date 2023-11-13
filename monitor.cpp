#include "monitor.h"
#include "ui_monitor.h"

monitor::monitor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::monitor)
{
    ui->setupUi(this);
    grafica= new QChart();
    generador_de_linea = new QSplineSeries();
    grafica->addSeries(generador_de_linea);

    maxX = new QDateTime;
    maxY = new double(30);

    QDateTimeAxis *xAxis = new QDateTimeAxis;
    xAxis->setMin(QDateTime::currentDateTime());
    xAxis->setMax(maxX->QDateTime::currentDateTime().addSecs(10));

    QValueAxis *yAxis = new QValueAxis;
    yAxis->setMax((*maxY) * 1.2);

    grafica->setAxisX(xAxis, generador_de_linea);
    grafica->setAxisY(yAxis, generador_de_linea);

    grafica->setTitle(tr("Entrada x"));

    ui->widgetChartViewer->setChart(grafica);
    ui->widgetChartViewer->setRenderHint(QPainter::Antialiasing, true);

}

monitor::~monitor()
{
    delete ui;
}

void monitor::on_comboBox_activated(const QString &arg1)
{

}
