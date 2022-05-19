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
#include <gtest/gtest.h>

TEST(Operator, multiplicationIdentity) {
    QVector<std::pair<UnitaryMatrix2x2, UnitaryMatrix2x2>> ops = identityPairs();

    for (int i = 0; i < ops.size(); ++i) {
        EXPECT_TRUE(UnitaryMatrix2x2::compareOperators(ops[i].first, ops[i].second))
            << "Not equal pair: " << i;
    }
}
