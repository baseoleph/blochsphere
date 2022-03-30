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

#include "operator.hpp"
#include "point.hpp"
#include "unitaryOperators.hpp"
#include <QMap>
#include <QtGlobal>
#include <gtest/gtest.h>
#include <qglobal.h>

UnitaryMatrix2x2 checkMatrixZXDecomposition(UnitaryMatrix2x2 op) {
    decomposition dec = Operator::zxDecomposition(op);

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

    UnitaryMatrix2x2 opActual;
    opActual.updateMatrix(a, b, c, d);
    return opActual;
}

TEST(Operator, zxDecompositionStatic) {
    QVector<UnitaryMatrix2x2> ops = unitaryOperators2x2();
    std::cout << "Static cases seed: " << SEED << "\n";

    for (int k = 0; k < ops.size(); ++k) {
        EXPECT_TRUE(UnitaryMatrix2x2::isUnitaryMatrix(ops[k]))
            << "Test case number " << k << "\n\n";

        EXPECT_TRUE(UnitaryMatrix2x2::compareOperators(ops[k], checkMatrixZXDecomposition(ops[k])))
            << "Test case number " << k << "\n\n";
    }
}

TEST(Operator, zxDecompositionRandom) {
    std::cout << "Static cases seed: " << SEED << "\n";
    int randomCnt = 10000;

    for (int k = 0; k < randomCnt; ++k) {
        quint64          seed = QRandomGenerator::global()->generate64();
        UnitaryMatrix2x2 op = Operator::genRandUnitaryMatrix(seed);
        EXPECT_TRUE(UnitaryMatrix2x2::isUnitaryMatrix(op))
            << "Random operator seed: " << seed << "\n\n";
        EXPECT_TRUE(UnitaryMatrix2x2::compareOperators(op, checkMatrixZXDecomposition(op)));
    }
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
