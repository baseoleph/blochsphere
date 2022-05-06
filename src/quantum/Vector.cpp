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

#include "Vector.h"
#include <cassert>

Vector::Vector() { initialSpike(); }

Vector::Vector(double x, double y, double z) : Qubit(x, y, z) { initialSpike(); }

Vector::Vector(double the, double phi) : Qubit(the, phi) { initialSpike(); }

Vector::Vector(complex a, complex b) : Qubit(a, b) { initialSpike(); }

Spike Vector::getSpike() const {
    if (path_.empty()) {
        return spike_;
    } else {
        return path_.last();
    }
}

void Vector::popPath() {
    assert(not path_.isEmpty());
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
    Trace tr;
    tr.first = QVector3D(path_[path_.size() - 2].point.x(), path_[path_.size() - 2].point.y(),
                         path_[path_.size() - 2].point.z());
    tr.last = getSpike().point;
    tr.color = traceColor_;
    trace_.append(tr);
}

void Vector::initialSpike() { spike_ = createSpike(x(), y(), z()); }

// QColor Vector::generateRandomColor() {
//    return {QRandomGenerator::global()->bounded(255), QRandomGenerator::global()->bounded(255),
//            QRandomGenerator::global()->bounded(255)};
//}

Spike Vector::createSpike(double x, double y, double z) {
    Spike s;
#if QT_VERSION >= 0x050000
    QQuaternion q = QQuaternion::rotationTo(QVector3D(0, 0, 1), QVector3D(x, y, z));
#else
    QQuaternion q = QQuaternionrotationTo(QVector3D(0, 0, 1), QVector3D(x, y, z));
#endif
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

QVector<Spike> Vector::normalizePath(const QVector<Spike> &s) {
    QVector<Spike> newS;
    int            dur = Utility::getDuration();
    newS.append(s.first());
    if (dur != 0) {
        int step = s.size() / dur;
        for (int i = 1; i < s.size() - 1; i += step) {
            newS.append(s[i]);
        }
    }
    newS.append(s.last());
    return newS;
}

Vector *Vector::getCopyState() {
    auto v = new Vector();

    v->spike_ = spike_;
    v->path_ = path_;
    v->trace_ = trace_;
    v->selfColor_ = selfColor_;
    v->traceColor_ = traceColor_;
    v->traceEnabled_ = traceEnabled_;
    v->isNowAnimate_ = isNowAnimate_;

    return v;
}
void Vector::takeStep() {
    if (hasPath()) {
        popPath();
    } else {
        isNowAnimate_ = false;
    }
}
void Vector::setColorByNameIndex() {
    if (_name == "") {
        setSelfColor(Qt::red);
    } else if (_name == "1") {
        setSelfColor(Qt::red);
    } else if (_name == "2") {
        setSelfColor(Qt::green);
    } else if (_name == "3") {
        setSelfColor(Qt::yellow);
    } else if (_name == "4") {
        setSelfColor(Qt::magenta);
    } else if (_name == "5") {
        setSelfColor(Qt::cyan);
    }
}

#if QT_VERSION < 0x050000
QQuaternion QQuaternionrotationTo(const QVector3D &from, const QVector3D &to) {
    // Based on Stan Melax's article in Game Programming Gems
    const QVector3D v0(from.normalized());
    const QVector3D v1(to.normalized());
    float           d = QVector3D::dotProduct(v0, v1) + 1.0f;
    // if dest vector is close to the inverse of source vector, ANY axis of rotation is valid
    if (qFuzzyIsNull(d)) {
        QVector3D axis = QVector3D::crossProduct(QVector3D(1.0f, 0.0f, 0.0f), v0);
        if (qFuzzyIsNull(axis.lengthSquared()))
            axis = QVector3D::crossProduct(QVector3D(0.0f, 1.0f, 0.0f), v0);
        axis.normalize();
        // same as QQuaternion::fromAxisAndAngle(axis, 180.0f)
        return QQuaternion(0.0f, axis.x(), axis.y(), axis.z());
    }
    d = std::sqrt(2.0f * d);
    const QVector3D axis(QVector3D::crossProduct(v0, v1) / d);
    return QQuaternion(d * 0.5f, axis).normalized();
}
#endif
