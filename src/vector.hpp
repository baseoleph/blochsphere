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

#ifndef VECTOR_HPP
#define VECTOR_HPP

#include "qubit.hpp"
#include <QColor>
#include <QDebug>
#include <QObject>
#include <QQuaternion>
#include <QRandomGenerator>
#include <QTimerEvent>
#include <QVector3D>
#include <QtMath>
#include <utility>

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

// TODO move all definitions to cpp
class Vector : public QObject, public Qubit {
    Q_OBJECT
public:
    Vector();
    Vector(double x, double y, double z);
    Vector(double the, double phi);
    Vector(complex a, complex b);

    [[nodiscard]] inline QColor getSelfColor() const { return selfColor_; }
    inline void                 setSelfColor(QColor color) { selfColor_ = std::move(color); }
    [[nodiscard]] inline QColor getTraceColor() const { return traceColor_; }
    inline void                 setTraceColor(QColor color) { traceColor_ = std::move(color); }
    inline void                 setEnableTrace(bool b) { traceEnabled_ = b; }
    [[nodiscard]] inline bool   isTraceEnabled() const { return traceEnabled_; }
    [[nodiscard]] inline QVector<Trace> const &getTrace() const { return trace_; }
    [[nodiscard]] Spike                        getSpike() const;

    inline QVector3D toQVector3D(Qubit const &q) const { return QVector3D(q.x(), q.y(), q.z()); }
    inline QVector3D toQVector3D() const { return QVector3D(x(), y(), z()); }
    [[nodiscard]] inline bool hasPath() const { return not path_.empty(); }

    // TODO when I should use inline?
    void popPath();

    void changeVector(Spike s) {
        path_.clear();
        spike_ = s;
        this->changeQubit(s.point.x(), s.point.y(), s.point.z());
    }

    void changeVector(QVector<Spike> s) {
        path_ = normalizePath(s);
        spike_ = s.last();
        this->changeQubit(s.first().point.x(), s.first().point.y(), s.first().point.z());
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

    static QColor generateRandomColor();

    [[nodiscard]] bool isNowAnimate() const { return isNowAnimate_; }

    void setAnimateState(bool animate) { isNowAnimate_ = animate; }

    static Spike createSpike(double x, double y, double z);
    static Spike createSpike(double the, double phi);
    static Spike createSpike(double a, complex b);

protected:
    void timerEvent(QTimerEvent *);

private:
    Spike          spike_;
    QVector<Spike> path_;
    QVector<Trace> trace_;
    QColor         selfColor_ = Qt::red;
    QColor         traceColor_ = Qt::red;
    bool           traceEnabled_ = true;
    bool           isNowAnimate_ = false;

    void                  tracePushBack();
    void                  initialSpike();
    static QVector<Spike> normalizePath(const QVector<Spike> &s);
};

#endif // VECTOR_HPP
