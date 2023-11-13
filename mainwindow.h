#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>
#include <QFile>
#include <QtCharts/qchart.h>
#include <QtCharts/qchartview.h>
#include <QtCharts/QtChartsVersion>
#include <QCharRef>
#include <QWidget>
#include "usbtransfer.h"
#include "code_editor.h"
#include "ui_mainwindow.h"
#include "code_editor.h"
#include "control.h"
#include "monitor.h"
#include <algorithm>
#include <QTimer>
#include <windows.h>
#include <lmcons.h>
#include <QProcess>

#define ADDR_init 0x0000
#define ADDR_end 0x6800

namespace Ui {
class MainWindow;
}

class QProcess;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionAbrir_triggered();
    void on_actionNuevo_triggered();
    void on_actionGuardar_triggered();
    void on_actionGuardar_como_triggered();
    void on_actionCerrar_todo_triggered();
    void on_actionSalir_triggered();
    void on_actionCompilar_triggered();
    void on_actionGrabar_triggered();
    void on_actionSobre_AutoUSB_KIT_triggered();
    void on_actionSobre_AutoPLC_triggered();
    void on_actionAutoIDE_triggered();
    void on_tabWidget_tabCloseRequested(int index);
    void on_code_terminal_textChanged();
    void set_folder_pro();
    void on_actionNuevo_proyecto_triggered();
    void on_actionXC8_triggered();
    void on_actionAdrir_proyecto_triggered();
    void on_actionQt_triggered();

private:
    Ui::MainWindow *ui;
    usbtransfer AUTO_USB;
    QDir pro_dir;
    QMap<QString,shared_ptr<code_editor>> code_list;
    QProcess *m_Process;

};



#endif // MAINWINDOW_H
