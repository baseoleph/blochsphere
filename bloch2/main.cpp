// Запуск главного она.
// --------------------
// Кадочников Д. 4 курс (06.10.2014)

#include "mainwid.h"
#include <QtGui/QApplication>
#include <ctime>
#include <QTextCodec>

int main(int argc, char *argv[])
{
    srand(time(NULL));
    QApplication a(argc, argv);

    a.setStyle("Cleanlooks");    

    QTextCodec *codec=QTextCodec::codecForName("WINDOWS-1251");
    QTextCodec::setCodecForTr(codec);
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForLocale(codec);

    MainWid *scene=new MainWid;

    scene->setWindowTitle("Bloch");
    scene->move(100,50);
    scene->show(); 
    scene->resize(800,600);

    return a.exec();
}
