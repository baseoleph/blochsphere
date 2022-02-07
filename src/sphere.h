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

#include <QGLWidget>

class Sphere : public QGLWidget {
    Q_OBJECT
public:
    explicit Sphere(QWidget *parent = nullptr);

protected:
    void initializeGL();
    void resizeGL(int nWidth, int nHeight);
    void paintGL();
    // TODO add shortcuts
    void mousePressEvent(QMouseEvent *pe);
    void mouseMoveEvent(QMouseEvent *pe);
    void wheelEvent(QWheelEvent *pe);

private:
    GLfloat xRot, yRot, zRot; // Углы поворота системы координат
    GLfloat nSca;             // Масштаб
    GLfloat sRadius;          // Радиус сферы
    float   iter;
    int     timerID;       // таймер
    bool    isTimerActive; // флаг активности таймера
    int     trCol;
    QFont   font;
    QPoint ptrMousePosition; // Позиция указателя мыши
    // --------------------------------------------------

    QColor           oZCol, oYCol, oXCol; // Цвет осей
    QVector<GLfloat> tracePts;            // Список точек тректории
    bool             traceFlag;
    bool             showNewAx;
    float            naX, naY, naZ;

    void drawSphere(double r, int lats, int longs);
    void drawCircle();
    void drawAxis(); // Отрисовка осей координат
    void drawNewAx();
    void scalePlus();
    void scaleMinus();
};

#endif // SPHERE_H
