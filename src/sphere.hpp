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

#ifndef SPHERE_HPP
#define SPHERE_HPP

#include "vector.hpp"
#include <QDebug>
#include <QGLWidget>

class Sphere : public QGLWidget {
    Q_OBJECT
public:
    explicit Sphere(QWidget *parent);
    // TODO realize safety add end detach vectors
    void addVector(Vector *v) { vectors.append(v); }
    void deleteVector(Vector *v);
    void toYoZ();
    void toXoY();
    void toZoX();
    void toNormal();

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    // TODO add shortcuts
    void mousePressEvent(QMouseEvent *pe) override;
    void mouseMoveEvent(QMouseEvent *pe) override;
    void wheelEvent(QWheelEvent *pe) override;

private:
    const GLfloat sphereRadius = 1;
    // TODO I don't like this font
    const QFont font = QFont("System", 11);
    // TODO better create functions with pattern views
    // like default view, xOy vew etc.
    GLfloat scaleFactor = 1;
    GLfloat xAngle = -60;
    GLfloat yAngle = 0;
    GLfloat zAngle = -135;

    QList<Vector *> vectors;

    QPoint ptrMousePosition;

    bool isNowAnimate = false;

    static void drawSphere(int lats, int longs);
    void        drawCircle();
    void        drawAxis();
    void        scalePlus() {
        if (scaleFactor < 5.) {
            scaleFactor *= 1.1;
        }
    }
    void scaleMinus() {
        if (scaleFactor > 0.2) {
            scaleFactor /= 1.1;
        }
    }
    void drawVectors();
};

#endif // SPHERE_HPP
