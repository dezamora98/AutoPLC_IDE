#ifndef CONTROL_H
#define CONTROL_H

#include <QWidget>
#include <QtCharts/qchartview.h>
namespace Ui {
class control;
}

class control : public QWidget
{
    Q_OBJECT

public:
    explicit control(QWidget *parent = nullptr);
    ~control();

private:
    Ui::control *ui;
};

#endif // CONTROL_H
