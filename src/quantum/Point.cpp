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

#include "Point.h"
#include <complex>

Point::Point() { evalPT(); }
Point::Point(double x, double y, double z) : x_(x), y_(y), z_(z) { evalPT(); }
Point::Point(double the, double phi) : the_(the), phi_(phi) { evalXYZ(); }

void Point::changePoint(double x, double y, double z) {
    x_ = x;
    y_ = y;
    z_ = z;
    evalPT();
}

void Point::changePoint(double the, double phi) {
    the_ = the;
    phi_ = phi;
    evalXYZ();
}

void Point::evalPT() {
    the_ = acos(z() / sqrt(x() * x() + y() * y() + z() * z()));
    phi_ = atan2(y(), x());
}

void Point::evalXYZ() {
    x_ = sin(the_) * cos(phi_);
    y_ = sin(the_) * sin(phi_);
    z_ = cos(the_);
}

double Point::getXyzLen() { return sqrt(x_ * x_ + y_ * y_ + z_ * z_); }
