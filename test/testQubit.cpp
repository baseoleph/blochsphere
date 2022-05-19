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

#include "qubits.h"
#include "src/quantum/Qubit.h"
#include "utility.h"
#include <gtest/gtest.h>

using namespace Utility;

bool compare(QubitFields qf1, QubitFields qf2, const std::string &title) {
    bool flag = true;
    if (qf1.aCheck) {
        EXPECT_TRUE(fuzzyCompare(qf1.a, qf2.a))
            << ((flag = false) ? "" : "") << title << " a;\nexpected: "
            << "(" << qf1.a.real() << ", " << qf1.a.imag() << ") "
            << "\n  actual: (" << qf2.a.real() << ", " << qf2.a.imag() << ")";
    }
    if (qf1.bCheck) {
        EXPECT_TRUE(fuzzyCompare(qf1.b, qf2.b))
            << ((flag = false) ? "" : "") << title << " b;\nexpected: "
            << "(" << qf1.b.real() << ", " << qf1.b.imag() << ") "
            << "\n  actual: (" << qf2.b.real() << ", " << qf2.b.imag() << ")";
    }

    if (qf1.theCheck) {
        EXPECT_TRUE(fuzzyCompare(qf1.the, qf2.the))
            << ((flag = false) ? "" : "") << title << " the;\nexpected: " << qf1.the
            << "\n  actual: " << qf2.the;
    }
    if (qf1.phiCheck) {
        EXPECT_TRUE(fuzzyCompare(qf1.phi, qf2.phi))
            << ((flag = false) ? "" : "") << title << " phi;\nexpected: " << qf1.phi
            << "\n  actual: " << qf2.phi;
    }

    if (qf1.xCheck) {
        EXPECT_TRUE(fuzzyCompare(qf1.x, qf2.x))
            << ((flag = false) ? "" : "") << title << " x;\nexpected: " << qf1.x
            << "\n  actual: " << qf2.x;
    }
    if (qf1.yCheck) {
        EXPECT_TRUE(fuzzyCompare(qf1.y, qf2.y))
            << ((flag = false) ? "" : "") << title << " y;\nexpected: " << qf1.y
            << "\n  actual: " << qf2.y;
    }
    if (qf1.zCheck) {
        EXPECT_TRUE(fuzzyCompare(qf1.z, qf2.z))
            << ((flag = false) ? "" : "") << title << " z;\nexpected: " << qf1.z
            << "\n  actual: " << qf2.z;
    }
    return flag;
}

TEST(Qubit, qubitRepresentation) {
    QVector<QubitFields> qfs = qubits();

    for (int i = 0; i < qubits().size(); ++i) {
        bool  flag = true;
        Qubit q1(qfs[i].a, qfs[i].b);
        Qubit q2(qfs[i].the, qfs[i].phi);
        Qubit q3(qfs[i].x, qfs[i].y, qfs[i].z);

        QubitFields expected = qfs[i];
        QubitFields initActual;
        initActual.a = q1.a();
        initActual.b = q1.b();
        initActual.the = q2.the();
        initActual.phi = q2.phi();
        initActual.x = q3.x();
        initActual.y = q3.y();
        initActual.z = q3.z();
        QubitFields q1Field = getQubitFields(q1);
        QubitFields q2Field = getQubitFields(q2);
        QubitFields q3Field = getQubitFields(q3);

        flag &= compare(expected, initActual, "Init");
        flag &= compare(expected, q1Field, "Complex");
        flag &= compare(expected, q2Field, "ThePhi");
        flag &= compare(expected, q3Field, "XYZ");

        EXPECT_TRUE(flag) << "Test case: " << i;
        if (not flag) {
            std::cout << "Complex\n";
            q1.printQubit();
            std::cout << "ThePhi\n";
            q2.printQubit();
            std::cout << "XYZ\n";
            q3.printQubit();
        }
    }
}
