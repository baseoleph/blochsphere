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

#include "sphere.h"
#include <QMouseEvent>
#include <QtMath>

Sphere::Sphere(QWidget *parent) : QGLWidget{parent} {
    xRot = -60;
    yRot = 0;
    zRot = -135;
    nSca = 1.2;
    sRadius = 1;
    font = QFont("System", 11);
}

void Sphere::initializeGL() {
    // TODO why I use it?
    qglClearColor(Qt::white);
}

void Sphere::resizeGL(int nWidth, int nHeight) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    GLfloat ratio = (GLfloat)nHeight / (GLfloat)nWidth;

    if (nWidth >= nHeight)
        glOrtho(-2.0 / ratio, 2.0 / ratio, -2.0, 2.0, -10.0, 10.0);
    else
        glOrtho(-2.0, 2.0, -2.0 * ratio, 2.0 * ratio, -10.0, 10.0);
    glViewport(0, 0, (GLfloat)nWidth, (GLfloat)nHeight);
}

void Sphere::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Масштаб
    glScalef(nSca, nSca, nSca);

    // Вращение системы координат
    glRotatef(xRot, 1.0f, 0.0f, 0.0f);
    glRotatef(yRot, 0.0f, 1.0f, 0.0f);
    glRotatef(zRot, 0.0f, 0.0f, 1.0f);

    // Отрисовка сферы
    glColor4f(0.85f, 0.85f, 0.85f, 0.5f);
    drawSphere(1.0, 50, 50);

    drawCircle();

    glEnable(GL_DEPTH_TEST);

    drawAxis();
    glDisable(GL_DEPTH_TEST);
}

void Sphere::mousePressEvent(QMouseEvent *pe) {
    this->setFocus();
    ptrMousePosition = pe->pos();
}

void Sphere::mouseMoveEvent(QMouseEvent *pe) {
    xRot += 180 / nSca * (GLfloat)(pe->y() - ptrMousePosition.y()) / height();
    zRot += 180 / nSca * (GLfloat)(pe->x() - ptrMousePosition.x()) / width();

    ptrMousePosition = pe->pos();

    updateGL();
}

void Sphere::wheelEvent(QWheelEvent *pe) {
    if (pe->angleDelta().y() > 0)
        scalePlus();
    else if (pe->angleDelta().y() < 0)
        scaleMinus();

    updateGL();
}

void Sphere::drawSphere(double r, int lats, int longs) {
    int i, j;
    for (i = 0; i <= lats; i++) {
        double lat0 = M_PI * (-0.5 + (double)(i - 1) / lats);
        double z0 = sin(lat0);
        double zr0 = cos(lat0);

        double lat1 = M_PI * (-0.5 + (double)i / lats);
        double z1 = sin(lat1);
        double zr1 = cos(lat1);

        glBegin(GL_QUAD_STRIP);
        for (j = 0; j <= longs; j++) {
            double lng = 2 * M_PI * (double)(j - 1) / longs;
            double x = cos(lng);
            double y = sin(lng);

            //            glNormal3f(x * zr0, y * zr0, z0);
            glVertex3f(r * x * zr0, r * y * zr0, r * z0);
            //            glNormal3f(x * zr1, y * zr1, z1);
            glVertex3f(r * x * zr1, r * y * zr1, r * z1);
        }
        glEnd();
    }
}

void Sphere::drawCircle() {
    glColor4f(0.7f, 0.8f, 0.8f, 0.5f);
    glBegin(GL_POLYGON);
    for (float i = 0.f; i < 2.f * 3.14f; i += 3.14f / 18.f)
        glVertex2f(sRadius * sin(i), sRadius * cos(i));
    glEnd();
    glLineWidth(1.5f);
    glColor3f(0.6f, 0.7f, 0.7f);
    glBegin(GL_LINE_LOOP);
    for (float i = 0.f; i < 2.f * 3.14f; i += 3.14f / 18.f)
        glVertex2f(sRadius * sin(i), sRadius * cos(i));
    glEnd();

    glColor3f(0.0f, 0.0f, 0.0f);
    renderText(0.0, 0.05, -1.2, "|1>", font);
    renderText(0.0, 0.05, 1.2, "|0>", font);
}

void Sphere::drawAxis() {
    float axSize = 1.7f;

    glLineWidth(2.0f);
    // OX
    qglColor(Qt::black);
    glBegin(GL_LINES);
    glVertex3f(axSize, 0.f, 0.f);
    glVertex3f(-axSize, 0.f, 0.f);

    glVertex3f(axSize, 0.f, 0.f);
    glVertex3f(axSize - 0.1, 0.f, 0.025f);
    glVertex3f(axSize, 0.f, 0.f);
    glVertex3f(axSize - 0.1, 0.f, -0.025f);

    glVertex3f(axSize, 0.f, 0.f);
    glVertex3f(axSize - 0.1f, 0.025f, 0.f);
    glVertex3f(axSize, 0.f, 0.f);
    glVertex3f(axSize - 0.1f, -0.025f, 0.f);
    glEnd();
    renderText(axSize + 0.1, 0.0, 0.0, "x", font);

    // OY
    qglColor(Qt::black);
    glBegin(GL_LINES);
    glVertex3f(0.f, axSize, 0.f);
    glVertex3f(0.f, -axSize, 0.f);

    glVertex3f(0.f, axSize, 0.f);
    glVertex3f(0.f, axSize - 0.1f, 0.025f);
    glVertex3f(0.f, axSize, 0.f);
    glVertex3f(0.f, axSize - 0.1f, -0.025f);

    glVertex3f(0.f, axSize, 0.f);
    glVertex3f(0.025f, axSize - 0.1f, 0.f);
    glVertex3f(0.f, axSize, 0.f);
    glVertex3f(-0.025f, axSize - 0.1f, 0.f);
    glEnd();
    renderText(0, axSize + 0.1f, 0.f, "y", font);

    // OZ
    qglColor(Qt::black);
    glBegin(GL_LINES);
    glVertex3f(0.f, 0.f, axSize);
    glVertex3f(0.f, 0.f, -axSize);

    glVertex3f(0.f, 0.f, axSize);
    glVertex3f(0.025f, 0.f, axSize - 0.1f);
    glVertex3f(0.f, 0.f, axSize);
    glVertex3f(-0.025f, 0.f, axSize - 0.1f);

    glVertex3f(0.f, 0.f, axSize);
    glVertex3f(0.f, 0.025f, axSize - 0.1f);
    glVertex3f(0.f, 0.f, axSize);
    glVertex3f(0.f, -0.025f, axSize - 0.1f);
    glEnd();
    renderText(0, 0, axSize + 0.1f, "z", font);
}

void Sphere::scalePlus() {
    nSca = nSca * 1.1;
}

void Sphere::scaleMinus() {
    nSca = nSca / 1.1;
}
