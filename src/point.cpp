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

#include "point.h"
#include <complex>

Point::Point() {
    the_ = 0;
    phi_ = 0;
    evalXYZ();
}

Point::Point(double x, double y, double z) : x_(x), y_(y), z_(z) {
    evalPT();
}

Point::Point(double the, double phi) : the_(the), phi_(phi) {
    evalXYZ();
}

// TODO after all may be set it inline?
void Point::evalPT() {
    the_ = acos(z_ / sqrt(x_ * x_ + y_ * y_ + z_ * z_));
    phi_ = atan2(y_, x_);
}

void Point::evalXYZ() {
    x_ = cos(phi_) * sin(the_);
    y_ = sin(phi_) * sin(the_);
    z_ = cos(the_);
}
