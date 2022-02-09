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

#include "vector.h"

Vector::Vector() {
    x_ = 0;
    y_ = 0;
    z_ = 0;
    evalPT();
    evalAB();
    generateRandomColor();
}

// TODO срезка!
Vector::Vector(double x, double y, double z) {
    x_ = x;
    y_ = y;
    z_ = z;
    evalPT();
    evalAB();
    generateRandomColor();
}

Vector::Vector(double the, double phi) {
    the_ = the;
    phi_ = phi;
    evalXYZ();
    evalAB();
    generateRandomColor();
}

Vector::Vector(complex a, complex b) {
    a_ = a;
    b_ = b;
    evalVertex();
    generateRandomColor();
}

void Vector::setColor(QColor color) {
    color_ = color;
}

const QColor &Vector::getColor() {
    return color_;
}

QVector3D Vector::getCurrentPos() {
    if (path_.empty()) {
        return QVector3D(x(), y(), z());
    } else {
        return QVector3D(path_.last().x(), path_.last().y(), path_.last().z());
    }
}

bool Vector::hasPath() {
    return not path_.empty();
}

void Vector::popPath() {
    Q_ASSERT(not path_.isEmpty());
    path_.pop_back();
}

void Vector::changeVector(QVector<Qubit> path) {
    path_ = path;
    changeVector(path_.last());
}

// TODO there is a way to copy qubit to vector by slicing
void Vector::changeVector(Qubit qbt) {
    //    a_ = qbt.a();
    //    b_ = qbt.b();
    //    evalVertex();
    x_ = qbt.x();
    y_ = qbt.y();
    z_ = qbt.z();
    //    x_ = y_ = z_ = 0;
    evalPT();
    evalAB();
}

void Vector::generateRandomColor() {
    color_ =
        QColor(QRandomGenerator::global()->bounded(255), QRandomGenerator::global()->bounded(255),
               QRandomGenerator::global()->bounded(255));
}
