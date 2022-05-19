// A Bloch sphere emulator program.
// Copyright (C) 2022 Vasiliy Stephanov <baseoleph@gmail.com>

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include "src/widgets/MainWindow.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QTextCodec>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

#if QT_VERSION < 0x050000
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf-8"));
#endif

    MainWindow      w;
    QDesktopWidget *desktop = QApplication::desktop();

    if (desktop->height() > 1200) {
        //        w.resize(1200, 1024);
        w.resize(1200, 800);
    } else {
        w.resize(1100, 800);
    }

    int x = (desktop->width() - w.width()) / 2;
    int y = (desktop->height() - w.height()) / 3;
    w.move(x, y);

    w.show();

    return app.exec();
}
