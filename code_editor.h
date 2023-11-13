#ifndef CODE_EDITOR_H
#define CODE_EDITOR_H

#include <QWidget>
#include <QTextEdit>

namespace Ui {
class code_editor;
}

class code_editor : public QWidget
{
    Q_OBJECT

public:
    explicit code_editor(QWidget *parent = nullptr);
    QTextEdit *get_text_edit();
    ~code_editor();

private:
    Ui::code_editor *ui;
};

#endif // CODE_EDITOR_H
