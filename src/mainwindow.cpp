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
    setupControlBlock();
}

int r() {
    return QRandomGenerator::global()->bounded(-100, 100);
}

QVector<Qubit> generatePath() {
    QVector<Qubit> pth;

    int   r1 = r();
    int   r2 = r();
    int   r3 = r();
    int   r4 = r();
    int   r5 = r();
    int   r6 = r();
    float i = 0;
    float dur = QRandomGenerator::global()->bounded(20, 300);
    float discrete = QRandomGenerator::global()->bounded(0.05) + 0.01;
    while (i < dur) {
        double x = sin(i) * r1 + cos(i) * r2 + r1;
        double y = sin(i) * r3 + cos(i) * r4 + r2;
        double z = sin(i) * r5 + cos(i) * r6 + r3;

        double length = sqrt(x * x + y * y + z * z);
        x /= length;
        y /= length;
        z /= length;

        pth.append(Qubit(x, y, z));
        i += discrete; // 3.14/20
    }

    std::reverse(pth.begin(), pth.end());
    return pth;
}

void MainWindow::addVector() {
    Vector *v = new Vector;
    vectors.insert(v, QVector<Sphere *>());
    //    v->changeVector(generatePath());
    spheres[0]->addVector(v);
    vectors[v].append(spheres[0]);
    v->changeVector(Qubit(qDegreesToRadians(90.), qDegreesToRadians(0.)));
    v->printVector();

    //    for (int i = 0; i < 10; ++i) {
    //        Vector *v = new Vector;
    //        vectors.insert(v, QVector<Sphere *>());
    //        v->changeVector(generatePath());

    //        int ind = QRandomGenerator::global()->bounded(spheres.size());
    //        spheres[ind]->addVector(v);
    //        vectors[v].append(spheres[ind]);
    //    }

    for (auto &e : spheres) {
        e->update();
    }
}

void MainWindow::removeVector(Vector *v) {
    for (auto &e : vectors[v]) {
        e->deleteVector(v);
        e->update();
    }
    vectors.remove(v);
    v->~Vector();
}

void MainWindow::removeAllVectors() {
    while (not vectors.isEmpty()) {
        removeVector(vectors.lastKey());
    }
}

void MainWindow::rotateVector() {
    for (auto &e : vectors.keys()) {
        e->changeVector(
            //            Operator::zxRotate(*e, qDegreesToRadians(90.), 0, qDegreesToRadians(180.),
            //            0));
            Operator::rXRotate(*e, qDegreesToRadians(180.)));
        e->printVector();
        Operator::printOperator(Operator::getRX(qDegreesToRadians(180.)));
    }

    for (auto &e : spheres) {
        e->update();
    }
}

void MainWindow::createScene() {
    controlWidget = new QWidget(this);

    QWidget *w = new QWidget(this);
    setCentralWidget(w);
    QGridLayout *layout = new QGridLayout(w);
    w->setLayout(layout);
    for (int i = 0; i < 1; ++i) {
        for (int j = 1; j < 2; ++j) {
            spheres.append(new Sphere(w, QString::number(i + 10 * j)));
            layout->addWidget(spheres.last(), i, j);
        }
    }
    layout->addWidget(controlWidget, 0, 0);
}

void MainWindow::setupControlBlock() {
    controlWidget->setMaximumWidth(200);
    QGridLayout *layout = new QGridLayout(controlWidget);
    controlWidget->setLayout(layout);
    QPushButton *bAddVector = new QPushButton("Add vector", this);
    QPushButton *bResetVectors = new QPushButton("Reset vector", this);
    QPushButton *bRotateVectors = new QPushButton("Rotate vector", this);
    layout->addWidget(bAddVector, 0, 0);
    layout->addWidget(bResetVectors, 0, 1);
    layout->addWidget(bRotateVectors, 0, 2);
    bAddVector->show();
    bResetVectors->show();
    bRotateVectors->show();
    connect(bAddVector, &QPushButton::clicked, this, &MainWindow::addVector);
    connect(bResetVectors, &QPushButton::clicked, this, &MainWindow::removeAllVectors);
    connect(bRotateVectors, &QPushButton::clicked, this, &MainWindow::rotateVector);
}
