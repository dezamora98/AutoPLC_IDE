#include "mainwindow.h"
#include <QtThemeSupport/QtThemeSupport>
#include <QApplication>


#if true

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyle("fusion");

    MainWindow w;
    w.show();
    return a.exec();

}
#endif

#if false
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    hexcode c;
    c.open("C:/Users/YOW/Desktop/test/comp/main.hex",addr_R(0x2800,0x8000));
    auto temp_list = *c.get_hex32_to_AUK6();
    for(auto i: temp_list)
    {
        qDebug() <<QByteArray::number(i.first.first,16)<<":";
        for(int j = 0; j < i.second.size(); ++j)
        {
            auto t = j;
            ++t;
            qDebug() << "    " <<QByteArray::number(static_cast<uint8_t>(i.second.at(j)),16)
                     <<"|"<<QByteArray::number(static_cast<uint8_t>(i.second.at(t)),16);
            j=t;
        }
    }
    return a.exec();
}
#endif


