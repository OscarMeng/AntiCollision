#include "mainwidget.h"
#include <QApplication>
#include "anticollisionarea.h"
#include "controlwidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QFont f("ZYSong18030",12);
    a.setFont(f);

    MainWidget w;//生成对象
    w.show();

    return a.exec();
}
