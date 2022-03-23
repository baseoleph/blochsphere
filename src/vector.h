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
#include <QDebug>
#include <QObject>
#include <QQuaternion>
#include <QRandomGenerator>
#include <QTimerEvent>
#include <QVector3D>
#include <QtMath>

struct Trace {
    QVector3D first;
    QVector3D last;
};

struct Spike {
    QVector3D point;
    QVector3D arrow1;
    QVector3D arrow2;
    QVector3D arrow3;
    QVector3D arrow4;
};

class Vector : public QObject, public Qubit {
    Q_OBJECT
public:
    Vector();
    Vector(double x, double y, double z);
    Vector(double the, double phi);
    Vector(complex a, complex b);

    inline QColor getSelfColor() const {
        return selfColor_;
    }
    inline void setSelfColor(QColor color) {
        selfColor_ = color;
    }
    inline QColor getTraceColor() const {
        return traceColor_;
    }
    inline void setTraceColor(QColor color) {
        traceColor_ = color;
    }
    inline void setEnableTrace(bool b) {
        traceEnabled_ = b;
    }
    inline bool isTraceEnabled() const {
        return traceEnabled_;
    }
    inline QVector<Trace> const &getTrace() const {
        return trace_;
    }
    Spike getSpike() const;

    inline QVector3D toQVector3D(Qubit const &q) const {
        return QVector3D(q.x(), q.y(), q.z());
    }
    inline QVector3D toQVector3D() const {
        return QVector3D(x(), y(), z());
    }
    inline bool hasPath() const {
        return not path_.empty();
    }

    // TODO when I should use inline?
    void popPath();

    void changeVector(Spike s) {
        path_.clear();
        spike_ = s;
    }

    void changeVector(QVector<Spike> s) {
        path_ = s;
        spike_ = s.last();
    }

    static Spike actOperator(QQuaternion q, Spike s) {
        s.point = q.rotatedVector(s.point);
        s.arrow1 = q.rotatedVector(s.arrow1);
        s.arrow2 = q.rotatedVector(s.arrow2);
        s.arrow3 = q.rotatedVector(s.arrow3);
        s.arrow4 = q.rotatedVector(s.arrow4);
        return s;
    }

    void printVector() const;

    QColor generateRandomColor() const;

    // TODO get set
    bool isNowAnimate = false;

protected:
    void timerEvent(QTimerEvent *);

private:
    Spike          spike_;
    QVector<Spike> path_;
    QVector<Trace> trace_;
    QColor         selfColor_ = Qt::red;
    QColor         traceColor_ = Qt::red;
    bool           traceEnabled_ = true;

    void tracePushBack();
    void initialSpike();
};

#endif // VECTOR_H
