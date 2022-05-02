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

#include "CircuitOperator.h"
#include "blochdialog.hpp"

CircuitOperator::CircuitOperator(QWidget *parent, Operator op) : QComboBox(parent), _op(op) {
    addItem("Id");
    addItem("X");
    addItem("Y");
    addItem("Z");
    addItem("H");
    addItem("S");
    addItem("T");
    addItem("Phi");
    addItem("Rx");
    addItem("Ry");
    addItem("Rz");
    setCurrentIndex(OPERATORS::ID);

    connect(this, SIGNAL(activated(int)), SLOT(slotOperatorChanged(int)));
}

QString CircuitOperator::getOperatorName() { return _op.getOperatorName(); }

void CircuitOperator::slotOperatorChanged(int index) {
    clearComboBoxNames();

    if (index == OPERATORS::ID) {
        _op.toId();
    } else if (index == OPERATORS::X) {
        _op.toX();
    } else if (index == OPERATORS::Y) {
        _op.toY();
    } else if (index == OPERATORS::Z) {
        _op.toZ();
    } else if (index == OPERATORS::H) {
        _op.toH();
    } else if (index == OPERATORS::S) {
        _op.toS();
    } else if (index == OPERATORS::T) {
        _op.toT();
    } else if (index == OPERATORS::PHI) {
        auto aIn = new BlochDialog((QWidget *)parent(), DIALOG_TYPE::ANGLE);
        if (aIn->exec() == QDialog::Accepted) {
            _op.toPhi(aIn->ang().toDouble() * M_PI / 180);
            this->setItemText(OPERATORS::PHI, _op.getOperatorName());
        } else {
            this->setCurrentIndex(lastActivated);
            this->setItemText(lastActivated, _op.getOperatorName());
        }
        delete aIn;
    } else if (index == OPERATORS::RX) {
        auto aIn = new BlochDialog((QWidget *)parent(), DIALOG_TYPE::ANGLE);
        if (aIn->exec() == QDialog::Accepted) {
            _op.toXrotate(aIn->ang().toDouble() * M_PI / 180);
            this->setItemText(OPERATORS::RX, _op.getOperatorName());
        } else {
            this->setCurrentIndex(lastActivated);
            this->setItemText(lastActivated, _op.getOperatorName());
        }
        delete aIn;
    } else if (index == OPERATORS::RY) {
        auto aIn = new BlochDialog((QWidget *)parent(), DIALOG_TYPE::ANGLE);
        if (aIn->exec() == QDialog::Accepted) {
            _op.toYrotate(aIn->ang().toDouble() * M_PI / 180);
            this->setItemText(OPERATORS::RY, _op.getOperatorName());
        } else {
            this->setCurrentIndex(lastActivated);
            this->setItemText(lastActivated, _op.getOperatorName());
        }
        delete aIn;
    } else if (index == OPERATORS::RZ) {
        auto aIn = new BlochDialog((QWidget *)parent(), DIALOG_TYPE::ANGLE);
        if (aIn->exec() == QDialog::Accepted) {
            _op.toZrotate(aIn->ang().toDouble() * M_PI / 180);
            this->setItemText(OPERATORS::RZ, _op.getOperatorName());
        } else {
            this->setCurrentIndex(lastActivated);
            this->setItemText(lastActivated, _op.getOperatorName());
        }
        delete aIn;
    } else {
        _op.toId();
    }

    lastActivated = index;
}

Operator &CircuitOperator::getOperator() { return _op; }

void CircuitOperator::setState(STATE state) {
    if (state == STATE::PASSIVE) {
        this->setStyleSheet("");
    } else if (state == STATE::ACTIVE) {
        this->setStyleSheet("QComboBox { font-weight: bold; }");
    }
}

void CircuitOperator::clearComboBoxNames() {
    this->setItemText(OPERATORS::PHI, "Phi");
    this->setItemText(OPERATORS::RX, "Rx");
    this->setItemText(OPERATORS::RY, "Ry");
    this->setItemText(OPERATORS::RZ, "Rz");
}
