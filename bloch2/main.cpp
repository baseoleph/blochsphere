// Запуск главного она.
// --------------------
// Кадочников Д. 4 курс (06.10.2014)

#include "mainwid.hpp"
#include <QApplication>
#include <QTextCodec>
#include <ctime>

int main(int argc, char *argv[]) {
    srand(time(NULL));
    QApplication a(argc, argv);

    a.setStyle("Cleanlooks");

    //    QTextCodec *codec = QTextCodec::codecForName("WINDOWS-1251");
    //    //    QTextCodec::setCodecForTr(codec);
    //    //    QTextCodec::setCodecForCStrings(codec);
    //    QTextCodec::setCodecForLocale(codec);

    MainWid scene;

    //    scene.setWindowTitle("Bloch");
    //    scene.move(100, 50);
    scene.show();
    //    scene.resize(800, 600);

    return a.exec();
}
