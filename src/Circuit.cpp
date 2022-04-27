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

#include "Circuit.h"

#include <QLabel>

Circuit::Circuit(QWidget *parent) : QWidget(parent) {
    mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(makeButtons());
    lenOfSteps = 1;
    removeStepBut->setEnabled(lenOfSteps > 1);
}

void Circuit::addQubit(Vector *v) {
    foreach (auto e, qubits) {
        if (v == e->getVector()) {
            return;
        }
    }
    auto qbt = new CircuitQubit(
        this, v, CircuitQubit::getPsiHtml(QString::number(qubits.size() + 1)), lenOfSteps);
    qubits.append(qbt);
    mainLayout->addWidget(qbt);

    qDebug() << "---------------------------------------------------------------------";
    foreach (auto e, qubits) { e->printOperators(); }
    qDebug() << "---------------------------------------------------------------------";
}

void Circuit::removeQubit() {
    if (not qubits.empty()) {
        mainLayout->removeWidget(qubits.last());
        delete qubits.last();
        qubits.pop_back();
    }

    qDebug() << "---------------------------------------------------------------------";
    foreach (auto e, qubits) { e->printOperators(); }
    qDebug() << "---------------------------------------------------------------------";
}
QWidget *Circuit::makeButtons() {
    auto wdt = new QWidget(this);
    auto layout = new QHBoxLayout(wdt);

    addStepBut = new QPushButton("Add step");
    layout->addWidget(addStepBut);
    connect(addStepBut, SIGNAL(clicked()), SLOT(slotAddStep()));

    removeStepBut = new QPushButton("Remove step");
    layout->addWidget(removeStepBut);
    connect(removeStepBut, SIGNAL(clicked()), SLOT(slotRemoveStep()));

    runCircuitBut = new QPushButton("Run");
    layout->addWidget(runCircuitBut);
    connect(runCircuitBut, SIGNAL(clicked()), SLOT(slotRun()));

    wdt->setLayout(layout);
    return wdt;
}

void Circuit::slotAddStep() {
    lenOfSteps += 1;
    foreach (auto e, qubits) { e->updateOperators(lenOfSteps); }

    addStepBut->setEnabled(lenOfSteps < MAX_COUNT_OF_STEPS);
    removeStepBut->setEnabled(lenOfSteps > 1);
}

void Circuit::slotRemoveStep() {
    lenOfSteps -= 1;
    foreach (auto e, qubits) { e->updateOperators(lenOfSteps); }

    addStepBut->setEnabled(lenOfSteps < MAX_COUNT_OF_STEPS);
    removeStepBut->setEnabled(lenOfSteps > 1);
}

void Circuit::slotRun() {
    if (not isParentAnimating) {
        emit signalStartAnimation();
    }
}

void Circuit::slotParentAnimating(bool f) { isParentAnimating = f; }
