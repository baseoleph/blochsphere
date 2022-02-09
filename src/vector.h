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

#ifndef VECTOR_H
#define VECTOR_H

#include "qubit.h"
#include <QColor>
#include <QObject>
#include <QRandomGenerator>
#include <QVector3D>

class Vector : public QObject, public Qubit {
    Q_OBJECT
public:
    Vector();
    Vector(double x, double y, double z);
    Vector(double the, double phi);
    Vector(complex a, complex b);

    // TODO is this correct way to share private data?
    const QColor &getColor();
    void          setColor(QColor color);
    void          generateRandomColor();

    QVector3D getCurrentPos();

    // TODO when I should use inline?
    bool hasPath();
    void popPath();

    void changeVector(QVector<Qubit> path);
    void changeVector(Qubit qbt);

private:
    QVector<Qubit> path_;
    QColor         color_ = Qt::red;
};

#endif // VECTOR_H
