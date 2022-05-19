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

#include "identityOperatorPairs.h"
#include "src/quantum/Operator.h"

QVector<std::pair<UnitaryMatrix2x2, UnitaryMatrix2x2>> identityPairs() {
    QVector<std::pair<UnitaryMatrix2x2, UnitaryMatrix2x2>> ops;
    Operator                                               op1;
    Operator                                               op2;
    UnitaryMatrix2x2                                       um;
    QRandomGenerator                                       rd(SEED);
    double                                                 the = 0;

    // 00
    op1.toS();
    op2.setOperator(um.getT() * um.getT());
    ops.append({op1.getOperator(), op2.getOperator()});

    // 01
    op1.setOperator(-um.getY());
    op2.setOperator(um.getX() * um.getY() * um.getX());
    ops.append({op1.getOperator(), op2.getOperator()});

    // 02
    op1.setOperator(um.getZ());
    op2.setOperator(um.getH() * um.getX() * um.getH());
    ops.append({op1.getOperator(), op2.getOperator()});

    // 03
    op1.setOperator(-um.getY());
    op2.setOperator(um.getH() * um.getY() * um.getH());
    ops.append({op1.getOperator(), op2.getOperator()});

    // 04
    op1.setOperator(um.getX());
    op2.setOperator(um.getH() * um.getZ() * um.getH());
    ops.append({op1.getOperator(), op2.getOperator()});

    // 05
    rd.seed(SEED);
    the = rd.bounded(360.) / 180 * M_PI;
    op1.setOperator(um.getYrotate(-the));
    op2.setOperator(um.getX() * um.getYrotate(the) * um.getX());
    ops.append({op1.getOperator(), op2.getOperator()});

    // 06
    rd.seed(SEED);
    the = rd.bounded(360.) / 180 * M_PI;
    op1.setOperator(um.getZrotate(-the));
    op2.setOperator(um.getX() * um.getZrotate(the) * um.getX());
    ops.append({op1.getOperator(), op2.getOperator()});

    // 07
    op1.setOperator(um.getXrotate(45. / 180 * M_PI));
    op2.setOperator(um.getH() * um.getT() * um.getH());
    ops.append({op1.getOperator(), op2.getOperator()});

    // 08
    op1.setOperator(um.getId());
    op2.setOperator(um.getX() * um.getX());
    ops.append({op1.getOperator(), op2.getOperator()});

    // 09
    op1.setOperator(um.getId());
    op2.setOperator(um.getY() * um.getY());
    ops.append({op1.getOperator(), op2.getOperator()});

    // 10
    op1.setOperator(um.getId());
    op2.setOperator(um.getZ() * um.getZ());
    ops.append({op1.getOperator(), op2.getOperator()});

    // 11
    op1.setOperator(um.getId());
    op2.setOperator(um.getH() * um.getH());
    ops.append({op1.getOperator(), op2.getOperator()});

    // 12
    op1.setOperator(um.getY());
    op2.setOperator(um.getZ() * um.getX());
    ops.append({op1.getOperator(), op2.getOperator()});

    // 13
    op1.setOperator(um.getT());
    op2.setOperator(um.getZrotate(45. / 180 * M_PI));
    ops.append({op1.getOperator(), op2.getOperator()});

    // 14
    op1.setOperator(um.getYrotate(-180. / 180 * M_PI));
    op2.setOperator(um.getZ() * um.getX());
    ops.append({op1.getOperator(), op2.getOperator()});

    // 15
    op1.setOperator(um.getY());
    op2.setOperator(um.getX() * um.getZ());
    ops.append({op1.getOperator(), op2.getOperator()});

    return ops;
}
