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
    selfColor_ = generateRandomColor();
    traceColor_ = generateRandomColor();
}

// TODO срезка!
Vector::Vector(double x, double y, double z) {
    x_ = x;
    y_ = y;
    z_ = z;
    evalPT();
    evalAB();
    selfColor_ = generateRandomColor();
    traceColor_ = generateRandomColor();
}

// TODO when vector creates it calls Point()
// then Qubit() and after all this
// that work any constructor for vector
// why?
Vector::Vector(double the, double phi) {
    the_ = the;
    phi_ = phi;
    evalXYZ();
    evalAB();
    selfColor_ = generateRandomColor();
    traceColor_ = generateRandomColor();
}

Vector::Vector(complex a, complex b) {
    a_ = a;
    b_ = b;
    evalVertex();
    selfColor_ = generateRandomColor();
    traceColor_ = generateRandomColor();
}

void Vector::setSelfColor(QColor color) {
    selfColor_ = color;
}

const QColor &Vector::getTraceColor() const {
    return traceColor_;
}

void Vector::setTraceColor(QColor color) {
    traceColor_ = color;
}

const QColor &Vector::getSelfColor() const {
    return selfColor_;
}

QVector3D Vector::getCurrentPos() const {
    if (path_.empty()) {
        return QVector3D(x(), y(), z());
    } else {
        return QVector3D(path_.last().x(), path_.last().y(), path_.last().z());
    }
}

bool Vector::hasPath() const {
    return not path_.empty();
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

void Vector::printVector() const {
    qDebug() << "---------------";
    qDebug() << "xyz" << x_ << y_ << z_;
    qDebug() << "pt" << the_ << phi_;
    qDebug() << "ab" << a_.real() << a_.imag() << b_.real() << b_.imag();
    qDebug() << "len" << sqrt(x_ * x_ + y_ * y_ + z_ * z_)
             << (sqrt(a_ * a_) * sqrt(a_ * a_) + sqrt(b_ * b_) * sqrt(b_ * b_)).real()
             << (sqrt(a_ * a_) * sqrt(a_ * a_) + sqrt(b_ * b_) * sqrt(b_ * b_)).imag();
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

void Vector::setEnableTrace(bool b) {
    traceEnabled_ = b;
}

bool Vector::isTraceEnabled() const {
    return traceEnabled_;
}

const QVector<Trace> &Vector::getTrace() const {
    return trace_;
}
