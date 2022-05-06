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

#ifndef OPITEM_HPP
#define OPITEM_HPP

#include "src/quantum/Operator.h"
#include <QListWidgetItem>

class OpItem : public QListWidgetItem {
    Operator oper;

public:
    OpItem(QListWidget *parent, const QString &opName, Operator op);
    Operator getOp();
};

#endif // OPITEM_HPP