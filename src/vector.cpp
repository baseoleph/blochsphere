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
    selfColor_ = generateRandomColor();
    traceColor_ = generateRandomColor();
}

Vector::Vector(double x, double y, double z) : Qubit(x, y, z) {
    selfColor_ = generateRandomColor();
    traceColor_ = generateRandomColor();
}

Vector::Vector(double the, double phi) : Qubit(the, phi) {
    selfColor_ = generateRandomColor();
    traceColor_ = generateRandomColor();
}

Vector::Vector(complex a, complex b) : Qubit(a, b) {
    selfColor_ = generateRandomColor();
    traceColor_ = generateRandomColor();
}

inline QVector3D Vector::getCurrentPos() const {
    if (path_.empty()) {
        return toQVector3D();
    } else {
        return toQVector3D(path_.last());
    }
}

void Vector::popPath() {
    Q_ASSERT(not path_.isEmpty());
    if (path_.size() > 1) {
        tracePushBack();
    }
    path_.pop_back();
}

// TODO it's bad to copy vector
void Vector::changeVector(QVector<Qubit> path) {
    path_ = path;
    changeVector(path_.first());
}

void Vector::printVector() const {
    qDebug() << "---------------";
    qDebug() << "xyz" << x() << y() << z();
    qDebug() << "pt" << qRadiansToDegrees(the()) << qRadiansToDegrees(phi());
    qDebug() << "ab" << a().real() << a().imag() << b().real() << b().imag();
    qDebug() << "len" << sqrt(x() * x() + y() * y() + z() * z())
             << (sqrt(a() * a()) * sqrt(a() * a()) + sqrt(b() * b()) * sqrt(b() * b())).real()
             << (sqrt(a() * a()) * sqrt(a() * a()) + sqrt(b() * b()) * sqrt(b() * b())).imag();
    qDebug() << "---------------";
}

void Vector::tracePushBack() {
    trace_.append(Trace{QVector3D(path_[path_.size() - 2].x(), path_[path_.size() - 2].y(),
                                  path_[path_.size() - 2].z()),
                        getCurrentPos()});
}

QColor Vector::generateRandomColor() const {
    return QColor(QRandomGenerator::global()->bounded(255),
                  QRandomGenerator::global()->bounded(255),
                  QRandomGenerator::global()->bounded(255));
}
