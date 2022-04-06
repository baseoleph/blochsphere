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

#include "vector.hpp"

// TODO test this case
Vector::Vector() {
    selfColor_ = generateRandomColor();
    traceColor_ = generateRandomColor();
    initialSpike();
}

Vector::Vector(double x, double y, double z) : Qubit(x, y, z) {
    selfColor_ = generateRandomColor();
    traceColor_ = generateRandomColor();
    initialSpike();
}

Vector::Vector(double the, double phi) : Qubit(the, phi) {
    selfColor_ = generateRandomColor();
    traceColor_ = generateRandomColor();
    initialSpike();
}

Vector::Vector(complex a, complex b) : Qubit(a, b) {
    selfColor_ = generateRandomColor();
    traceColor_ = generateRandomColor();
    initialSpike();
}

Spike Vector::getSpike() const {
    if (path_.empty()) {
        return spike_;
    } else {
        return path_.last();
    }
}

void Vector::timerEvent(QTimerEvent *t) {
    this->setAnimateState(false);
    if (hasPath()) {
        popPath();
        this->setAnimateState(true);
    }

    if (not isNowAnimate()) {
        this->killTimer(t->timerId());
    }
}

// TODO Q_ASSERT works incorrect
void Vector::popPath() {
    Q_ASSERT(not path_.isEmpty());
    if (path_.size() > 1) {
        tracePushBack();
    }
    spike_ = path_.last();
    path_.pop_back();
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
    trace_.append(
        Trace{QVector3D(path_[path_.size() - 2].point.x(), path_[path_.size() - 2].point.y(),
                        path_[path_.size() - 2].point.z()),
              getSpike().point});
}

void Vector::initialSpike() { spike_ = createSpike(x(), y(), z()); }

QColor Vector::generateRandomColor() {
    return {QRandomGenerator::global()->bounded(255), QRandomGenerator::global()->bounded(255),
            QRandomGenerator::global()->bounded(255)};
}
Spike Vector::createSpike(double x, double y, double z) {
    Spike       s;
    QQuaternion q = QQuaternion::rotationTo(QVector3D(0, 0, 1), QVector3D(x, y, z));
    s.point = QVector3D(x, y, z);
    s.arrow1 = q.rotatedVector(QVector3D(0.02, 0.0, 0.9));
    s.arrow2 = q.rotatedVector(QVector3D(-0.02, 0.0, 0.9));
    s.arrow3 = q.rotatedVector(QVector3D(0.0, 0.02, 0.9));
    s.arrow4 = q.rotatedVector(QVector3D(0.0, -0.02, 0.9));
    return s;
}

Spike Vector::createSpike(double the, double phi) {
    Qubit q(the, phi);
    return createSpike(q.x(), q.y(), q.z());
}

Spike Vector::createSpike(double a, complex b) {
    Qubit q(a, b);
    return createSpike(q.x(), q.y(), q.z());
}
