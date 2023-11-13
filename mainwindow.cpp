#include "mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    QIcon tab_icon;
    tab_icon.addFile("/Iconos/Iconos/icons8-online-coding-64.png");
    ui->setupUi(this);

    m_Process = new QProcess(this);

    connect(m_Process,&QProcess::readyReadStandardOutput,[&]()
    {
        auto data = m_Process->readAllStandardOutput();
        ui->code_terminal->append(data);
        ui->code_terminal->append(QString("\nTerminal@AutoIDE:~$ "));
        ui->code_terminal->moveCursor(QTextCursor::End);
    });
    connect(m_Process,&QProcess::readyReadStandardError,[&]()
    {
        auto data = m_Process->readAllStandardError();
        ui->code_terminal->append(data);
        ui->code_terminal->append(QString("\nTerminal@AutoIDE:~$ "));
        ui->code_terminal->moveCursor(QTextCursor::End);
    });


    ui->tabWidget->clear();

    ui->code_terminal->setPalette(QColor(0,0,0));

    ui->code_terminal->setTextColor(QColor(255,255,255));

    ui->code_terminal->setText(QString("Terminal@AutoIDE:~$ "));
    ui->statusBar->showMessage(QString("Desconectado"));
    pro_dir.setPath(QDir::home().dirName());
    pro_dir.setNameFilters(QStringList()<<"*.c"<<"*.h");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionAbrir_triggered()
{
    QFile code;
    QTextStream io;
    QString code_name = QFileDialog::getOpenFileName(this,"Abrir",pro_dir.path()+"/");
    if(code_name.isEmpty()) return;

    code.setFileName(code_name);
    code.open(QIODevice::ReadOnly | QIODevice::Text);
    if(!code.isOpen())
    {
        QMessageBox::critical(this,"ERROR",code.errorString());
        return;
    }

    code_list.insert(code_name,make_shared<code_editor>());
    io.setDevice(&code);
    code_list.find(code_name).value().get()->get_text_edit()->setPlainText(io.readAll());
    code.close();
    ui->tabWidget->addTab(code_list.find(code_name).value().get(),
                          code_name.mid(code_name.lastIndexOf("/")+1,code_name.size()));

}

void MainWindow::on_actionNuevo_triggered()
{
    QString name_code;
    QString name_file;
    QMessageBox x;

    if(pro_dir==QDir::home().dirName())
    {
        x.warning(this,"Error pro","Debe crear primeramente un proyecto");
        return;
    }

    while(true)
    {
        name_code = QFileDialog::getSaveFileName(this,"Abrir",pro_dir.path()+"/","C (*.c,*.h)");
        if(name_code.isEmpty()) return;
        name_file = name_code.mid(name_code.lastIndexOf("/")+1,name_code.size());
        if(name_file.contains(" "))
        {
            x.warning(this,"Error dir","la dirección no puede contener espacios");
            continue;
        }
        break;
    }

    QFile code_file;
    QTextStream io;

    code_file.setFileName(name_code);
    code_file.open(QIODevice::WriteOnly | QIODevice::Text);
    if(!code_file.isOpen())
    {
        QMessageBox::critical(this,"ERROR",code_file.errorString());
        return;
    }
    code_file.flush();
    code_file.close();

    code_list.insert(name_code,make_shared<code_editor>());
    ui->tabWidget->addTab(code_list.find(name_code).value().get(),name_file);
}

void MainWindow::on_actionGuardar_triggered()
{
    QFile code_s; //fichero
    QTextStream io;

    for(auto code_s_name: code_list.keys())
    {
        code_s.setFileName(code_s_name);
        code_s.open(QIODevice::WriteOnly | QIODevice::Text);
        if(!code_s.isOpen())
        {
            QMessageBox::critical(this,"ERROR",code_s.errorString());
            return;
        }

        io.setDevice(&code_s);
        io << code_list.find(code_s_name).value().get()->get_text_edit()->toPlainText();
        code_s.flush();
        code_s.close();
    }
}

void MainWindow::on_actionGuardar_como_triggered()
{
    if(ui->tabWidget->count()==0) return;
    QFile code_s; //fichero
    QTextStream io;
    QString code_s_name = ui->tabWidget->tabWhatsThis(ui->tabWidget->tabPosition()); //nombre del fichero

    for(auto i: code_list.keys())
    {
        if(i.contains(code_s_name))
        {
            code_s_name =i;
            break;
        }
    }
    code_s.setFileName(QFileDialog::getSaveFileName(this,"Guardar como",(pro_dir.path()+"/"),"C (*.c,*.h)"));
    code_s.open(QIODevice::WriteOnly | QIODevice::Text);
    if(!code_s.isOpen())
    {
        QMessageBox::critical(this,"ERROR",code_s.errorString());
        return;
    }

    io.setDevice(&code_s);
    io << code_list.find(code_s_name).value().get()->get_text_edit()->toPlainText();
    code_s.flush();
    code_s.close();
}

void MainWindow::on_actionCerrar_todo_triggered()
{
    pro_dir.home();
    ui->tabWidget->clear();

}

void MainWindow::on_actionSalir_triggered()
{
    close();
}

void MainWindow::on_actionCompilar_triggered()
{
    if(pro_dir.isEmpty())
    {
        QMessageBox::critical(this,"error de proyecto","El proyecto no existe");
    }

    QStringList arg;
    arg <<  "-mcpu=18f4550"
         <<  "--outdir=" + QString(34) + pro_dir.path() + "/comp/" + QString(34)
          <<  "-O2"
           <<  "-maddrqual=request"
            <<  "-std=c90"
             <<  "-gdwarf-3"
              <<  "-Wl,--data-init -mno-keep-startup"
               //<<  "-mno-download"
                <<  "-mdefault-config-bits"
                 <<  "-mc90lib"
                  <<  "-fshort-double"
                   <<  "-fshort-float"
                    <<  "-memi=wordwrite"
                     <<  "-ginhx032"
                      <<  "-Wa,-a="+code_list.keys().first()
                       <<  "-msummary=-psect,-class,+mem,-hex,-file"
                        <<  "-mrom=0-6800";

    for(auto i:pro_dir.entryList(QStringList()<<"*.c"))
    {
        arg<<pro_dir.path() + "/" + i;
    }

    ui->statusBar->showMessage(QString("Compilando"));
    m_Process->setArguments(arg);
    m_Process->setProgram("xc8-cc");
    m_Process->start();

}

void MainWindow::on_actionGrabar_triggered()
{
    m_Process->waitForFinished();
    hexcode x(make_pair(0x000,0x6800),0x6800);
    QDir y(pro_dir.path()+"/comp/");
    QString hex_dir;

    ui->statusBar->showMessage(QString("Detectando fichero .hex"));

    int count=static_cast<int>(y.count());
    for(int i=0;i<count;++i)
    {
        if(y[i].contains(".hex"))
        {
            hex_dir = pro_dir.path()+"/comp/"+y[i];
            break;
        }
    }
    if(y.isEmpty())
    on_actionCompilar_triggered();
    m_Process->waitForFinished();

    for(int i=0;i<count;++i)
    {
        if(y[i].contains(".hex"))
        {
            hex_dir = pro_dir.path()+"/comp/"+y[i];
            break;
        }
    }

    if(!x.open(hex_dir) || y.isEmpty())
    {
        QMessageBox::critical(this,"HEX error","Fichero hex corrupto");
        return;
    }

    ui->statusBar->showMessage(QString("Conectando"));

    for(int i=0;i<10;++i)
    {
        if(AUTO_USB.usb_init_com())
        {
            QMessageBox::information(this,"USB_conect","AutoUSB-KIT v6 \n Conectado");
            i=100;
        }
        else if(i==9)
        {
            QMessageBox::warning(this,"USB_conect","Dispositivo no detectado");
            return;
        }
        else
        {
            QMessageBox::information(this,"USB_conect","Conecte el AutoUSB-KIT");
        }
    }

    ui->statusBar->showMessage(QString("Conectado"));

    if(AUTO_USB.init_MODE(BOOT_MODE,INIT))
    {
        ui->code_terminal->append(QString("\nModo bootloader iniciado"));
        ui->code_terminal->moveCursor(QTextCursor::End);

    }else
    {
        QMessageBox::critical(this,"boot","Error al iniciar el modo bootloader");
        ui->code_terminal->append(QString("\nError al iniciar el modo bootloader"));
        ui->code_terminal->append(QString("\nTerminal@AutoIDE:~$ "));
        ui->code_terminal->moveCursor(QTextCursor::End);
        return;
    }

    ui->statusBar->showMessage(QString("Modo bootloader"));

    // proceso de grabado
    QMessageBox::warning(this,"Grabar","Se perderán todos los datos del AutoUSB-KIT");


    if(AUTO_USB.erase_flash())
    {
        ui->code_terminal->append(QString("\nMemoria borrada"));
        ui->code_terminal->moveCursor(QTextCursor::End);
    }
    else
    {
        QMessageBox::critical(this,"boot","Error al borrar la memoria");
        ui->code_terminal->append(QString("\nError al borrar la memoria"));
        ui->code_terminal->append(QString("\nTerminal@AutoIDE:~$ "));
        ui->code_terminal->moveCursor(QTextCursor::End);
        return;
    }

    ui->statusBar->showMessage(QString("Memoria borrada"));

    if(AUTO_USB.write_PACK(x.get_list_pack32()))
    {
        ui->code_terminal->append(QString("\nPrograma copiado"));
        ui->code_terminal->moveCursor(QTextCursor::End);
    }
    else
    {
        QMessageBox::critical(this,"boot","Error al copiar el programa");
        ui->code_terminal->append(QString("\nError al copiar el programa"));
        ui->code_terminal->append(QString("\nTerminal@AutoIDE:~$ "));
        ui->code_terminal->moveCursor(QTextCursor::End);
        return;
    }

    ui->statusBar->showMessage(QString("Dispositivo grabado correctamente"));

    QMessageBox::information(this,"Grabar","Dispositivo grabado correctamente");

    AUTO_USB.init_MODE(APP_MODE,INIT);
    ui->code_terminal->append(QString("\nModo aplicación iniciado"));
    ui->code_terminal->append(QString("\nTerminal@AutoIDE:~$ "));
    ui->code_terminal->moveCursor(QTextCursor::End);

    AUTO_USB.~usbtransfer();

    ui->statusBar->showMessage(QString("Modo aplicación iniciado"));
}

void MainWindow::on_tabWidget_tabCloseRequested(int index)
{
    on_actionGuardar_triggered();
    for(auto i: code_list.keys())
    {
        if(i.contains(ui->tabWidget->tabText(index)))
        {
            QString temp = i.mid(0,i.lastIndexOf("/")-1);
            if(pro_dir==temp) break;
            else code_list.remove(i);
            break;
        }
    }
    ui->tabWidget->removeTab(index);
}

void MainWindow::on_code_terminal_textChanged()
{
    /* si es un enter ver comandos*/
}

void MainWindow::set_folder_pro()
{
    QDir x;
    pro_dir = QFileDialog::getSaveFileName(this,tr("Carpeta de proyecto"),"C:/Users/Public/Documents",tr("AutoIDEpro (*.Apro)"));
    x.mkpath(pro_dir.path()+"/comp/");
}

void MainWindow::on_actionNuevo_proyecto_triggered()
{
    set_folder_pro();

    QString name_code = pro_dir.path()+"/main.c";
    QFile code_file;
    QTextStream io;

    code_file.setFileName(name_code);
    code_file.open(QIODevice::WriteOnly | QIODevice::Text);
    if(!code_file.isOpen())
    {
        QMessageBox::critical(this,"ERROR",code_file.errorString());
        return;
    }
    code_file.flush();
    code_file.close();

    code_list.insert(name_code,make_shared<code_editor>());
    ui->tabWidget->addTab(code_list.find(name_code).value().get(),"main.c");

}

void MainWindow::on_actionAdrir_proyecto_triggered()
{
    pro_dir = QFileDialog::getExistingDirectory(this,"Carpeta de proyecto");
    for(QString j:pro_dir.entryList(QStringList()<<"*.c"<<"*.h"))
    {
        QFile code;
        QString code_name = pro_dir.path()+"/"+j;
        QTextStream io;

        code.setFileName(code_name);
        code.open(QIODevice::ReadOnly | QIODevice::Text);
        if(!code.isOpen())
        {
            QMessageBox::critical(this,"ERROR",code.errorString());
            pro_dir.home();
            ui->tabWidget->clear();
            return;
        }

        code_list.insert(code_name,make_shared<code_editor>());
        io.setDevice(&code);
        code_list.find(code_name).value().get()->get_text_edit()->setPlainText(io.readAll());
        code.close();
        ui->tabWidget->addTab(code_list.find(code_name).value().get(),
                              code_name.mid(code_name.lastIndexOf("/")+1,code_name.size()));
    }
}

void MainWindow::on_actionXC8_triggered()
{
    m_Process->setArguments(QStringList("--ver"));
    m_Process->start("xc8");
    m_Process->waitForReadyRead();
    ui->code_terminal->append(QString("\nTerminal@AutoIDE:~$ "));
    ui->code_terminal->moveCursor(QTextCursor::End);
}

void MainWindow::on_actionSobre_AutoUSB_KIT_triggered()
{
    QMessageBox::about(this,"AutoUSB-KIT",
                       "Esta placa de desarrollo fue creada en\n"
                       "la Universidad Tecnológica de la Habana\n"
                       "José Antonio Echeverría (CUJAE), basada\n"
                       "en las placas de hardware libre Pingüino.\n"
                       "Actualmente se encuentra en su versión 6.0.\n"
                       "Autor: Daniel Enrique Zamora Sifredo\n"
                       "fecha: 1/10/2021\n"
                       "(C) AutoUSB-KIT");
}

void MainWindow::on_actionSobre_AutoPLC_triggered()
{
    QMessageBox::about(this,"AutoPLC",
                       "Es un módulo para la placa AutoUSB-KIT v6,\n"
                       "que imita las características de un PLC para\n"
                       "para aplicaciones de bajo y medio nivel de \n"
                       "complejidad. Actualmente se encuentra en su\n"
                       "versión beta.\n"
                       "Autor: Daniel Enrique Zamora Sifredo\n"
                       "fecha: 1/10/2021\n"
                       "(C) AutoPLC");
}

void MainWindow::on_actionAutoIDE_triggered()
{
    QMessageBox::about(this,"AutoIDE",
                       "Es un entorno de desarrollo para AutoUSB-KIT\n"
                       "Autor: Daniel Enrique Zamora Sifredo\n"
                       "fecha: 1/10/2021\n"
                       "(C) AutoIDE");
}

void MainWindow::on_actionQt_triggered()
{
    QMessageBox::aboutQt(this);
}
