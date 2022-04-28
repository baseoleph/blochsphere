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

#include "CircuitQubit.h"
#include <QLabel>

CircuitQubit::CircuitQubit(QWidget *parent, Vector *v, const QString &name, int cntOperators)
    : QWidget(parent), _v(v) {
    mainLayout = new QHBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignLeft);
    mainLayout->setSpacing(1);
    mainLayout->setMargin(0);

    updateOperators(cntOperators);
    _name = name;

    auto nameLabel = new QLabel(_name);
    nameLabel->setFixedSize(cellWidth, cellHigh);
    mainLayout->addWidget(nameLabel);
    foreach (auto e, operators) { mainLayout->addWidget(e); }
}

QString CircuitQubit::getPsiHtml(QString index) { return "Psi<sub><big>" + index + "</big></sub>"; }

void CircuitQubit::updateOperators(int len) {
    assert(len > 0);
    if (operators.size() < len) {
        while (operators.size() != len) {
            operators.append(new CircuitOperator(this, Operator()));
            operators.last()->setFixedSize(cellWidth, cellHigh);
            mainLayout->addWidget(operators.last());
        }
    }
    if (operators.size() > len) {
        while (operators.size() != len) {
            mainLayout->removeWidget(operators.last());
            delete operators.last();
            operators.pop_back();
        }
    }
}

void CircuitQubit::printOperators() {
    QString str = _name + " ";
    foreach (auto e, operators) { str += e->getOperatorName() + " "; }
    qDebug() << str;
}

Operator &CircuitQubit::getOperator(int ind) {
    assert(ind >= 0 and ind < operators.size());
    if (lastOperator) {
        lastOperator->setState(STATE::PASSIVE);
    }
    operators[ind]->setState(STATE::ACTIVE);
    lastOperator = operators[ind];
    return operators[ind]->getOperator();
}
void CircuitQubit::resetState() {
    if (lastOperator) {
        lastOperator->setState(STATE::PASSIVE);
        lastOperator = nullptr;
    }
}
