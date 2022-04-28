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

#ifndef CIRCUIT_H
#define CIRCUIT_H

#include "CircuitQubit.h"
#include "vector.hpp"
#include <QGridLayout>
#include <QPushButton>
#include <QWidget>

class Circuit : public QWidget {
    Q_OBJECT
public:
    explicit Circuit(QWidget *parent);

    void addQubit(Vector *v);
    void removeQubit();

    QWidget                       *makeButtons();
    const QVector<CircuitQubit *> &getQubits() { return qubits; }
    int                            getSizeOfSteps() { return lenOfSteps; }

    QPushButton *runCircuitBut = nullptr;
    QPushButton *addStepBut = nullptr;
    QPushButton *removeStepBut = nullptr;

signals:
    void signalStartAnimation();

public slots:
    void slotAddStep();
    void slotRemoveStep();
    void slotRun();
    void slotParentAnimating(bool f);

private:
    QVBoxLayout            *mainLayout = nullptr;
    QVBoxLayout            *qubitsLayout = nullptr;
    QVector<CircuitQubit *> qubits;
    int                     lenOfSteps = 1;
    bool                    isParentAnimating = true;
};

#endif // CIRCUIT_H
