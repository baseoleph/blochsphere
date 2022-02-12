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
#include <QQuaternion>
#include <QtMath>

Sphere::Sphere(QWidget *parent, const QString objName) : QGLWidget{parent} {
    this->setObjectName(objName);
}

void Sphere::initializeGL() {
    // TODO why I use it?
    qglClearColor(Qt::white);
}

void Sphere::resizeGL(int w, int h) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    GLfloat ratio = static_cast<GLfloat>(h) / w;

    // TODO check calculations
    if (w >= h)
        glOrtho(-2.0 / ratio, 2.0 / ratio, -2.0, 2.0, -10.0, 10.0);
    else
        glOrtho(-2.0, 2.0, -2.0 * ratio, 2.0 * ratio, -10.0, 10.0);
    glViewport(0, 0, w, h);
}

void Sphere::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glScalef(scaleFactor, scaleFactor, scaleFactor);

    glRotatef(xAngle, 1.0f, 0.0f, 0.0f);
    glRotatef(yAngle, 0.0f, 1.0f, 0.0f);
    glRotatef(zAngle, 0.0f, 0.0f, 1.0f);

    glColor4f(0.85f, 0.85f, 0.85f, 0.5f);
    drawSphere(50, 50);

    drawCircle();

    glEnable(GL_DEPTH_TEST);

    drawAxis();
    glDisable(GL_DEPTH_TEST);
    drawVectors();
}

void Sphere::mousePressEvent(QMouseEvent *pe) {
    this->setFocus();
    ptrMousePosition = pe->pos();
}

void Sphere::mouseMoveEvent(QMouseEvent *pe) {
    // TODO check theoretical range of xyzAgnles
    xAngle += 180 / scaleFactor * static_cast<GLfloat>(pe->y() - ptrMousePosition.y()) / height();
    zAngle += 180 / scaleFactor * static_cast<GLfloat>(pe->x() - ptrMousePosition.x()) / width();

    ptrMousePosition = pe->pos();

    updateGL();
}

void Sphere::wheelEvent(QWheelEvent *pe) {
    if (pe->angleDelta().y() > 0) {
        scalePlus();
    } else if (pe->angleDelta().y() < 0) {
        scaleMinus();
    }

    updateGL();
}

void Sphere::timerEvent(QTimerEvent *) {
    isNowAnimate = false;
    for (auto &e : vectors) {
        if (e->hasPath()) {
            e->popPath();
            isNowAnimate = true;
        }
    }

    if (not isNowAnimate) {
        killTimer(0);
    }

    update();
}

// TODO optimize function. remove c-style cast
void Sphere::drawSphere(int lats, int longs) {
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
            glVertex3f(x * zr0, y * zr0, z0);
            //            glNormal3f(x * zr1, y * zr1, z1);
            glVertex3f(x * zr1, y * zr1, z1);
        }
        glEnd();
    }
}

void Sphere::drawCircle() {
    glColor4f(0.7f, 0.8f, 0.8f, 0.5f);

    // TODO Isn't 20.f too much? (also check sphere slices)
    glBegin(GL_POLYGON);
    float i = 0;
    while (i < 6.28f) {
        glVertex2f(sphereRadius * sin(i), sphereRadius * cos(i));
        i += 0.157f; // 3.14/20
    }
    glEnd();

    glLineWidth(1.5f);
    glColor3f(0.6f, 0.7f, 0.7f);

    glBegin(GL_LINE_LOOP);
    i = 0;
    while (i < 6.28f) {
        glVertex2f(sphereRadius * sin(i), sphereRadius * cos(i));
        i += 0.157f; // 3.14/20
    }
    glEnd();

    glColor3f(0.0f, 0.0f, 0.0f);

    // TODO Why this symbols renders here?
    renderText(0.0, 0.05, -1.2, "|1>", font);
    renderText(0.0, 0.05, 1.2, "|0>", font);
}

// TODO I don't like how xyz renders (i mean letters)
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

void Sphere::drawVectors() {
    for (auto &e : vectors) {
        if (e->isTraceEnabled()) {
            glColor3f(e->getTraceColor().redF(), e->getTraceColor().greenF(),
                      e->getTraceColor().blueF());
            glLineWidth(2.5f);
            glBegin(GL_LINES);
            for (auto &segment : e->getTrace()) {
                glVertex3f(segment.first.x(), segment.first.y(), segment.first.z());
                glVertex3f(segment.last.x(), segment.last.y(), segment.last.z());
            }
            glEnd();
        }
        glColor3f(e->getSelfColor().redF(), e->getSelfColor().greenF(), e->getSelfColor().blueF());
        glLineWidth(2.5f);

        glBegin(GL_LINES);
        glVertex3f(0, 0, 0);

        if (e->hasPath() && not isNowAnimate) {
            startTimer(ANIMATION_INTERVAL);
        }
        QVector3D vertex = e->getCurrentPos();

        glVertex3f(vertex.x(), vertex.y(), vertex.z());

        glEnd();

        glBegin(GL_LINES);

        QQuaternion        q = QQuaternion::rotationTo(QVector3D(0, 0, 1), vertex);
        QVector<QVector3D> arrowhead;
        arrowhead.append(q.rotatedVector(QVector3D(0.02, 0.0, 0.9)));
        arrowhead.append(q.rotatedVector(QVector3D(-0.02, 0.0, 0.9)));
        arrowhead.append(q.rotatedVector(QVector3D(0.0, 0.02, 0.9)));
        arrowhead.append(q.rotatedVector(QVector3D(0.0, -0.02, 0.9)));
        for (int i = 0; i < arrowhead.size(); i++) {
            glVertex3f(vertex.x(), vertex.y(), vertex.z());
            glVertex3f(arrowhead[i].x(), arrowhead[i].y(), arrowhead[i].z());
        }
        glEnd();

        //        glBegin(GL_TRIANGLES);
        //        for (int k = 0; k <= 360; k += 5) {
        //            //        glColor3f(0.0, 0.0, 1.0);
        //            glVertex3f(0, 0, 1);
        //            //        glColor3f(0.0, 1.0, 1.0);
        //            glVertex3f(cos(k) * 0.02, sin(k) * 0.02, 0.9);
        //            //        glColor3f(1.0, 0.0, 0.0);
        //            glVertex3f(cos(k + 5) * 0.02, sin(k + 5) * 0.02, 0.9);
        //        }
        //        glEnd();
    }
}
