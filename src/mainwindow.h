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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "operator.h"
#include "qubit.h"
#include "sphere.h"
#include <QMainWindow>
#include <QMap>
#include <QVector>
#include <QtMath>

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    void function();

public slots:
    void addVector();
    void removeVector(Vector *v);
    void removeAllVectors();
    void rotateVector();

protected:
    void timerEvent(QTimerEvent *);

private:
    QVector<Sphere *>                 spheres;
    QMap<Vector *, QVector<Sphere *>> vectors;

    QWidget *controlWidget;
    void     createScene();
    void     setupControlBlock();
};

#endif // MAINWINDOW_H
