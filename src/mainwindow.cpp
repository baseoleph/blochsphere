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

#include "mainwindow.hpp"
#include <QGridLayout>
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent) : QMainWindow{parent} {
    createScene();
    setupControlBlock();
}

int r() {
    //    return QRandomGenerator::global()->bounded(-100, 100);
    return QRandomGenerator::global()->bounded(0, 360);
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
    Vector *v = new Vector(qDegreesToRadians(0.), qDegreesToRadians(0.));
    vectors.insert(v, QVector<Sphere *>());
    spheres[0]->addVector(v);
    vectors[v].append(spheres[0]);
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

QVector<Spike> wow(Spike ss) {
    QVector<Spike> s;
    s.append(Operator::rXRotate(ss, r()));
    for (int i = 0; i < 100; ++i) {
        s.append(Operator::rZRotate(s.last(), r()));
        s.append(Operator::rYRotate(s.last(), r()));
        s.append(Operator::rXRotate(s.last(), r()));
    }
    std::reverse(s.begin(), s.end());
    return s;
}
void MainWindow::function() {
    for (auto &e : vectors.keys()) {
        if (not e->isNowAnimate()) {
            complex a = complex(0, 0);
            complex b = complex(1, 0);
            complex c = complex(1, 0);
            complex d = complex(0, 0);
            //            e->changeVector(Operator::applyZXDecomposition(e->getSpike(), a, b, c,
            //            d));
            UnitaryMatrix2x2 op;
            op.updateMatrix(a, b, c, d);
            e->changeVector(Operator::applyOperator(e->getSpike(), op));
            e->printVector();
        }
    }
}
void MainWindow::rotateVector() {
    function();
    //    startTimer(3000);
}

void MainWindow::timerEvent(QTimerEvent *) {
    addVector();
    function();
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
