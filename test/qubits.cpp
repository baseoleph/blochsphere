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

#include "qubits.h"
#include "unitaryMatrix2x2.hpp"
#include <QRandomGenerator>

QVector<QubitFields> qubits() {
    QVector<QubitFields> qbs;
    QubitFields          qf;
    QRandomGenerator     rd(SEED);
    complex              i{0, 1};

    // 00
    {
        rd.seed(SEED);
        qf.a = 1;
        qf.b = 0;
        qf.the = 0;
        qf.phi = rd.bounded(360.);
        qf.pCheck = false;
        qf.x = 0;
        qf.y = 0;
        qf.z = 1;
        qbs.append(qf);
    }

    // 01
    {
        rd.seed(SEED);
        qf.a = 0;
        qf.b = 1;
        qf.bCheck = false;
        qf.the = 180. / 180 * M_PI;
        qf.phi = rd.bounded(360.) / 180 * M_PI;
        qf.pCheck = false;
        qf.x = 0;
        qf.y = 0;
        qf.z = -1;
        qbs.append(qf);
    }

    // 02
    {
        qf.a = 1 / sqrt(2);
        qf.b = 1 / sqrt(2);
        qf.the = 90. / 180 * M_PI;
        qf.phi = 0. / 180 * M_PI;
        qf.pCheck = false;
        qf.x = 1;
        qf.y = 0;
        qf.z = 0;
        qbs.append(qf);
    }

    // 03
    {
        qf.a = 1 / sqrt(2);
        qf.b = -1 / sqrt(2);
        qf.the = 90. / 180 * M_PI;
        qf.phi = 180. / 180 * M_PI;
        qf.pCheck = false;
        qf.x = -1;
        qf.y = 0;
        qf.z = 0;
        qbs.append(qf);
    }

    // 04
    {
        qf.a = 1 / sqrt(2);
        qf.b = complex(0, 1 / sqrt(2));
        qf.the = 90. / 180 * M_PI;
        qf.phi = 90. / 180 * M_PI;
        qf.pCheck = false;
        qf.x = 0;
        qf.y = 1;
        qf.z = 0;
        qbs.append(qf);
    }

    // 05
    {
        qf.a = 1 / sqrt(2);
        qf.b = -complex(0, 1 / sqrt(2));
        qf.the = 90. / 180 * M_PI;
        qf.phi = -90. / 180 * M_PI;
        qf.pCheck = false;
        qf.x = 0;
        qf.y = -1;
        qf.z = 0;
        qbs.append(qf);
    }

    // 06
    {
        qf.a = sqrt(3) / 2;
        qf.b = -i / complex(2);
        qf.the = 60. / 180 * M_PI;
        qf.phi = -90. / 180 * M_PI;
        qf.pCheck = false;
        qf.x = 0;
        qf.y = -0.866025;
        qf.z = 0.5;
        qbs.append(qf);
    }

    // 07
    {
        qf.a = sqrt(3) / 2;
        qf.b = i / complex(2);
        qf.the = 60. / 180 * M_PI;
        qf.phi = 90. / 180 * M_PI;
        qf.pCheck = false;
        qf.x = 0;
        qf.y = 0.866025;
        qf.z = 0.5;
        qbs.append(qf);
    }

    // 08
    {
        qf.a = 1. / 2;
        qf.b = i * sqrt(3) / complex(2);
        qf.the = 120. / 180 * M_PI;
        qf.phi = 90. / 180 * M_PI;
        qf.pCheck = false;
        qf.x = 0;
        qf.y = 0.866025;
        qf.z = -0.5;
        qbs.append(qf);
    }

    // 09
    {
        qf.a = 1. / 2;
        qf.b = -i * sqrt(3) / complex(2);
        qf.the = 120. / 180 * M_PI;
        qf.phi = -90. / 180 * M_PI;
        qf.pCheck = false;
        qf.x = 0;
        qf.y = -0.866025;
        qf.z = -0.5;
        qbs.append(qf);
    }

    // 10
    {
        qf.a = 1. / sqrt(2);
        qf.b = (complex(1) + i) / 2.;
        qf.the = 90. / 180 * M_PI;
        qf.phi = 45. / 180 * M_PI;
        qf.pCheck = false;
        qf.x = 0.707107;
        qf.y = 0.707107;
        qf.z = 0;
        qbs.append(qf);
    }

    // 11
    {
        qf.a = 1. / sqrt(2);
        qf.b = exp(-i * M_PI / 4.) * 1. / sqrt(2);
        qf.the = 90. / 180 * M_PI;
        qf.phi = -45. / 180 * M_PI;
        qf.pCheck = false;
        qf.x = 0.707107;
        qf.y = -0.707107;
        qf.z = 0;
        qbs.append(qf);
    }

    // 12
    {
        qf.a = 0;
        qf.b = i;
        qf.the = 180. / 180 * M_PI;
        qf.phi = 90. / 180 * M_PI;
        qf.pCheck = false;
        qf.x = 0;
        qf.y = 0;
        qf.z = -1;
        qbs.append(qf);
    }

    // 13
    {
        qf.a = 1 / sqrt(2);
        qf.b = -1 / sqrt(2);
        qf.the = 90. / 180 * M_PI;
        qf.phi = 180. / 180 * M_PI;
        qf.pCheck = false;
        qf.x = -1;
        qf.y = 0;
        qf.z = 0;
        qbs.append(qf);
    }

    // 14
    {
        qf.a = 1 / sqrt(2);
        qf.b = 1 / sqrt(2);
        qf.the = 90. / 180 * M_PI;
        qf.phi = 0. / 180 * M_PI;
        qf.pCheck = false;
        qf.x = 1;
        qf.y = 0;
        qf.z = 0;
        qbs.append(qf);
    }

    // 15
    {
        qf.a = 1 / 2.;
        qf.b = sqrt(3) / 2;
        qf.the = 120. / 180 * M_PI;
        qf.phi = 0. / 180 * M_PI;
        qf.pCheck = false;
        qf.x = 0.866025;
        qf.y = 0;
        qf.z = -0.5;
        qbs.append(qf);
    }

    return qbs;
}

QubitFields getQubitFields(const Qubit &q) {
    QubitFields qf;
    qf.a = q.a();
    qf.b = q.b();
    qf.the = q.the();
    qf.phi = q.phi();
    qf.x = q.x();
    qf.y = q.y();
    qf.z = q.z();
    return qf;
}
