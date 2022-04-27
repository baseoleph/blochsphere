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

#ifndef CIRCUITOPERATOR_H
#define CIRCUITOPERATOR_H

#include "operator.hpp"
#include <QComboBox>

enum OPERATORS { ID = 0, X, Y, Z, H, S, T };

class CircuitOperator : public QComboBox {
    Q_OBJECT
public:
    CircuitOperator(QWidget *parent, Operator op = Operator());

    QString getOperatorName();
    Operator& getOperator();

public slots:
    void slotOperatorChanged(int index);

private:
    Operator _op;
};

#endif // CIRCUITOPERATOR_H
