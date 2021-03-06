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

#include "src/quantum/Operator.h"
#include "unitaryOperators.h"
#include <QMap>
#include <gtest/gtest.h>
#include <qglobal.h>

UnitaryMatrix2x2 checkMatrixDecomposition(UnitaryMatrix2x2 op,
                                          decomposition (*getDec)(UnitaryMatrix2x2),
                                          matrix2x2(getMatrix)(decomposition)) {
    UnitaryMatrix2x2 opActual;
    opActual.updateMatrix(getMatrix(getDec(op)));
    return opActual;
}

void staticTestDecomposition(decomposition (*getDec)(UnitaryMatrix2x2),
                             matrix2x2(getMatrix)(decomposition)) {
    QVector<UnitaryMatrix2x2> ops = unitaryOperators2x2();

    for (int k = 0; k < ops.size(); ++k) {
        EXPECT_TRUE(UnitaryMatrix2x2::isUnitaryMatrix(ops[k]))
            << "Not unitary; test case number " << k << "\n\n";

        EXPECT_TRUE(UnitaryMatrix2x2::compareOperators(
            ops[k], checkMatrixDecomposition(ops[k], getDec, getMatrix)))
            << "Not equal; test case number " << k << "\n\n";
    }
}

void randomTestDecomposition(decomposition (*getDec)(UnitaryMatrix2x2),
                             matrix2x2(getMatrix)(decomposition)) {
    int randomCnt = 10000;

    for (int k = 0; k < randomCnt; ++k) {
        quint64          seed = QRandomGenerator::global()->generate64();
        UnitaryMatrix2x2 op = Operator::genRandUnitaryMatrix(seed);
        EXPECT_TRUE(UnitaryMatrix2x2::isUnitaryMatrix(op))
            << "Random operator seed: " << seed << "\n\n";
        EXPECT_TRUE(UnitaryMatrix2x2::compareOperators(
            op, checkMatrixDecomposition(op, getDec, getMatrix)));
    }
}

TEST(Operator, zxDecompositionStatic) {
    staticTestDecomposition(Operator::zxDecomposition, Operator::getMatrixByZxDec);
}
TEST(Operator, zxDecompositionRandom) {
    randomTestDecomposition(Operator::zxDecomposition, Operator::getMatrixByZxDec);
}

TEST(Operator, zyDecompositionStatic) {
    staticTestDecomposition(Operator::zyDecomposition, Operator::getMatrixByZyDec);
}
TEST(Operator, zyDecompositionRandom) {
    randomTestDecomposition(Operator::zyDecomposition, Operator::getMatrixByZyDec);
}

TEST(Operator, xyDecompositionStatic) {
    staticTestDecomposition(Operator::xyDecomposition, Operator::getMatrixByXyDec);
}

TEST(Operator, xyDecompositionRandom) {
    randomTestDecomposition(Operator::xyDecomposition, Operator::getMatrixByXyDec);
}

TEST(Operator, zyxDecompositionStatic) {
    staticTestDecomposition(Operator::zyxDecomposition, Operator::getMatrixByZyxDec);
}

TEST(Operator, zyxDecompositionRandom) {
    randomTestDecomposition(Operator::zyxDecomposition, Operator::getMatrixByZyxDec);
}

TEST(Operator, vectorAngleStatic) {
    QVector<UnitaryMatrix2x2> ops = unitaryOperators2x2();

    for (int k = 0; k < ops.size(); ++k) {
        EXPECT_TRUE(UnitaryMatrix2x2::isUnitaryMatrix(ops[k]))
            << "Not unitary; test case number " << k << "\n\n";

        UnitaryMatrix2x2 opActual;
        opActual.updateMatrix(Operator::getMatrixByVecAng(Operator::vectorAngleDec(ops[k])));

        EXPECT_TRUE(UnitaryMatrix2x2::compareOperators(ops[k], opActual))
            << "Not equal; test case number " << k << "\n\n";
    }
}
