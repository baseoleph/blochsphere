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

#include "OpItem.h"
#include "MainWindow.h"
#include <QGridLayout>
#include <QLineEdit>
#include <QMenuBar>
#include <QString>
#include <QTime>
#include <QToolBar>

OpItem::OpItem(QListWidget *parent, const QString &opName, Operator op)
    : QListWidgetItem(opName, parent), oper(op) {
    this->setToolTip(op.getCurOperatorMatrixStr());
}

Operator OpItem::getOp() { return oper; }