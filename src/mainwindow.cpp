// The program helps to see a geometric representation of qubits
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

#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow{parent} {
    createScene();
}

void MainWindow::createScene() {
    // Many spheres

    //    QWidget *w = new QWidget(this);
    //    setCentralWidget(w);
    //    QGridLayout *layout = new QGridLayout(w);
    //    w->setLayout(layout);
    //    for (int i = 0; i < 2; ++i) {
    //        for (int j = 0; j < 5; ++j) {
    //            spheres.append(new Sphere(w));
    //            layout->addWidget(spheres.last(), i, j);
    //        }
    //    }

    // One sphere

    spheres.append(new Sphere(this, "first sphere"));
    this->setCentralWidget(spheres[0]);
    spheres[0]->setFocus();

    Vector *a = new Vector(0, 0, 1);
    Vector *b = new Vector(0, 1, 0);
    Vector *c = new Vector(0.2, 0.4, 0.9);

    spheres[0]->addVector(a);
    spheres[0]->addVector(b);
    spheres[0]->addVector(c);
}
