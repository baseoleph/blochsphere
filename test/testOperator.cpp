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

#include "operator.h"
#include "point.h"
#include "unitaryOperators.hpp"
#include <QMap>
#include <QRandomGenerator64>
#include <QtGlobal>
#include <gtest/gtest.h>
#include <qglobal.h>

bool fuzzyCompare(double a, double b) {
    return qAbs(a - b) <= EPSILON;
}

bool compareOperators2x2(unitaryMatrix op1, unitaryMatrix op2) {
    bool cmp = true;
    cmp &= fuzzyCompare(op1.a.real(), op2.a.real());
    cmp &= fuzzyCompare(op1.a.imag(), op2.a.imag());

    cmp &= fuzzyCompare(op1.b.real(), op2.b.real());
    cmp &= fuzzyCompare(op1.b.imag(), op2.b.imag());

    cmp &= fuzzyCompare(op1.c.real(), op2.c.real());
    cmp &= fuzzyCompare(op1.c.imag(), op2.c.imag());

    cmp &= fuzzyCompare(op1.d.real(), op2.d.real());
    cmp &= fuzzyCompare(op1.d.imag(), op2.d.imag());

    if (!cmp) {
        std::cout << "----------------------------------------------\n";
        std::cout << "Expected\n";
        std::cout << "(" << op1.a.real() << ", " << op1.a.imag() << ") \t";
        std::cout << "(" << op1.b.real() << ", " << op1.b.imag() << ") \n";
        std::cout << "(" << op1.c.real() << ", " << op1.c.imag() << ") \t";
        std::cout << "(" << op1.d.real() << ", " << op1.d.imag() << ") \n";
        std::cout << "\nActual\n";
        std::cout << "(" << op2.a.real() << ", " << op2.a.imag() << ") \t";
        std::cout << "(" << op2.b.real() << ", " << op2.b.imag() << ") \n";
        std::cout << "(" << op2.c.real() << ", " << op2.c.imag() << ") \t";
        std::cout << "(" << op2.d.real() << ", " << op2.d.imag() << ") \n";
        std::cout << "----------------------------------------------\n";
    }

    return cmp;
}

TEST(Operator, zxDecomposition) {
    QVector<unitaryMatrix> ops = unitaryOperators2x2();

    for (int k = 0; k < ops.size(); ++k) {
        decomposition dec = Operator::zxDecomposition(ops[k]);

        dec.alpha *= M_PI / 180;
        dec.beta *= M_PI / 180;
        dec.delta *= M_PI / 180;
        dec.gamma *= M_PI / 180;

        complex i{0, 1};
        double  v = dec.gamma / 2.;
        complex a = exp(i * (dec.alpha - dec.beta / 2.0 - dec.delta / 2.0)) * cos(v);
        complex b = -i * exp(i * (dec.alpha - dec.beta / 2.0 + dec.delta / 2.0)) * sin(v);
        complex c = -i * exp(i * (dec.alpha + dec.beta / 2.0 - dec.delta / 2.0)) * sin(v);
        complex d = exp(i * (dec.alpha + dec.beta / 2.0 + dec.delta / 2.0)) * cos(v);

        unitaryMatrix opActual{a, b, c, d};
        EXPECT_TRUE(compareOperators2x2(ops[k], opActual)) << "Test case number " << k << "\n\n";
    }
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
