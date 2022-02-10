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

struct Trace;

class Vector : public QObject, public Qubit {
    Q_OBJECT
public:
    Vector();
    Vector(double x, double y, double z);
    Vector(double the, double phi);
    Vector(complex a, complex b);

    // TODO is this correct way to share private data?
    const QColor &getSelfColor();
    void          setSelfColor(QColor color);
    const QColor &getTraceColor();
    void          setTraceColor(QColor color);
    QColor        generateRandomColor();

    void setEnableTrace(bool b);

    // TODO check other functions for const
    bool                  isTraceEnabled() const;
    const QVector<Trace> &getTrace() const;

    QVector3D getCurrentPos();

    // TODO when I should use inline?
    bool hasPath();
    void popPath();

    void changeVector(QVector<Qubit> path);
    void changeVector(Qubit qbt);

private:
    // TODO must be queue
    QVector<Qubit> path_;
    QVector<Trace> trace_;
    QColor         selfColor_ = Qt::red;
    QColor         traceColor_ = Qt::red;
    bool           traceEnabled_ = true;

    void tracePushBack();
};

struct Trace {
    QVector3D first;
    QVector3D last;
    // TODO why there is need a semicolon?
};

#endif // VECTOR_H
