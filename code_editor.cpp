#include "code_editor.h"
#include "ui_code_editor.h"

code_editor::code_editor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::code_editor)
{
    ui->setupUi(this);
}

QTextEdit* code_editor::get_text_edit()
{
    return ui->textEdit;
}

code_editor::~code_editor()
{
    delete ui;
}
