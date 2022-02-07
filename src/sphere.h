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

#ifndef SPHERE_H
#define SPHERE_H

#include "vector.h"
#include <QDebug>
#include <QGLWidget>

class Sphere : public QGLWidget {
    Q_OBJECT
public:
    Sphere(QWidget *parent, const QString objName);

    void addVector(Vector *v);

protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
    // TODO add shortcuts
    void mousePressEvent(QMouseEvent *pe);
    void mouseMoveEvent(QMouseEvent *pe);
    void wheelEvent(QWheelEvent *pe);

private:
    const GLfloat sphereRadius = 1;
    // TODO I don't like this font
    const QFont font = QFont("System", 11);
    // TODO better create functions with pattern views
    GLfloat           scaleFactor = 1;
    GLfloat           xAngle = -60;
    GLfloat           yAngle = 0;
    GLfloat           zAngle = -135;
    QVector<Vector *> vectors;

    QPoint ptrMousePosition;

    void drawSphere(int lats, int longs);
    void drawCircle();
    void drawAxis();
    void scalePlus();
    void scaleMinus();
    void drawVectors();
};

#endif // SPHERE_H
