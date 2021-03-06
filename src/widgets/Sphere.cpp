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

#include "Sphere.h"
#include <QMouseEvent>
#include <QQuaternion>

Sphere::Sphere(QWidget *parent) : QGLWidget{parent} { toNormal(); }

void Sphere::deleteVector(Vector *v) {
    if (vectors.indexOf(v) != -1) {
        vectors.removeOne(v);
    }
}

void Sphere::initializeGL() { qglClearColor(Qt::white); }

void Sphere::resizeGL(int w, int h) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    GLfloat ratio = static_cast<GLfloat>(h) / w;

    if (w >= h) {
        glOrtho(-2.0 / ratio, 2.0 / ratio, -2.0, 2.0, -10.0, 10.0);
    } else {
        glOrtho(-2.0, 2.0, -2.0 * ratio, 2.0 * ratio, -10.0, 10.0);
    }
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
    xAngle += 180 / scaleFactor * static_cast<GLfloat>(pe->y() - ptrMousePosition.y()) / height();
    zAngle += 180 / scaleFactor * static_cast<GLfloat>(pe->x() - ptrMousePosition.x()) / width();

    ptrMousePosition = pe->pos();

    updateGL();
}

void Sphere::wheelEvent(QWheelEvent *pe) {
#if QT_VERSION >= 0x050000
    if (pe->angleDelta().y() > 0) {
        scalePlus();
    } else if (pe->angleDelta().y() < 0) {
        scaleMinus();
    }
#else
    if (pe->delta() > 0) {
        scalePlus();
    } else if (pe->delta() < 0) {
        scaleMinus();
    }
#endif

    updateGL();
}

void Sphere::drawSphere(int lats, int longs) {
    for (int i = 0; i <= lats; i++) {
        double lat0 = M_PI * (-0.5 + static_cast<double>(i - 1.) / lats);
        double z0 = sin(lat0);
        double zr0 = cos(lat0);

        double lat1 = M_PI * (-0.5 + static_cast<double>(i) / lats);
        double z1 = sin(lat1);
        double zr1 = cos(lat1);

        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= longs; j++) {
            double lng = 2 * M_PI * static_cast<double>(j - 1) / longs;
            double x = cos(lng);
            double y = sin(lng);

            glVertex3f(x * zr0, y * zr0, z0);
            glVertex3f(x * zr1, y * zr1, z1);
        }
        glEnd();
    }
}

void Sphere::drawCircle() {
    glColor4f(0.7f, 0.8f, 0.8f, 0.5f);

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

    renderText(0.0, 0.05, -1.2, "|1>", font);
    renderText(0.0, 0.05, 1.2, "|0>", font);
}

void Sphere::drawAxis() {
    float axSize = 1.7f;

    glLineWidth(2.3f);
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
            renderText(1.2, -1.2, 1.2, e->getInfo(), font);
            glEnable(GL_DEPTH_TEST);
            for (auto &segment : e->getTrace()) {
                glColor3f(segment.color.redF(), segment.color.greenF(), segment.color.blueF());
                glLineWidth(2.5f);
                glBegin(GL_LINES);

                glVertex3f(segment.first.x(), segment.first.y(), segment.first.z());
                glVertex3f(segment.last.x(), segment.last.y(), segment.last.z());

                glEnd();
            }
            glDisable(GL_DEPTH_TEST);
        }

        if (e->isRotateVectorEnable()) {
            qglColor(Qt::blue);
            glLineWidth(3.f);
            glBegin(GL_LINES);
            glVertex3f(e->rotateVector().x(), e->rotateVector().y(), e->rotateVector().z());
            glVertex3f(-e->rotateVector().x(), -e->rotateVector().y(), -e->rotateVector().z());
            glEnd();
        }

        glColor3f(e->getSelfColor().redF(), e->getSelfColor().greenF(), e->getSelfColor().blueF());
        glLineWidth(2.5f);

        glBegin(GL_LINES);
        glVertex3f(0, 0, 0);

        QVector3D vertex = e->getSpike().point;

        glVertex3f(vertex.x(), vertex.y(), vertex.z());

        glEnd();

        glBegin(GL_LINES);

        QVector<QVector3D> arrowhead;
        arrowhead.append(e->getSpike().arrow1);
        arrowhead.append(e->getSpike().arrow2);
        arrowhead.append(e->getSpike().arrow3);
        arrowhead.append(e->getSpike().arrow4);
        for (auto &i : arrowhead) {
            glVertex3f(vertex.x(), vertex.y(), vertex.z());
            glVertex3f(i.x(), i.y(), i.z());
        }
        glEnd();
    }
}

void Sphere::toYoZ() {
    scaleFactor = 1;
    xAngle = -90;
    yAngle = 0;
    zAngle = -90;
    update();
}

void Sphere::toXoY() {
    scaleFactor = 1;
    xAngle = 0;
    yAngle = 0;
    zAngle = 0;
    update();
}

void Sphere::toZoX() {
    scaleFactor = 1;
    xAngle = -90;
    yAngle = 0;
    zAngle = -180;
    update();
}

void Sphere::toNormal() {
    scaleFactor = 1;
    xAngle = -60;
    yAngle = 0;
    zAngle = -135;
    update();
}
void Sphere::easterEggRotate() {
    GLfloat scale = scaleFactor;
    GLfloat x = xAngle;
    GLfloat y = yAngle;
    GLfloat z = zAngle;
    for (int i = 0; i < 360; i += 5) {
        xAngle = x + i;
        yAngle = y + i;
        zAngle = z + i;
        if (i < 180) {
            scaleFactor /= 1.05;
        } else {
            scaleFactor *= 1.05;
        }
        update();
        Utility::delay();
    }
    scaleFactor = scale;
    xAngle = x;
    yAngle = y;
    zAngle = z;
    update();
}
