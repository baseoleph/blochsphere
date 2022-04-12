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
#include "unitaryOperators.hpp"
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
    std::cout << "Static cases seed: " << SEED << "\n";

    for (int k = 0; k < ops.size(); ++k) {
        EXPECT_TRUE(UnitaryMatrix2x2::isUnitaryMatrix(ops[k]))
            << "Test case number " << k << "\n\n";

        EXPECT_TRUE(UnitaryMatrix2x2::compareOperators(
            ops[k], checkMatrixDecomposition(ops[k], getDec, getMatrix)))
            << "Test case number " << k << "\n\n";
    }
}

void randomTestDecomposition(decomposition (*getDec)(UnitaryMatrix2x2),
                             matrix2x2(getMatrix)(decomposition)) {
    std::cout << "Static cases seed: " << SEED << "\n";
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

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
