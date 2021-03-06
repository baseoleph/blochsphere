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

#ifndef VECTOR_HPP
#define VECTOR_HPP

#include "Qubit.h"
#include <QColor>
#include <QDebug>
#include <QObject>
#include <QQuaternion>
#include <QTimerEvent>
#include <QVector3D>
#include <utility>

#if QT_VERSION >= 0x050000
#include <QtMath>
#else
inline double qDegreesToRadians(double degrees) { return degrees * (M_PI / 180); }

inline double qRadiansToDegrees(double radians) { return radians * (180 / M_PI); }

QQuaternion QQuaternionrotationTo(const QVector3D &from, const QVector3D &to);
#endif

struct Trace {
    QVector3D first;
    QVector3D last;
    QColor    color;
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

    inline QColor                getSelfColor() const { return selfColor_; }
    inline void                  setSelfColor(QColor color) { selfColor_ = color; }
    void                         setColorByNameIndex();
    inline QColor                getTraceColor() const { return traceColor_; }
    inline void                  setTraceColor(QColor color) { traceColor_ = color; }
    inline void                  setEnableTrace(bool b) { traceEnabled_ = b; }
    inline bool                  isTraceEnabled() const { return traceEnabled_; }
    inline QVector<Trace> const &getTrace() const { return trace_; }
    Spike                        getSpike() const;
    inline void                  clearTrace() { trace_.clear(); }

    inline bool hasPath() const { return not path_.empty(); }

    void             setEnabledRotateVector(bool f) { _isRotateVectorEnable = f; }
    void             setRotateVector(QVector3D v) { _rotateVector = v; }
    bool             isRotateVectorEnable() const { return _isRotateVectorEnable; }
    const QVector3D &rotateVector() const { return _rotateVector; }

    void popPath();

    void changeVector(Spike s);
    void changeVector(QVector<Spike> s);

    static Spike actOperator(QQuaternion q, Spike s);

    Vector *getCopyState();

    void printVector() const;

    bool isNowAnimate() const { return isNowAnimate_; }
    void takeStep();

    void setAnimateState(bool animate) { isNowAnimate_ = animate; }

    static Spike createSpike(double x, double y, double z);
    static Spike createSpike(double the, double phi);
    static Spike createSpike(double a, complex b);

    void    setName(QString str) { _name = str; }
    void    setOperator(QString str) { _operator = str; }
    QString getInfo() { return _name + (_operator == "" ? "" : ": " + _operator); }

private:
    Spike          spike_;
    QVector<Spike> path_;
    QVector<Trace> trace_;
    QColor         selfColor_ = Qt::red;
    QColor         traceColor_ = Qt::gray;
    bool           traceEnabled_ = true;
    bool           isNowAnimate_ = false;
    QVector3D      _rotateVector;
    bool           _isRotateVectorEnable = false;
    QString        _name;
    QString        _operator;

    void                  tracePushBack();
    void                  initialSpike();
    static QVector<Spike> normalizePath(const QVector<Spike> &s);
};

#endif // VECTOR_HPP
