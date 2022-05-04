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

#include "unitaryOperators.hpp"
#include "operator.hpp"
#include <gtest/gtest.h>

QVector<UnitaryMatrix2x2> unitaryOperators2x2() {
    QVector<UnitaryMatrix2x2> ops;
    complex                   a, b, c, d;
    UnitaryMatrix2x2          op;
    complex                   i = complex(0, 1);
    QRandomGenerator          rd(SEED);

    // 00
    {
        a = sqrt(1.0 / 3.0);
        b = sqrt(2.0 / 3.0);
        c = sqrt(2.0 / 3.0);
        d = -sqrt(1.0 / 3.0);
        EXPECT_TRUE(op.updateMatrix({a, b, c, d})) << "Not Unitary!";
        ops.append(op);
    }

    // 01
    {
        a = 0;
        b = -1.;
        c = 1.;
        d = 0;
        EXPECT_TRUE(op.updateMatrix({a, b, c, d})) << "Not Unitary!";
        ops.append(op);
    }

    // 02
    {
        a = 0;
        b = complex(0, 1);
        c = -complex(0, 1);
        d = 0;
        EXPECT_TRUE(op.updateMatrix({a, b, c, d})) << "Not Unitary!";
        ops.append(op);
    }

    // 03
    {
        a = -1;
        b = 0;
        c = 0;
        d = -complex(0, 1);
        EXPECT_TRUE(op.updateMatrix({a, b, c, d})) << "Not Unitary!";
        ops.append(op);
    }

    // 04
    {
        double t = 1.0 + sqrt(2.0);
        double t1 = 1.0 - sqrt(2.0);
        a = t / sqrt(6.0);
        b = t1 / sqrt(6.0);
        c = -t1 / sqrt(6.0);
        d = t / sqrt(6.0);
        EXPECT_TRUE(op.updateMatrix({a, b, c, d})) << "Not Unitary!";
        ops.append(op);
    }

    // 05 gate G(q)
    {
        rd.seed(SEED);
        double q = rd.generateDouble();
        a = sqrt(q);
        b = -sqrt(1.0 - q);
        c = sqrt(1.0 - q);
        d = sqrt(q);
        EXPECT_TRUE(op.updateMatrix({a, b, c, d})) << "Not Unitary!";
        ops.append(op);
    }

    // 06 gate G(p)
    {
        rd.seed(SEED);
        int p = rd.bounded(1, 100);
        a = sqrt(1.0 / p);
        b = -sqrt(1.0 - 1.0 / p);
        c = sqrt(1.0 - 1.0 / p);
        d = sqrt(1.0 / p);
        EXPECT_TRUE(op.updateMatrix({a, b, c, d})) << "Not Unitary!";
        ops.append(op);
    }

    // 07 approximate Z-X-decomposition for X^(-1/4)
    {
        double k1 = cos(M_PI / 8.0) * cos(M_PI / 8.0);
        double k2 = sin(M_PI / 8.0) * sin(M_PI / 8.0);
        double k3 = sin(M_PI / 8.0) * cos(M_PI / 8.0);
        a = complex(k1, -k3);
        b = complex(k2, k3);
        c = complex(k2, k3);
        d = complex(k1, -k3);
        EXPECT_TRUE(op.updateMatrix({a, b, c, d})) << "Not Unitary!";
        ops.append(op);
    }

    // 08 Z-X-decomposition for Z^(1/4)
    {
        a = 1.0;
        b = 0;
        c = 0;
        d = 1.0 / sqrt(2.0) * complex(1, 1);
        EXPECT_TRUE(op.updateMatrix({a, b, c, d})) << "Not Unitary!";
        ops.append(op);
    }

    // 09 approximate Z-X-decomposition for X^(1/4)
    {
        double k1 = cos(M_PI / 8.0) * cos(M_PI / 8.0);
        double k2 = sin(M_PI / 8.0) * sin(M_PI / 8.0);
        double k3 = sin(M_PI / 8.0) * cos(M_PI / 8.0);
        a = complex(k1, k3);
        b = complex(k2, -k3);
        c = complex(k2, -k3);
        d = complex(k1, k3);
        EXPECT_TRUE(op.updateMatrix({a, b, c, d})) << "Not Unitary!";
        ops.append(op);
    }

    // 10 approximate Z-X-decomposition for Y^(1/8)
    {
        a = complex(0.570364338882867, -0.140122339689182);
        b = complex(0.7320210135325, 0.345246993611228);
        c = complex(-0.251478818691494, 0.769291007746919);
        d = complex(0.493436356158812, -0.318544049654007);
        EXPECT_TRUE(op.updateMatrix({a, b, c, d})) << "Not Unitary!";
        ops.append(op);
    }

    // 11
    {
        a = 0;
        b = -1.0;
        c = 1.0;
        d = 0;
        EXPECT_TRUE(op.updateMatrix({a, b, c, d})) << "Not Unitary!";
        ops.append(op);
    }

    // 12
    {
        double t = 1.0 / sqrt(2.0);
        a = t * i;
        b = -t;
        c = -t;
        d = t * i;
        EXPECT_TRUE(op.updateMatrix({a, b, c, d})) << "Not Unitary!";
        ops.append(op);
    }

    // 13
    {
        double t = 1.0 / sqrt(2.0);
        a = -t * i;
        b = -t;
        c = t;
        d = t * i;
        EXPECT_TRUE(op.updateMatrix({a, b, c, d})) << "Not Unitary!";
        ops.append(op);
    }

    // 14
    {
        double t = 1.0 / sqrt(2.0);
        a = t;
        b = t * i;
        c = t * i;
        d = t;
        EXPECT_TRUE(op.updateMatrix({a, b, c, d})) << "Not Unitary!";
        ops.append(op);
    }

    // 15
    {
        double t = 1.0 / sqrt(2.0);
        a = t * i;
        b = t;
        c = t;
        d = t * i;
        EXPECT_TRUE(op.updateMatrix({a, b, c, d})) << "Not Unitary!";
        ops.append(op);
    }

    // 16
    {
        a = complex(1, 1) / 2.0;
        b = complex(1, 1) / 2.0;
        c = complex(-1, 1) / 2.0;
        d = complex(1, -1) / 2.0;
        EXPECT_TRUE(op.updateMatrix({a, b, c, d})) << "Not Unitary!";
        ops.append(op);
    }

    // 17
    {
        a = complex(1, 1) / 2.0;
        b = complex(-1, -1) / 2.0;
        c = complex(1, -1) / 2.0;
        d = complex(1, -1) / 2.0;
        EXPECT_TRUE(op.updateMatrix({a, b, c, d})) << "Not Unitary!";
        ops.append(op);
    }

    // 18 Rk operator for quantum Fourier transform
    {
        rd.seed(SEED);
        int k = rd.bounded(1, 9);
        a = 1;
        b = 0;
        c = 0;
        d = exp(i * M_PI / pow(2.0, k - 1));
        EXPECT_TRUE(op.updateMatrix({a, b, c, d})) << "Not Unitary!";
        ops.append(op);
    }

    // 19 Phase(g)
    {
        rd.seed(SEED);
        double g = rd.bounded(360.);
        g *= M_PI / 180.0;
        a = 1.0;
        b = 0;
        c = 0;
        d = exp(i * g);
        EXPECT_TRUE(op.updateMatrix({a, b, c, d})) << "Not Unitary!";
        ops.append(op);
    }

    // 20 sqrt(H)
    {
        double t1 = 4 + 2 * sqrt(2.0), t2 = 4 - 2 * sqrt(2.0);
        a = (3.0 + 2.0 * sqrt(2.0)) / t1 + i * (3.0 - 2.0 * sqrt(2.0)) / t2;
        b = (1 + sqrt(2.0)) / t1 + i * (1 - sqrt(2.0)) / t2;
        c = (1 + sqrt(2.0)) / t1 + i * (1 - sqrt(2.0)) / t2;
        d = 1.0 / t1 + i * 1.0 / t2;
        EXPECT_TRUE(op.updateMatrix({a, b, c, d})) << "Not Unitary!";
        ops.append(op);
    }

    // 21
    {
        a = sqrt(6) / 4.0 + i * sqrt(2) / 4.0;
        b = -sqrt(2) / 4.0 + i * sqrt(6) / 4.0;
        c = sqrt(2) / 4.0 + i * sqrt(6) / 4.0;
        d = sqrt(6) / 4.0 - i * sqrt(2) / 4.0;
        EXPECT_TRUE(op.updateMatrix({a, b, c, d})) << "Not Unitary!";
        ops.append(op);
    }

    // 22
    {
        double t = sqrt(1.0 / 3.0) + sqrt(1.0 / 6.0);
        double s = sqrt(1.0 / 3.0) - sqrt(1.0 / 6.0);
        a = t;
        b = s;
        c = -s;
        d = t;
        EXPECT_TRUE(op.updateMatrix({a, b, c, d})) << "Not Unitary!";
        ops.append(op);
    }

    // 23
    {
        a = sqrt(2.0 / 3.0);
        b = 1.0 / sqrt(3.0);
        c = 1.0 / sqrt(3.0);
        d = -sqrt(2.0 / 3.0);
        EXPECT_TRUE(op.updateMatrix({a, b, c, d})) << "Not Unitary!";
        ops.append(op);
    }

    // 24
    {
        a = sqrt(3.0) / 2.0;
        b = 0.5;
        c = 0.5;
        d = -sqrt(3.0) / 2.0;
        EXPECT_TRUE(op.updateMatrix({a, b, c, d})) << "Not Unitary!";
        ops.append(op);
    }

    // 25
    {
        a = -sqrt(2.0) / 2.0;
        b = -i * sqrt(2.0) / 2.0;
        c = i * sqrt(2.0) / 2.0;
        d = sqrt(2.0) / 2.0;
        EXPECT_TRUE(op.updateMatrix({a, b, c, d})) << "Not Unitary!";
        ops.append(op);
    }

    // 26
    {
        double  t = 1.0 / sqrt(2.0);
        complex fi = exp(i * M_PI / 4.0);
        a = fi * t;
        b = -fi * t * i;
        c = -fi * t * i;
        d = fi * t;
        EXPECT_TRUE(op.updateMatrix({a, b, c, d})) << "Not Unitary!";
        ops.append(op);
    }

    // 27 quantum operator HTH
    {
        a = 0.5 * (complex(1, 0) + exp(i * M_PI / 4.0));
        b = 0.5 * (complex(1, 0) - exp(i * M_PI / 4.0));
        c = 0.5 * (complex(1, 0) - exp(i * M_PI / 4.0));
        d = 0.5 * (complex(1, 0) + exp(i * M_PI / 4.0));
        EXPECT_TRUE(op.updateMatrix({a, b, c, d})) << "Not Unitary!";
        ops.append(op);
    }

    // 28
    {
        a = complex(1, -1) / 2.0;
        b = complex(1, 1) / 2.0;
        c = complex(1, 1) / 2.0;
        d = complex(1, -1) / 2.0;
        EXPECT_TRUE(op.updateMatrix({a, b, c, d})) << "Not Unitary!";
        ops.append(op);
    }

    // 29 quantum operator T(HTH)
    {
        complex r = exp(i * M_PI / 4.0);
        a = (1.0 + r) / 2.0;
        b = (1.0 - r) / 2.0;
        c = (r - i) / 2.0;
        d = (r + i) / 2.0;
        EXPECT_TRUE(op.updateMatrix({a, b, c, d})) << "Not Unitary!";
        ops.append(op);
    }

    // 30
    {
        a = complex(1.0 / sqrt(2.0), -1.0 / sqrt(2.0));
        b = 0;
        c = 0;
        d = complex(1.0 / sqrt(2.0), 1.0 / sqrt(2.0));
        EXPECT_TRUE(op.updateMatrix({a, b, c, d})) << "Not Unitary!";
        ops.append(op);
    }

    // 31
    {
        a = complex(0.342324444138781, -0.470196017743415);
        b = complex(0.611739821124854, -0.536194061038949);
        c = complex(-0.687696955655616, 0.434514185067998);
        d = complex(0.547986875225991, -0.194886389306039);
        EXPECT_TRUE(op.updateMatrix({a, b, c, d})) << "Not Unitary!";
        ops.append(op);
    }

    // 32
    {
        double t = 1.0 / 2.0;
        a = t * complex(1, 1);
        b = t * complex(1, -1);
        c = t * complex(-1, -1);
        d = t * complex(1, -1);
        EXPECT_TRUE(op.updateMatrix({a, b, c, d})) << "Not Unitary!";
        ops.append(op);
    }

    // 33
    {
        double t = 1.0 / 2.0;
        a = t * complex(-1, -1);
        b = t * complex(1, 1);
        c = t * complex(-1, 1);
        d = t * complex(-1, 1);
        EXPECT_TRUE(op.updateMatrix({a, b, c, d})) << "Not Unitary!";
        ops.append(op);
    }

    // 34
    {
        double z1 = cos(M_PI / 8.0);
        double z2 = sin(M_PI / 8.0);
        a = z1 * z1 + i * z2 * z2;
        b = (complex(1, 0) - i) * z2 * z1;
        c = (complex(1, 0) - i) * z2 * z1;
        d = z2 * z2 + i * z1 * z1;
        EXPECT_TRUE(op.updateMatrix({a, b, c, d})) << "Not Unitary!";
        ops.append(op);
    }

    // 35
    {
        double z1 = cos(M_PI / 8.0);
        double z2 = sin(M_PI / 8.0);
        a = z1 * z1 - i * z2 * z2;
        b = (complex(1, 0) + i) * z2 * z1;
        c = (complex(1, 0) + i) * z2 * z1;
        d = z2 * z2 - i * z1 * z1;
        EXPECT_TRUE(op.updateMatrix({a, b, c, d})) << "Not Unitary!";
        ops.append(op);
    }

    // 36 V(theta, phi) from A. Barenco
    {
        rd.seed(SEED);
        double the = rd.bounded(360.);
        double phi = rd.bounded(360.);
        the = the * M_PI / 180.0;
        phi = phi * M_PI / 180.0;
        a = cos(the / 2.0);
        b = -i * exp(-i * phi) * sin(the / 2.0);
        c = -i * exp(i * phi) * sin(the / 2.0);
        d = cos(the / 2.0);
        EXPECT_TRUE(op.updateMatrix({a, b, c, d})) << "Not Unitary!";
        ops.append(op);
    }

    // 37
    {
        rd.seed(SEED);
        int    n = rd.bounded(0, 1000);
        double alpha;
        alpha = sqrt(n);
        a = i * cos(M_PI * alpha / 2.0);
        b = sin(M_PI * alpha / 2.0);
        c = sin(M_PI * alpha / 2.0);
        d = i * cos(M_PI * alpha / 2.0);
        EXPECT_TRUE(op.updateMatrix({a, b, c, d})) << "Not Unitary!";
        ops.append(op);
    }

    // 38
    {
        rd.seed(SEED);
        double theta = rd.bounded(360.);
        theta = theta * M_PI / 180.0;
        a = cos(theta);
        b = sin(theta);
        c = sin(theta);
        d = -cos(theta);
        EXPECT_TRUE(op.updateMatrix({a, b, c, d})) << "Not Unitary!";
        ops.append(op);
    }

    // 39 X
    {
        a = 0;
        b = 1;
        c = 1;
        d = 0;
        EXPECT_TRUE(op.updateMatrix({a, b, c, d})) << "Not Unitary!";
        ops.append(op);
    }

    // 40 Y
    {
        a = 0;
        b = -i;
        c = i;
        d = 0;
        EXPECT_TRUE(op.updateMatrix({a, b, c, d})) << "Not Unitary!";
        ops.append(op);
    }

    // 41 Z
    {
        a = 1;
        b = 0;
        c = 0;
        d = -1;
        EXPECT_TRUE(op.updateMatrix({a, b, c, d})) << "Not Unitary!";
        ops.append(op);
    }

    // 42 H
    {
        a = 1 / sqrt(2);
        b = 1 / sqrt(2);
        c = 1 / sqrt(2);
        d = -1 / sqrt(2);
        EXPECT_TRUE(op.updateMatrix({a, b, c, d})) << "Not Unitary!";
        ops.append(op);
    }

    // 43 S
    {
        a = 1;
        b = 0;
        c = 0;
        d = i;
        EXPECT_TRUE(op.updateMatrix({a, b, c, d})) << "Not Unitary!";
        ops.append(op);
    }

    // 44 T
    {
        a = 1;
        b = 0;
        c = 0;
        d = exp(i * (M_PI / 4));
        EXPECT_TRUE(op.updateMatrix({a, b, c, d})) << "Not Unitary!";
        ops.append(op);
    }

    return ops;
}
