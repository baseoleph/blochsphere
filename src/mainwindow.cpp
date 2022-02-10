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
#include <QGridLayout>
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent) : QMainWindow{parent} {
    createScene();
    QPushButton *bt = new QPushButton("push", this);
    bt->show();
    connect(bt, &QPushButton::clicked, this, &MainWindow::pushed);
}

int r() {
    return QRandomGenerator::global()->bounded(-100, 100);
}

QVector<Qubit> generatePath() {
    QVector<Qubit> pth;

    int r1 = r();
    int r2 = r();
    int r3 = r();
    int r4 = r();
    int r5 = r();
    int r6 = r();
    //    qDebug() << r1 << r2 << r3 << r4 << r5 << r6;
    //    int r1 = -81;
    //    int r2 = 1;
    //    int r3 = -68;
    //    int r4 = 25;
    //    int r5 = -14;
    //    int r6 = 57;
    //    qDebug() << "2";
    float i = 0;
    float dur = QRandomGenerator::global()->bounded(1., 30);
    //    float dur = 3;
    //    qDebug() << dur;
    float discrete = QRandomGenerator::global()->bounded(0.05);
    //    float discrete = 0.0157;
    while (i < dur) {
        //        Qubit *t = new Qubit(0, 0.5, sin(i));
        double x = sin(i) * r1 + cos(i) * r2;
        double y = sin(i) * r3 + cos(i) * r4;
        double z = sin(i) * r5 + cos(i) * r6;

        double length = sqrt(x * x + y * y + z * z);
        x /= length;
        y /= length;
        z /= length;

        //        qDebug() << x << y << z;
        pth.append(Qubit(x, y, z));
        //        i += 0.0157f; // 3.14/20
        i += discrete; // 3.14/20
    }

    std::reverse(pth.begin(), pth.end());
    return pth;
}

void MainWindow::pushed() {
    for (auto &e : vectors) {
        //        e->updateVector(generatePath());
        e->setEnableTrace(false);
    }

    for (int i = 0; i < 10; ++i) {
        vectors.append(new Vector);
        vectors.last()->changeVector(generatePath());
        spheres[QRandomGenerator::global()->bounded(spheres.size())]->addVector(vectors.last());
    }

    for (auto &e : spheres) {
        e->update();
    }
}

void MainWindow::createScene() {
    // Many spheres

    QWidget *w = new QWidget(this);
    setCentralWidget(w);
    QGridLayout *layout = new QGridLayout(w);
    w->setLayout(layout);
    for (int i = 0; i < 1; ++i) {
        for (int j = 0; j < 2; ++j) {
            spheres.append(new Sphere(w, QString::number(i + 10 * j)));
            layout->addWidget(spheres.last(), i, j);
        }
    }

    // One sphere

    //    spheres.append(new Sphere(this, "first sphere"));
    //    this->setCentralWidget(spheres[0]);
    //    spheres[0]->setFocus();

    //    Vector *a = new Vector(0, 0.0001, 1);
    //    Vector *b = new Vector(0.5, 1, 0);
    //    Vector *c = new Vector(0.2, 0.4, 0.9);
    //    vectors.append(a);
    //    vectors.append(b);
    //    vectors.append(c);
    //    vectors.append(new Vector(-0.56, 0.44, 0.7));
    //    vectors.append(new Vector(45.5, 141.8));
    //    vectors.append(new Vector((complex)0.922, complex(-0.304, 0.239)));
    //    vectors.append(new Vector(0, 0, 0));

    //    for (auto &e : vectors) {
    //        spheres[0]->addVector(e);
    //    }
}
